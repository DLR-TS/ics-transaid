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
 * Author:  Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es> 
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
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
#include "wimax-app.h"
#include "ns3/vehicle-sta-mgnt.h" 
#include "ns3/wimax-bs-mgnt.h" 
#include "ns3/ss-net-device.h" 
#include "ns3/bs-net-device.h" 
#include "ns3/ss-command-manager.h" 
#include "ns3/bs-command-manager.h" 
#include "ns3/app-index-tag.h"
#include "ns3/storage.h"
#include "ns3/message-id-tag.h"

NS_LOG_COMPONENT_DEFINE ("WimaxApp");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WimaxApp);

TypeId
WimaxApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WimaxApp")
    .SetParent<Application> ()
    .AddConstructor<WimaxApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&WimaxApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&WimaxApp::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("PortIP", "The port of the ip messages",    
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&WimaxApp::m_portIP),     
    		   MakeUintegerChecker<uint16_t> ())
    ;
  return tid;
}

WimaxApp::WimaxApp()
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
}

WimaxApp::~WimaxApp()
{
}

void
WimaxApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_IPv4socket = 0;
  Application::DoDispose ();
}

void
WimaxApp::StartApplication (void)
{
}

void
WimaxApp::SetSockets (void)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][WimaxApp]******************************* set sockets on node ****************************\n");

  if (!m_IPv4socket)
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][WimaxApp] Creating IP Socket of Type = UdpSocketFactory | Binds at PORT number = "<<m_portIP<<"\n");
      m_IPv4socket = Socket::CreateSocket (GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portIP);
      m_IPv4socket->Bind (local);
      m_IPv4socket->SetRecvCallback (MakeCallback(&WimaxApp::ReceiveIP, this));
      NS_LOG_INFO("[ns3][WimaxApp]********* IP socket created and binded ***********\n");    
    }
  else
    {
      NS_FATAL_ERROR ("[ns3][WimaxApp] creation attempt of an ip socket for WimaxApp that has already an ip socket active");
      return;
    }
}


void 
WimaxApp::StopApplication(void)
{
}

void 
WimaxApp::StopTransmitting(void)
{
  NS_LOG_INFO("[ns3][WimaxApp] ********STOP Wimax APP on node " << GetNode()->GetId() <<" ******\n");
  m_runningIP = false;
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][WimaxApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********\n");;
    }
  if (m_IPv4socket)
    {
      m_IPv4socket->Close ();
    }
}

void 
WimaxApp::StartTransmitting (Ipv4Address address )
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
      NS_LOG_INFO("[ns3][WimaxApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********\n");
    }
  InitializeINCIvariablesWiMAX();
  DoSendIP (address,m_destinationaddress,m_MessRegenerationTime,m_StartTime);
}

void
WimaxApp::DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
 
  Ptr<Node> local = GetNode();
  if (local->IsMobileNode ())
    {
      DoSendIPFromVehicle (address, destinationAddress, messRegenerationTime, startTime);
    }
  else
    {
      DoSendIPFromBs (address, destinationAddress, messRegenerationTime, startTime);
    }
}

void
WimaxApp::DoSendIPFromVehicle (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
   NS_LOG_INFO("\n");
   NS_LOG_INFO("[ns3][WimaxApp] ====== vehicle " << GetNode()->GetId() <<": connection attempt to address " << address<<"   =====\n");

  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  if (PrepareWimaxConnection ())
    {

      m_IPv4socket->Connect (destinationAddress);
      //AddInciPacketTags (packet);
      AddInciPacketTagsWimax (packet);
      m_sendCountIP ++;
      
      NS_LOG_INFO("\n");  
      NS_LOG_INFO("[ns3][DoSendIPFromVehicle] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"\n");

      m_IPv4socket->Send (packet);
      InitiateBandwidthRequest ();
    }
}

void
WimaxApp::DoSendIPFromBs (Ipv4Address address, Address destinationAddress,double messRegenerationTime,double startTime)
{
   NS_LOG_INFO("\n");
   NS_LOG_INFO("[ns3][WimaxApp] ======  " << GetNode()->GetId() <<": connection attempt to address " << address<<"   =====\n");
 
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  Ptr<Node> node = GetNode ();
  Ptr<WimaxBsMgnt> staMgnt = node->GetObject<WimaxBsMgnt> ();
  if (*staMgnt->GetIpBroadcastAddress () == address)
    {
      Ptr<BaseStationNetDevice> bsNetDevice = DynamicCast<BaseStationNetDevice> (staMgnt->GetNetDevice ());
      Ptr<BsCommandManager> bsCommMgnr = bsNetDevice->GetObject<BsCommandManager> ();
      bsCommMgnr->TriggerVehiclesScanning ();
      m_IPv4socket->Connect (destinationAddress);
     // AddInciPacketTags (packet);
      AddInciPacketTagsWimax (packet);
      m_sendCountIP ++;
      NS_LOG_INFO("\n");  
      NS_LOG_INFO("[ns3][WimaxApp::DoSendIPFromBs] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"\n");
      m_IPv4socket->Send (packet);
      bsCommMgnr->PrepareBroadcastTxon ();
    }
  else if (PrepareWimaxConnection ())
    {
      m_IPv4socket->Connect (destinationAddress);
    //  AddInciPacketTags (packet);
      AddInciPacketTagsWimax (packet);
      m_sendCountIP ++;
      
      NS_LOG_INFO("\n");  
      NS_LOG_INFO("[ns3][WimaxApp::DoSendIPFromBs] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"\n");

      m_IPv4socket->Send (packet);
      ActivateWimaxCommandMgnrs ();
    }
}

void
WimaxApp::AddInciPacketTagsWimax (Ptr<Packet> p)
{
  // Sender ID tag
  Ptr<Node> local = GetNode();
  uint32_t nodeID = local->GetId();

  NodeIdTag nodeTag;
  nodeTag.Set(nodeID);

  // Application Index tag
  AppIndexTag appindexTag;
  appindexTag.Set(m_app_index);
  
  MessageIdTag messageIdTag;
  messageIdTag.Set(m_messageId);
  p->AddPacketTag(messageIdTag);

   // update of timestep and time step sequence number
  m_currentTimeStepWiMAX = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  if(m_currentTimeStepWiMAX>m_previousTimeStepWiMAX)
    {
      m_stepSequenceNumberWiMAX=0;
      m_previousTimeStepWiMAX=m_currentTimeStepWiMAX;
    }
  else
    {
      m_stepSequenceNumberWiMAX++;  
    }
 
  // time step tag
  TimeStepTag timeStepTag;
  timeStepTag.Set(m_currentTimeStepWiMAX);
  
  // time step sequence number tag
  TStepSequenceNumberTag TSSeqNTag;
  TSSeqNTag.Set(m_stepSequenceNumberWiMAX);
  
  NS_LOG_INFO("\n");

  if (local->IsMobileNode ())
    {
      Ptr<VehicleStaMgnt> staMgnt = local->GetObject<VehicleStaMgnt> ();
      Ptr<SubscriberStationNetDevice> ssNetDevice = DynamicCast<SubscriberStationNetDevice> (staMgnt->GetIpNetDevice ("Wimax"));
      Ptr<SsCommandManager> ssCommMgnr = ssNetDevice->GetObject<SsCommandManager> ();
      SsCommandManager::PacketTags* packetTags = new SsCommandManager::PacketTags;
      packetTags->nodeTag = nodeTag;
      packetTags->appIndexTag = appindexTag;
      packetTags->timeStepTag = timeStepTag;
      packetTags->tSSeqNTag = TSSeqNTag;
      ssCommMgnr->AddPacketTags (m_IPAddress, packetTags);
      NS_LOG_INFO("[ns3][WimaxApp::AddInciPacketTagsWimax on WiMAX Vehicle] service =  "<<m_servicetype<<"         app index =  "<<m_app_index<<"          sender nodeID = "<<packetTags->nodeTag.Get ()<<"      timestep = "<<packetTags->timeStepTag.Get ()<<"        timestepSeqNo = "<<packetTags->tSSeqNTag.Get ()<<" address set= "<<m_IPAddress<<"\n");
    }
  else
    {
      Ptr<WimaxBsMgnt> staMgnt = local->GetObject<WimaxBsMgnt> ();
      Ptr<BaseStationNetDevice> bsNetDevice = DynamicCast<BaseStationNetDevice> (staMgnt->GetNetDevice ());
      Ptr<BsCommandManager> bsCommMgnr = bsNetDevice->GetObject<BsCommandManager> ();
      BsCommandManager::PacketTags* packetTags = new BsCommandManager::PacketTags;
      packetTags->nodeTag = nodeTag;
      packetTags->appIndexTag = appindexTag;
      packetTags->timeStepTag = timeStepTag;
      packetTags->tSSeqNTag = TSSeqNTag;
      bsCommMgnr->AddPacketTags (m_IPAddress, packetTags);
      NS_LOG_INFO("[ns3][WimaxApp::AddInciPacketTagsWimax on WiMAX BS] service =  "<<m_servicetype<<"         app index =  "<<m_app_index<<"          sender nodeID = "<<packetTags->nodeTag.Get ()<<"      timestep = "<<packetTags->timeStepTag.Get ()<<"        timestepSeqNo = "<<packetTags->tSSeqNTag.Get ()<<" address set= "<<m_IPAddress<<"\n");

    }

}

void
WimaxApp::RetrieveInciPacketTagsWimax (Ptr<Packet> packet, Ipv4Address ipAddress)
{
  Ptr<Node> local = GetNode();

  MessageIdTag msgTag;
  packet->PeekPacketTag(msgTag);

  if (local->IsMobileNode ())
      {
	Ptr<VehicleStaMgnt> staMgnt = local->GetObject<VehicleStaMgnt> ();
	Ptr<SubscriberStationNetDevice> ssNetDevice = DynamicCast<SubscriberStationNetDevice> (staMgnt->GetIpNetDevice ("Wimax"));
	Ptr<SsCommandManager> ssCommMgnr = ssNetDevice->GetObject<SsCommandManager> ();
	Ptr<BsCommandManager> bsCommMgnr = ssCommMgnr->GetRegisteredBsManager ();
        BsCommandManager::PacketTags* packetTags = NULL;
	
	packetTags = bsCommMgnr->GetPacketTags (*ssCommMgnr-> GetVehicleIpAddress ());

	if (packetTags)
	  {
	    uint32_t senderId = packetTags->nodeTag.Get ();
	    uint32_t timeStep = packetTags->timeStepTag.Get ();
	    uint32_t timeStepSN = packetTags->tSSeqNTag.Get ();
	    //TODO check to see if RSSI and SNR may also be integrated
	    tcpip::Storage *genericTagContainer = new tcpip::Storage(); // alternative if Ptr does not work here


	    NS_LOG_INFO("\n");
	    NS_LOG_INFO("[ns3][WimaxApp::RetrieveInciPacketTagsWimax on Vehicle for unicast tx] RECEPTION: service =  "<<m_servicetype<<"     sender nodeID = "<<senderId<<"      timestep = "<<timeStep<<"        timestepSeqNo = "<<timeStepSN <<"\n");
	  
	    m_forwardIcs (senderId, m_servicetype, timeStep, timeStepSN, msgTag.Get(), genericTagContainer);
	  }
	  else
	  {
	      Ptr<Node> servingBS = bsCommMgnr->GetBs()->GetNode();
              Ptr<WimaxBsMgnt> BSstaMgnt = servingBS->GetObject<WimaxBsMgnt> ();
	      
	      packetTags = bsCommMgnr->GetPacketTags (*BSstaMgnt->GetIpBroadcastAddress ());
	      
	      if(packetTags)
	      {
	      	    uint32_t senderId = packetTags->nodeTag.Get ();
	            uint32_t timeStep = packetTags->timeStepTag.Get ();
	            uint32_t timeStepSN = packetTags->tSSeqNTag.Get ();
	            //TODO check to see if RSSI and SNR may also be integrated
	            tcpip::Storage *genericTagContainer = new tcpip::Storage(); // alternative if Ptr does not work here


	            NS_LOG_INFO("\n");
	            NS_LOG_INFO("[ns3][WimaxApp::RetrieveInciPacketTagsWimax on Vehicle for broadcast tx] RECEPTION: service =  "<<m_servicetype<<"     sender nodeID = "<<senderId<<"      timestep = "<<timeStep<<"        timestepSeqNo = "<<timeStepSN <<"\n");
	  
	            m_forwardIcs (senderId, m_servicetype, timeStep, timeStepSN, msgTag.Get(), genericTagContainer);
	      }
	      else
	      {
	          NS_LOG_INFO("[ns3][WimaxApp::RetrieveInciPacketTagsWimax on Vehicle]  no packet tags");
	      }
	  }
      }
    else
      {
	Ptr<WimaxBsMgnt> staMgnt = local->GetObject<WimaxBsMgnt> ();
	Ptr<BaseStationNetDevice> bsNetDevice = DynamicCast<BaseStationNetDevice> (staMgnt->GetNetDevice ());
	Ptr<BsCommandManager> bsCommMgnr = bsNetDevice->GetObject<BsCommandManager> ();
	Ptr<SsCommandManager> ssCommMgnr = bsCommMgnr->GetSsManagerByIpAddress (ipAddress);
	SsCommandManager::PacketTags* packetTags = NULL;
	packetTags = ssCommMgnr->GetPacketTags (*bsCommMgnr->GetBsIpAddress ());
	if (packetTags)
	  {
	    uint32_t senderId = packetTags->nodeTag.Get ();
	    uint32_t timeStep = packetTags->timeStepTag.Get ();
	    uint32_t timeStepSN = packetTags->tSSeqNTag.Get ();
	    //TODO check to see if RSSI and SNR may also be integrated
        tcpip::Storage *genericTagContainer = new tcpip::Storage(); // alternative if Ptr does not work here


	    NS_LOG_INFO("\n");
	    NS_LOG_INFO("[ns3][WimaxApp::RetrieveInciPacketTagsWimax on BS] RECEPTION: service =  "<<m_servicetype<<"     sender nodeID = "<<senderId<<"      timestep = "<<timeStep<<"        timestepSeqNo = "<<timeStepSN <<"\n");
	  
	    m_forwardIcs (senderId, m_servicetype, timeStep, timeStepSN, msgTag.Get(), genericTagContainer);
	  }
	   else
	  {
	      NS_LOG_INFO("[ns3][WimaxApp::RetrieveInciPacketTagsWimax on BS]::  no packet tags");
	  }
      }
}

bool
WimaxApp::PrepareWimaxConnection (void)
{
  bool result = false;
  Ptr<Node> node = GetNode ();
  if (node->IsMobileNode ())
    {
      Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
      Ptr<SubscriberStationNetDevice> ssNetDevice = DynamicCast<SubscriberStationNetDevice> (staMgnt->GetIpNetDevice ("Wimax"));
      Ptr<SsCommandManager> ssCommMgnr = ssNetDevice->GetObject<SsCommandManager> ();
      ssCommMgnr->PrepareConnection (m_IPAddress);
      result = true;
    }
  else
    {
      Ptr<WimaxBsMgnt> staMgnt = node->GetObject<WimaxBsMgnt> ();
      Ptr<BaseStationNetDevice> bsNetDevice = DynamicCast<BaseStationNetDevice> (staMgnt->GetNetDevice ());
      Ptr<BsCommandManager> bsCommMgnr = bsNetDevice->GetObject<BsCommandManager> ();
      result = bsCommMgnr->PrepareConnection (m_IPAddress);
    }
  return result;
}

void
WimaxApp::InitiateBandwidthRequest (void)
{
  Ptr<Node> node = GetNode ();
  if (node->IsMobileNode ())
    {
      Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
      Ptr<SubscriberStationNetDevice> ssNetDevice = DynamicCast<SubscriberStationNetDevice> (staMgnt->GetIpNetDevice ("Wimax"));
      Ptr<SsCommandManager> ssCommMgnr = ssNetDevice->GetObject<SsCommandManager> ();
      ssCommMgnr->InitiateBandwidthRequest ();
    }
}

void
WimaxApp::ActivateWimaxCommandMgnrs (void)
{
  Ptr<Node> node = GetNode ();
  if (!node->IsMobileNode ())
    {
      Ptr<WimaxBsMgnt> staMgnt = node->GetObject<WimaxBsMgnt> ();
      Ptr<BaseStationNetDevice> bsNetDevice = DynamicCast<BaseStationNetDevice> (staMgnt->GetNetDevice ());
      Ptr<BsCommandManager> bsCommMgnr = bsNetDevice->GetObject<BsCommandManager> ();
      bsCommMgnr->ActivateNodes (m_IPAddress);
    }
}

void 
WimaxApp::ReceiveIP (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][WimaxApp] Start Receiving - Call SocketIP -> RecvFrom()\n");;
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
          Ipv4Address ipAddress = address.GetIpv4();
	  //RetrieveInciPacketTags (packet);
	  RetrieveInciPacketTagsWimax (packet, ipAddress);
          m_recvCountIP++;
          NS_LOG_INFO("[ns3][WimaxApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIP<<" from " <<
          address.GetIpv4()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes\n");
          NS_LOG_INFO("[ns3][WimaxApp]========= SUCCESS : IP reception on node " << GetNode()->GetId() <<"  ==============\n");
        }
    }
}

void 
WimaxApp::SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][WimaxApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********\n");
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
WimaxApp::ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIP)
    {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEventIP = Simulator::Schedule (tNext, &WimaxApp::SendPacketIP, this,address,destinationAddress,messRegenerationTime,startTime);
    }
  else
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][WimaxApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********\n");
    }
}


void 
WimaxApp::SetMessRegenerationTime (double MessRegenerationTime)
{
  m_MessRegenerationTime=MessRegenerationTime;
}

void 
WimaxApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
WimaxApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
WimaxApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}

void 
WimaxApp::SetIPAddress(Ipv4Address address)
{
  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
}

void
WimaxApp::InitializeINCIvariablesWiMAX(void)
{
  m_previousTimeStepWiMAX = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  m_currentTimeStepWiMAX =0;
  m_stepSequenceNumberWiMAX=-1;  
}

} // Namespace ns3
