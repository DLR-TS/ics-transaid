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
#include "wimax-vehicle-scan-mngr.h"
#include "ns3/wimax-net-device.h"
#include "ns3/mobility-model.h"
#include "ns3/ss-command-manager.h"
#include "ns3/bs-command-manager.h"

NS_LOG_COMPONENT_DEFINE ("WimaxVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WimaxVehicleScanMngr);

TypeId WimaxVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WimaxVehicleScanMngr")
    .SetParent<Object> ()            
    .AddConstructor<WimaxVehicleScanMngr> ()       
    ;
  return tid;
}

WimaxVehicleScanMngr::~WimaxVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
WimaxVehicleScanMngr::SetSsCommandManager (void)
{
  m_ssCommandManager = m_netDevice->GetObject<SsCommandManager> (); 
  NS_LOG_DEBUG ("[WimaxVehicleScanMngr::SetSsCommandManager] m_ssCommandManager=" << m_ssCommandManager );
}

Ptr<IpBaseStation> 
WimaxVehicleScanMngr::GetBestServingBs (void)
{  
  Ptr<IpBaseStation> station = NULL;
  if (m_ssCommandManager->PeriodicScanning ())
    {
      station = CreateObject<IpBaseStation>();
      Ptr<BsCommandManager> bsMgnr = m_ssCommandManager->GetRegisteredBsManager ();
      station->SetIpAddress(*(bsMgnr->GetBsIpAddress ())); 
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

//!!! NEW FUNCTION !!!
Ptr<IpBaseStation> 
WimaxVehicleScanMngr::GetBestServingBs (STACK stack)
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
      /*Need to modify this for Ipv6*/
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

void 
WimaxVehicleScanMngr::ActivateNode (void)
{
  m_ssCommandManager->ActivateNode ();
}
    
void 
WimaxVehicleScanMngr::DeactivateNode (void)
{
  m_ssCommandManager->DeactivateNode ();
}

} // namespace ns3
