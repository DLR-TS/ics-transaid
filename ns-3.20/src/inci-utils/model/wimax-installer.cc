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

#include "wimax-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/simple-ofdm-wimax-phy.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/service-list-helper.h"
#include "ip-interface-list.h"

NS_LOG_COMPONENT_DEFINE("WimaxInstaller");

namespace ns3 {

Ipv4AddressHelper WimaxInstaller::m_ipAddressHelper;
NodeContainer WimaxInstaller::m_vehicleContainer;
NodeContainer WimaxInstaller::m_baseStationContainer;
NetDeviceContainer WimaxInstaller::m_baseStationDeviceContainer;
NetDeviceContainer WimaxInstaller::m_vehicleDeviceContainer;
Ptr<SimpleOfdmWimaxChannel> WimaxInstaller::m_channel = NULL;
WimaxHelper WimaxInstaller::m_wimax;
WimaxCommandManagerHelper WimaxInstaller::m_commandMgnr;
BsCommandManagerContainer WimaxInstaller::m_bsMgnrCont;
SsCommandManagerContainer WimaxInstaller::m_ssMgnrCont;

NS_OBJECT_ENSURE_REGISTERED(WimaxInstaller);

TypeId WimaxInstaller::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::WimaxInstaller")
                        .SetParent<Object> ()

                        ;
    return tid;
}

WimaxInstaller::WimaxInstaller() {
    m_servListHelper = new ServiceListHelper();
    m_wimaxAppHelper = NULL;
    m_ipAddressHelper.SetBase("10.4.0.0", "255.255.0.0");

    if (!m_channel) {
        m_channel = CreateObject<SimpleOfdmWimaxChannel> ();
        m_channel->SetPropagationModel(SimpleOfdmWimaxChannel::COST231_PROPAGATION);
    }

    m_wimaxParameters.frequency = 5000000;
    m_wimaxParameters.bandwidth = 20000000;
    m_wimaxParameters.txPower = 30;
    m_wimaxParameters.txGain = 0;
    m_wimaxParameters.rxGain = 0;
    m_wimaxParameters.noiseFigure = 5;
    m_wimaxParameters.coverageRange = 500;
    m_wimaxParameters.bsAntennaHeight = 50;
    m_wimaxParameters.ssAntennaHeight = 1.5;
}

WimaxInstaller::~WimaxInstaller() {
    delete m_servListHelper;
    delete m_wimaxAppHelper;
    m_servListHelper = NULL;
    m_wimaxAppHelper = NULL;
}


void
WimaxInstaller::Install(NodeContainer container) {
    NetDeviceContainer netDevices = DoInstall(container);
    m_ipAddressHelper.Assign(netDevices);
    AddInterfacesToIpInterfaceList(container);
    SetWimaxParameters(netDevices);
    AddServiceFlow(netDevices);
}

void
WimaxInstaller::AddServiceFlow(NetDeviceContainer netDevices) {
    for (NetDeviceContainer::Iterator i = netDevices.Begin(); i != netDevices.End(); i++) {
        Ptr<Node> node = (*i)->GetNode();
        if (node->IsMobileNode()) {
            Ptr<SubscriberStationNetDevice> ssDevice = DynamicCast<SubscriberStationNetDevice> (*i);
            Ptr<IpInterfaceList> interfaceList = node->GetObject <IpInterfaceList> ();
            Ipv4Address ssIpAddress = interfaceList->GetIpAddress("Wimax");

            IpcsClassifierRecord UlClassifierUgs(ssIpAddress,
                                                 Ipv4Mask("255.255.255.255"),
                                                 Ipv4Address("0.0.0.0"),
                                                 Ipv4Mask("0.0.0.0"),
                                                 0,
                                                 65000,
                                                 0,
                                                 100,
// 					      6, // TCP
                                                 17, // UDP
                                                 1);
            ServiceFlow UlServiceFlowUgs = m_wimax.CreateServiceFlow(ServiceFlow::SF_DIRECTION_UP,
                                           ServiceFlow::SF_TYPE_NRTPS,
                                           UlClassifierUgs);

            ssDevice->AddServiceFlow(UlServiceFlowUgs);
        }
    }
}

void
WimaxInstaller::Configure(std::string filename) {
    NS_LOG_DEBUG("Reading config file for WIMAX from file " << filename);
    xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str());
    if (reader == NULL) {
        NS_FATAL_ERROR("Error at xmlReaderForFile");
    }



    int rc;
    rc = xmlTextReaderRead(reader);
    while (rc > 0) {
        const xmlChar* tag = xmlTextReaderConstName(reader);
        if (tag == 0) {
            NS_FATAL_ERROR("Invalid value");
        }

        if (std::string((char*)tag) == "phy") {
            xmlChar* attribute = xmlTextReaderGetAttribute(reader, BAD_CAST "attribute");
            xmlChar* value = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
            if (!strcmp((char*)attribute, "Frequency")) {
                m_wimaxParameters.frequency = ConvertToInt((char*)value);
                NS_LOG_DEBUG("Frequency attribute read. Value=" << m_wimaxParameters.frequency);
            } else if (!strcmp((char*)attribute, "Bandwidth")) {
                m_wimaxParameters.bandwidth = ConvertToInt((char*)value);
                NS_LOG_DEBUG("Bandwidth attribute read. Value=" << m_wimaxParameters.bandwidth);
            } else if (!strcmp((char*)attribute, "TxPower")) {
                m_wimaxParameters.txPower = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("TxPower attribute read. Value=" << m_wimaxParameters.txPower);
            } else if (!strcmp((char*)attribute, "TxGain")) {
                m_wimaxParameters.txGain = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("TxGain attribute read. Value=" << m_wimaxParameters.txGain);
            } else if (!strcmp((char*)attribute, "RxGain")) {
                m_wimaxParameters.rxGain = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("RxGain attribute read. Value=" << m_wimaxParameters.rxGain);
            } else if (!strcmp((char*)attribute, "NoiseFigure")) {
                m_wimaxParameters.noiseFigure = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("NoiseFigure attribute read. Value=" << m_wimaxParameters.noiseFigure);
            } else if (!strcmp((char*)attribute, "CoverageRange")) {
                m_wimaxParameters.coverageRange = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("CoverageRange attribute read. Value=" << m_wimaxParameters.coverageRange);
            } else if (!strcmp((char*)attribute, "BSAntennaHeight")) {
                m_wimaxParameters.bsAntennaHeight = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("BSAntennaHeight attribute read. Value=" << m_wimaxParameters.bsAntennaHeight);
            } else if (!strcmp((char*)attribute, "SSAntennaHeight")) {
                m_wimaxParameters.ssAntennaHeight = ConvertToDouble((char*)value);
                NS_LOG_DEBUG("SSAntennaHeight attribute read. Value=" << m_wimaxParameters.ssAntennaHeight);
            } else {
                NS_LOG_DEBUG("Attribute name=" << (char*)attribute << " not found.");
            }

            xmlFree(attribute);
            xmlFree(value);
        }

        // ipConfiguration
        if (std::string((char*)tag) == "ip") {
            xmlChar* address = xmlTextReaderGetAttribute(reader, BAD_CAST "address");
            if (address != 0) {
                xmlChar* mask = xmlTextReaderGetAttribute(reader, BAD_CAST "mask");
                if (mask != 0) {
                    m_ipAddressHelper.SetBase((char*)address, (char*)mask);
                    NS_LOG_DEBUG("ip address=" << (char*)address << " mask=" << (char*)mask);
                }
                xmlFree(mask);
            }
            xmlFree(address);
        }

        // Applications
        if (std::string((char*)tag) == "application") {
            ProcessApplicationInstall(reader);
        }

        rc = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
}

uint32_t
WimaxInstaller::ConvertToInt(std::string cadena) {
    std::stringstream temp;
    uint32_t valueInt;
    temp << cadena;
    temp >> valueInt;
    return valueInt;
}

double
WimaxInstaller::ConvertToDouble(std::string cadena) {
    std::stringstream temp;
    double valueDouble;
    temp << cadena;
    temp >> valueDouble;
    return valueDouble;
}

void
WimaxInstaller::AddInterfacesToIpInterfaceList(NodeContainer container) {
    for (NodeContainer::Iterator i = container.Begin(); i != container.End(); ++i) {
        // Check if the node has the object IpInterfaceList installed
        Ptr<IpInterfaceList> interfaceList = (*i)->GetObject <IpInterfaceList> ();
        if (interfaceList == NULL) {
            interfaceList = CreateObject <IpInterfaceList> ();
            (*i)->AggregateObject(interfaceList);
            NS_LOG_INFO("The object IpInterfaceList has been attached to the node");
        }
        Ptr<Ipv4> ipStack = (*i)->GetObject <Ipv4> ();
        uint32_t index = ipStack->GetNInterfaces();
        bool res = interfaceList->AddIpInterface("Wimax", ipStack->GetAddress(index - 1, 0));
        NS_ASSERT_MSG(res, "WimaxInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
        NS_LOG_INFO("IP address " << ipStack->GetAddress(index - 1, 0));
    }
}

void
WimaxInstaller::SetWimaxParameters(NetDeviceContainer devices) {
    for (NetDeviceContainer::Iterator i = devices.Begin(); i != devices.End(); i++) {
        Ptr<WimaxNetDevice> device = DynamicCast<WimaxNetDevice> (*i);
        Ptr<SimpleOfdmWimaxPhy> phy = DynamicCast<SimpleOfdmWimaxPhy> (device->GetPhy());
        phy->SetFrequency(m_wimaxParameters.frequency);
        phy->SetChannelBandwidth(m_wimaxParameters.bandwidth);
        phy->SetTxPower(m_wimaxParameters.txPower);
        phy->SetNoiseFigure(m_wimaxParameters.noiseFigure);
        phy->SetTxGain(m_wimaxParameters.txGain);
        phy->SetRxGain(m_wimaxParameters.rxGain);
    }
    DoSetWimaxParameters(devices);
}

void
WimaxInstaller::ProcessApplicationInstall(xmlTextReaderPtr reader) {
    int rc;
    std::string appType, appName;

    rc = xmlTextReaderRead(reader);
    while (rc > 0) {
        const xmlChar* tag = xmlTextReaderConstName(reader);
        if (tag == 0) {
            NS_FATAL_ERROR("Invalid value");
        }

        //NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

        if (std::string((char*)tag) == "WimaxApp") {
            appType = "WimaxApp";
            xmlChar* name = xmlTextReaderGetAttribute(reader, BAD_CAST "itetrisName");

            if (name != 0) {
                appName = std::string((char*)name);
                m_wimaxAppHelper = new WimaxAppHelper();
                NS_LOG_DEBUG("Wimax Application itetrisName = " << std::string((char*)name));
            }
            xmlChar* attribute = xmlTextReaderGetAttribute(reader, BAD_CAST "attribute");
            if (attribute != 0) {
                xmlChar* value = xmlTextReaderGetAttribute(reader, BAD_CAST "value");
                if (value != 0) {
                    if (m_wimaxAppHelper) {
                        m_wimaxAppHelper->SetAttribute((char*)attribute, StringValue((char*)value));
                        NS_LOG_DEBUG("Wimax App attribute=" << attribute << " value=" << value);
                    }
                }

                xmlFree(value);
            }
            xmlFree(name);
            xmlFree(attribute);
        }

        else if (std::string((char*)tag) == "application") {
            DoProcessApplicationInstall(appName);
        }
        rc = xmlTextReaderRead(reader);
    }
}

} // namespace ns3
