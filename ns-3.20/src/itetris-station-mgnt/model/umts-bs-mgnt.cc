/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es> based on the code of Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/log.h"
#include "umts-bs-mgnt.h"
#include "ns3/umts-net-device.h"
#include "ns3/umts-basestation-manager.h"
#include "ns3/umts-userequipment-manager.h"
#include "ns3/addressing-support.h"

NS_LOG_COMPONENT_DEFINE ("UmtsBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsBsMgnt);

TypeId UmtsBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsBsMgnt")
    .SetParent<IpBaseStaMgnt> ()        
    .AddConstructor<UmtsBsMgnt> ()               
    ;
  return tid;
}

UmtsBsMgnt::~UmtsBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ipv4Address* 
UmtsBsMgnt::GetIpAddress (uint32_t nodeId) const
{
  Ipv4Address* address = NULL;
  TriggerVehiclesScanning (); 
  if (nodeId == ID_BROADCAST)
    { 
      address = GetIpBroadcastAddress ();
    }
  else
    {     
      address =DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->GetNodeUEIpAddress(nodeId);
    }
  return (address);
}


//NEW FUNCTION
Ipv6Address* 
UmtsBsMgnt::GetIpv6Address (uint32_t nodeId) const
{
  Ipv6Address* address = new Ipv6Address();
  TriggerVehiclesScanning (); 
  if (nodeId == ID_MULTICAST)
    { 
      *address = "ff02::1";
      //address = GetIpv6MulticastAddress ();
    }
  else
    {     
      address =DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->GetNodeUEIpv6Address(nodeId);
    }
  return (address);
}


void
UmtsBsMgnt::AddVehicle(Ptr<UMTSNetDevice> netDevice)
{

  DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->
      AddNodeUE(DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(netDevice)->GetManager()));  
}

void 
UmtsBsMgnt::TriggerVehiclesScanning (void) const
{  
  DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->TriggerNodeUEScanning();
}

uint32_t 
UmtsBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
UmtsBsMgnt::GetNumberOfRegisteredUsers (void) const
{

	// check if correct and add the method
	 TriggerVehiclesScanning();
	 uint32_t users = DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->getRegistedUsers();
	return users;
}

double
UmtsBsMgnt::GetCoverageRange (void) const
{
	return DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
}

} // namespace ns3
