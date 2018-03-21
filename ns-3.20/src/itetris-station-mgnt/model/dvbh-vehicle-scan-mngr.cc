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
#include "dvbh-vehicle-scan-mngr.h"
#include "ns3/dvbh-net-device.h"
#include "ns3/dvbh-manager-UserEquip.h"
#include "ns3/dvbh-manager-BaseStation.h"
#include "ns3/mobility-model.h"

NS_LOG_COMPONENT_DEFINE ("DvbhVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DvbhVehicleScanMngr);

TypeId DvbhVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhVehicleScanMngr")
    .SetParent<Object> ()            
    .AddConstructor<DvbhVehicleScanMngr> ()       
    ;
  return tid;
}

DvbhVehicleScanMngr::~DvbhVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
DvbhVehicleScanMngr::AddBaseStation(Ptr<DvbhNetDevice> netDevice)
{
  DynamicCast<DVBHManagerUserEquip>(DynamicCast<DvbhNetDevice>(m_netDevice)->GetManager())->
      AddBaseStation(DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(netDevice)->GetManager()));
}

Ptr<IpBaseStation> 
DvbhVehicleScanMngr::GetBestServingBs (void)
{
  NS_LOG_INFO ("Obtaining best serving DVBH base station");
  Ptr<IpBaseStation> station=CreateObject<IpBaseStation>();
  
  
  return (station);

}

Ptr<IpBaseStation> 
DvbhVehicleScanMngr::GetBestServingBs (STACK stack)
{
  NS_LOG_INFO ("Obtaining best serving DVBH base station");
  if(stack == IPv4)
  {
        Ptr<IpBaseStation> station=CreateObject<IpBaseStation>();
        return (station);
  }
  else
  {
        Ptr<IpBaseStation> station=CreateObject<IpBaseStation>();
        return (station);
  }

}

} // namespace ns3
