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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#ifndef UMTS_INSTALLER_H
#define UMTS_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/umts-helper.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "comm-module-installer.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/Umts-App-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/itetris-types.h"


namespace ns3 {

class UmtsInstaller : public CommModuleInstaller {
public:
    static TypeId GetTypeId(void);
    UmtsInstaller(void);

    void Install(NodeContainer container);
    //!!! Modify func !!!
    void Install(NodeContainer container, STACK stack);

    void Configure(std::string filename);
    void AssignIpAddress(NetDeviceContainer devices);
    Ipv6Address AllocateIpv6Prefix();
    void ProcessApplicationInstall(xmlTextReaderPtr reader);
    ~UmtsInstaller();

    virtual void DoInstall(NodeContainer container, NetDeviceContainer devices, STACK stack) = 0;
    void AddVehicles(NodeContainer container, NetDeviceContainer devices);
    void AddBaseStations(NodeContainer container, NetDeviceContainer netDevices);

protected:
    void AddInterfacesToIpInterfaceList(NodeContainer container);
    void AddInterfacesToIpv6InterfaceList(NodeContainer container);
    UMTSHelper umts;
    UMTSPhyHelper umtsPhyUE;
    UMTSPhyHelper umtsPhyBS;

    static Ipv4AddressHelper m_ipAddressHelper;
    Ipv6AddressHelper m_ipv6AddressHelper;

    static NodeContainer vehicleContainer;
    static NodeContainer baseStationContainer;

    static NetDeviceContainer baseStationDeviceContainer;
    static NetDeviceContainer vehicleDeviceContainer;

    std::string m_nodeType;

    C2CIPHelper* m_c2cIpHelper;
    UMTSAppHelper* m_umtsAppHelper;
    ServiceListHelper* m_servListHelper;

};

}

#endif





