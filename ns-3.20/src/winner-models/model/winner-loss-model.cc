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
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

#include "winner-loss-model.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "visibility-model.h"
#include <stdio.h>
#include "ns3/enum.h"
#include "ns3/log.h"
#include <math.h>
#include <algorithm>
#include "ns3/pointer.h"
#include "shadowing-model.h"
#include "fading-model.h" 

NS_LOG_COMPONENT_DEFINE ("WinnerLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WinnerLossModel);

TypeId 
WinnerLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WinnerLossModel")
    .SetParent<PropagationLossModel> ()
    .AddAttribute ("Lambda", 
                   "The wavelength  (default is 5.9 GHz at 300 000 km/s).",
                   DoubleValue (300000000.0 / 5.90e9),
                   MakeDoubleAccessor (&WinnerLossModel::m_lambda),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Frequency", 
                   "The frequency",
                   DoubleValue (5.90e9),
                   MakeDoubleAccessor (&WinnerLossModel::m_frequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("StreetWidth", "The street width",
                   DoubleValue (20.0),
                   MakeDoubleAccessor (&WinnerLossModel::m_streetWidth),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("EffEnvironmentHeight", "The effective environment height takes into account the effect of the road traffic on the radio propagation",
                   DoubleValue (0),
                   MakeDoubleAccessor (&WinnerLossModel::m_effEnvironmentHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("VisibilityModel", "A pointer to the visibility model attached to this loss model.",
                   PointerValue (0),
                   MakePointerAccessor (&WinnerLossModel::SetVisibilityModel),
                   MakePointerChecker<VisibilityModel> ())
    .AddAttribute ("ShadowingModel", "A pointer to the shadowing model attached to this loss model.",
                   PointerValue (0),
                   MakePointerAccessor (&WinnerLossModel::m_shadowingModel),
                   MakePointerChecker<ShadowingModel> ())
    .AddAttribute ("FadingModel", "A pointer to the fading model attached to this loss model.",
                   PointerValue (0),
                   MakePointerAccessor (&WinnerLossModel::SetFadingModel),
                   MakePointerChecker<FadingModel> ())
    .AddAttribute ("MinDistance", 
                   "The distance under which the propagation model refuses to give results (m)",
                   DoubleValue (10),
                   MakeDoubleAccessor (&WinnerLossModel::SetMinDistance,
                                       &WinnerLossModel::GetMinDistance),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

WinnerLossModel::WinnerLossModel ()
  : m_visibilityModel (0),
    m_shadowingModel (0),
    m_fadingModel (0)
{}

WinnerLossModel::~WinnerLossModel ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
WinnerLossModel::SetVisibilityModel (Ptr<VisibilityModel> visibilityModel)
{
  m_visibilityModel = visibilityModel;
  m_visibilityModel->InitializeVisibilityModel ();
}

void 
WinnerLossModel::SetFadingModel (Ptr<FadingModel> fadingModel)
{
  m_fadingModel = fadingModel;
  m_fadingModel->Initialize();
}

void 
WinnerLossModel::SetStreetWidth (double streetWidth)
{
  m_streetWidth = streetWidth;
}

double 
WinnerLossModel::GetStreetWidth (void) const
{
  return m_streetWidth;
}

void 
WinnerLossModel::SetEffEnvironmentHeight (double effEnvironmentHeight)
{
  m_effEnvironmentHeight = effEnvironmentHeight;
}

double 
WinnerLossModel::GetEffEnvironmentHeight (void) const
{
  return m_effEnvironmentHeight;
}

void 
WinnerLossModel::SetMinDistance (double minDistance)
{
  m_minDistance = minDistance;
}

double 
WinnerLossModel::GetMinDistance (void) const
{
  return m_minDistance;
}

void 
WinnerLossModel::SetLambda (double frequency, double speed)
{
  m_lambda = speed / frequency;
}

void 
WinnerLossModel::SetFrequency (double freq) 
{
  m_frequency = freq;
}

void 
WinnerLossModel::SetLambda (double lambda)
{
  m_lambda = lambda;
}

double 
WinnerLossModel::GetLambda (void) const
{
  return m_lambda;
}

double 
WinnerLossModel::DbmToW (double dbm) const
{
  double mw = pow(10.0,dbm/10.0);
  return mw / 1000.0;
}

double
WinnerLossModel::DbmFromW (double w) const
{
  double dbm = log10 (w * 1000.0) * 10.0;
  return dbm;
}

double 
WinnerLossModel::DoCalcRxPower (double txPowerDbm,
                                          Ptr<MobilityModel> a,
                                          Ptr<MobilityModel> b) const
{
  double loss;
  if (m_visibilityModel == NULL)
    {
      double distance = a->GetDistanceFrom (b);
      loss = CalculateLossInLos (a,b,distance);
      if (m_shadowingModel)
        {
          loss = loss - m_shadowingModel->GetLosShadowing (a,b);
        }
      if(m_fadingModel)                               
        {
          loss = loss - m_fadingModel->GetLosFading (a,b);
        }
    }
  else if (m_visibilityModel->GetVisibility (a,b))
    {
      // Line of Sight
      double distance = a->GetDistanceFrom (b);
      loss = CalculateLossInLos (a,b,distance);
      if (m_shadowingModel)
        {
          loss = loss - m_shadowingModel->GetLosShadowing (a,b);
        }
      if(m_fadingModel)                
        {
          loss = loss - m_fadingModel->GetLosFading (a,b);
        }
    }
  else
    {
      // Non Line Of Sight
      double d1, d2;
      m_visibilityModel->GetNlosDistances(a, b, d1, d2);
      loss = CalculateLossInNlos (a,b,d1,d2);
      if (m_shadowingModel)
        {
          loss = loss - m_shadowingModel->GetNLosShadowing (a,b);
        }
      if(m_fadingModel)                           
        {
          loss = loss - m_fadingModel->GetNLosFading (a,b);
        }
    }
 double PR = txPowerDbm - loss;

  return PR;
}

double
WinnerLossModel::GetMax (double num1, double num2) const
{
  return (num1 < num2 ? num2 : num1);
}

double
WinnerLossModel::GetMin (double num1, double num2) const
{
  return (num1 < num2 ? num1 : num2);
}

// added by Andrea
int64_t WinnerLossModel::DoAssignStreams(int64_t stream)
{
	return 0;
}

} // namespace ns3
