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

#include "winner-d1-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include <math.h>

NS_LOG_COMPONENT_DEFINE ("WinnerD1LossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WinnerD1LossModel);

TypeId 
WinnerD1LossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WinnerD1LossModel")
    .SetParent<WinnerLossModel> ()
    .AddConstructor<WinnerD1LossModel> ()
    ;
  return tid;
}

WinnerD1LossModel::WinnerD1LossModel ()
{}

double 
WinnerD1LossModel::CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const
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
      loss = 21.5 * log10 (distance) + 44.2 + 20.0 * log10 (m_frequency / 5.0e9);
    }
  else
    {
      loss = 40.0 * log10 (distance) + 10.5 - 18.5 * log10 (htx) - 18.5 * log10 (hrx) + 1.5 * log10 (m_frequency / 5.0e9);
    }
  NS_LOG_DEBUG ("Distance = " << distance << " loss LOS = " << loss << " dbp=" << dbp << " frequency = " << m_frequency << " htx = " << htx);
  return (loss);
}

double 
WinnerD1LossModel::CalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const
{
  double loss, htx, hrx;
  htx = a->GetAntennaHeight ();
  hrx = b->GetAntennaHeight ();
  loss = 55.4 + 25.1 * log10 (d1) + 21.3 * log10 (m_frequency / 5.0e9) - 0.13 * (htx - 25) * log10 (d1 / 100) - 0.9 * (hrx - 1.5);
  NS_LOG_DEBUG ("Distance d1 = " << d1 << " d2 = " << d2 << " loss NLOS = " << loss );
  return (loss);
}

} // namespace ns3
