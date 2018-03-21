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
#ifndef WAVE_INSTALLER_H
#define WAVE_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/qos-wifi-mac-helper.h"
#include "ns3/switching-manager-helper.h"
#include "ns3/c2c-interface-helper.h"
#include "ns3/channel-tag.h"
#include "ns3/ipv6-address-helper.h"
#include "comm-module-installer.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>

namespace ns3
{

class YansWifiChannel;
class CAMmanageHelper;
class C2CIPHelper;
class ServiceListHelper;

class WaveInstaller : public CommModuleInstaller
{

  public:

    static TypeId GetTypeId (void);
    WaveInstaller();
    ~WaveInstaller ();
    void Install (NodeContainer container); 
    void Configure (std::string filename);
    void RelateInstaller (Ptr<CommModuleInstaller> installer);
    Ptr<YansWifiChannel> GetWaveCch (void);
    Ptr<YansWifiChannel> GetWaveSch (void);
    void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);

  protected:

    typedef struct {
      std::string name;
      AttributeValue* value;
    } AttributesChannel;

    virtual void DoInstall (NodeContainer container, NetDeviceContainer cchDevices, NetDeviceContainer schDevices) = 0;
    void SetAntennaHeightInNodes (NodeContainer container);

    // Functions to read XML configuration file
    void ProcessApplicationInstall (xmlTextReaderPtr reader);

    // Functions and members to configure the CCH and SCH channels
    void SetChannelType (NetDeviceContainer devices, ChannelType channel);
    void ConfigureWaveChannel (void);
    std::vector<AttributesChannel>::iterator GetFirstEmptyElement (void);
    std::vector <AttributesChannel> m_attributesChannel;
    std::string m_channelName;
    Ptr<YansWifiChannel> m_waveCch;
    Ptr<YansWifiChannel> m_waveSch;

    // Helpers to configure WAVE
    Wifi80211pHelper wave;
    YansWifiPhyHelper wavePhyCch;
    YansWifiPhyHelper wavePhySch;
    YansWifiChannelHelper waveChannel;
    QosWaveMacHelper waveMac;
    SwitchingManagerHelper switchingHelper;
    c2cInterfaceHelper inf;
    ObjectFactory m_visibilityObject;
    ObjectFactory m_shadowingObject;
    ObjectFactory m_fadingObject;
    bool FADING; 
    float m_interferenceRangeV;         
    float m_interferenceRangeC;   
 

    // Application helpers
    CAMmanageHelper* m_camHelper;
    C2CIPHelper* m_c2cIpHelper;
    ServiceListHelper* m_servListHelper;
    Ipv6AddressHelper m_ipv6AddressHelper;

    // Mobility Model
    float m_antennaHeight;
};

}

#endif


