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
#include "umts-vehicle-scan-mngr.h"
#include "ns3/umts-net-device.h"
#include "ns3/umts-basestation-manager.h"
#include "ns3/umts-userequipment-manager.h"
#include "ns3/mobility-model.h"


NS_LOG_COMPONENT_DEFINE ("UmtsVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsVehicleScanMngr);

TypeId UmtsVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsVehicleScanMngr")
    .SetParent<Object> ()            
    .AddConstructor<UmtsVehicleScanMngr> ()       
    ;
  return tid;
}

UmtsVehicleScanMngr::~UmtsVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
UmtsVehicleScanMngr::AddBaseStation(Ptr<UMTSNetDevice> netDevice)
{
  DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->
      AddNodeBStation(DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(netDevice)->GetManager()));

}

void
UmtsVehicleScanMngr::JoinMulticastGroup(std::string serviceName)
{
  DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->JoinMulticastGroup(serviceName);
}

void
UmtsVehicleScanMngr::LeaveMulticastGroup(std::string serviceName)
{
  DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->LeaveMulticastGroup(serviceName);
}


Ptr<IpBaseStation> 
UmtsVehicleScanMngr::GetBestServingBs (STACK stack)
{
  Ptr<IpBaseStation> station=CreateObject<IpBaseStation>();
  
  Ptr<UmtsBaseStationManager> manager_=DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->GetBestUmtsBaseStationManager(stack);
  if(manager_!=0)
  {
    if(stack == IPv4)
    {
        station->SetIpAddress(manager_->GetIpAddress());
    }
    //Add IPv6 case.
    else
    {
         //std::cout<<"UmtsVehicleScanMngr::GetBestServingBs NEW IPV6 ADDRESS:"<<manager_->GetIpv6Address()<<std::endl;
         station->SetIpv6Address(manager_->GetIpv6Address());
    }
    station->SetNodeId(manager_->GetNodeIdentifier());
    Ptr<MobilityModel> model=manager_->GetMobility();
    station->SetLat(model->GetPosition().x);
    station->SetLon(model->GetPosition().x);
  }
  
  return (station);

}


Ptr<IpBaseStation> 
UmtsVehicleScanMngr::GetBestServingBs (void)
{  
  Ptr<IpBaseStation> station=CreateObject<IpBaseStation>();
  
  Ptr<UmtsBaseStationManager> manager_=DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->GetBestUmtsBaseStationManager();
  if(manager_!=0)
  {
    station->SetIpAddress(manager_->GetIpAddress()); 
    station->SetNodeId(manager_->GetNodeIdentifier());
    Ptr<MobilityModel> model=manager_->GetMobility();
    station->SetLat(model->GetPosition().x);
    station->SetLon(model->GetPosition().x);
  }
  
  return (station);

}

} // namespace ns3
