/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Eurecom, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author:  Fatma Hrizi <hrizi@eurecom.fr>, Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#include "ns3/log.h"
#include "c2c-route.h"
#include "ns3/net-device.h"

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (c2cRoute);
NS_LOG_COMPONENT_DEFINE("c2cRoute");

TypeId 
c2cRoute::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cRoute")
    .SetParent<Object> ()
    .AddConstructor<c2cRoute> ()
    ;
  return tid;
}

c2cRoute::c2cRoute ()
{}

void
c2cRoute::SetDestination (Ptr<c2cAddress> dest)
{
  m_dest = dest;
}

Ptr<c2cAddress>
c2cRoute::GetDestination (void) const
{
  return m_dest;
}

void
c2cRoute::SetSource (Ptr<c2cAddress> src)
{
  m_source = src;
}

Ptr<c2cAddress>
c2cRoute::GetSource (void) const
{
  return m_source;
}

void
c2cRoute::SetGateway (Ptr<c2cAddress> gw)
{
  NS_LOG_FUNCTION (this);
  m_gateway = gw;
}

Ptr<c2cAddress>
c2cRoute::GetGateway (void) const
{
  return m_gateway;
}

void
c2cRoute::SetOutputDevice (Ptr<NetDevice> outputDevice)
{
  m_outputDevice = outputDevice;
}

Ptr<NetDevice>
c2cRoute::GetOutputDevice (void) const
{
 return m_outputDevice;
}

}//namespace ns3
