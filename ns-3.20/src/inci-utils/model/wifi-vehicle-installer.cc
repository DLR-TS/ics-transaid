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

#include "wifi-vehicle-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"

// Specific objects for WifiVehicleStas
#include "ns3/wifi-vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/c2c-facilities-helper.h"
#include "ns3/iTETRISns3Facilities.h"

NS_LOG_COMPONENT_DEFINE ("WifiVehicleInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WifiVehicleInstaller);

TypeId WifiVehicleInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiVehicleInstaller")
    .SetParent<Object> ()     
    .AddConstructor<WifiVehicleInstaller> ()             
    ;
  return tid;
}
    
void
WifiVehicleInstaller::Install (NodeContainer container) 
{
  NetDeviceContainer netDevices = wifi.Install (wifiPhy, wifiMac, container);
  m_ipAddressHelper.Assign (netDevices);
  AddInterfacesToIpInterfaceList (container);
  ConfigureDerivedWifiStation (container, netDevices);
}

void
WifiVehicleInstaller::ConfigureDerivedWifiStation (NodeContainer container, NetDeviceContainer devices)
{
  NS_LOG_INFO ("WifiVehicleInstaller");
  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      Ptr<NetDevice> device = devices.Get(index);

      // Check if the NetDevice has the object WifiScanMngr already installed
      Ptr<VehicleScanMngr> vehicleScanMngr = device->GetObject <WifiVehicleScanMngr> ();
      if (vehicleScanMngr == NULL)
	{
          vehicleScanMngr = CreateObject <WifiVehicleScanMngr> ();
          vehicleScanMngr->SetNetDevice (device);
          NS_LOG_INFO ("The object WifiScanMngr has been attached to the NetDevice");
        }

      // Check if the vehicle has the object VehicleStaMgnt already installed
      Ptr<VehicleStaMgnt> vehicleStaMgnt = (*i)->GetObject <VehicleStaMgnt> ();
      if (vehicleStaMgnt == NULL)
	{
          vehicleStaMgnt = CreateObject <VehicleStaMgnt> ();
          vehicleStaMgnt->SetNode (*i);
          (*i)->AggregateObject (vehicleStaMgnt);
          NS_LOG_INFO ("The object VehicleStaMgnt has been installed in the vehicle");
        }
      vehicleStaMgnt->AddIpTechnology ("Wifi",device, vehicleScanMngr);

       // Check if the vehicle has the Facilities already installed
      Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
      if (facilities == NULL)
	{
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.AddDefaultServices (m_servListHelper);
          facilitiesHelper.Install (*i);
          NS_LOG_INFO ("The object iTETRISns3Facilities has been installed in the vehicle");
        }
      else
        {
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.SetServiceListHelper (m_servListHelper);
          facilitiesHelper.AddServices (facilities, *i);
          NS_LOG_INFO ("New services have been installed in the vehicle");
        }


      index ++;
    }
}

} // namespace ns3
