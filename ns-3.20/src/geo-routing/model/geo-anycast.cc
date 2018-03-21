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

#include "geo-anycast.h"
#include "utils.h"
#include "geoBroadAnycast-header.h"

NS_LOG_COMPONENT_DEFINE ("geoAnycast");

namespace ns3
{

TypeId
geoAnycast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::geo-routing::geoAnycast")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<geoAnycast> ()
  ;
  return tid;
}

geoAnycast::geoAnycast ()
{
}

geoAnycast::~geoAnycast ()
{
}

void
geoAnycast::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);

  m_c2c = c2c;
}

void
geoAnycast::DoDispose ()
{
  m_c2c = 0;
  c2cRoutingProtocol::DoDispose ();
}

struct c2cRoutingProtocol::output
geoAnycast::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                         Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                         Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr)
{
struct c2cRoutingProtocol::output result;
return result;
}

bool
geoAnycast::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                        Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                        LocalDeliverCallback lcb, ErrorCallback ecb)
{
return false;
}

}
