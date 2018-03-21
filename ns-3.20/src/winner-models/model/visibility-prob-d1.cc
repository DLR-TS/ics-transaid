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

#include "visibility-prob-d1.h"
#include <math.h>
#include "ns3/double.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("VisibilityProbD1");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (VisibilityProbD1);

TypeId 
VisibilityProbD1::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VisibilityProbD1")
    .SetParent<VisibilityModel> ()
    .AddConstructor<VisibilityProbD1> ()
    .AddAttribute ("RefDistance", "Reference distance",
                   DoubleValue (1000.0),
                   MakeDoubleAccessor (&VisibilityProbD1::m_refDistance),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

VisibilityProbD1::VisibilityProbD1 ()
{
  m_refDistance = 1000.0;
  m_random = UniformVariable(0.0,1.0); 
}

VisibilityProbD1::~VisibilityProbD1 ()
{}

void
VisibilityProbD1::GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const
{
  dist1 = a->GetDistanceFrom (b);
  dist2 = 0.0;
  NS_LOG_DEBUG ("Distances d1 = " << dist1 << " d2 = " << dist2);
}

bool
VisibilityProbD1::GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  bool vis = true;
  double distance, probLos;
  distance = a->GetDistanceFrom (b);
  probLos = exp (- distance / m_refDistance);

  if (m_random.GetValue () > probLos)
    {
      vis = false;
    }
   NS_LOG_DEBUG ("Visibility = " << vis << " distance = "<< distance <<" LOS probability = " << probLos);
   return vis;
}

} // namespace ns3
