/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ *
+ * Author:
+ */


#include "lte-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/lte-helper.h"
// Added for reading xml files 
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/vehicle-scan-mngr.h"
#include "ip-interface-list.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/lte-net-device.h"
#include "ns3/lte-bs-mgnt.h"

#include "ns3/epc-enb-application.h"
#include "ns3/epc-helper.h" 
#include "ns3/itetris-technologies.h"

NS_LOG_COMPONENT_DEFINE ("LteInstaller");
using namespace std;
namespace ns3
{

Ipv4AddressHelper LteInstaller::m_ipAddressHelper;

Ptr<LteHelper> LteInstaller::lteHelper;
//jin modified
//Ptr<EpcHelper> LteInstaller::epcHelper; 
Ptr<PointToPointEpcHelper>LteInstaller:: epcHelper ;


NodeContainer LteInstaller::enbNodeContainer;
NetDeviceContainer LteInstaller::enbDeviceContainer;

NS_OBJECT_ENSURE_REGISTERED (LteInstaller);

TypeId LteInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteInstaller")
    .SetParent<Object> ()    
     
    ;
  return tid;
}


    
LteInstaller::LteInstaller () 
{
   m_lteAppHelper=NULL;
   m_servListHelper = new ServiceListHelper ();
   m_ipAddressHelper.SetBase ("10.3.0.0", "255.255.0.0");
}

LteInstaller::~LteInstaller()
{ 
  delete m_lteAppHelper;
  delete m_servListHelper;  
  m_lteAppHelper=NULL;  
  m_servListHelper = NULL; 
}


//jin original part

void
LteInstaller::Install (NodeContainer container) 
{

  if(lteHelper==NULL)
    {
      lteHelper = CreateObject<LteHelper> ();
//jin : replace EpcHelper with PointTOpointEpcHelper
      //epcHelper = CreateObject<EpcHelper> (); 
      epcHelper = CreateObject<PointToPointEpcHelper> ();
      lteHelper->SetEpcHelper(epcHelper); 
    } 

  NetDeviceContainer createdDevices;
  InternetStackHelper internet;  

  if(m_nodeType=="enbNode")
  {
  //jin modified part
  //  createdDevices = lteHelper->InstallEnbDevice(container,m_nodeType);
  createdDevices = lteHelper->InstallEnbDevice(container);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  //jin modified following
  //  Ptr<Node> remoteHost = container.Get(0);
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
//jin end

  internet.Install (remoteHost); 

  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
  
  Ipv4InterfaceContainer internetIpIfaces = m_ipAddressHelper.Assign(internetDevices);
  Ipv4Address remoteHostAddress = internetIpIfaces.GetAddress(1); 
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  DoInstall(container, createdDevices);

  enbNodeContainer.Add(container);
  enbDeviceContainer.Add(createdDevices);            
  } 

  createdDevices = lteHelper->InstallUeDevice(container,m_nodeType);

  internet.Install (container); 

  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address(createdDevices);
  for (uint32_t u = 0; u < container.GetN (); ++u)
  {
          Ptr<Node> ueNode = container.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  }

  DoInstall(container, createdDevices);

  AddVehicles(enbNodeContainer, createdDevices);
  lteHelper->Attach(createdDevices,enbDeviceContainer.Get(0));
  AddInterfacesToIpInterfaceList(container);

}




void
LteInstaller::Configure (std::string filename) 
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for LTE");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
        {
          NS_FATAL_ERROR ("Invalid value");
        }

      // LTEPhy type setting
      if (std::string ((char*)tag) == "LtePhy")
        {
          xmlChar *nodeType = xmlTextReaderGetAttribute (reader, BAD_CAST "name");

          m_nodeType=(char *)nodeType; 
          xmlFree (nodeType);
        }

        // Applications
      if (std::string ((char*)tag) == "application")
        {
          ProcessApplicationInstall (reader);
        }

      rc = xmlTextReaderRead (reader);
    }
  xmlFreeTextReader (reader);
} 


void 
LteInstaller::AddInterfacesToIpInterfaceList (NodeContainer container)
{
int index=0;
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
      bool res = interfaceList->AddIpInterface("Lte", ipStack->GetAddress (index-1,0));
      NS_ASSERT_MSG (res, "LteInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      NS_LOG_INFO ("IP address " << ipStack->GetAddress (index-1,0));
      
      index++;
    }
}


void
LteInstaller::AddVehicles(NodeContainer container,NetDeviceContainer netDevices)  
{
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
  {
      for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
      {	
	(*i)->GetObject <LteBsMgnt> ()->AddVehicle(DynamicCast<LteNetDevice>(*iterator));
      }
      
  }
}


//TODO Support for applications
void
LteInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader) 
{
  int rc;
  std::string appType, appName;

  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

       if (std::string ((char*)tag) == "LteApp")
	{
          appType = "LteApp";
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");

	  if (name != 0)
	    {
	      appName = std::string ((char*)name);
	      m_lteAppHelper = new LTEAppHelper();
	      NS_LOG_DEBUG ("LTE Application itetrisName = "<<std::string ((char*)name));
	    }
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	      if (value != 0)
		{
		  if (m_lteAppHelper)
		    {
		      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)value));
		      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << value);
		    }

		    if(std::string((char*)value)=="BROADCAST"||std::string((char*)value)=="MULTICAST")
		    {
		      xmlChar *ip = xmlTextReaderGetAttribute (reader, BAD_CAST "ip");
		      if (ip != 0)
			{
			  if (m_lteAppHelper)
			    {
			      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)ip));
			      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << ip);
			    }


			}
			xmlFree (ip);
		    }

		}

	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
	}
      else if (std::string ((char*)tag) == "application")
	{
	      m_servListHelper->Add (m_lteAppHelper, appName);
	      NS_LOG_DEBUG ("LteApp application with itetrisName="<<appName<<" has been added to the ServiceListHelper");
	      m_lteAppHelper = NULL;
	  return;
	}
      rc = xmlTextReaderRead (reader);
    }
}


//jin : build this part for inheritant from epc-helper,all functions are empty
// jin : inside , need to define later
/*
void
LteInstaller::AddEnb (Ptr<Node> enbNode, Ptr<NetDevice> lteEnbNetDevice, uint16_t cellId){
}
void
LteInstaller::AddUe (Ptr<NetDevice> ueLteDevice, uint64_t imsi){
}

uint8_t
LteInstaller::ActivateEpsBearer (Ptr<NetDevice> ueLteDevice, uint64_t imsi, Ptr<EpcTft> tft, EpsBearer bearer){
return 0;
}

void 
LteInstaller::AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2){
}
Ptr<Node>
LteInstaller::GetPgwNode (){
}
Ipv4InterfaceContainer
LteInstaller::AssignUeIpv4Address (NetDeviceContainer ueDevices){
}
Ipv4Address 
LteInstaller::GetUeDefaultGatewayAddress () {
}
*/
/*****jin : end****/

} // namespace ns3
