/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010,CBT EU FP7 iTETRIS project
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

#include "umts-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/umts-helper.h"
// Added for reading xml files 
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/umts-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/umts-bs-mgnt.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/Umts-App-helper.h"
#include "ip-interface-list.h"
#include "ns3/service-list-helper.h"

#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-raw-socket-factory.h"

NS_LOG_COMPONENT_DEFINE ("UmtsInstaller");

namespace ns3
{

Ipv4AddressHelper UmtsInstaller::m_ipAddressHelper;
//Ipv6AddressHelper UmtsInstaller::m_ipv6AddressHelper;
NodeContainer UmtsInstaller::vehicleContainer;
NodeContainer UmtsInstaller::baseStationContainer;

NetDeviceContainer UmtsInstaller::baseStationDeviceContainer;
NetDeviceContainer UmtsInstaller::vehicleDeviceContainer;

NS_OBJECT_ENSURE_REGISTERED (UmtsInstaller);

TypeId UmtsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsInstaller")
    .SetParent<Object> ()     
                 
    ;
  return tid;
}
    
UmtsInstaller::UmtsInstaller () {

   
   umtsPhyUE =  UMTSPhyHelper::Default ();
   umtsPhyBS =  UMTSPhyHelper::Default ();
   m_servListHelper = new ServiceListHelper ();
   m_c2cIpHelper = NULL;
   m_umtsAppHelper=NULL;
   //m_ipAddressHelper.SetBase ("10.3.0.0", "255.255.0.0");
   //m_ipv6AddressHelper.NewNetwork ("2001::", 64);
}

UmtsInstaller::~UmtsInstaller()
{ 
  delete m_c2cIpHelper; 
  delete m_servListHelper;  
  delete m_umtsAppHelper;  
  m_c2cIpHelper = NULL; 
  m_servListHelper = NULL; 
  m_umtsAppHelper=NULL;
}

Ipv6Address
UmtsInstaller::AllocateIpv6Prefix()
{
	static uint64_t id = 0;
	id++;
	Ipv6Address prefix = new Ipv6Address("2001::");
	//uint8_t* prefix2 = prefix.GetAddress();


	//*(prefix2 + 2) = (id >> 40) & 0xff;
	prefix.GetAddress()[2] = (id >> 40) & 0xff;
	prefix.GetAddress()[3] = (id >> 32) & 0xff;
	prefix.GetAddress()[4] = (id >> 24) & 0xff;
	prefix.GetAddress()[5] = (id >> 16) & 0xff;
	prefix.GetAddress()[6] = (id >> 8) & 0xff;
	prefix.GetAddress()[7] = (id >> 0) & 0xff;

	std::cout<<"WaveRsuInstaller::AllocateIpv6Prefix()"<<prefix<<std::endl;
	return prefix;
}

//!!! Modify function for IPv6 support.
void
UmtsInstaller::Install (NodeContainer container, STACK stack) 
{
  if(m_nodeType=="NodeUE")
  { 
    //At each node of the Node Container install a Umts net device and return the list of devices!
    NetDeviceContainer netDevices = umts.Install (umtsPhyUE,m_nodeType, container);   
    //For each NetDevice assign an Ipv4 address.
    //if(m_ipAddressHelper!= NULL){
    if(stack == IPv4)
    {
        m_ipAddressHelper.Assign (netDevices);
        AddInterfacesToIpInterfaceList (container);
    }
    //}
    //if(m_ipv6AddressHelper!=NULL)
    else
    {
       m_ipv6AddressHelper.AssignWithoutAddress(netDevices);
       //m_ipv6AddressHelper.Assign(netDevices);
       /*Probably here you have to add the configuration for the radvd interface */
       /*uint32_t indexRouter = iic1.GetInterfaceIndex (1); // R interface (n0 - R) 
       uint32_t indexRouter2 = iic2.GetInterfaceIndex (1); // R interface (R - n1) 
       Ptr<Radvd> radvd = CreateObject<Radvd> ();
       Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter, 5000, 1000);*/
       
       //AddInterfacesToIpv6InterfaceList (container);
    }

    DoInstall(container,netDevices,stack);      
    vehicleContainer.Add(container);
    if(baseStationDeviceContainer.GetN()>0)
    {
	AddVehicles(baseStationContainer,netDevices);
    }
    
    AddBaseStations(container,baseStationDeviceContainer);
    
    vehicleDeviceContainer.Add(netDevices);
  }
  else
  {
     NetDeviceContainer netDevices = umts.Install (umtsPhyBS,m_nodeType, container);   
     if(stack == IPv4)
    {
        m_ipAddressHelper.Assign (netDevices);
        AddInterfacesToIpInterfaceList (container);
    }
    //}
    //if(m_ipv6AddressHelper!=NULL)
    else
    {

    	Ipv6Address pref = AllocateIpv6Prefix();
    	Ipv6Prefix pref2 = new Ipv6Prefix(pref.GetAddress());
    	std::cout<<"UmtsInstaller::Install: "<<pref2<<std::endl;
    	m_ipv6AddressHelper.NewNetwork();
    	m_ipv6AddressHelper.SetBase(pref, pref2);

       /*Ipv6Address pref = AllocateIpv6Prefix();
       m_ipv6AddressHelper.NewNetwork (pref, (char*)"64");*/

       Ipv6InterfaceContainer iicr = m_ipv6AddressHelper.Assign(netDevices);
       iicr.SetForwarding(0, true);
       iicr.SetDefaultRouteInAllNodes(0);
       //Probably here you have to add the configuration for the radvd interface
       Ipv6Address prefix = m_ipv6AddressHelper.GetPrefix(); // create the prefix 
       uint32_t indexRouter = iicr.GetInterfaceIndex (0); // R interface (n0 - R)  
       Ptr<Radvd> radvd = CreateObject<Radvd> ();
       //Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter);
       //Remember: 15000, 1000
       Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter, 12000, 1000);
       Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 100, 120);
       routerInterface->AddPrefix (routerPrefix);
       radvd->AddConfiguration (routerInterface);
       uint32_t app_index = container.Get(0)->AddApplication(radvd);
       //NEW PART
       //container.Get(0)->GetObject <Ipv6L3Protocol> ()->AddRawSocket(radvd->GetSocket());
       radvd->SetAppIndex(app_index);
       //Start time: 1.0
       //Stop time: 30.0
       radvd->SetStartTime (Seconds (1.0));
       radvd->SetStopTime (Seconds (50.0));
       AddInterfacesToIpv6InterfaceList (container);
    }


     //m_ipAddressHelper.Assign (netDevices);
     //AddInterfacesToIpInterfaceList (container);
      
      
     DoInstall(container,netDevices,stack);
     baseStationContainer.Add(container);
     if(vehicleDeviceContainer.GetN()>0)
     {
        AddBaseStations(vehicleContainer,netDevices);
     }
     
     AddVehicles(container,vehicleDeviceContainer);
     baseStationDeviceContainer.Add(netDevices);
     
  }
}



void
UmtsInstaller::Install (NodeContainer container) 
{
  STACK stack = IPv4;
  if(m_nodeType=="NodeUE")
  { 
    //At each node of the Node Container install a Umts net device and return the list of devices!
    NetDeviceContainer netDevices = umts.Install (umtsPhyUE,m_nodeType, container);   
    //For each NetDevice assign an Ipv4 address.
    //if(m_ipAddressHelper!= NULL){
    m_ipAddressHelper.Assign (netDevices);
    AddInterfacesToIpInterfaceList (container);
    
    DoInstall(container,netDevices,stack);      
    vehicleContainer.Add(container);
    if(baseStationDeviceContainer.GetN()>0)
    {
	AddVehicles(baseStationContainer,netDevices);
    }
    
    AddBaseStations(container,baseStationDeviceContainer);
    
    vehicleDeviceContainer.Add(netDevices);
  }
  else
  {
     NetDeviceContainer netDevices = umts.Install (umtsPhyBS,m_nodeType, container);   
     m_ipAddressHelper.Assign (netDevices);
     AddInterfacesToIpInterfaceList (container);
      
      
     DoInstall(container,netDevices,stack);
     baseStationContainer.Add(container);
     if(vehicleDeviceContainer.GetN()>0)
     {
        AddBaseStations(vehicleContainer,netDevices);
     }
     
     AddVehicles(container,vehicleDeviceContainer);
     baseStationDeviceContainer.Add(netDevices);
     
  }
}

//Through this function we put all the umts-vehicles in a list which is held in UmtsBsMngnt. Then during the
//periodic scanning procedure we check if the vehicles are within each BS's coverage
void
UmtsInstaller::AddVehicles(NodeContainer container,NetDeviceContainer netDevices)
{
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
  {
      for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
      {	
	(*i)->GetObject <UmtsBsMgnt> ()->AddVehicle(DynamicCast<UMTSNetDevice>(*iterator));
      }
      
  }
}

void
UmtsInstaller::AddBaseStations(NodeContainer container,NetDeviceContainer netDevices)
{  
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
  {
      
      for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
      {
	  
	  (*i)->GetObject <UmtsVehicleScanMngr>()->AddBaseStation(DynamicCast<UMTSNetDevice>(*iterator));
      }
  }

}

void
UmtsInstaller::Configure (std::string filename)
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for UMTS");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      // UMTSPhy type setting
      if (std::string ((char*)tag) == "UmtsPhy")
	{
          xmlChar *nodeType = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  
	  m_nodeType=(char *)nodeType;

	  
	  if(m_nodeType=="NodeUE")
	  {
	    umtsPhyUE.SetNodeType(m_nodeType);
	  }
	  else
	  {
	    umtsPhyBS.SetNodeType(m_nodeType);
	  }
	  
	
	  xmlFree (nodeType);
        }
        
        if (std::string ((char*)tag) == "phy")
	{
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	 
	  if(m_nodeType=="NodeUE")
	  {
	    NS_LOG_DEBUG ("UMTSPhyUE attribute=" << attribute <<" value=" << value); 
	    umtsPhyUE.Set((char *)attribute,StringValue((char *)value));
	  }
	  else
	  {
	     NS_LOG_DEBUG ("UMTSPhyBS attribute=" << attribute <<" value=" << value); 
	    umtsPhyBS.Set((char *)attribute,StringValue((char *)value));
	  }
	  
	  	
	  xmlFree (attribute);
	  xmlFree (value);
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
         //std::cout<<"umts installer::Configure:"<<std::endl;
         //std::cout<<"UmtsInstaller::Configure ip address:"<<(char*)address<<std::endl;
         //std::cout<<"UmtsInstaller::Configure ip mask:"<<(char*)mask<<std::endl;
 		  m_ipAddressHelper.SetBase ((char*)address, (char*)mask);
		 
		  NS_LOG_DEBUG ("ip address="<<(char*)address<<" mask=" << (char*)mask); 
                }
	      xmlFree (mask);
	    }
	  xmlFree (address);
        }

//NEW PART FOR IPv6
        if (std::string ((char*)tag) == "ipv6")
	{

          xmlChar *network = xmlTextReaderGetAttribute (reader, BAD_CAST "network");
	  if (network != 0)
	    {
              xmlChar *prefix = xmlTextReaderGetAttribute (reader, BAD_CAST "prefix");
              if (prefix != 0)
              {
            	  m_ipv6AddressHelper.NewNetwork ();
            	  m_ipv6AddressHelper.SetBase(network, prefix);

            	  NS_LOG_DEBUG ("ipv6 network="<<(char*)network<<" prefix=" << (char*)prefix);
              }
	      xmlFree (prefix);
	    }
	  xmlFree (network);
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
UmtsInstaller::AddInterfacesToIpInterfaceList (NodeContainer container)
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
      bool res = interfaceList->AddIpInterface("Umts", ipStack->GetAddress (index-1,0));
      NS_ASSERT_MSG (res, "UmtsInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      NS_LOG_INFO ("IP address " << ipStack->GetAddress (index-1,0));
      
      
    }
}


//NEW FUNCTION
void 
UmtsInstaller::AddInterfacesToIpv6InterfaceList (NodeContainer container)
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
      Ptr<Ipv6> ipStack = (*i)->GetObject <Ipv6> ();
       //std::cout<<"CHECK 4!!!"<<std::endl;
       //std::cout<<"IP address " << ipStack->GetAddress(index-1,0)<<std::endl;
      uint32_t index = ipStack->GetNInterfaces ();
      //Crucial point here!!!
      //std::cout<<"INDEX -1!!!!!!!!!!"<<index-1<<std::endl;
      
      //*** Change here ***//
      //bool res = interfaceList->AddIpInterface("Umts", "::"); 
      bool res = interfaceList->AddIpInterface("Umts", ipStack->GetAddress (index-1,1));
      NS_ASSERT_MSG (res, "UmtsInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      //std::cout<<"!!!!!!!!!!!!!! UmtsInstaller::IPv6 Address !!!!!!!!!!!!!!!:"<<ipStack->GetAddress (index-1,1)<<std::endl;
      NS_LOG_INFO ("IPv6 address " << ipStack->GetAddress (index-1,0));
      
      
    }
}



void 
UmtsInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader)
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

      //NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

       if (std::string ((char*)tag) == "UmtsApp")
	{
          appType = "UmtsApp";
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
	  
	  if (name != 0)
	    {
	      appName = std::string ((char*)name);
	      m_umtsAppHelper = new UMTSAppHelper();
	      NS_LOG_DEBUG ("UMTS Application itetrisName = "<<std::string ((char*)name)); 
	    }
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	      if (value != 0)
		{
		  if (m_umtsAppHelper)
		    {
		      m_umtsAppHelper->SetAttribute((char*)attribute,StringValue((char*)value));
		      NS_LOG_DEBUG ("UMTSApp attribute=" << attribute <<" value=" << value); 
		    }
		    
		    if(std::string((char*)value)=="BROADCAST"||std::string((char*)value)=="MULTICAST")
		    {
		      xmlChar *ip = xmlTextReaderGetAttribute (reader, BAD_CAST "ip");
		      if (ip != 0)
			{
			  if (m_umtsAppHelper)
			    {
			      m_umtsAppHelper->SetAttribute((char*)attribute,StringValue((char*)ip));
			      NS_LOG_DEBUG ("UMTSApp attribute=" << attribute <<" value=" << ip); 
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
       else if (std::string ((char*)tag) == "C2CIP")
 	{
           appType = "C2CIP";
           
 	  
 	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
          
 	  if (name != 0)
 	    {
 	      appName = std::string ((char*)name);	      
 	      m_c2cIpHelper = new C2CIPHelper("ns3::c2cl4TSocketFactory", "ns3::UdpSocketFactory");
 	      NS_LOG_DEBUG ("Application itetrisName="<<std::string ((char*)name)); 
 	    }
 	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
 	  if (attribute != 0)
 	    {
 	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
 	      if (value != 0)
 		{
 		  if (m_c2cIpHelper)
 		    {
 		      m_c2cIpHelper->SetAttribute((char*)attribute,StringValue((char*)value));
 		      NS_LOG_DEBUG ("C2CIP attribute=" << attribute <<" value=" << value); 
 		    }
 		}
 	      xmlFree (value);
 	    }
 	  xmlFree (name);
 	  xmlFree (attribute);
 	}
      else if (std::string ((char*)tag) == "application")
	{
           if (appType=="UmtsApp")
             {
	      if(m_nodeType=="NodeUE")
	      {
		appName="UMTS-"+appName;
	      }
	      m_servListHelper->Add (m_umtsAppHelper, appName);
	      NS_LOG_DEBUG ("UmtsApp application with itetrisName="<<appName<<" has been added to the ServiceListHelper"); 
              m_umtsAppHelper = NULL;
 	    }
 	    else if (appType=="C2CIP")
             {
 	      m_servListHelper->Add (m_c2cIpHelper, appName);

 	      NS_LOG_DEBUG ("C2CIP application with itetrisName="<<appName<<" has been added to the ServiceListHelper"); 
               m_c2cIpHelper = NULL;
 	    }
           else
 	    {
 	      NS_FATAL_ERROR ("Type of application not valid ");
 	    }
	  return;
	}
      rc = xmlTextReaderRead (reader);
    }
}

} // namespace ns3
