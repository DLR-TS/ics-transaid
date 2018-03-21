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
#include "wifi-vehicle-scan-mngr.h"

NS_LOG_COMPONENT_DEFINE ("WifiVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WifiVehicleScanMngr);

TypeId WifiVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiVehicleScanMngr")
    .SetParent<Object> ()   
    .AddConstructor<WifiVehicleScanMngr> ()                  
    ;
  return tid;
}

WifiVehicleScanMngr::~WifiVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<IpBaseStation> 
WifiVehicleScanMngr::GetBestServingBs (void)
{
  NS_LOG_INFO ("Obtaining best serving Wifi base station");
  //   TODO Call the corresponding NetDevice (m_netDevice) to obtain the best serving base station
  //   In the current implementation the IpBaseStation is hardcoded
  // TODO (JHNOTE): remove the hard coded value with a configurable value
  Ptr<IpBaseStation> station = CreateObject<IpBaseStation> (1,Ipv4Address("192.168.1.134"),20,43);
      
  return (station);

}

Ptr<IpBaseStation> 
WifiVehicleScanMngr::GetBestServingBs (STACK stack)
{
  NS_LOG_INFO ("Obtaining best serving Wifi base station");
  //   TODO Call the corresponding NetDevice (m_netDevice) to obtain the best serving base station
  //   In the current implementation the IpBaseStation is hardcoded
  // TODO (JHNOTE): add a configurable IpBaseStation configuration
  if (stack == IPv4)
  {
        Ptr<IpBaseStation> station = CreateObject<IpBaseStation> (1,Ipv4Address("192.168.1.134"),20,43);
        return (station);
  }
  else
  {
      //TODO (JHNOTE) check why IPv6 is not enabled here
      //Ptr<IpBaseStation> station = CreateObject<IpBaseStation> (1,Ipv6Address("2607:f0d0:1002:0051:0000:0000:0000:0004"),20,43);
        return NULL;
  }

}

} // namespace ns3
