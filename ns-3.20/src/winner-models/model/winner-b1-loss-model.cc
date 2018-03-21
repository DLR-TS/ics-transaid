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

#include "winner-b1-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include <math.h>

NS_LOG_COMPONENT_DEFINE ("WinnerB1LossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WinnerB1LossModel);

TypeId 
WinnerB1LossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WinnerB1LossModel")
    .SetParent<WinnerLossModel> ()
    .AddConstructor<WinnerB1LossModel> ()
    ;
  return tid;
}

WinnerB1LossModel::WinnerB1LossModel ()
{}

double 
WinnerB1LossModel::CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const
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
      loss = 22.7 * log10 (distance) + 41.0 + 20.0 * log10 (m_frequency / 5.0e9);
    }
  else
    {
      loss = 40.0 * log10 (distance) + 9.45 - 17.3 * log10 (htx) - 17.3 * log10 (hrx) + 2.7 * log10 (m_frequency / 5.0e9);
    }
  NS_LOG_DEBUG ("Distance = " << distance << " loss LOS = " << loss << " dbp=" << dbp << " frequency = " << m_frequency << " htx = " << htx << " hrx = " << hrx);
  return (loss);
}

double 
WinnerB1LossModel::CalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const
{
  double pr1, pr2, pr;
  pr1 = DoCalculateLossInNlos (a,b,d1,d2);
  pr2 = DoCalculateLossInNlos (a,b,d2,d1);
  pr = GetMin (pr1, pr2);
  return pr;
}

double 
WinnerB1LossModel::DoCalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const
{
  double loss = CalculateLossInLos (a,b,d1);
  if (d2 > (m_streetWidth / 2) )
    {
      double nj = GetMax (2.8 - 0.0024 * d1, 1.84);
      loss = loss + 20.0 - 12.5 * nj + 10.0 * nj * log10 (d2) + 3.0 * log10 (m_frequency / 5.0e9);
    }
  NS_LOG_DEBUG ("Distance d1 = " << d1 << " d2 = " << d2 << " loss NLOS = " << loss );
  return (loss);
}

} // namespace ns3
