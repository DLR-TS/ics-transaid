/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
 */
/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/****************************************************************************************
 *                          C2C-IP.cc
 * This is a generic iTETRIS C-ITS service capable of sending
 * both IPv6/IPv4 or C2C packets.
 * This method is used by any C-ITS applications not having a dedicated service
 * such as CAM or DENM. The ITS applications must be specified externally,
 * either in the ns-3 scratch or in the iTETRIS application module.
 *
***************************************************************************************/


#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable.h"
#include "ns3/socketc2c.h"
#include "ns3/simulator.h"
#include "ns3/socket-factoryc2c.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/c2cl4T-socket-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/ipv6-interface-address.h"
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/channel-tag.h"
#include "ns3/string.h"
#include "ns3/umts-basestation-manager.h"
#include "ns3/umts-userequipment-manager.h"
#include "ns3/umts-bs-mgnt.h"
#include "ns3/iTETRIS-Application.h"
#include "ns3/constant-velocity-mobility-model.h"  
#include "ns3/node-list.h"

#include "C2C-IP-app.h"


NS_LOG_COMPONENT_DEFINE ("C2CIPApp");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (C2CIPApp);

TypeId
C2CIPApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::C2CIPApp")
    .SetParent<Application> ()
    .AddConstructor<C2CIPApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&C2CIPApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&C2CIPApp::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("PortC2C", "The port for c2c messages",     
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&C2CIPApp::m_portC2C),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PortIP", "The port of the ip messages",    
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&C2CIPApp::m_portIP),     
    		   MakeUintegerChecker<uint16_t> ())
     .AddAttribute ("PortIPv6", "The port of the ipv6 messages",    
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&C2CIPApp::m_portIPv6),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SocketFactorytype1", "The type of the socket factory 1 to use.",   
               TypeIdValue (),
               MakeTypeIdAccessor (&C2CIPApp::m_firstSocketfactory),
               MakeTypeIdChecker ())
    .AddAttribute ("SocketFactorytype2", "The type of the socket factory 2 to use.",
               TypeIdValue (),
               MakeTypeIdAccessor (&C2CIPApp::m_secondSocketfactory),
               MakeTypeIdChecker ())
     .AddAttribute ("ApplicationType", " Determines the Transmission Mode used to transmit Data (AM,MULTICAST,BROADCAST,etc) ",     // modified by SV
    		   StringValue ("UM-NON_FRAG"),
    		   MakeStringAccessor (&C2CIPApp::m_applicationType),     // modified by SV
    		   MakeStringChecker())
    ;
  return tid;
}

C2CIPApp::C2CIPApp()
{
    m_C2Csocket = 0;
    m_IPv4socket = 0;
    m_IPv6socket = 0;
    m_portC2C = 0;
    m_portIP = 0;
    m_portIPv6 = 0;
    m_packetSize = 0;
    m_dataRate = 0;
    m_frequency = 0;
    m_runningC2C = false;
    m_packetsSentC2C = 0;
    m_sendCountC2C = 0;
    m_recvCountC2C = 0;
    m_runningIP = false;
    m_runningIPv6 = false;
    m_packetsSentIP = 0;
    m_packetsSentIPv6 = 0;
    m_sendCountIP = 0;
    m_sendCountIPv6 = 0;
    m_recvCountIP = 0;
    m_MessRegenerationTime = -1;
    m_StartTime =0;
    m_rndOffset = UniformVariable(0.0,0.999); 
}

C2CIPApp::~C2CIPApp()
{
}

void
C2CIPApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_C2Csocket = 0;
  m_IPv4socket = 0;
  m_IPv6socket = 0;
  Application::DoDispose ();
}

void
C2CIPApp::StartApplication (void)
{
}

void
C2CIPApp::SetSockets (void)
{
   NS_LOG_INFO("[ns3][C2CIPApp]******************************* set sockets on node "<<GetNode()->GetId()<<"****************************");

  if (!m_C2Csocket)
    {
      NS_LOG_INFO("[ns3][C2CIPApp] Creating c2c Socket of Type = "<<m_firstSocketfactory<<" | Binds at PORT number = "<<m_portC2C);
      m_C2Csocket = Socketc2c::CreateSocket (GetNode(), m_firstSocketfactory);
      m_C2Csocket->Bind (m_portC2C);
      m_C2Csocket->SetRecvCallback(MakeCallback (&C2CIPApp::ReceiveC2C, this));
      NS_LOG_INFO("[ns3][C2CIPApp]*********** C2C socket created and binded ************");
    }
    else
    {
      NS_FATAL_ERROR ("[ns3][C2CIPApp] creation attempt of a c2c socket for C2CIPApp that has already a c2c socket active");
      return;
    }

    if (!m_IPv4socket)
      {
        NS_LOG_INFO("[ns3][C2CIPApp] Creating IP Socket of Type = "<<m_secondSocketfactory<<" | Binds at PORT number = "<<m_portIP);

        m_IPv4socket = Socket::CreateSocket (GetNode(), m_secondSocketfactory);
        InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portIP);
        m_IPv4socket->Bind (local);
        m_IPv4socket->SetRecvCallback (MakeCallback(&C2CIPApp::ReceiveIP, this));
        NS_LOG_INFO("[ns3][C2CIPApp]********* IP socket created and binded ***********");
      }
    else
      {
        NS_FATAL_ERROR ("[ns3][C2CIPApp] creation attempt of an ip socket for C2CIPApp that has already an ip socket active");
        return;
      }

    if (!m_IPv6socket)
      {
        NS_LOG_INFO("[ns3][C2CIPApp] Creating IP Socket of Type = "<<m_secondSocketfactory<<" | Binds at PORT number = "<<m_portIPv6);

        m_IPv6socket = Socket::CreateSocket (GetNode(), m_secondSocketfactory);
        Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_portIPv6);
        m_IPv6socket->Bind (local6);
        m_IPv6socket->SetRecvCallback (MakeCallback(&C2CIPApp::ReceiveIPv6, this));
        
        // Added by Panos. This addition is useful for the case of Infrastructure Nodes (Base Stations, RSUs)
        //who need to transmit IPv6 Link-local Multicast messages (i.e. destination address: ff02::1)
        //In this case there has to be a specification of the NetDevice that is going to be used in the 
        //ipv6-static-routing::RouteOutput() level. The static nodes are expected to have only one type of
        //net device supporting IPv6!
        if(!m_node->IsMobileNode())
        {
           m_IPv6socket->BindToNetDevice(m_node->GetDevice(1));
        }
        NS_LOG_INFO("[ns3][C2CIPApp]********* IPv6 socket created and binded ***********");
      }
    else
      {
        NS_FATAL_ERROR ("[ns3][C2CIPApp] creation attempt of an ipv6 socket for C2CIPApp that has already an ip socket active");
        return;
      }
}


void 
C2CIPApp::StopApplication(void)
{
}

void 
C2CIPApp::StopTransmitting(void)
{
  NS_LOG_INFO("[ns3][C2CIPApp] ********STOP C2C-IP APP on node " << GetNode()->GetId() <<" ******");
  m_runningC2C = false;
  m_runningIP = false;
  m_runningIPv6 = false;

  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp] ********STOP C2C transmission on node " << GetNode()->GetId() <<"  while running ******");
    }
  if (m_C2Csocket)
    {
      m_C2Csocket->Close ();
    }

  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********");;
    }
  if (m_sendEventIPv6.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIPv6);
      NS_LOG_INFO("[ns3][C2CIPApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********");;
    }
  if (m_IPv4socket)
    {
      m_IPv4socket->Close ();
    }
  if (m_IPv6socket)
    {
      m_IPv6socket->Close ();
    }
}

void
C2CIPApp::FindNodeIdentifier(Ipv6Address address)
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i)))
    {
      if(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetNodeType()=="NodeUE")
	{
	  m_destinationId=DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetManager())
	  ->GetNodeBIdentifier();
	  break;
	}
	else
	{
	  m_destinationId=DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetManager())
	  ->GetNodeUEIdentifier(address);	  
	  break;
	}
      
      
    }
    
   
  }
}

void
C2CIPApp::ConfigureNode(Ipv6Address address)
{
  uint32_t netDeviceNumber=m_node->GetNDevices();
  Ipv6InterfaceAddress interface;
  
  for(uint32_t i=0;i<netDeviceNumber;i++)
  {
    if(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i)))
    {
        if(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetNodeType()=="NodeUE")
        {
            DynamicCast<UmtsUserEquipmentManager>(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetManager())->NewApplicationConfiguration(m_applicationType,m_app_index,10000,m_packetSize);
            break;
        }
        else
        {
            if(address.IsMulticast())
            {
                DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetManager())->NewApplicationConfiguration("MULTICAST",m_app_index,100000,ID_MULTICAST,address);
            }
            else
            {
                DynamicCast<UmtsBaseStationManager>(DynamicCast<UMTSNetDevice>(m_node->GetDevice(i))->GetManager())->NewApplicationConfiguration(m_applicationType,m_app_index,1000000,m_destinationId,address);
            }
        }
    }
  }
}

void 
C2CIPApp::StartTransmitting(Ipv4Address address )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningIP = true;
    
  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
  
  m_StartTime = (Simulator::Now()).GetSeconds ();
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********");
    }
  InitializeINCIvariables();
  DoSendIP(address,m_destinationaddress,m_MessRegenerationTime,m_StartTime);
}


void 
C2CIPApp::StartTransmitting(Ipv6Address address)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO("[ns3][C2CIPApp]********* START current IP periodic transmission before transmitting a new message on node " <<" *********");
  m_runningIPv6 = true;
  NS_LOG_INFO("[ns3][C2CIPApp]********* Transmitting node's position: " <<m_node->GetObject<MobilityModel>()->GetPosition()<<" *********");

  if(m_applicationType!="BROADCAST"&&m_applicationType!="MULTICAST")
  {
       FindNodeIdentifier(address);
  }

  NS_LOG_INFO("[ns3][C2CIPApp] ======NODE " << GetNode()->GetId() <<" Tx to address " << address<<" AppType "<< m_applicationType<<"  ===========");
  ConfigureNode(address);
    
  m_IPv6Address = address;
  Address destinationaddress = Inet6SocketAddress(m_IPv6Address, m_portIPv6);
  NS_LOG_INFO("[ns3][C2C-IPApp] DESTINATION:"<<address<<"  ===========");
  
  m_destinationaddress = destinationaddress;
  
  m_StartTime = (Simulator::Now()).GetSeconds ();
  if (m_sendEventIPv6.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIPv6);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********");
    }
  InitializeINCIvariables();
  DoSendIP(address,m_destinationaddress,m_MessRegenerationTime,m_StartTime);
}


void 
C2CIPApp::StartTransmitting(Ptr<c2cAddress> address)
{
  NS_LOG_FUNCTION("Node "<<GetNode()->GetId());
  m_runningC2C = true;
  m_c2cAddress = address;
  m_StartTime = (Simulator::Now()).GetSeconds ();
  if (m_sendEventC2C.IsRunning ())
  {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp]****** STOP current c2c periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" **********");
  }
  InitializeINCIvariables();
  DoSendC2C(address,m_MessRegenerationTime,m_StartTime);
}

void
C2CIPApp::DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
 
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  NS_LOG_INFO("[ns3][C2CIPApp] ======NODE " << GetNode()->GetId() <<": connection attempt to address " << address<<"   =====");

  m_IPv4socket->Connect (destinationAddress);
  AddInciPacketTags(packet);
  m_sendCountIP ++;
  
  NS_LOG_INFO("[ns3][C2CIPApp] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize());

  m_IPv4socket->Send (packet);
  ScheduleTxIP (address,destinationAddress,messRegenerationTime,startTime);
}


void
C2CIPApp::DoSendIP(Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  NS_LOG_INFO("[ns3][C2CIPApp] ======NODE " << GetNode()->GetId() <<": connection attempt to address " << address<<"   =====");

  m_IPv6socket->Connect (destinationAddress);
  AddInciPacketTags(packet);
  m_sendCountIPv6 ++;
  
  NS_LOG_INFO("[ns3][C2CIPApp] SENDING IP packet no. "<<m_sendCountIPv6<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize());

  m_IPv6socket->Send (packet);
  ScheduleTxIP (address,destinationAddress,messRegenerationTime,startTime);
}



void
C2CIPApp::DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  AddInciPacketTags(packet);
  m_sendCountC2C ++;
  
  NS_LOG_INFO("[ns3][C2CIPApp][node "<<GetNode()->GetId()<<"] SENDING C2C packet no. "<<m_sendCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"| address = "<<address);

  m_C2Csocket->DoSendTo(packet, address, m_portC2C,m_MsgLifeTime,1,m_sendCountC2C);
  ScheduleTxC2C (address,messRegenerationTime,startTime);
}


void
C2CIPApp::ReceiveC2C (Ptr<Socketc2c> socketc2c)
{
  Ptr<Packet> packet;
  NS_LOG_INFO("[ns3][C2CIPApp] Start Receiving - Call Socketc2c -> Recv()");
  packet=socketc2c->Recv();

  RetrieveInciPacketTags (packet);
  m_recvCountC2C++;

  NS_LOG_INFO("[ns3][C2CIPApp] SUCCESS: Receiving c2c message no. "<<m_recvCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<< " seconds | c2c message size  = "<<packet->GetSize()<<" Bytes");
  NS_LOG_INFO("[ns3][C2CIPApp]========= SUCCESS : C2C Reception on node " << GetNode()->GetId() <<"  ==========");;
}


void 
C2CIPApp::ReceiveIP (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][C2CIPApp] Start Receiving - Call SocketIP -> RecvFrom()");;
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
          RetrieveInciPacketTags (packet);
          m_recvCountIP++;

          NS_LOG_INFO("[ns3][C2CIPApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIP<<" from " <<
          address.GetIpv4()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes");
          NS_LOG_INFO("[ns3][C2CIPApp]========= SUCCESS : IP reception on node " << GetNode()->GetId() <<"  ==============");
          
        }
    }
}

void 
C2CIPApp::ReceiveIPv6 (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][C2CIPApp] Start Receiving - Call SocketIP -> RecvFrom()");;
      if (Inet6SocketAddress::IsMatchingType (from))
        {
          Inet6SocketAddress address = Inet6SocketAddress::ConvertFrom (from);
          RetrieveInciPacketTags (packet);
          m_recvCountIPv6++;

          NS_LOG_INFO("[ns3][C2CIPApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIPv6<<" from " <<
          address.GetIpv6()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes");
          NS_LOG_INFO("[ns3][C2CIPApp]========= SUCCESS : IPv6 reception on node " << GetNode()->GetId() <<"  ==============");
          NS_LOG_INFO("[ns3][C2CIPApp]********* Receiveing node's position: " <<m_node->GetObject<MobilityModel>()->GetPosition()<<" *********");
        }
    }
}

void 
C2CIPApp::SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIP.IsRunning ())
  {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********");
  }
  if(messRegenerationTime< 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
  {
      DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
  }
  else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
  {
      double now = (Simulator::Now()).GetSeconds();
      double lifetime =  now - startTime;
      if( lifetime <= messRegenerationTime)
      {
          DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
      }
      else
      {
          messRegenerationTime= -1;  // set to default value
      }
  }
}

void 
C2CIPApp::SendPacketIPv6 (Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIPv6.IsRunning ())
  {
      Simulator::Cancel (m_sendEventIPv6);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********");
  }
  if(messRegenerationTime< 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
  {
      DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
  }
  else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
  {
      double now = (Simulator::Now()).GetSeconds();
      double lifetime =  now - startTime;
      if( lifetime <= messRegenerationTime)
      {
          DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
      }
      else
      {
          messRegenerationTime= -1;  // set to default value
      }
  }
}

void 
C2CIPApp::SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  if (m_sendEventC2C.IsRunning ())
  {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current C2C periodic transmission " << GetNode()->GetId() <<" *********");;
  }
  if(messRegenerationTime < 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
  {
      DoSendC2C(address,messRegenerationTime,startTime);
  }
  else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
  {
      double now = (Simulator::Now()).GetSeconds();
      double lifetime =  now - startTime;
      if( lifetime <= messRegenerationTime)
      {
        DoSendC2C(address,messRegenerationTime,startTime);
      }
      else
      {
          messRegenerationTime= -1;  // set to default value
      }
  }
}

void 
C2CIPApp::ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{

  //Remove the rebroadcast of messages
  // Deprecated method - explicite calls to recurring TX for better periodicity adjustment
  //JHNOTE (18/03/2018) - to check why it has been turned off
  return;

  NS_LOG_FUNCTION_NOARGS ();
  if (m_runningC2C)
  {
       m_dataRate = (m_packetSize * 8) * m_frequency;
       Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
       NS_LOG_INFO("ScheduleTxC2C next on time "<<tNext);
       m_sendEventC2C = Simulator::Schedule (tNext, &C2CIPApp::SendPacketC2C, this,address,messRegenerationTime,startTime);
  }
  else
  {
      NS_LOG_INFO("[ns3][C2CIPApp]***************  STOP C2C transmission ON NODE  " << GetNode()->GetId() <<"**************");
  }
}

void 
C2CIPApp::ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIP)
  {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEventIP = Simulator::Schedule (tNext, &C2CIPApp::SendPacketIP, this,address,destinationAddress,messRegenerationTime,startTime);
  }
  else
  {
      NS_LOG_INFO("[ns3][C2CIPApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********");
  }
}

void 
C2CIPApp::ScheduleTxIP (Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIPv6)
  {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEventIPv6 = Simulator::Schedule (tNext, &C2CIPApp::SendPacketIPv6, this,address,destinationAddress,messRegenerationTime,startTime);
  }
  else
  {
      NS_LOG_INFO("[ns3][C2CIPApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********");
  }
}


void 
C2CIPApp::SetMessRegenerationTime (double MessRegenerationTime)
{
  m_MessRegenerationTime=MessRegenerationTime;
}

void 
C2CIPApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
C2CIPApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
C2CIPApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}

void 
C2CIPApp::SetIPAddress(Ipv4Address address)
{
  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
}

void 
C2CIPApp::SetIPAddress(Ipv6Address address)
{
  m_IPv6Address = address;
  Address destinationaddress = Inet6SocketAddress(m_IPv6Address, m_portIPv6);
  m_destinationaddress = destinationaddress;
}

void 
C2CIPApp::SetC2CAddress(Ptr<c2cAddress> address)
{
  m_c2cAddress = address;
}

void
C2CIPApp::SetChTag(ChannelTag ch_tag)
{
  m_channeltag = ch_tag;
}

} // Namespace ns3
