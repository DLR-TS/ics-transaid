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
#include "wifi-ip-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("WifiIpInstaller");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WifiIpInstaller);

TypeId WifiIpInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::WifiIpInstaller")
                        .SetParent<Object> ()
                        .AddConstructor<WifiIpInstaller> ()
                        ;
    return tid;
}

WifiIpInstaller::WifiIpInstaller() {

    wifiPhy =  YansWifiPhyHelper::Default();
    wifiPhy.Set("RxGain", DoubleValue(-10));

    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel(wifiChannel.Create());

    wifiMac = NqosWifiMacHelper::Default();
    std::string phyMode("wifib-1mbs");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue(phyMode),
                                 "ControlMode", StringValue(phyMode));
    wifiMac.SetType("ns3::AdhocWifiMac");

    list.Add(staticRouting, 0);
    list.Add(olsr, 10);
    internet.SetRoutingHelper(list);

}

void
WifiIpInstaller::Install(NodeContainer container) {

    wifi.Install(wifiPhy, wifiMac, container);
    internet.Install(container);

}

}
