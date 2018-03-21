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
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

/****************************************************************************************
 * Edited by Jérôme Härri <haerri@eurecom.fr>
 * EURECOM 2013
 * Added multi-channel (multi-interface support by default)
 * a WAVE node has by default two DOT11 interfaces: CCH and a SCH
 * no channel switching according to ETSI standards.
***************************************************************************************/

/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#include "wave-installer.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h" 
#include "ns3/fading-model.h"
#include "ns3/shadowing-model.h"
#include "ns3/visibility-model.h"
#include "ns3/pointer.h"

#include "ns3/fatal-error.h"
#include "ns3/attribute.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/CAMmanage-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/yans-wifi-phy.h"

NS_LOG_COMPONENT_DEFINE ("WaveInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WaveInstaller);

TypeId WaveInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WaveInstaller")
    .SetParent<Object> ()               
    ;
  return tid;
}
    
WaveInstaller::WaveInstaller () 
{
  m_camHelper = NULL;
  m_c2cIpHelper = NULL;
  m_servListHelper = new ServiceListHelper ();
  m_channelName = ""; 
  m_antennaHeight = 1.5;
  FADING=0;                               
  m_interferenceRangeV = 0;          
  m_interferenceRangeC = 0;

  for (uint8_t i=0; i<8; i++)
    {
      AttributesChannel attrChannel;
      attrChannel.name = "";
      attrChannel.value = new EmptyAttributeValue ();
      m_attributesChannel.push_back (attrChannel);
    }

  wavePhyCch = YansWifiPhyHelper::Default ();
  wavePhySch = YansWifiPhyHelper::Default ();

  wave = Wifi80211pHelper::Default();
  waveMac = QosWaveMacHelper::Default();

} 

WaveInstaller::~WaveInstaller ()
{
  delete m_camHelper; 
  delete m_c2cIpHelper; 
  delete m_servListHelper; 
  m_camHelper = NULL;
  m_c2cIpHelper = NULL; 
  m_servListHelper = NULL; 
}

void
WaveInstaller::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create <Object> ();
  node->AggregateObject (protocol);
}

/**
 * @brief WaveInstaller::Install  this installs netDevices (CCH and SCH), and the beaconing protocol for WAVE nodes
 * @param container the Node container, where nodes need to be installed
 */

void
WaveInstaller::Install (NodeContainer container) 
{
  NS_LOG_FUNCTION("");
  SetAntennaHeightInNodes (container);

  // 802.11p CCH NetDevice configuration
  NetDeviceContainer CchDevices = wave.Install (wavePhyCch, waveMac, container);
  SetChannelType (CchDevices,CCH); // Set 1st ITS-G5A Netdevice to operate on CCH

  // 802.11p SCH NetDevice configuration
  NetDeviceContainer SchDevices = wave.Install (wavePhySch, waveMac, container);
  SetChannelType (SchDevices,SCH1); // Set 2nd ITS-G5A Netdevice to operate on SCH

  // 802.11p switching manager configuration
  // Deprecated - (WAVE) removed
  //switchingHelper.Install(SchDevices);

  inf.Assign(ITS_CCH, CchDevices);
  inf.Assign(ITS_SCH, SchDevices);

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
    {
      Ptr<Node> node = *i;
      CreateAndAggregateObjectFromTypeId (node, "ns3::BeaconingProtocol");
      int num = node->GetNDevices();
      for(int i = 0; i < num; ++i)
      {
      	Ptr<NetDevice> dev = node->GetDevice(i);
      	if (dev->GetInstanceTypeId() != WifiNetDevice::GetTypeId())
      		continue;
      	Ptr<WifiNetDevice> device = dev->GetObject<WifiNetDevice>();
				int freq = device->GetPhy()->GetFrequency();
				int channel = device->GetPhy()->GetChannelNumber();
				Ptr<YansWifiPhy> phy = device->GetPhy()->GetObject<YansWifiPhy>(YansWifiPhy::GetTypeId());
				int freqMhz = phy->GetChannelFrequencyMhz();
				NS_LOG_INFO("Node phy: GetFrequency " << freq << " GetChannelNumber " << channel << " GetChannelFrequencyMhz " << freqMhz);
      }
    }

  DoInstall (container, CchDevices, SchDevices);
}

void 
WaveInstaller::SetAntennaHeightInNodes (NodeContainer container)
{
  for (uint32_t i = 0; i < container.GetN (); i++)
    {
      container.Get (i)->GetObject <MobilityModel>()->SetAntennaHeight (m_antennaHeight);
    }
}

void
WaveInstaller::ConfigureWaveChannel (void)
{                           
  waveChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  waveChannel.AddPropagationLoss (m_channelName,
    m_attributesChannel[0].name, *m_attributesChannel[0].value,
    m_attributesChannel[1].name, *m_attributesChannel[1].value,
    m_attributesChannel[2].name, *m_attributesChannel[2].value,
    m_attributesChannel[3].name, *m_attributesChannel[3].value,
    m_attributesChannel[4].name, *m_attributesChannel[4].value,
    m_attributesChannel[5].name, *m_attributesChannel[5].value,
    m_attributesChannel[6].name, *m_attributesChannel[6].value,
    m_attributesChannel[7].name, *m_attributesChannel[7].value); 
    if (m_interferenceRangeV != 0 && m_interferenceRangeC !=0)
    {
      m_waveCch = waveChannel.Create (m_interferenceRangeV, m_interferenceRangeC);
      m_waveSch = waveChannel.Create (m_interferenceRangeV, m_interferenceRangeC);
    }
    else
    {
      m_waveCch = waveChannel.Create ();
      m_waveSch = waveChannel.Create (); 
    }
      
  wavePhyCch.SetChannel (m_waveCch);
  wavePhySch.SetChannel (m_waveSch);
}

Ptr<YansWifiChannel> 
WaveInstaller::GetWaveCch (void)
{
  return (m_waveCch);
}

Ptr<YansWifiChannel> 
WaveInstaller::GetWaveSch (void)
{
  return (m_waveSch);
}

/**
 * @brief WaveInstaller::RelateInstaller
 * @param installer
 */

void
WaveInstaller::RelateInstaller (Ptr<CommModuleInstaller> installer)
{
  Ptr<WaveInstaller> relatedInstaller = DynamicCast<WaveInstaller> (installer);
  NS_ASSERT (relatedInstaller->GetWaveCch ());
  NS_ASSERT (relatedInstaller->GetWaveSch ());
  wavePhyCch.SetChannel (relatedInstaller->GetWaveCch ());
  wavePhySch.SetChannel (relatedInstaller->GetWaveSch ());
  NS_LOG_DEBUG ("Linking installers in WaveInstaller::RelateInstaller");
}

/**
 * @brief WaveInstaller::SetChannelType
 * @param devices
 * @param channel
 */

void 
WaveInstaller::SetChannelType (NetDeviceContainer devices, ChannelType channel)
{
  for (NetDeviceContainer::Iterator i = devices.Begin (); i != devices.End (); ++i)
    {
      Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(*i);
      Ptr<WifiPhy> phy = device->GetPhy ();
      phy->SetChannelNumber (channel);
    }
}

/**
 * @brief WaveInstaller::GetFirstEmptyElement
 * @return vector of channel attribute for te WAVE configuration
 */

std::vector<WaveInstaller::AttributesChannel>::iterator 
WaveInstaller::GetFirstEmptyElement (void)
{
  int i = 0;
  for(std::vector<WaveInstaller::AttributesChannel>::iterator it = m_attributesChannel.begin(); it != m_attributesChannel.end(); ++it)
  {
    if ( (*it).name == "" )
      {
        return (it);
      }
    i++;
  }
  return m_attributesChannel.end();
}

/**
 * @brief WaveInstaller::Configure   this method reads the configuaration file and configure a WAVE node. It notably reads IPv6 configs as well as fading parameters.
 * @param filename  the configuration file to be read to configure the WAVE node
 */

void
WaveInstaller::Configure (std::string filename)
{
  
  NS_LOG_DEBUG ("WAVE-INSTALLER: trying to read file from path = "<< filename);
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR (" Wave-installer: Error at reading the configuration file for wave-installer");
    }

  NS_LOG_DEBUG ("Reading config file for WAVE");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      //NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

      // PropagationLossModel
      if (std::string ((char*)tag) == "propagationLoss")
	{
          xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  if (name != 0)
	    {
              m_channelName = std::string ((char*)name);
              NS_LOG_DEBUG ("propagationLoss nameModel="<<m_channelName); 
	    }
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
              xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
              if (value != 0)
                {
		  std::vector<WaveInstaller::AttributesChannel>::iterator it = GetFirstEmptyElement ();
		  (*it).name = std::string ((char*)attribute);
		  (*it).value = new StringValue ((char*)value);
		  NS_LOG_DEBUG ("propagationLoss attribute="<<(*it).name<<" value=" << value); 
                }
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
        }

      // VisibilityModel
      if (std::string ((char*)tag) == "visibilityModel")
	{
          xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  if (name != 0)
	    {
	      m_visibilityObject.SetTypeId (std::string ((char*)name)); 
              NS_LOG_DEBUG ("visibilityModel nameModel= "<< name); 
	    }
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
              xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
              if (value != 0)
                {
		  m_visibilityObject.Set (std::string ((char*)attribute),StringValue ((char*)value));
		  Ptr<VisibilityModel> visibilityModel = m_visibilityObject.Create()->GetObject <VisibilityModel> ();
		  NS_ASSERT (visibilityModel);
		  std::vector<WaveInstaller::AttributesChannel>::iterator it = GetFirstEmptyElement ();
		  (*it).name = std::string ("VisibilityModel");
		  (*it).value = new PointerValue(visibilityModel); 
		  NS_LOG_DEBUG ("visibilityModel attribute= "<<attribute<<" value= " << value); 
                }
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
        }

      // ShadowingModel
      if (std::string ((char*)tag) == "shadowingModel")
	{
          xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  if (name != 0)
	    {
	      m_shadowingObject.SetTypeId (std::string ((char*)name)); 
              NS_LOG_DEBUG ("ShadowingModel nameModel="<< name); 
	    }
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
              xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
              if (value != 0)
                {
		  m_shadowingObject.Set (std::string ((char*)attribute),StringValue ((char*)value));
		  Ptr<ShadowingModel> shadow = m_shadowingObject.Create ()->GetObject<ShadowingModel> ();
		  NS_ASSERT (shadow);
		  std::vector<WaveInstaller::AttributesChannel>::iterator it = GetFirstEmptyElement ();
		  (*it).name = std::string ("ShadowingModel");
		  (*it).value = new PointerValue(shadow); 
		  NS_LOG_DEBUG ("shadowingModel attribute= "<<attribute<<" value= " << value); 
                }
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
        }
        
      // fadingModel
      if (std::string ((char*)tag) == "fadingModel")
	{
          xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "name");
	  if (name != 0)
	    {
	      m_fadingObject.SetTypeId (std::string ((char*)name)); 
              NS_LOG_DEBUG ("FadingModel nameModel="<< name); 
	    }
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
              xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
              if (value != 0)
                {
		  FADING=1;
		  m_fadingObject.Set (std::string ((char*)attribute),StringValue ((char*)value));
		  NS_LOG_DEBUG ("fadingModel attribute="<<attribute<<" value=" << value); 
                }
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
        }
        
        // Interference Ranges
	if (std::string ((char*)tag) == "InterferenceRange")
	{
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      if(std::string ((char*)attribute) == "Vehicle")
	      {
                 xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
                 if (value != 0)
                 {
                    m_interferenceRangeV= atof ((char*)value);
		    NS_LOG_DEBUG ("Interference Range attribute="<<attribute<<" value=" << value); 
                 }
                 xmlFree (value);
	      }
	      if(std::string ((char*)attribute) == "RSU")
	      {
                 xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
                 if (value != 0)
                 {
                    m_interferenceRangeC= atof ((char*)value);
		    NS_LOG_DEBUG ("Interference Range attribute="<<attribute<<" value=" << value); 
                 }
                 xmlFree (value);
	      }
	    }
	  xmlFree (attribute);
        }

      // YansWifiPhy
      else if (std::string ((char*)tag) == "wavePhyCCH" || std::string ((char*)tag) == "wavePhySCH" )
	{
          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'attribute'");
	    }
          xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	  if (value == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'value'");
	    }
          if (std::string ((char*)tag) == "wavePhyCCH")
            {
	      NS_LOG_DEBUG ("wavePhyCch attribute="<<attribute<<" value="<<value);
	      wavePhyCch.Set((char*)attribute,StringValue((char*)value));
            }
          else
            {
	      NS_LOG_DEBUG ("wavePhySch attribute="<<attribute<<" value="<<value);
	      wavePhySch.Set((char*)attribute,StringValue((char*)value));
            }
	  xmlFree (attribute);
	  xmlFree (value);

        }

      // QosWifiMac
      else if (std::string ((char*)tag) == "wifiRemoteStationManager")
	{
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'attribute'");
	    }
          xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	  if (value == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'value'");
	    }
	  NS_LOG_DEBUG ("wifiRemoteStationManager attribute="<<attribute<<" value="<<value);
	  wave.Set((char*)attribute, StringValue((char*)value));
	  xmlFree (attribute);
	  xmlFree (value);
        }

      // MobilityModel
      else if (std::string ((char*)tag) == "antenna")
	{
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'attribute'");
	    }
          xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	  if (value == 0)
	    {
	      NS_FATAL_ERROR ("Error getting attribute 'value'");
	    }
	  NS_LOG_DEBUG ("antenna attribute="<<attribute<<" value="<<value);
	  m_antennaHeight = atof ((char*)value);
	  xmlFree (attribute);
	  xmlFree (value);
        }

      //ipconfiguration
      else if (std::string ((char*)tag) == "ipv6")
	{
          xmlChar *network = xmlTextReaderGetAttribute (reader, BAD_CAST "network");
	  if (network != 0)
	    {
              xmlChar *prefix = xmlTextReaderGetAttribute (reader, BAD_CAST "prefix");
              if (prefix != 0)
                {
            	  m_ipv6AddressHelper.NewNetwork();
            	  m_ipv6AddressHelper.SetBase((char*)network, (char*)prefix);

		 
		  NS_LOG_DEBUG ("ipv6 network="<<(char*)network<<" prefix=" << (char*)prefix); 
                }
	      xmlFree (prefix);
	    }
	  xmlFree (network);
        }

      // Applications
      else if (std::string ((char*)tag) == "application")
	{
	  ProcessApplicationInstall (reader);
	}

      rc = xmlTextReaderRead (reader);
    }
  xmlFreeTextReader (reader);
  
  if (m_channelName != "")
    {
      if(FADING){
         Ptr<FadingModel> fading = m_fadingObject.Create ()->GetObject<FadingModel> ();
         NS_ASSERT (fading);
         std::vector<WaveInstaller::AttributesChannel>::iterator it = GetFirstEmptyElement ();
         (*it).name = std::string ("FadingModel");
         (*it).value = new PointerValue(fading); 
      }
      ConfigureWaveChannel ();
    }
}

void 
WaveInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader)
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


      // CAMmanage application
      if (std::string ((char*)tag) == "CAMmanage")
	{
          appType = "CAMmanage";
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
	  if (name != 0)
	    {
	      m_camHelper = new CAMmanageHelper("ns3::c2cl4TSocketFactory");
	      appName = std::string ((char*)name);
	      NS_LOG_DEBUG ("Application itetrisName="<<std::string ((char*)name)); 
	    }
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	      if (value != 0)
		{
		  if (m_camHelper)
		    {
		      m_camHelper->SetAttribute((char*)attribute,StringValue((char*)value));
		      NS_LOG_DEBUG ("CAMmanage attribute=" << attribute <<" value=" << value); 
		    }
		}
	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
	}

      // C2CIP application
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
          if (appType == "CAMmanage")
	    {
	      if (appName != "CAM")
                {
		  m_servListHelper->Add (m_camHelper, appName);
		  NS_LOG_DEBUG ("CAMmanage application with itetrisName="<<appName<<" has been added to the ServiceListHelper"); 
		  m_camHelper = NULL;
                }
              else
                {
                  NS_FATAL_ERROR ("WaveInstaller::ProcessApplicationInstall: The name 'CAM' cannot be assigned to an Application (it is reserved)."); 
		}
	    }
          else if ("C2CIP")
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
