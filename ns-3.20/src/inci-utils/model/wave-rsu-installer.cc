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
#include "wave-rsu-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"

// Includes for facilties
#include "ns3/c2c-facilities-helper.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/ipv6-address.h"

#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"

// Specific objects for RsuVehicleStas
#include "ns3/rsu-sta-mgnt.h"

// Specific objects for channel load monitoring for ITS-G5
#include "ns3/channel-load-monitor-mngr.h"
#include "ns3/ETSI-channel-load-monitor-mngr.h"
#include "ns3/node.h"

NS_LOG_COMPONENT_DEFINE("WaveRsuInstaller");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WaveRsuInstaller);

TypeId WaveRsuInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::WaveRsuInstaller")
                        .SetParent<WaveInstaller> ()
                        .AddConstructor<WaveRsuInstaller> ()
                        ;
    return tid;
}

Ipv6Address
WaveRsuInstaller::AllocateIpv6Prefix() {
    static uint64_t id = 0;
    id = id + 1;
    Ipv6Address prefix = new Ipv6Address("3001::");
    //uint8_t* prefix2 = prefix.GetAddress();


    //*(prefix2 + 2) = (id >> 40) & 0xff;
    prefix.GetAddress()[2] = (id >> 40) & 0xff;
    prefix.GetAddress()[3] = (id >> 32) & 0xff;
    prefix.GetAddress()[4] = (id >> 24) & 0xff;
    prefix.GetAddress()[5] = (id >> 16) & 0xff;
    prefix.GetAddress()[6] = (id >> 8) & 0xff;
    prefix.GetAddress()[7] = (id >> 0) & 0xff;

    std::cout << "WaveRsuInstaller::AllocateIpv6Prefix()" << prefix << std::endl;
    return prefix;
}

/*Ipv6Prefix
WaveRsuInstaller::AllocateIpv6Prefix()
{
	static uint64_t id = 0;
	id++;
	Ipv6Prefix prefix = new Ipv6Prefix("3001::");
	//prefix.m_prefix[0] = (id >> 56) & 0xff;
	//prefix.m_prefix[1] = (id >> 48) & 0xff;
	prefix.m_prefix[2] = (id >> 40) & 0xff;
	prefix.m_prefix[3] = (id >> 32) & 0xff;
	prefix.m_prefix[4] = (id >> 24) & 0xff;
	prefix.m_prefix[5] = (id >> 16) & 0xff;
	prefix.m_prefix[6] = (id >> 8) & 0xff;
	prefix.m_prefix[7] = (id >> 0) & 0xff;
	std::cout<<"WaveRsuInstaller::AllocateIpv6Prefix()"<<prefix<<std::endl;
	return prefix;
}*/

void
WaveRsuInstaller::DoInstall(NodeContainer container, NetDeviceContainer cchDevices, NetDeviceContainer schDevices) {

//std::cout<<"WaveRsuInstaller::DoInstall"<<std::endl;
    uint32_t index = 0;

    Ipv6Address pref = AllocateIpv6Prefix();
    Ipv6Prefix pref2 = new Ipv6Prefix(pref.GetAddress());
//std::cout<<"WaveRsuInstaller::DoInstall: "<<pref2<<std::endl;
    m_ipv6AddressHelper.NewNetwork();
    m_ipv6AddressHelper.SetBase(pref, pref2);
    //*** Change schDevices for IPv6
    Ipv6InterfaceContainer iicr = m_ipv6AddressHelper.Assign(cchDevices);
    //iicr.SetRouter (0, true); deprecato
    iicr.SetDefaultRouteInAllNodes(0);
    iicr.SetForwarding(0, true);
    //Probably here you have to add the configuration for the radvd interface
    Ipv6Address prefix = m_ipv6AddressHelper.GetPrefix(); // create the prefix
    uint32_t indexRouter = iicr.GetInterfaceIndex(0);  // R interface (n0 - R)
    Ptr<Radvd> radvd = CreateObject<Radvd> ();
    //Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter);
    //Remember: 15000, 1000
    Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter, 15000, 1000);
    Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 100, 120);
    routerInterface->AddPrefix(routerPrefix);
    radvd->AddConfiguration(routerInterface);
    uint32_t app_index = container.Get(0)->AddApplication(radvd);
    //NEW PART
    //container.Get(0)->GetObject <Ipv6L3Protocol> ()->AddRawSocket(radvd->GetSocket());
    radvd->SetAppIndex(app_index);
    //Start time: 1.0
    //Stop time: 30.0
    radvd->SetStartTime(Seconds(1.0));
    radvd->SetStopTime(Seconds(40.0));

    for (NodeContainer::Iterator i = container.Begin(); i != container.End(); i++) {
        // Check if the RSU has the object RsuStaMgnt already installed
        Ptr<RsuStaMgnt> rsuStaMgnt = (*i)->GetObject <RsuStaMgnt> ();
        (*i)->SetMobileNode(false);
        if (rsuStaMgnt == NULL) {
            rsuStaMgnt = CreateObject <RsuStaMgnt> ();
            rsuStaMgnt->SetNode(*i);
            (*i)->AggregateObject(rsuStaMgnt);
            NS_LOG_INFO("The object RsuStaMgnt has been installed in the RSU");
        }
        rsuStaMgnt->AddC2cTechnology("WaveCch", cchDevices.Get(index));
        rsuStaMgnt->AddC2cTechnology("WaveSch", schDevices.Get(index));
        Ptr<ChannelLoadMonitorMngr> cchChannelLoadMonitor = cchDevices.Get(index)->GetNode()->GetObject <ChannelLoadMonitorMngr>();
        cchChannelLoadMonitor = CreateObject <ETSIChannelLoadMonitorMngr>();
        cchDevices.Get(index)->GetNode()->AddChannelLoadMonitor(cchDevices.Get(index)->GetIfIndex(), cchChannelLoadMonitor);

        Ptr<ChannelLoadMonitorMngr> schChannelLoadMonitor = schDevices.Get(index)->GetNode()->GetObject <ChannelLoadMonitorMngr>();
        schChannelLoadMonitor = CreateObject <ETSIChannelLoadMonitorMngr>();
        schDevices.Get(index)->GetNode()->AddChannelLoadMonitor(schDevices.Get(index)->GetIfIndex(), schChannelLoadMonitor);


        rsuStaMgnt->GetObject<Node>()-> StartChannelLoadMonitoring();

        // Check if the vehicle has the Facilities already installed
        Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
        if (facilities == NULL) {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.AddDefaultServices(m_servListHelper);
            facilitiesHelper.Install(*i);
            //std::cout<<"WaveRSUInstaller::DoInstall After facilities helper install"<<std::endl;
            NS_LOG_INFO("The object iTETRISns3Facilities has been installed in the RSU");
        } else {
            C2CFacilitiesHelper facilitiesHelper;
            facilitiesHelper.SetServiceListHelper(m_servListHelper);
            facilitiesHelper.AddServices(facilities, *i);
            NS_LOG_INFO("New services have been installed in the RSU");
        }
        index ++;
    }

    // Activate CCH devices
    for (NetDeviceContainer::Iterator i = cchDevices.Begin(); i != cchDevices.End(); i++) {
        DynamicCast<WifiNetDevice>(*i)->GetPhy()->SetNodeStatus(true);
    }

    // Activate SCH devices
    for (NetDeviceContainer::Iterator i = schDevices.Begin(); i != schDevices.End(); i++) {
        DynamicCast<WifiNetDevice>(*i)->GetPhy()->SetNodeStatus(true);
    }
}

} // namespace ns3
