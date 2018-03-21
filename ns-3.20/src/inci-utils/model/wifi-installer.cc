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

#include "wifi-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"

// Added for reading xml files 
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>

#include "ip-interface-list.h"

NS_LOG_COMPONENT_DEFINE ("WifiInstaller");

namespace ns3
{

Ipv4AddressHelper WifiInstaller::m_ipAddressHelper("10.1.0.0", "255.255.0.0");

NS_OBJECT_ENSURE_REGISTERED (WifiInstaller);

TypeId WifiInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiInstaller")
    .SetParent<Object> ()     
    .AddConstructor<WifiInstaller> ()             
    ;
  return tid;
}
    
WifiInstaller::WifiInstaller () {

  wifiPhy =  YansWifiPhyHelper::Default ();

  wifiPhy.Set ("RxGain", DoubleValue (-10) ); 

  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  wifiMac = NqosWifiMacHelper::Default ();
  std::string phyMode ("wifib-1mbs");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                   "ControlMode",StringValue(phyMode));

  wifiMac.SetType ("ns3::AdhocWifiMac");

} 

void
WifiInstaller::Install (NodeContainer container) 
{
  NetDeviceContainer netDevices = wifi.Install (wifiPhy, wifiMac, container);
  m_ipAddressHelper.Assign (netDevices);
  AddInterfacesToIpInterfaceList (container);
}

void
WifiInstaller::Install (NodeContainer container, STACK stack) 
{
  NetDeviceContainer netDevices = wifi.Install (wifiPhy, wifiMac, container);
  m_ipAddressHelper.Assign (netDevices);
  if (stack == IPv4)
        AddInterfacesToIpInterfaceList (container);
  else
        AddInterfacesToIpInterfaceList (container);
}

void
WifiInstaller::Configure (std::string filename)
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for WiFi");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      // ipConfiguration
      if (std::string ((char*)tag) == "ip")
	{
          xmlChar *address = xmlTextReaderGetAttribute (reader, BAD_CAST "address");
	  if (address != 0)
	    {
              xmlChar *mask = xmlTextReaderGetAttribute (reader, BAD_CAST "mask");
              if (mask != 0)
                {
		  m_ipAddressHelper.SetBase ((char*)address, (char*)mask);
		  NS_LOG_DEBUG ("ip address="<<(char*)address<<" mask=" << (char*)mask); 
                }
	      xmlFree (mask);
	    }
	  xmlFree (address);
        }
      rc = xmlTextReaderRead (reader);
    }
  xmlFreeTextReader (reader);
} 

void 
WifiInstaller::AddInterfacesToIpInterfaceList (NodeContainer container)
{
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
    {
    // Check if the node has the object IpInterfaceList installed
    Ptr<IpInterfaceList> interfaceList = (*i)->GetObject <IpInterfaceList> ();
    if (interfaceList == NULL)
      {
        interfaceList = CreateObject <IpInterfaceList> ();
        (*i)->AggregateObject (interfaceList);
        NS_LOG_INFO ("The object IpInterfaceList has been attached to the node");
      }
      Ptr<Ipv4> ipStack = (*i)->GetObject <Ipv4> (); 
      uint32_t index = ipStack->GetNInterfaces ();
      bool res = interfaceList->AddIpInterface("Wifi", ipStack->GetAddress (index-1,0));
      NS_ASSERT_MSG (res, "WifiInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      NS_LOG_INFO ("IP address " << ipStack->GetAddress (index-1,0));
    }
}

} // namespace ns3
