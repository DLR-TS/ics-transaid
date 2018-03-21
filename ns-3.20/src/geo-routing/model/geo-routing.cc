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


#include "ns3/log.h"
#include "geo-routing.h"

NS_LOG_COMPONENT_DEFINE ("GeoRoutingProtocol");

namespace ns3
{
NS_OBJECT_ENSURE_REGISTERED (GeoRoutingProtocol);

GeoRoutingProtocol::GeoRoutingProtocol ()
{
  m_geoanycast = CreateObject<geoAnycast> ();
  m_geobroadcast = CreateObject<geoBroadcast> ();
  m_geounicast = CreateObject<geoUnicast> ();
  m_topobroadcast = CreateObject<topoBroadcast> ();
}

TypeId
GeoRoutingProtocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GeoRoutingProtocol")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<GeoRoutingProtocol> ()
  ;
  return tid;
}

GeoRoutingProtocol::~GeoRoutingProtocol ()
{
}

void
GeoRoutingProtocol::DoDispose ()
{
  c2cRoutingProtocol::DoDispose ();
}

void
GeoRoutingProtocol::SetAny(Ptr<geoAnycast> any)
{
  m_geoanycast = any;
}

void
GeoRoutingProtocol::SetBroad(Ptr<geoBroadcast> broad)
{
  m_geobroadcast = broad;
}

void
GeoRoutingProtocol::SetUni(Ptr<geoUnicast> uni)
{
  m_geounicast = uni;
}

void
GeoRoutingProtocol::SetTopo(Ptr<topoBroadcast> topo)
{
  m_topobroadcast = topo;
}

struct c2cRoutingProtocol::output
GeoRoutingProtocol::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                                 Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                                 Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p->GetUid ());
  sockerr = Socketc2c::ERROR_NOTERROR;
  struct c2cRoutingProtocol::output result;

  // case 1 GeoUnicast
  if (daddr->IsGeoUnicast())
  {
    result = m_geounicast->RouteOutput (p, sourcePosVector, daddr, lt, sn, oif, sockerr);
  }

  // case 2 GeoAnycast
  if (daddr->IsGeoAnycast())
  {
    result = m_geoanycast->RouteOutput (p, sourcePosVector, daddr, lt, sn, oif, sockerr);
  }

  // case 3 GeoBroadcast
  if (daddr->IsGeoBroadcast())
  {
    result = m_geobroadcast->RouteOutput (p, sourcePosVector, daddr, lt, sn, oif, sockerr);
  }

  // case 4 TopoBroadcast
  if (daddr->IsTopoBroadcast())
  {
    result = m_topobroadcast->RouteOutput (p, sourcePosVector, daddr, lt, sn, oif, sockerr);
  }

  return result;
}

bool
GeoRoutingProtocol::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &cheader, 
                                Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                                LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p->GetUid() << idev->GetAddress());

  NS_ASSERT (m_c2c != 0);
  // Check if input device supports c2c
  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0);
  bool result;

  // case 1 GeoUnicast
  if ((uint16_t)cheader.GetHtype() == Node::C2C_UNICAST)
  {
    result = m_geounicast->RouteInput (p, cheader, idev, ucb, lcb, ecb);
  }

  // case 2 GeoAnycast
  if ((uint16_t)cheader.GetHtype() == Node::C2C_GEOANYCAST)
  {
    result = m_geoanycast->RouteInput (p, cheader, idev, ucb, lcb, ecb);
  }

  // case 3 GeoBroadcast
  if ((uint16_t)cheader.GetHtype() == Node::C2C_GEOBCAST)
  {
    result = m_geobroadcast->RouteInput (p, cheader, idev, ucb, lcb, ecb);
  }

  // case 4 TopoBroadcast
  if ((uint16_t)cheader.GetHtype() == Node::C2C_TSB)
  {
    result = m_topobroadcast->RouteInput (p, cheader, idev, ucb, lcb, ecb);
  }

  return result;
}


void
GeoRoutingProtocol::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);
  m_c2c = c2c;
  m_geoanycast->Setc2c(c2c);
  m_geobroadcast->Setc2c(c2c);
  m_geounicast->Setc2c(c2c);
  m_topobroadcast->Setc2c(c2c);
}

}
