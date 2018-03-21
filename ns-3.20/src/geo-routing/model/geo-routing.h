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
 * Edited by Ramon Bauza
 *
 */

#ifndef GEOROUTINGPROTOCOL_H
#define GEOROUTINGPROTOCOL_H

#include <stdio.h>

#include "ns3/node.h"
#include "ns3/c2c-routing-protocol.h"
#include "ns3/c2c-l3-protocol.h"
#include "ns3/c2c-route.h"
#include "ns3/geo-broadcast.h"
#include "ns3/geo-unicast.h"
#include "ns3/geo-anycast.h"
#include "ns3/topo-broadcast.h"
#include "ns3/c2c.h"
#include "ns3/c2c-address.h"
#include "ns3/c2c-common-header.h"

namespace ns3
{
/**
 * \ingroup geo-routing
 *
 * \brief Geo routing protocol
 */

class GeoRoutingProtocol : public c2cRoutingProtocol
{
public:
  static TypeId GetTypeId (void);

  GeoRoutingProtocol ();
  virtual ~GeoRoutingProtocol();

  virtual struct c2cRoutingProtocol::output RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                                                         Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                                                         Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr);

  virtual bool RouteInput (Ptr<const Packet> p, const c2cCommonHeader &cheader,
                           Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);

  void SetTopo(Ptr<topoBroadcast> topo);
  void SetAny(Ptr<geoAnycast> any);
  void SetBroad(Ptr<geoBroadcast> broad);
  void SetUni(Ptr<geoUnicast> uni);
  virtual void Setc2c (Ptr<c2c> c2c);

protected:
  virtual void DoDispose (void);

private:
  /// c2c protocol
  Ptr<c2c> m_c2c;

  /// Geo Anycast protocol
  Ptr<geoAnycast> m_geoanycast;

  /// Geo Broadcast protocol
  Ptr<geoBroadcast> m_geobroadcast;

  /// Geo Unicast protocol
  Ptr<geoUnicast> m_geounicast;

  /// Topo Broadcast protocol
  Ptr<topoBroadcast> m_topobroadcast;
};
}
#endif /* GEOROUTINGPROTOCOL_H */
