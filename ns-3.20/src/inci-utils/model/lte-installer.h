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
 * Author: Sendoa Vaz
 */
#ifndef LTE_INSTALLER_H
#define LTE_INSTALLER_H

#include "ns3/Lte-App-helper.h"
#include "ns3/lte-helper.h"
#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "comm-module-installer.h"
#include "ns3/service-list-helper.h"
#include "ns3/epc-helper.h"
//jin add
#include "ns3/point-to-point-epc-helper.h"

namespace ns3
{

class LteInstaller : public CommModuleInstaller
{
  public:
    static TypeId GetTypeId (void);
    LteInstaller(void);
    void Install (NodeContainer container); 
    void Configure (std::string filename);
    void AssignIpAddress(NetDeviceContainer devices);
    void ProcessApplicationInstall (xmlTextReaderPtr reader);
    ~LteInstaller();

    static Ptr<LteHelper> lteHelper; 
   //Jin modified this
    static Ptr<PointToPointEpcHelper> epcHelper;
    //static Ptr<EpcHelper> epcHelper; 
  
    static NodeContainer enbNodeContainer;
    static NetDeviceContainer enbDeviceContainer;

    std::string m_nodeType; 

    LTEAppHelper* m_lteAppHelper;
    static Ipv4AddressHelper m_ipAddressHelper;
    ServiceListHelper* m_servListHelper;
    Ipv4StaticRoutingHelper  ipv4RoutingHelper;

    virtual void DoInstall (NodeContainer container, NetDeviceContainer createdDevices) = 0;
    void AddVehicles(NodeContainer container,NetDeviceContainer netDevices);     
    void AddInterfacesToIpInterfaceList (NodeContainer container);

//jin : add this part as inherited from EpcHelper
/*
     virtual void AddEnb (Ptr<Node> enbNode, Ptr<NetDevice> lteEnbNetDevice, uint16_t cellId);
      virtual void AddUe (Ptr<NetDevice> ueLteDevice, uint64_t imsi);
      virtual void AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2);
      virtual uint8_t ActivateEpsBearer (Ptr<NetDevice> ueLteDevice, uint64_t imsi, Ptr<EpcTft> tft, EpsBearer bearer);
      virtual Ptr<Node> GetPgwNode ();
      virtual Ipv4InterfaceContainer AssignUeIpv4Address (NetDeviceContainer ueDevices);
      virtual Ipv4Address GetUeDefaultGatewayAddress (); 
*/
};

}

#endif

