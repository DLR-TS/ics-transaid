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

#ifndef WIFI_INSTALLER_H
#define WIFI_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "comm-module-installer.h"
#include "ns3/itetris-types.h"

namespace ns3
{

class ServiceListHelper;

class WifiInstaller : public CommModuleInstaller
{
  public:
    static TypeId GetTypeId (void);
    WifiInstaller(void);
    void Install (NodeContainer container); 
    void Install (NodeContainer container, STACK stack);
    void Configure (std::string filename);

  protected:
    void AddInterfacesToIpInterfaceList (NodeContainer container);
    WifiHelper wifi;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    NqosWifiMacHelper wifiMac;
    static Ipv4AddressHelper m_ipAddressHelper;
    ServiceListHelper* m_servListHelper;
};

}

#endif





