/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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

#ifndef WIMAX_INSTALLER_H
#define WIMAX_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/wimax-helper.h"
#include "ns3/wimax-command-manager-helper.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/ipv4-address-helper.h"
#include "comm-module-installer.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/wimax-app-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/bs-command-manager-container.h"
#include "ns3/ss-command-manager-container.h"

namespace ns3
{

class WimaxInstaller : public CommModuleInstaller
{
  public:
    static TypeId GetTypeId (void);
    WimaxInstaller(void);    
    void Install (NodeContainer container); 
    void Configure (std::string filename);
    void AssignIpAddress(NetDeviceContainer devices);
    ~WimaxInstaller();
    virtual NetDeviceContainer DoInstall (NodeContainer container) = 0;

  protected:

    struct WimaxParameters {
      uint32_t frequency; // in KHz
      uint32_t bandwidth;
      double txPower;
      double txGain;
      double rxGain;
      double noiseFigure;
      double coverageRange;
      double bsAntennaHeight;
      double ssAntennaHeight;
    };

    WimaxParameters m_wimaxParameters;

    void ProcessApplicationInstall (xmlTextReaderPtr reader);
    virtual void DoProcessApplicationInstall (std::string appName) {};
    void SetWimaxParameters (NetDeviceContainer devices);
    virtual void DoSetWimaxParameters (NetDeviceContainer devices) {}; 
    uint32_t ConvertToInt (std::string cadena);
    double ConvertToDouble (std::string cadena);
      
    void AddInterfacesToIpInterfaceList (NodeContainer container);
    void AddServiceFlow (NetDeviceContainer netDevices);

    static WimaxHelper m_wimax;
    static WimaxCommandManagerHelper m_commandMgnr;
    static BsCommandManagerContainer m_bsMgnrCont;
    static SsCommandManagerContainer m_ssMgnrCont;
    
    static Ipv4AddressHelper m_ipAddressHelper;
    static NodeContainer m_vehicleContainer;
    static NodeContainer m_baseStationContainer;
    static NetDeviceContainer m_baseStationDeviceContainer;
    static NetDeviceContainer m_vehicleDeviceContainer;
    
    std::string m_nodeType;
    ServiceListHelper* m_servListHelper;
    static Ptr<SimpleOfdmWimaxChannel> m_channel;
    WimaxAppHelper* m_wimaxAppHelper;
 
};

}

#endif





