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

#include "dvbh-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/dvbh-helper.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/dvbh-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/Dvbh-App-helper.h"
#include "ns3/dvbh-bs-mgnt.h"
#include "ns3/C2C-IP-helper.h"
#include "ip-interface-list.h"
#include "ns3/service-list-helper.h"

NS_LOG_COMPONENT_DEFINE ("DvbhInstaller");

namespace ns3
{

Ipv4AddressHelper DvbhInstaller::m_ipAddressHelper;
NodeContainer DvbhInstaller::vehicleContainer;
NodeContainer DvbhInstaller::baseStationContainer;

NetDeviceContainer DvbhInstaller::baseStationDeviceContainer;
NetDeviceContainer DvbhInstaller::vehicleDeviceContainer;

NS_OBJECT_ENSURE_REGISTERED (DvbhInstaller);

TypeId DvbhInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhInstaller")
    .SetParent<Object> ()     
                 
    ;
  return tid;
}
    
DvbhInstaller::DvbhInstaller () {

   
   dvbhOfdm =  DvbhOfdmLayerHelper::Default ();
   m_servListHelper = new ServiceListHelper ();
   m_dvbhAppHelper=NULL;
   m_ipAddressHelper.SetBase ("10.8.0.0", "255.255.0.0");
}

DvbhInstaller::~DvbhInstaller()
{ 
  delete m_servListHelper; 
  delete m_dvbhAppHelper;
  m_dvbhAppHelper=NULL;
  m_servListHelper = NULL; 
}


void
DvbhInstaller::Install (NodeContainer container) 
{
  
  NetDeviceContainer netDevices = dvbh.Install (dvbhOfdm,container,m_nodeType);   
  m_ipAddressHelper.Assign (netDevices);
  AddInterfacesToIpInterfaceList (container);
  
  
  DoInstall(container,netDevices);
      
  if(m_nodeType=="NodeUE")
  { 
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
DvbhInstaller::AddVehicles(NodeContainer container,NetDeviceContainer netDevices)
{
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
  {
      for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
      {	
	(*i)->GetObject <DvbhBsMgnt> ()->AddVehicle(DynamicCast<DvbhNetDevice>(*iterator));
      }
      
  }
}

void
DvbhInstaller::AddBaseStations(NodeContainer container,NetDeviceContainer netDevices)
{  
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
  {
      
      for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
      {
	  
	  (*i)->GetObject <DvbhVehicleScanMngr>()->AddBaseStation(DynamicCast<DvbhNetDevice>(*iterator));
      }
  }
}

void
DvbhInstaller::Configure (std::string filename)
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for DVBH");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      // dvbhOfdm type setting
      if (std::string ((char*)tag) == "DvbhOfdm")
	{
          xmlChar *nodeType = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  
	  m_nodeType=(char *)nodeType;
	
	  xmlFree (nodeType);
        }
        
        if (std::string ((char*)tag) == "ofdm")
	{
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");	  
	  dvbhOfdm.Set((char *)attribute,StringValue((char *)value));
	   NS_LOG_DEBUG ("DVB-H OFDM attribute=" << attribute <<" value=" << value); 
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
// 		  m_ipAddressHelper.SetBase ((char*)address, (char*)mask);		 
		  NS_LOG_DEBUG ("ip address="<<(char*)address<<" mask=" << (char*)mask); 
                }
	      xmlFree (mask);
	    }
	  xmlFree (address);
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
DvbhInstaller::AddInterfacesToIpInterfaceList (NodeContainer container)
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
      bool res = interfaceList->AddIpInterface("Dvbh", ipStack->GetAddress (index-1,0));
      NS_ASSERT_MSG (res, "DvbhInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      NS_LOG_INFO ("IP address " << ipStack->GetAddress (index-1,0));
      
      
    }
}

void 
DvbhInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader)
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

       if (std::string ((char*)tag) == "DvbhApp")
	{
          appType = "DvbhApp";
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
	  if (name != 0)
	    {
	      appName = std::string ((char*)name);	      
	      m_dvbhAppHelper = new DvbhAppHelper();
	      NS_LOG_DEBUG ("Application itetrisName="<<std::string ((char*)name)); 
	    }
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	      if (value != 0)
		{
		  if (m_dvbhAppHelper)
		    {
		      m_dvbhAppHelper->SetAttribute((char*)attribute,StringValue((char*)value));
		      NS_LOG_DEBUG ("DVB-H attribute=" << attribute <<" value=" << value); 
		    }
		}
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
	}      	  
	else if (std::string ((char*)tag) == "streams")
	{
	  ProcessStreamInstall (reader);
	}
	else if (std::string ((char*)tag) == "application")
	{
	      if(m_nodeType=="NodeUE")
	      {
		appName="DVBH-"+appName;
	      }
	      m_servListHelper->Add (m_dvbhAppHelper, appName);

	      NS_LOG_DEBUG ("DVB-H application with itetrisName="<<appName<<" has been added to the ServiceListHelper"); 

	  return;
	}
	
      rc = xmlTextReaderRead (reader);
    }
}

void 
DvbhInstaller::ProcessStreamInstall (xmlTextReaderPtr reader)
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

       if (std::string ((char*)tag) == "servicestream")
	{          
	  
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
	  if (name != 0)
	    {
	      appName = std::string ((char*)name);	      
// 	      m_dvbhAppHelper = new DvbhAppHelper();
	      NS_LOG_DEBUG ("Stream itetrisName="<<std::string ((char*)name)); 
	    }
	  xmlChar *streamId = xmlTextReaderGetAttribute (reader, BAD_CAST "StreamId");
	  if (m_dvbhAppHelper)
	    {
	      
	      m_dvbhAppHelper->AddStreamToService(atoi((char*)streamId),appName);
	      NS_LOG_DEBUG ("New Stream DVB-H value=" << streamId); 
	    }
	    
	  xmlFree (name);
	  xmlFree (streamId);
	}
	else if (std::string ((char*)tag) == "streams")
	{          
	  return;
	
	}
    
      rc = xmlTextReaderRead (reader);
    }
}

} // namespace ns3
