/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "dvbh-vehicle-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/dvbh-net-device.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/dvbh-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/c2c-facilities-helper.h"

 
NS_LOG_COMPONENT_DEFINE ("DvbhVehicleInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (DvbhVehicleInstaller);

TypeId DvbhVehicleInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhVehicleInstaller")
    .SetParent<Object> ()     
    .AddConstructor<DvbhVehicleInstaller>()                
    ;
  return tid;
}
  
void 
DvbhVehicleInstaller::DoInstall (NodeContainer container,NetDeviceContainer devices)
{
  NS_LOG_INFO ("*** DvbhVehicleInstaller ***");
  
   uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      Ptr<NetDevice> device = devices.Get(index);

      // Check if the NetDevice has the object UmtsScanMngr already installed
      Ptr<VehicleScanMngr> vehicleScanMngr = device->GetObject <DvbhVehicleScanMngr> ();
      if (vehicleScanMngr == NULL)
	{
          vehicleScanMngr = CreateObject <DvbhVehicleScanMngr> ();
          vehicleScanMngr->SetNetDevice (device);
	  vehicleScanMngr->SetNode (*i);	//Added by Sendoa
	  (*i)->AggregateObject (vehicleScanMngr);
          NS_LOG_INFO ("The object DvbhVehicleScanMngr has been attached to the NetDevice");
        }

      DynamicCast<DvbhNetDevice>(devices.Get(index))->SetIpAddress();
      
       // Check if the vehicle has the Facilties already installed
      Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
      if (facilities == NULL)
	{
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.SetServiceListHelper (m_servListHelper);
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
