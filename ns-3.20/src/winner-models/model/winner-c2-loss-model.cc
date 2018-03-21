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

#include "winner-c2-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include <math.h>

NS_LOG_COMPONENT_DEFINE ("WinnerC2LossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WinnerC2LossModel);

TypeId 
WinnerC2LossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WinnerC2LossModel")
    .SetParent<WinnerLossModel> ()
    .AddConstructor<WinnerC2LossModel> ()
    ;
  return tid;
}

WinnerC2LossModel::WinnerC2LossModel ()
{}

double 
WinnerC2LossModel::CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const
{
  double htx = a->GetAntennaHeight () - GetEffEnvironmentHeight ();
  double hrx = b->GetAntennaHeight () - GetEffEnvironmentHeight ();
  double dbp = 4 * htx * hrx / m_lambda;
  double loss;

  if ( distance < m_minDistance )
    {
      loss = 0;
    }
  else if ( distance < dbp )
    {
      loss = 26.0 * log10 (distance) + 39.0 + 20.0 * log10 (m_frequency / 5.0e9);
    }
  else
    {
      loss = 40.0 * log10 (distance) + 13.47 - 14.0 * log10 (htx) - 14.0 * log10 (hrx) + 6.0 * log10 (m_frequency / 5.0e9);
    }
  NS_LOG_DEBUG ("Distance = " << distance << " loss LOS = " << loss << " dbp=" << dbp << " frequency = " << m_frequency << " htx = " << htx);
  return (loss);
}

double 
WinnerC2LossModel::CalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const
{
  double loss, htx;
  htx = a->GetAntennaHeight ();
  loss = (44.9 - 6.55 * log10 (htx)) * log10 (d1) + 34.46 + 5.83 * log10 (htx) + 23 * log10 (m_frequency / 5.0e9);       
  NS_LOG_DEBUG ("Distance d1 = " << d1 << " d2 = " << d2 << " loss NLOS = " << loss );
  return (loss);
}


} // namespace ns3
