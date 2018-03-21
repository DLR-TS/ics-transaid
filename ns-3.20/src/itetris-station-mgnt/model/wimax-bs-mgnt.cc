/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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
#include "wimax-bs-mgnt.h"
#include "ns3/wimax-net-device.h"
#include "ns3/addressing-support.h"
#include "ns3/bs-command-manager.h"

NS_LOG_COMPONENT_DEFINE ("WimaxBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WimaxBsMgnt);

TypeId WimaxBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WimaxBsMgnt")
    .SetParent<IpBaseStaMgnt> ()        
    .AddConstructor<WimaxBsMgnt> ()               
    ;
  return tid;
}

WimaxBsMgnt::WimaxBsMgnt ()
  : m_bsCommandManager (0)
{}

WimaxBsMgnt::~WimaxBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
WimaxBsMgnt::SetBsCommandManager (void)
{
  m_bsCommandManager = m_netDevice->GetObject<BsCommandManager> (); 
  NS_LOG_DEBUG ("[WimaxBsMgnt::SetBsCommandManager] m_bsCommandManager=" << m_bsCommandManager );
}

Ipv4Address* 
WimaxBsMgnt::GetIpAddress (uint32_t nodeId) const
{
  Ipv4Address* address = NULL;
  TriggerVehiclesScanning (); 
  if (nodeId == ID_BROADCAST)
    { 
      address = GetIpBroadcastAddress ();
    }
  else
    {     
      address = m_bsCommandManager->GetVehicleIpAddress (nodeId);
    }
  return (address);
}

//NEW FUNCTION
Ipv6Address* 
WimaxBsMgnt::GetIpv6Address (uint32_t nodeId) const
{
  Ipv6Address* address = NULL;
  TriggerVehiclesScanning (); 
  if (nodeId == ID_BROADCAST)
    { 
      address = GetIpv6MulticastAddress ();
    }
  else
    {     
      address = m_bsCommandManager->GetVehicleIpv6Address (nodeId);
    }
  return (address);
}


void 
WimaxBsMgnt::TriggerVehiclesScanning (void) const
{  
  m_bsCommandManager->TriggerVehiclesScanning ();
}

uint32_t 
WimaxBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
WimaxBsMgnt::GetNumberOfRegisteredUsers (void) const
{
  TriggerVehiclesScanning();
  return (m_bsCommandManager->GetNumberOfRegisteredSs ());
}

double
WimaxBsMgnt::GetCoverageRange (void) const
{
  return (m_bsCommandManager->GetCoverageRange ());
}

} // namespace ns3
