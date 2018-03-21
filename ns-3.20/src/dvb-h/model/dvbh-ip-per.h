/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */


#ifndef DVBH_IPPER_TABLE_H
#define DVBH_IPPER_TABLE_H
#include "ns3/object.h"


namespace ns3 {


class DvbhIpPer : public Object
{
public: 
  DvbhIpPer ();
  double GetIpPer (double ebno,uint8_t tableType);
  static TypeId GetTypeId (void);
  static double QpskTable1_2[2][11];            // 1 row: Eb/No
  static double QpskTable3_4[2][11];            // 1 row: Eb/No
  static double QamTable1_2[2][14];            // 1 row: Eb/No
  static double QamTable3_4[2][15];            // 1 row: Eb/No
  
};
}
#endif /* __bler_h__ */
