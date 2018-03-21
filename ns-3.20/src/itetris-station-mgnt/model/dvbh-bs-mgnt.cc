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
#include "dvbh-bs-mgnt.h"
#include "ns3/dvbh-net-device.h"
#include "ns3/dvbh-manager-UserEquip.h"
#include "ns3/dvbh-manager-BaseStation.h"

NS_LOG_COMPONENT_DEFINE ("DvbhBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DvbhBsMgnt);

TypeId DvbhBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhBsMgnt")
    .SetParent<IpBaseStaMgnt> ()        
    .AddConstructor<DvbhBsMgnt> ()               
    ;
  return tid;
}

DvbhBsMgnt::~DvbhBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ipv4Address* 
DvbhBsMgnt::GetIpAddress (uint32_t nodeId) const
{
  Ipv4Address* address = NULL;
  TriggerVehiclesScanning ();
  if (nodeId == ID_BROADCAST)
    {
      address = GetIpBroadcastAddress ();
    }
  else
    {
            
      address =DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->GetVehicleIpAddress(nodeId);
    }
  return (address);
}


Ipv6Address* 
DvbhBsMgnt::GetIpv6Address (uint32_t nodeId) const
{
  Ipv6Address* address = NULL;
  TriggerVehiclesScanning ();
  if (nodeId == ID_BROADCAST)
    {
      address = GetIpv6MulticastAddress ();
    }
  else
    {
            
      address =DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->GetVehicleIpv6Address(nodeId);
    }
  return (address);
}



void
DvbhBsMgnt::AddVehicle(Ptr<DvbhNetDevice> netDevice)
{
  DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->
      AddVehicle(DynamicCast<DVBHManagerUserEquip>(DynamicCast<DvbhNetDevice>(netDevice)->GetManager()));
}

void
DvbhBsMgnt::AddBaseStation(Ptr<DvbhNetDevice> netDevice)
{
    DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->
    AddBaseStationManager(DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(netDevice)->GetManager()));
}

void 
DvbhBsMgnt::TriggerVehiclesScanning (void) const
{
  
  DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->TriggerVehicleScanning();
}

uint32_t 
DvbhBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
DvbhBsMgnt::GetNumberOfRegisteredUsers (void) const
{
	TriggerVehiclesScanning();
	uint32_t users = DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->getRegistedUsers();
	return users;
}

double
DvbhBsMgnt::GetCoverageRange (void) const
{
	return DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
}

} // namespace ns3
