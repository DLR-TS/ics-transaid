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

#include "ns3/log.h"
#include "ip-base-station.h"

NS_LOG_COMPONENT_DEFINE ("IpBaseStation");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IpBaseStation);

TypeId IpBaseStation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::IpBaseStation")
    .SetParent<Object> ()    
    .AddConstructor<IpBaseStation> ()                
    ;
  return tid;
}
IpBaseStation::IpBaseStation ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

IpBaseStation::IpBaseStation (uint32_t nodeId, Ipv4Address ipAddress, uint32_t lat, uint32_t lon)
  : m_nodeId (nodeId),
    m_ipAddress (ipAddress),
    m_lat (lat),
    m_lon (lon)
{}

IpBaseStation::IpBaseStation (uint32_t nodeId, Ipv6Address ipAddress, uint32_t lat, uint32_t lon)
  : m_nodeId (nodeId),
    m_ipv6Address (ipAddress),
    m_lat (lat),
    m_lon (lon)
{}

IpBaseStation::IpBaseStation (uint32_t nodeId, Ipv4Address ipAddress1, Ipv6Address ipAddress2, uint32_t lat, uint32_t lon)
  : m_nodeId (nodeId),
    m_ipAddress (ipAddress1),
    m_ipv6Address(ipAddress2),
    m_lat (lat),
    m_lon (lon)
{}

IpBaseStation::~IpBaseStation ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void IpBaseStation::SetNodeId (uint32_t nodeId)
{
  m_nodeId = nodeId;
}

void IpBaseStation::SetIpAddress (Ipv4Address ipAddress)
{
  m_ipAddress = ipAddress;
}

//!!! NEW FUNCTION !!!
void IpBaseStation::SetIpv6Address (Ipv6Address ipAddress)
{
  m_ipv6Address = ipAddress;
}

void IpBaseStation::SetLat (uint32_t lat)
{
  m_lat = lat;
}

void IpBaseStation::SetLon (uint32_t lon)
{
  m_lon = lon;
}

uint32_t IpBaseStation::GetNodeId (void) const
{
  return (m_nodeId);
}

Ipv4Address IpBaseStation::GetIpAddress (void) const
{
  return (m_ipAddress);
}

Ipv6Address IpBaseStation::GetIpv6Address (void) const
{
  return (m_ipv6Address);
}


uint32_t IpBaseStation::GetLat (void) const
{ 
  return (m_lat);
}

uint32_t IpBaseStation::GetLon (void) const
{
  return (m_lon);
}

} // namespace ns3
