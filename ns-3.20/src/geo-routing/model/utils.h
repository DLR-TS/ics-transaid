/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

/*
 *
 * Edited by Ramon Bauza
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <math.h>

#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/wifi-net-device.h"
#include "ns3/c2c-l3-protocol.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/c2c-common-header.h"
#include "ns3/c2c-address.h"
#include "ns3/location-table.h"

namespace ns3 {
  double CartesianDistance (double lat2, double lon2, double lat1, double lon1);
  struct c2cCommonHeader::LongPositionVector getMinDistToDest (Ptr <LocationTable> ntable, Ptr<c2cAddress> daddr);
  Ptr<c2cAddress> DirectNeighbour (Ptr <LocationTable> ntable, Ptr<c2cAddress> daddr);
  void store (Ptr<const Packet> p);
  double ArcInRadians(uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1);
  double DistanceInMeters(uint32_t lat2, uint32_t lon2, uint32_t lat1, uint32_t lon1);

}; // namespace ns3

#endif /* UTILS_H */
