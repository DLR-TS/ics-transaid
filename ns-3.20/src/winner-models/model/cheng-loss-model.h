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

#ifndef CHENG_LOSS_MODEL_H
#define CHENG_LOSS_MODEL_H

#include "winner-loss-model.h"
#include "ns3/random-variable.h"

namespace ns3 {     

class ChengLossModel : public WinnerLossModel
{
public:
  static TypeId GetTypeId (void);
  ChengLossModel ();
      void prTraceC(double dist, double PR, uint isLOS, uint32_t receiverID); //////////

private:
  double CalculateLossInLos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double distance) const;
  double CalculateLossInNlos (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double d1, double d2) const {return 0.0;}
  RandomVariable* m_shortDistShadowing;
  RandomVariable* m_largeDistShadowing; 
  

};

} // namespace ns3

#endif /* CHENG_LOSS_MODEL_H */
