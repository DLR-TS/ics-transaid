/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
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
 * Author: Sendoa Vaz
 */
#include "ns3/log.h"
#include "lte-vehicle-scan-mngr.h"
#include "ns3/lte-net-device.h"
#include "ns3/mobility-model.h"


NS_LOG_COMPONENT_DEFINE ("LteVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LteVehicleScanMngr);

TypeId LteVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteVehicleScanMngr")
    .SetParent<Object> ()            
    .AddConstructor<LteVehicleScanMngr> ()
    ;
  return tid;
}

LteVehicleScanMngr::~LteVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}



Ptr<IpBaseStation> 
LteVehicleScanMngr::GetBestServingBs (void)
{  
  Ptr<IpBaseStation> station = CreateObject<IpBaseStation> (); 
  return (station);
}

/*****Jin : add this function; according to wifi-vehicle-scan-mngr****/

Ptr<IpBaseStation>
LteVehicleScanMngr:: GetBestServingBs (STACK stack)
{
 return (0);
}
//Jin : The following part is borrowed from wifi-vehicle-scan-mngr*; as using IpBaseStation as well
/*
{
  Ptr<IpBaseStation> station = NULL;
  if (m_ssCommandManager->PeriodicScanning ())
    {
      station = CreateObject<IpBaseStation>();
      Ptr<BsCommandManager> bsMgnr = m_ssCommandManager->GetRegisteredBsManager ();
      if (stack==IPv4)
      {
        station->SetIpAddress(*(bsMgnr->GetBsIpAddress ()));
      }
      //Need to modify this for Ipv6
      else
      {
         station->SetIpAddress(*(bsMgnr->GetBsIpAddress ()));
      }
      station->SetNodeId(bsMgnr->GetNodeId ());
      station->SetLat(bsMgnr->GetLatitude ());
      station->SetLon(bsMgnr->GetLongitude ());      
    }
  else
    {
      NS_LOG_DEBUG ("[WimaxVehicleScanMngr::GetBestServingBs] No BS in coverage");
    }
  
  return (station); 

}
*/

} // namespace ns3


