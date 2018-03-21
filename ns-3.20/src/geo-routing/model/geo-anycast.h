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

#ifndef GEOANYCAST_H_
#define GEOANYCAST_H_

#include <map>

#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/c2c-routing-protocol.h"
#include "ns3/c2c-interface.h"
#include "ns3/c2c-l3-protocol.h"
#include "ns3/c2c-address.h"
#include "ns3/location-table.h"

namespace ns3
{
/**
 * \ingroup geoRouting
 *
 * \brief  iTETRIS [WP600] - geoAnycast routing protocol
 */
class geoAnycast : public c2cRoutingProtocol
{
public:
  static TypeId GetTypeId (void);

  geoAnycast ();
  virtual ~geoAnycast();
  virtual struct c2cRoutingProtocol::output RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                                                         Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                                                         Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr);
  virtual bool RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                           Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);


  virtual void Setc2c (Ptr<c2c> c2c);

protected:
  virtual void DoDispose (void);

private:
  // c2c protocol
  Ptr<c2c> m_c2c;
  typedef std::vector<uint32_t> T;
  std::map<uint64_t,T> packetReceived;
};

}

#endif /* GEOANYCAST_H_ */
