/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#ifndef C2C_END_POINT_DEMUX_H
#define C2C_END_POINT_DEMUX_H

#include <stdint.h>
#include <list>
#include "c2c-interface.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class c2cEndPoint;
//class Node;

/**
 * \brief  iTETRIS [WP600] - Demultiplexes packets to various transport layer endpoints
 *
 * This class serves as a lookup table to match partial or full Lookup criteria information
 * to an ns3::c2cEndPoint.  It internally contains a list of endpoints, and has APIs to
 * add and find endpoints in this demux.  This demux sits between ns3's c2c layer four
 * and the socket layer
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

class c2cEndPointDemux {
public:
  typedef std::list<c2cEndPoint *> EndPoints;
  typedef std::list<c2cEndPoint *>::iterator EndPointsI;

  c2cEndPointDemux ();
  ~c2cEndPointDemux ();

  EndPoints GetAllEndPoints (void);
  bool LookupPortLocal (uint16_t port);
  bool LookupLocal (uint64_t nodeId, uint16_t port);

  EndPoints Lookupc2c (Ptr<const c2cAddress> daddr,
				    uint16_t dport,
				    Ptr<const c2cAddress> saddr,
				    uint16_t sport);

  c2cEndPoint *Allocate (uint16_t port);

  void DeAllocate (c2cEndPoint *endPoint);

 private:
  EndPoints m_endPoints;
};

} // namespace ns3

#endif /* C2C_END_POINT_DEMUX_H */
