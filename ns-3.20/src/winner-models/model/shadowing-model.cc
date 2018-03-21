/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "shadowing-model.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "visibility-model.h"

#include "ns3/enum.h"
#include "ns3/log.h"
#include <math.h>
#include <algorithm>
#include "ns3/pointer.h"

NS_LOG_COMPONENT_DEFINE ("ShadowingModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ShadowingModel);

TypeId 
ShadowingModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ShadowingModel")
    .SetParent<Object> ()
    .AddConstructor<ShadowingModel> ()
    .AddAttribute ("CorrelatedShadowing", "Correlated shadowing",
                   BooleanValue (false),
                   MakeBooleanAccessor (&ShadowingModel::m_correlatedShadowing),
                   MakeBooleanChecker ())
    .AddAttribute ("LosStd", 
                   "Standard deviation of the shadowing in LOS",
                   DoubleValue (3),
                   MakeDoubleAccessor (&ShadowingModel::SetLosStd),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

ShadowingModel::ShadowingModel ()
  : m_correlatedShadowing(false),
    m_losStd (3),
    m_nLosStd (4)
{
  m_lossShadowing = new NormalVariable (0.0,3*3);
  m_nLossShadowing = new NormalVariable (0.0,4*4);
  m_shadowingHFilt = new float[7];
  for (uint32_t i=0; i<7; i++) 
    {
      m_shadowingHFilt[i] = 0.866/pow(2.0, i);
    }
}

ShadowingModel::~ShadowingModel ()
{
  NS_LOG_FUNCTION_NOARGS ();
  delete m_lossShadowing;
  delete m_nLossShadowing;
  delete m_shadowingHFilt;
  m_lossShadowing = NULL;
  m_nLossShadowing = NULL;
  m_shadowingHFilt = NULL;
  while (m_shadowingSpline.size () > 1)
    {
      NS_LOG_DEBUG ("m_shadowingSpline.size ()="<<m_shadowingSpline.size ());
      while (m_shadowingSpline.back()->size () > 0)
        {
          NS_LOG_DEBUG ("m_shadowingSpline.back()->size ()="<<m_shadowingSpline.back()->size ());
          if (m_shadowingSpline.back()->back() != NULL)
            {
	      delete m_shadowingSpline.back()->back ();
            }
          m_shadowingSpline.back()->back () = NULL;
          m_shadowingSpline.back()->erase(m_shadowingSpline.back()->end ()-1);
	}
      if (m_shadowingSpline.back() != NULL)
        {
          delete m_shadowingSpline.back();
        }
      m_shadowingSpline.back() = NULL;
      m_shadowingSpline.erase(m_shadowingSpline.end ()-1);
    }
  NS_LOG_DEBUG ("m_shadowingSpline.size ()="<<m_shadowingSpline.size ());
  m_shadowingSpline.back() = NULL;
  m_shadowingSpline.erase(m_shadowingSpline.end ()-1);
}

double 
ShadowingModel::GetLosShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
  if (!m_correlatedShadowing)
    {
      return m_lossShadowing->GetValue ();
    }
  else
    {
      return GetLosCorrelatedShadowing(a,b);
    }
}

double 
ShadowingModel::GetNLosShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
  if (!m_correlatedShadowing)
    {
      return m_nLossShadowing->GetValue ();
    }
  else
    {
      return GetNLosCorrelatedShadowing (a,b);
    }
}

double 
ShadowingModel::GetNLosCorrelatedShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b) 
{
  uint32_t maxId = GetMax (a->GetNode()->GetId(),b->GetNode()->GetId());
  uint32_t minId = GetMin (a->GetNode()->GetId(),b->GetNode()->GetId());
  UpdateShadowingSpline (maxId,minId);
  ShadowingSpline* shadowSpline = m_shadowingSpline[maxId]->at(minId);
  
  double speed = 0.0, distmax = 10000.0, std_db = 0.0, PrShadow;
  if (a->GetDistanceFrom(b) < distmax) 
  {
    double txSpeed, rxSpeed;
    txSpeed = CalculateSpeed (a->GetVelocity ());
    rxSpeed = CalculateSpeed (b->GetVelocity ());
    speed = GetMax (txSpeed, rxSpeed); // speed = the maximum speed of both nodes
  }

  // get power loss by adding a log-normal random variable (shadowing)
  PrShadow = shadowSpline->GetSample(std_db, speed, m_shadowingHFilt, Simulator::Now ().GetSeconds (), m_nLossShadowing);
  NS_LOG_INFO ("NLOS correlated shadowing. [Speed|PrShadow] ["<<speed<<"|"<<PrShadow<<"]");
  return PrShadow;
}

double 
ShadowingModel::GetLosCorrelatedShadowing (Ptr<MobilityModel> a, Ptr<MobilityModel> b) 
{
  uint32_t maxId = GetMax (a->GetNode()->GetId(),b->GetNode()->GetId());
  uint32_t minId = GetMin (a->GetNode()->GetId(),b->GetNode()->GetId());
  UpdateShadowingSpline (maxId,minId);
  ShadowingSpline* shadowSpline = m_shadowingSpline[maxId]->at(minId);
  
  double speed = 0.0, distmax = 10000.0, std_db = 0.0, PrShadow;
  if (a->GetDistanceFrom(b) < distmax) 
  {
    double txSpeed, rxSpeed;
    txSpeed = CalculateSpeed (a->GetVelocity ());
    rxSpeed = CalculateSpeed (b->GetVelocity ());
    speed = GetMax (txSpeed, rxSpeed); // speed = the maximum speed of both nodes
  }

  // get power loss by adding a log-normal random variable (shadowing)
  PrShadow = shadowSpline->GetSample(std_db, speed, m_shadowingHFilt, Simulator::Now ().GetSeconds (), m_lossShadowing);
  NS_LOG_INFO("LOS correlated shadowing. [Speed|PrShadow] ["<<speed<<"|"<<PrShadow<<"]");
  return PrShadow;
}

double 
ShadowingModel::CalculateSpeed (Vector speed)
{
  return sqrt(speed.x*speed.x+speed.y*speed.y+speed.z*speed.z);
}

void
ShadowingModel::UpdateShadowingSpline (uint32_t maxId, uint32_t minId) 
{
  if (maxId + 1 > m_shadowingSpline.size ())
    {
      NS_LOG_DEBUG ("Creating ShadowingSpline for node ID = "<<maxId);
      while (maxId + 1 > m_shadowingSpline.size ())
        {
          m_shadowingSpline.push_back (NULL);
        }
      m_shadowingSpline[maxId] = new ShadowingLinks ();
    }
  else if (m_shadowingSpline[maxId] == NULL)
    {
      NS_LOG_DEBUG ("Creating ShadowingSpline for node ID = "<<maxId);
      m_shadowingSpline[maxId] = new ShadowingLinks ();
    }
  NS_ASSERT_MSG  (m_shadowingSpline[maxId],"maxId="<<maxId<<" m_shadowingSpline.size()="<<m_shadowingSpline.size ());
  UpdateShadowingLinks (m_shadowingSpline[maxId], maxId, minId);
}

void 
ShadowingModel::UpdateShadowingLinks (ShadowingLinks* shadowingLinks, uint32_t maxId, uint32_t minId) 
{
  while (shadowingLinks->size () != maxId)
    {
      shadowingLinks->push_back (NULL);
    }
  if (shadowingLinks->at(minId) == NULL)
    {
      NS_LOG_DEBUG ("Creating new link in ShadowingSpline for the pair of nodes = ("<<maxId<<","<<minId<<")");
      shadowingLinks->at(minId) =  new ShadowingSpline ();
    }
}

void 
ShadowingModel::SetIsCorrelatedShadowing (bool correlatedShadowing)
{
  m_correlatedShadowing = correlatedShadowing;
}
  
bool 
ShadowingModel::IsCorrelatedShadowing (void)
{
  return m_correlatedShadowing;
}

void 
ShadowingModel::SetLosStd (double losStd)
{
  delete m_lossShadowing;
  m_lossShadowing = NULL;
  m_lossShadowing = new NormalVariable (0.0,losStd*losStd);
}

void 
ShadowingModel::SetNLosStd (double nLosStd)
{
  delete m_nLossShadowing;
  m_nLossShadowing = NULL;
  m_nLossShadowing = new NormalVariable (0.0,nLosStd*nLosStd);
}

double 
ShadowingModel::DbmToW (double dbm) const
{
  double mw = pow(10.0,dbm/10.0);
  return mw / 1000.0;
}

double
ShadowingModel::DbmFromW (double w) const
{
  double dbm = log10 (w * 1000.0) * 10.0;
  return dbm;
}

double 
ShadowingModel::ConvertToDb (double shadow) const
{
  double shadowDb = log10 (shadow) * 10.0;
  return shadowDb;
}

double
ShadowingModel::GetMax (double num1, double num2) const
{
  return (num1 < num2 ? num2 : num1);
}

double
ShadowingModel::GetMin (double num1, double num2) const
{
  return (num1 < num2 ? num1 : num2);
}

} // namespace ns3
