/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)
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
 * Author:
 */

#include "lte-vehicle-installer.h"
#include "ns3/IPCIU-facilities-helper.h"
#include "ns3/IPCIUFacilities.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/c2c-facilities-helper.h"
#include "ns3/iTETRISns3Facilities.h"

#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/lte-net-device.h"
#include "ns3/lte-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"



NS_LOG_COMPONENT_DEFINE("LteVehicleInstaller");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(LteVehicleInstaller);

TypeId LteVehicleInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::LteVehicleInstaller")
                        .SetParent<Object> ()
                        .AddConstructor<LteVehicleInstaller>()
                        ;
    return tid;
}

void
LteVehicleInstaller::DoInstall(NodeContainer container, NetDeviceContainer createdDevices) {
    NS_LOG_INFO("*** LteVehicleInstaller ***");

    uint32_t index = 0;

    for (NodeContainer::Iterator it = container.Begin(); it != container.End(); it++) {
        if (!(*it)->IsMobileNode()) {
            NS_LOG_INFO("Node LTE defined as MobileNode");
            (*it)->SetMobileNode(true);
        }

        Ptr<NetDevice> device = (createdDevices).Get(index);

        // Check if the NetDevice has the object LteScanMngr already installed
        Ptr<VehicleScanMngr> vehicleScanMg = device->GetObject <LteVehicleScanMngr> ();
        if (vehicleScanMg == NULL) {
            vehicleScanMg = CreateObject <LteVehicleScanMngr> ();
            vehicleScanMg->SetNetDevice(device);
            vehicleScanMg->SetNode(*it);
            (*it)->AggregateObject(vehicleScanMg);
            NS_LOG_INFO("The object LteScanMngr has been attached to the NetDevice");
        }

        // Check if the vehicle has the object VehicleStaMgnt already installed
        Ptr<VehicleStaMgnt> vehicleStaMg = (*it)->GetObject <VehicleStaMgnt> ();
        if (vehicleStaMg == NULL) {
            vehicleStaMg = CreateObject <VehicleStaMgnt> ();
            vehicleStaMg->SetNode(*it);
            (*it)->AggregateObject(vehicleStaMg);
            NS_LOG_INFO("The object VehicleStaMgnt has been installed in the vehicle");
        }
        vehicleStaMg->AddIpTechnology("Lte", device, vehicleScanMg);

        // DynamicCast<LteNetDevice>(createdDevices.Get(index))->SetIpAddress(); //TTN - should add SetIPAddress to LteNetDevice


        /*   // Check if the vehicle has the Facilties already installed
           Ptr<IPCIUFacilities> facilities = (*it)->GetObject <IPCIUFacilities> ();
           if (facilities == NULL)
           {
         	  IPCIUFacilitiesHelper facilitiesHelper;
         	  facilitiesHelper.SetServiceListHelper (m_servListHelper);
         	  facilitiesHelper.Install (*it);
         	  NS_LOG_INFO ("The object IPCIUFacilities has been installed in the vehicle");
           }

          */
        // Check if the vehicle has the Facilties already installed
        Ptr<iTETRISns3Facilities> facilities = (*it)->GetObject <iTETRISns3Facilities> ();
        if (facilities == NULL) {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.AddDefaultServices(m_servListHelper);
            facilitiesHelper.Install(*it);
            NS_LOG_INFO("The object iTETRISns3Facilities has been installed in the vehicle");
        } else {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.SetServiceListHelper(m_servListHelper);
            facilitiesHelper.AddServices(facilities, *it);
            NS_LOG_INFO("New services have been installed in the vehicle");
        }


        index++ ;
    }

}

} // namespace ns3

