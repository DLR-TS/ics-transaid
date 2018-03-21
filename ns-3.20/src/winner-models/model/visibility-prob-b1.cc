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

#include "visibility-prob-b1.h"
#include <math.h>
#include "ns3/double.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("VisibilityProbB1");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (VisibilityProbB1);

TypeId 
VisibilityProbB1::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VisibilityProbB1")
    .SetParent<VisibilityModel> ()
    .AddConstructor<VisibilityProbB1> ()
    ;
  return tid;
}

VisibilityProbB1::VisibilityProbB1 ()
{
  m_random = UniformVariable(0.0,1.0); 
}

VisibilityProbB1::~VisibilityProbB1 ()
{}

void
VisibilityProbB1::GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const
{
  dist1 = dist2 = a->GetDistanceFrom (b) / sqrt(2);
  NS_LOG_DEBUG ("Distances d1 = " << dist1 << " d2 = " << dist2);
}

bool
VisibilityProbB1::GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  bool vis = true;
  double distance, probLos;
  distance = a->GetDistanceFrom (b);
  probLos = GetMin(18 / distance, 1) * (1 - exp (- distance / 36)) + exp (- distance / 36);
  if (m_random.GetValue () > probLos)
    {
      vis = false;
    }
  NS_LOG_DEBUG ("Visibility = " << vis << " distance = "<< distance <<" LOS probability = " << probLos);
  return vis;
}

double
VisibilityProbB1::GetMin (double num1, double num2) const
{
  return (num1 < num2 ? num1 : num2);
}

} // namespace ns3
