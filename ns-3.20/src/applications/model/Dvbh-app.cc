/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, CBT EU FP7 iTETRIS project
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
 * Author:  Sendoa Vaz <svaz@cbt.es>,
 */

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable.h"

#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/iTETRIS-Application.h"  
#include "Dvbh-app.h"
#include "ns3/dvbh-net-device.h"
#include "ns3/string.h"
#include "ns3/ip-interface-list.h"
#include "ns3/dvbh-tags.h"
#include "ns3/dvbh-manager-BaseStation.h"
#include "ns3/dvbh-manager-UserEquip.h"
#include "ns3/dvbh-bs-mgnt.h"
#include "ns3/itetris-types.h"
#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("DvbhApp");

using namespace std;

namespace ns3 {

  
NS_OBJECT_ENSURE_REGISTERED (DvbhApp);

TypeId
DvbhApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhApp")
    .SetParent<Application> ()
    .AddConstructor<DvbhApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&DvbhApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("BurstSize", "The size of the burst going to be sent ",
               UintegerValue (10000),
               MakeUintegerAccessor (&DvbhApp::m_burstSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("PortIP", "The port of the ip messages",     // modified by SV
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&DvbhApp::m_portIP),     // modified by SV
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ApplicationType", " Determines the Transmission Mode used to transmit Data (MULTICAST,BROADCAST) ",     // modified by SV
    		   StringValue ("MULTICAST"),
    		   MakeStringAccessor (&DvbhApp::m_applicationType),     // modified by SV
    		   MakeStringChecker())   
     .AddAttribute ("ServiceIp", " In case of defining a MULTICAST/BROADCAST application it is necessary to define the IP Address",     // modified by SV
    		   Ipv4AddressValue ("255.255.255.255"),
    		   MakeIpv4AddressAccessor (&DvbhApp::m_serviceIp),     // modified by SV
    		   MakeIpv4AddressChecker()) 
     .AddAttribute ("ServiceId", " ",     // modified by SV
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&DvbhApp::SetServiceId),     // modified by SV
    		   MakeUintegerChecker<uint32_t> ())
     .AddAttribute ("DeltaT", " In case of defining a MULTICAST/BROADCAST application it is necessary to define the IP Address",     // modified by SV
    		   DoubleValue (2),
    		   MakeDoubleAccessor (&DvbhApp::m_deltaT),     // modified by SV
    		   MakeDoubleChecker<double> ())


    ;
  return tid;
}

DvbhApp::DvbhApp()
{    
    m_IPv4socket = 0;    
    m_portIP = 0;
    m_packetSize = 0;
    m_dataRate = 0;
    m_frequency = 0;    
    m_runningIP = false;
    m_packetsSentIP = 0;
    m_sendCountIP = 0;
    m_recvCountIP = 0;
    m_MessRegenerationTime = -1;
    m_StartTime =0;
    m_rndOffset = UniformVariable(0.0,0.999); 
    m_destinationId=0;
    m_applicationType="MULTICAST";
    m_serviceIp=Ipv4Address("255.255.255.255");
    m_broadcastIp=Ipv4Address("255.255.255.255");
    m_serviceId=0;
   
}

DvbhApp::~DvbhApp()
{
}

void
DvbhApp::SetServiceId(uint32_t serviceId)
{
  m_serviceId=serviceId;         
}

void
DvbhApp::ConfigureVehicle()
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  Ipv4InterfaceAddress interface;
      
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {    
    if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i)))
    {      
      if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetNodeType()=="NodeUE")
	{
	  if(m_applicationType=="BROADCAST")
	  {
	    
	    DynamicCast<DVBHManagerUserEquip>(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetManager())
	    ->JoinService(m_serviceId,m_applicationType);
	  }
	 
	}	
    }
    
   
  } 
}

void
DvbhApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  
  m_IPv4socket = 0;
  Application::DoDispose ();
}

void
DvbhApp::StartApplication (void)
{
}

void
DvbhApp::ConfigureBaseStation(Ipv4Address address)
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  Ipv4InterfaceAddress interface;
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i)))
    {      
      if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetNodeType()=="NodeB")
	{
	  
	  DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetManager())
	->AddNewTransportStream(200000,"Transport0");
	  
	  if(address==*(m_node->GetObject<DvbhBsMgnt>()->GetIpAddress(ID_BROADCAST)))	    	  
	  {	    	    	    
	    DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetManager())
	    ->AddNewService("TV",m_serviceId,address,m_burstSize,"BROADCAST","Emergency",0);
	    
	    for(StreamIdListI iterator=streamList.begin();iterator!=streamList.end();iterator++)
	    {
		DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetManager())
		->AddElementaryStreamToService (m_serviceId,(*iterator).second,(*iterator).first,m_burstSize);
	    }
	  }
	  else if(address.IsMulticast())
	  {	    	    	    
	    DynamicCast<DVBHManagerBaseStation>(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetManager())
	    ->AddNewService("TV",m_app_index,address,m_burstSize,"MULTICAST","Emergency",0);
	  }
	
	}	
    }
       
  }
  
}

void 
DvbhApp::AddStreams(std::list<std::pair<std::string,uint32_t > > streams)
{
 streamList=streams;
}

void
DvbhApp::UninstallFlowInNode()
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i)))
    {
      if(DynamicCast<DvbhNetDevice>(m_node->GetDevice(i))->GetNodeType()=="NodeUE")
	{
	    //It is not already necessary. For future implementation
	  break;
	}
	else
	{
	    //It is not already necessary. For future implementation
	}
            
    }
    
   
  }
}


void
DvbhApp::SetSockets (void)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][DvbhApp]******************************* set sockets on node ****************************\n");

  if (!m_IPv4socket)
  {
	//---------------- check iTETRIS ----------------------------;
	NS_LOG_INFO("\n");
	NS_LOG_INFO("[ns3][DvbhApp] Creating IP Socket of Type = UdpSocketFactory | Binds at PORT number = "<<m_portIP<<"\n");
	//---------------- check iTETRIS ----------------------------

	m_IPv4socket = Socket::CreateSocket (GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));

        InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portIP);
        m_IPv4socket->Bind (local);
        m_IPv4socket->SetRecvCallback (MakeCallback(&DvbhApp::ReceiveIP, this));
        NS_LOG_INFO("[ns3][DvbhApp]********* IP socket created and binded ***********\n");
	        
  }
  else
  {
     NS_FATAL_ERROR ("[ns3][DvbhApp] creation attempt of an ip socket for DvbhApp that has already an ip socket active");
         return;
  }
  
  ConfigureVehicle();
}


void 
DvbhApp::StopApplication(void)
{
}

void 
DvbhApp::StopTransmitting(void){
  
  UninstallFlowInNode();
  m_runningIP = false;

    if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][DvbhApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********\n");;
    }
  if (m_IPv4socket)
    {
     m_IPv4socket->Close ();
    }
}

void 
DvbhApp::StartTransmitting(Ipv4Address address )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningIP = true;
    

  ConfigureBaseStation(address);  

  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
      
  m_StartTime = (Simulator::Now()).GetSeconds ();
   if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][DvbhApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********\n");
    }
  InitializeINCIvariables();
  DoSendIP(address,destinationaddress,m_MessRegenerationTime,m_StartTime);

}

void
DvbhApp::DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  
   if(m_fragmentList.size()>0)
    {
      
      
      m_IPv4socket->Connect (destinationAddress);
      Ptr<Packet> packet=m_fragmentList.front();
      m_fragmentList.erase(m_fragmentList.begin());
      
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	NS_LOG_INFO("\n");  
	NS_LOG_INFO("[ns3][DvbhApp] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<" ServiceId "<<m_serviceId<<" "<<Simulator::Now()<<"\n");
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
      
      m_IPv4socket->Send (packet->Copy()); 
      m_sendCountIP ++;
      if(m_fragmentList.size()!=0)
      {
	m_sendEventIP = Simulator::Schedule (MicroSeconds(500), &DvbhApp::DoSendIP, this,address,destinationAddress,messRegenerationTime,startTime); 
      }
      
      
    }
    else
    {
     if(m_packetSize<4000)
      {
	Ptr<Packet> packet = Create<Packet> (m_packetSize);

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	NS_LOG_INFO("\n");
	NS_LOG_INFO("[ns3][DvbhApp] ======NODE " << GetNode()->GetId() <<": connection attempt to address " << m_IPAddress <<"   ===========\n");
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	m_IPv4socket->Connect (destinationAddress);
	AddInciPacketTags(packet);
	  
	DvbhDeltaTTag deltaTag;
	deltaTag.Set(5);
	packet->AddPacketTag(deltaTag);
	
	AppIndexTag indexTag;
		    
	m_sendCountIP ++;
	
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////
      NS_LOG_INFO("\n");  
      NS_LOG_INFO("[ns3][DvbhApp] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<" ServiceId "<<m_serviceId<<" "<<Simulator::Now()<<"\n");
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	  

	packet->RemovePacketTag(indexTag);
	indexTag.Set(m_serviceId);
	packet->AddPacketTag(indexTag);
	
	m_IPv4socket->Send (packet->Copy());       
      //   ScheduleTxIP (address,destinationAddress,messRegenerationTime,startTime);
      }
      else
      {
	
	int fragments=m_packetSize/4000;
      
	for(int i=0;i<fragments;i++)
	{	
	  Ptr<Packet> packet = Create<Packet> (4000);            
	  AddInciPacketTags(packet);
	    
	  DvbhDeltaTTag deltaTag;
	  deltaTag.Set(5);
	  packet->AddPacketTag(deltaTag);
	  
	  AppIndexTag indexTag;
		      
	  packet->RemovePacketTag(indexTag);
	  indexTag.Set(m_serviceId);
	  packet->AddPacketTag(indexTag);
	    
	  m_fragmentList.push_back(packet->Copy());
	  
	}
	
	Ptr<Packet> packet2 = Create<Packet> (m_packetSize-fragments*4000);

	AddInciPacketTags(packet2);
	  
	DvbhDeltaTTag deltaTag;
	deltaTag.Set(5);
	packet2->AddPacketTag(deltaTag);
	
	AppIndexTag indexTag;
			  
	packet2->RemovePacketTag(indexTag);
	indexTag.Set(m_serviceId);
	packet2->AddPacketTag(indexTag);
	
	m_fragmentList.push_back(packet2->Copy());
      
	DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
	
      }
    }
      

}

void 
DvbhApp::ReceiveIP (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][DvbhApp] Start Receiving - Call SocketIP -> RecvFrom()\n");;
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);

          RetrieveInciPacketTags (packet);

          m_recvCountIP++;

         NS_LOG_INFO("[ns3][DvbhApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIP<<" from " <<
         address.GetIpv4()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes\n");
         NS_LOG_INFO("[ns3][DvbhApp]========= SUCCESS : IP reception on node " << GetNode()->GetId() <<" On Time "<<Simulator::Now()<<" ==============\n");
        }
    }
}

void 
DvbhApp::SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][DvbhApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********\n");
    }
   // always keep transmitting
   if(messRegenerationTime < 0)
   {
        DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
   }
   else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
      {
            double now = (Simulator::Now()).GetSeconds();
            double lifetime =  now - startTime;

       if( lifetime <= messRegenerationTime){
          DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
       }
       else{
          messRegenerationTime= -1;  // set to default value
       } 
      } 
}

void 
DvbhApp::ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIP)
    {
	  m_dataRate = (m_packetSize * 8) * m_frequency;
	  Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
	  m_sendEventIP = Simulator::Schedule (tNext, &DvbhApp::SendPacketIP, this,address,destinationAddress,messRegenerationTime,startTime);
    }
   else
   {
        //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][DvbhApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********\n");
  //---------------- check iTETRIS ----------------------------
   }
}


void 
DvbhApp::SetMessRegenerationTime (double MessRegenerationTime)
{
   m_MessRegenerationTime=MessRegenerationTime;
}

void 
DvbhApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
DvbhApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
DvbhApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}

} // Namespace ns3
