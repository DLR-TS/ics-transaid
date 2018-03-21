/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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

#ifndef ITETRIS_NET_DEVICES_H
#define ITETRIS_NET_DEVICES_H

#include "ns3/uinteger.h"

namespace ns3 {

enum NetDeviceType {
  ITS_CCH = 0,
  ITS_SCH = 1,
  UMTS = 3,
  WIMAX = 4,
  DVB = 5,
  LTE_DEVICE = 6, 
  DEV_UNDEF
};

} //namespace ns3

#endif /* ITETRIS_NET_DEVICES_H */
