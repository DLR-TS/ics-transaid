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

#include "cheng-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include <math.h>
#include <stdio.h>
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("ChengLossModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ChengLossModel);

TypeId 
ChengLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ChengLossModel")
    .SetParent<WinnerLossModel> ()
    .AddConstructor<ChengLossModel> ()
    ;
  return tid;
}

ChengLossModel::ChengLossModel ()
{
  m_shortDistShadowing = new NormalVariable (0.0,5.9*5.9);
  m_largeDistShadowing = new NormalVariable (0.0,6.6*6.6);
}

double 
ChengLossModel::CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const
{

  double loss, shadow;

  if ( distance < 10 )
    {
      loss = 0.0;
      shadow = 0.0;
    }
  else if ( distance < 220 )
    {
      loss = 80.5 + 19 * log10 (distance / 10);
      shadow = m_shortDistShadowing->GetValue ();

    }
  else
    {
      loss = 80.5 + 19 * log10 (220 / 10) + 40 * log10 (distance / 220);
      shadow = m_largeDistShadowing->GetValue ();
    }
  NS_LOG_DEBUG ("Distance = " << distance << " loss LOS[dB]= " << loss << " shadow[dB]=" << shadow);
  
 //  uint32_t receiverID = a->GetNode()->GetId(); ////////
 //  prTraceC(distance, (loss + shadow), 1, receiverID); ////////////
  
  return (loss + shadow);
}

void 
ChengLossModel::prTraceC(double dist, double PR, uint isLOS, uint32_t receiverID )
{
  
  FILE *fp = fopen("/home/michele/new/ns-3/PR3.txt", "a+");

    fprintf(fp, "%.9f %f %f %d %d \n",(Simulator::Now()).GetSeconds(), dist, PR, isLOS, receiverID);
    fclose(fp);
}


} // namespace ns3