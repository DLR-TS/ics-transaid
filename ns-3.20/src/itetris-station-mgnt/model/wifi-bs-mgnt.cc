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
#include "wifi-bs-mgnt.h"


NS_LOG_COMPONENT_DEFINE ("WifiBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WifiBsMgnt);

TypeId WifiBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiBsMgnt")
    .SetParent<IpBaseStaMgnt> ()         
    .AddConstructor<WifiBsMgnt> ()       
    ;
  return tid;
}

WifiBsMgnt::~WifiBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ipv4Address* 
WifiBsMgnt::GetIpAddress (uint32_t nodeId) const
{
  Ipv4Address* address = NULL;
  if (nodeId == ID_BROADCAST)
    {
      address = GetIpBroadcastAddress ();
    }
  else
    {
      TriggerVehiclesScanning ();
      // TODO: Call the corresponding NetDevice (m_netDevice in base class) to update get the Ipv4Address of the vehicle with ID='nodeId'
      // address = DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->GetVehicleIpAddress(nodeId);
      address = new Ipv4Address("192.172.1.76");
    }
  return (address);
}

//NEW FUNCTION
Ipv6Address* 
WifiBsMgnt::GetIpv6Address (uint32_t nodeId) const
{
  Ipv6Address* address = NULL;
  if (nodeId == ID_BROADCAST)
    {
      address = GetIpv6MulticastAddress ();
    }
  else
    {
      TriggerVehiclesScanning ();
      // TODO: Call the corresponding NetDevice (m_netDevice in base class) to update get the Ipv4Address of the vehicle with ID='nodeId'
      // address = DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->GetVehicleIpAddress(nodeId);
      address = new Ipv6Address("3ffe:1900:4545:0003:2000:f8ff:fe21:67cf");
    }
  return (address);
}

  
void 
WifiBsMgnt::TriggerVehiclesScanning (void) const
{
  // TODO: Call the corresponding NetDevice (m_netDevice in base class) to update the set of vehicles registered with the base station
  
}

uint32_t 
WifiBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
WifiBsMgnt::GetNumberOfRegisteredUsers (void) const
{
    // TriggerVehiclesScanning();
	// TODO: call the corresponding NetDevice and get the list of registered users;
	// uint32_t users = DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->getAssociatedUsers();
	return (0);
}

double
WifiBsMgnt::GetCoverageRange (void) const
{
	// call the corresponding NetDevice and get the coverage range
	// TODO complete the CoverageRange of RSUSTAMANAGER
    //return DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
	return (0.0);
}

} // namespace ns3
