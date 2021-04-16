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
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/


#include "wave-vehicle-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"

// Includes for facilties
#include "ns3/c2c-facilities-helper.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/wifi-vehicle-scan-mngr.h"

// Specific objects for WaveVehicleStas
#include "ns3/vehicle-sta-mgnt.h"

// Specific objects for channel load monitoring for ITS-G5
#include "ns3/channel-load-monitor-mngr.h"
#include "ns3/ETSI-channel-load-monitor-mngr.h"
#include "ns3/node.h"

NS_LOG_COMPONENT_DEFINE("WaveVehicleInstaller");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WaveVehicleInstaller);

TypeId WaveVehicleInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::WaveVehicleInstaller").SetParent<WaveInstaller>().AddConstructor <
                        WaveVehicleInstaller > ();
    return tid;
}

void WaveVehicleInstaller::DoInstall(NodeContainer container, NetDeviceContainer cchDevices,
                                     NetDeviceContainer schDevices) {
    uint32_t index = 0;

    m_ipv6AddressHelper.AssignWithoutAddress(cchDevices);  // by default for CCH, but can be changed to SCH if necessary
    for (NodeContainer::Iterator i = container.Begin(); i != container.End(); i++) {
        if (!(*i)->IsMobileNode()) {
            NS_LOG_INFO("Node WAVE defined as MobileNode");
            (*i)->SetMobileNode(true);
        }

        //Adding part for IPv6 support on CCH by default
        Ptr<NetDevice> device = cchDevices.Get(index);
        Ptr<VehicleScanMngr> vehicleScanMngr = device->GetObject<WifiVehicleScanMngr>();
        if (vehicleScanMngr == NULL) {
            vehicleScanMngr = CreateObject<WifiVehicleScanMngr>();
            vehicleScanMngr->SetNetDevice(device);
            NS_LOG_INFO("The object WifiScanMngr has been attached to the NetDevice");
        }

        // Check if the vehicle has the object VehicleStaMgnt already installed
        Ptr<VehicleStaMgnt> vehicleStaMgnt = (*i)->GetObject<VehicleStaMgnt>();
        if (vehicleStaMgnt == NULL) {
            vehicleStaMgnt = CreateObject<VehicleStaMgnt>();
            vehicleStaMgnt->SetNode(*i);
            (*i)->AggregateObject(vehicleStaMgnt);
            NS_LOG_INFO("The object VehicleStaMgnt has been installed in the vehicle");
        }
        vehicleStaMgnt->AddC2cTechnology("WaveCch", cchDevices.Get(index));
        vehicleStaMgnt->AddC2cTechnology("WaveSch", schDevices.Get(index));

        Ptr<ChannelLoadMonitorMngr> cchChannelLoadMonitor = cchDevices.Get(index)->GetNode()->GetObject <
                ChannelLoadMonitorMngr > ();

        cchChannelLoadMonitor = CreateObject<ETSIChannelLoadMonitorMngr>();
        cchDevices.Get(index)->GetNode()->AddChannelLoadMonitor(cchDevices.Get(index)->GetIfIndex(),
                cchChannelLoadMonitor);

        Ptr<ChannelLoadMonitorMngr> schChannelLoadMonitor = schDevices.Get(index)->GetNode()->GetObject <
                ChannelLoadMonitorMngr > ();
        schChannelLoadMonitor = CreateObject<ETSIChannelLoadMonitorMngr>();
        schDevices.Get(index)->GetNode()->AddChannelLoadMonitor(schDevices.Get(index)->GetIfIndex(),
                schChannelLoadMonitor);

        vehicleStaMgnt->GetObject<Node>()->StartChannelLoadMonitoring();

        vehicleStaMgnt->AddIpTechnology("WaveIP", device, vehicleScanMngr);

        // Check if the vehicle has the Facilities already installed
        Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject<iTETRISns3Facilities>();
        if (facilities == NULL) {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.AddDefaultServices(m_servListHelper);
            facilitiesHelper.Install(*i);
            NS_LOG_INFO("The object iTETRISns3Facilities has been installed in the vehicle");
        } else {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.SetServiceListHelper(m_servListHelper);
            facilitiesHelper.AddServices(facilities, *i);
            NS_LOG_INFO("New services have been installed in the vehicle");
        }

        index++;
    }
}

} // namespace ns3
