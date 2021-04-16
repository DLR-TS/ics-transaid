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

#include "wimax-vehicle-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/wimax-net-device.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/wimax-vehicle-scan-mngr.h"
#include "ns3/wimax-version-type.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/c2c-facilities-helper.h"


NS_LOG_COMPONENT_DEFINE("WimaxVehicleInstaller");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WimaxVehicleInstaller);

TypeId WimaxVehicleInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::WimaxVehicleInstaller")
                        .SetParent<Object> ()
                        .AddConstructor<WimaxVehicleInstaller>()
                        ;
    return tid;
}

NetDeviceContainer
WimaxVehicleInstaller::DoInstall(NodeContainer container) {
    NS_LOG_INFO("*** WimaxVehicleInstaller ***");

    NetDeviceContainer devices = m_wimax.Install(container, WimaxHelper::DEVICE_TYPE_SUBSCRIBER_STATION, WimaxHelper::SIMPLE_PHY_TYPE_OFDM, m_channel, WimaxHelper::SCHED_TYPE_SIMPLE, ns3::WIMAX_VERSION_ITETRIS);

    m_vehicleContainer.Add(container);
    m_vehicleDeviceContainer.Add(devices);

    SsCommandManagerContainer mgnrCont = m_commandMgnr.InstallSsCommandManagers(devices);
    m_ssMgnrCont.Add(mgnrCont);
    if (m_bsMgnrCont.GetN() > 0) {
        m_commandMgnr.LinkBsAndSsCommandManagers(m_bsMgnrCont, mgnrCont);
    }

    uint32_t index = 0;

    for (NodeContainer::Iterator i = container.Begin(); i != container.End(); i++) {
        if (!(*i)->IsMobileNode()) {
            NS_LOG_INFO("Node Wimax defined as MobileNode");
            (*i)->SetMobileNode(true);
        }

        Ptr<NetDevice> device = devices.Get(index);

        // Check if the NetDevice has the object WimaxScanMngr already installed
        Ptr<WimaxVehicleScanMngr> vehicleScanMngr = device->GetObject <WimaxVehicleScanMngr> ();
        if (vehicleScanMngr == NULL) {
            vehicleScanMngr = CreateObject <WimaxVehicleScanMngr> ();
            vehicleScanMngr->SetNetDevice(device);
            vehicleScanMngr->SetNode(*i);
            vehicleScanMngr->SetSsCommandManager();
            (*i)->AggregateObject(vehicleScanMngr);
            NS_LOG_INFO("The object WimaxScanMngr has been attached to the NetDevice");
        }

        // Check if the vehicle has the object VehicleStaMgnt already installed
        Ptr<VehicleStaMgnt> vehicleStaMgnt = (*i)->GetObject <VehicleStaMgnt> ();
        if (vehicleStaMgnt == NULL) {
            vehicleStaMgnt = CreateObject <VehicleStaMgnt> ();
            vehicleStaMgnt->SetNode(*i);
            (*i)->AggregateObject(vehicleStaMgnt);
            NS_LOG_INFO("The object VehicleStaMgnt has been installed in the vehicle");
        }

        vehicleStaMgnt->AddIpTechnology("Wimax", device, vehicleScanMngr);

        // Check if the vehicle has the Facilties already installed
        Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
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

        index ++;
    }

    return devices;

}

void
WimaxVehicleInstaller::DoProcessApplicationInstall(std::string appName) {
    appName = "Wimax-" + appName;
    m_servListHelper->Add(m_wimaxAppHelper, appName);
    NS_LOG_DEBUG("Wimax application with itetrisName=" << appName << " has been added to the ServiceListHelper");
    m_wimaxAppHelper = NULL;
}

} // namespace ns3
