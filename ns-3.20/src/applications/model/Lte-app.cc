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
 * Author:  Sendoa Vaz
 */

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Lte-app.h"
#include "ns3/iTETRIS-Application.h"  
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
#include "ns3/ip-interface-list.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/socket-factory.h"
#include "ns3/string.h"
#include "ns3/itetris-types.h"
#include "ns3/udp-socket.h"

NS_LOG_COMPONENT_DEFINE ("LteApp");

using namespace std;

namespace ns3 {

  
NS_OBJECT_ENSURE_REGISTERED (LteApp);

TypeId
LteApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteApp")
    .SetParent<Application> ()
    .AddConstructor<LteApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&LteApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&LteApp::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("PortIP", "The port of the ip messages",   
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&LteApp::m_portIP),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ApplicationType", " Determines the Transmission Mode used to transmit Data (AM,MULTICAST,BROADCAST,etc) ",     
    		   StringValue ("UM-NON_FRAG"),
    		   MakeStringAccessor (&LteApp::m_applicationType),     
    		   MakeStringChecker())   
    ;
  return tid;
}

LteApp::LteApp()
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
    m_applicationType="UM-NON_FRAG";
    m_serviceIp=Ipv4Address("255.255.255.255");
    m_broadcastIp=Ipv4Address("255.255.255.255");
   
}

LteApp::~LteApp()
{
}

void
LteApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_IPv4socket = 0;
  Application::DoDispose ();
}

void
LteApp::StartApplication (void)
{
}


void
LteApp::ConfigureNode(Ipv4Address address)  
{
 uint32_t netDeviceNumber=m_node->GetNDevices();
  Ipv4InterfaceAddress interface;
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<LteNetDevice>(m_node->GetDevice(i)))
    {
      if(DynamicCast<LteNetDevice>(m_node->GetDevice(i))->GetNodeType()=="ueNode")
	{
	  //For future implementation
	}
	else
	{
	  if(address==*(m_node->GetObject<LteBsMgnt>()->GetIpAddress(ID_BROADCAST)))	    	  
	  {	    	   
	    //For future implementation
	  }
	  else if(address.IsMulticast())
	  {
	    //..
	  }	    
	  else 
	  {
	    //..
	  }
	}
    }   
  }
}

void
LteApp::UninstallFlowInNode()
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<LteNetDevice>(m_node->GetDevice(i)))
    {
        if(DynamicCast<LteNetDevice>(m_node->GetDevice(i))->GetNodeType()=="ueNode")
	{
         //For future implementation
	}
	else
	{ 
         //For future implementation
	}
        
    }
  }
}


void
LteApp::SetSockets (void)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][LteApp]******************************* set sockets on node ****************************\n");

  if (!m_IPv4socket)
  {
	//---------------- check iTETRIS ----------------------------;
	NS_LOG_INFO("\n");
	NS_LOG_INFO("[ns3][LteApp] Creating IP Socket of Type = UdpSocketFactory | Binds at PORT number = "<<m_portIP<<"\n");
	//---------------- check iTETRIS ----------------------------

	m_IPv4socket = Socket::CreateSocket (GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));

        InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portIP);
        m_IPv4socket->Bind (local);
        m_IPv4socket->SetRecvCallback (MakeCallback(&LteApp::ReceiveIP, this));
        NS_LOG_INFO("[ns3][LteApp]********* IP socket created and binded ***********\n");
	        
  }
  else
  {
     NS_FATAL_ERROR ("[ns3][LteApp] creation attempt of an ip socket for LteApp that has already an ip socket active");
         return;
  }
}


void 
LteApp::StopApplication(void)
{
}

void 
LteApp::StopTransmitting(void){
  
  //UninstallFlowInNode(); 
  m_runningIP = false;

    if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][LteApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********\n");;
    }
  
}


void
LteApp::FindNodeIdentifier(Ipv4Address address)
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<LteNetDevice>(m_node->GetDevice(i)))
    {
      if(DynamicCast<LteNetDevice>(m_node->GetDevice(i))->GetNodeType()=="ueNode")
	{
	  //For future implementation
	  break;
	}
	else
	{
	 //For future implementatio
	  break;
	}
      
      
    }
    
   
  }
}

void 
LteApp::StartTransmitting(Ipv4Address address )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningIP = true;
    
  m_stepSequenceNumber=0;

 /*if(m_applicationType!="BROADCAST"&&m_applicationType!="MULTICAST") 
  {    
    FindNodeIdentifier(address);
  }*/
  
  
  NS_LOG_INFO("[ns3][LteApp] ======NODE " << GetNode()->GetId() <<" Tx to address " << address<<" AppType "<< m_applicationType<<"  ===========\n");
 
  //ConfigureNode(address); 

  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
      
  m_StartTime = (Simulator::Now()).GetSeconds ();
   if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][LteApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********\n");
    }
  InitializeINCIvariables();
  DoSendIP(address,m_destinationaddress,m_MessRegenerationTime,m_StartTime);


}

void
LteApp::DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][LteApp] ======NODE " << GetNode()->GetId() <<": connection attempt to address " << address <<"   ===========\n");
///////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  m_IPv4socket->Connect (destinationAddress);
  AddInciPacketTags(packet);
    
      
  m_sendCountIP ;
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
 NS_LOG_INFO("\n");  
 NS_LOG_INFO("[ns3][LteApp] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<" "<<Simulator::Now()<<"\n");
///////////////////////////////////////////////////////////////////////////////////////////////////////////

   m_IPv4socket->Send (packet);
}

void 
LteApp::ReceiveIP (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][LteApp] Start Receiving - Call SocketIP -> RecvFrom()\n");;
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);

          RetrieveInciPacketTags (packet);

          m_recvCountIP;

         NS_LOG_INFO("[ns3][LteApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIP<<" from " <<
         address.GetIpv4()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes\n");
         NS_LOG_INFO("[ns3][LteApp]========= SUCCESS : IP reception on node " << GetNode()->GetId() <<" On Time "<<Simulator::Now()<<" ==============\n");
        }
    }
}

void 
LteApp::SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][LteApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********\n");
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
          messRegenerationTime= -1;  
       } 
      } 
}

void 
LteApp::ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIP)
    {
	  m_dataRate = (m_packetSize * 8) * m_frequency;
	  Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
	  m_sendEventIP = Simulator::Schedule (tNext, &LteApp::SendPacketIP, this,address,destinationAddress,messRegenerationTime,startTime);
    }
   else
   {
        //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][LteApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********\n");
  //---------------- check iTETRIS ----------------------------
   }
}


void 
LteApp::SetMessRegenerationTime (double MessRegenerationTime)
{
   m_MessRegenerationTime=MessRegenerationTime;
}

void 
LteApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
LteApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
LteApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}


} 

