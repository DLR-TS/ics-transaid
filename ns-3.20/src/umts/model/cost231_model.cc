

/* -*- Mode:C; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008, 2009 INRIA, UDcast
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 * <amine.ismail@udcast.com>
 */


#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include <math.h>
#include "cost231_model.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("Cost231Propagation");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED ( Cost231Propagation);

TypeId
Cost231Propagation::GetTypeId (void)
{
 static TypeId
 tid =
 TypeId ("ns3::Cost231Propagation")  .AddConstructor<Cost231Propagation> ();
 return tid;
}

Cost231Propagation::Cost231Propagation ()
{
 C = 0;
 m_shadowing = 10;
}

void
Cost231Propagation::SetLambda (double frequency, double speed)
{
 m_lambda = speed / frequency;
 m_frequency = frequency;
}

double
Cost231Propagation::GetShadowing (void)
{
 return m_shadowing;
}
void
Cost231Propagation::SetShadowing (double shadowing)
{
 m_shadowing = shadowing;
}

void
Cost231Propagation::SetLambda (double lambda)
{
 m_lambda = lambda;
 m_frequency = 300000000 / lambda;
}

double
Cost231Propagation::GetLambda (void) const
{
 return m_lambda;
}

void
Cost231Propagation::SetMinDistance (double minDistance)
{
 m_minDistance = minDistance;
 m_minDistance=m_minDistance/1000;
}
double
Cost231Propagation::GetMinDistance (void) const
{
 return m_minDistance;
}

void
Cost231Propagation::SetBSAntennaHeight (double height)
{
 m_BSAntennaHeight = height;
}

double
Cost231Propagation::GetBSAntennaHeight (void) const
{
 return m_BSAntennaHeight;
}

void
Cost231Propagation::SetSSAntennaHeight (double height)
{
 m_SSAntennaHeight = height;
}

double
Cost231Propagation::GetSSAntennaHeight (void) const
{
 return m_SSAntennaHeight;
}

void
Cost231Propagation::SetEnvironment (Environment env)
{
 m_environment = env;
}
Cost231Propagation::Environment
Cost231Propagation::GetEnvironment (void) const
{
 return m_environment;
}

double
Cost231Propagation::GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{

 double distance = a->GetDistanceFrom (b);
//   distance=distance/1000; // Commented by Ramon
  
 if (distance <= m_minDistance)
 {
 return 0.0;
 }
 


double log_f = log (m_frequency / 1000000000) / 2.302;
double C_H = 0.8 + ((1.11 * log_f) - 0.7) * m_SSAntennaHeight - (1.56 * log_f);
 double log_BSH = log (m_BSAntennaHeight) / 2.303;

 // from the COST231 wiki entry
 // 2.303 is for the logarithm base change

 double loss_in_db = 46.3 + (33.9 * log_f) - (13.82 * log_BSH) - C_H + ((44.9
 - 6.55 * log_BSH) * log (distance) / 2.303) + C + m_shadowing;


//  //std::cout << "Cost231. Loss="<< -loss_in_db<< " log_f="<< log_f << " C_H="<< C_H << 
// " log_BSH="<< log_BSH <<
// " distance="<< distance <<
// " C="<< C <<
// " m_shadowing="<< m_shadowing << std::endl;

 NS_LOG_DEBUG("COST231. Dist =" << distance*1000 << ", Path Loss = " << loss_in_db<<"\n");


 return (0 - loss_in_db);

}

double
Cost231Propagation::cost231_formula (double d)
{
 double C_H = 0.0; // the mobile antenna height correction factor
 double log_f = 0.0;
 double log_BSH = 0.0;

 if (d < m_minDistance)
 {
 NS_LOG_DEBUG ("Mobile nodes are too close to the BS");
 exit (1);
 }
 log_f = log (m_frequency);
 C_H = 0.8 + (1.1 * log_f / 2.303 - 0.7) * m_SSAntennaHeight - 1.56 * log_f / 2.303;
 log_BSH = log (m_BSAntennaHeight);

 // from the COST231 wiki entry
 // 2.303 is for the logarithm base change

double loss_in_db = 46.3 + 33.9 * log_f / 2.303 - 13.82 * log_BSH / 2.303
- C_H + (44.9 - 6.55 * (log_BSH / 2.303)) * log (d) / 2.303 + C;
return loss_in_db;
}
} 