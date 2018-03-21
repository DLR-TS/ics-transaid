/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010,  EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Michele Rondinone <mrondinone@umh.es>, Ramon Bauza <rbauza@umh.es>
 */

/****************************************************************************************
 *                          C2C-IP.cc
 * This is the iTETRIS C-ITS CA Service sending CAM on the C2C stack
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
#include "ns3/iTETRIS-Application.h"
#include "ns3/channel-tag.h"
#include "ns3/c2c-address.h"
#include "ns3/node-id-tag.h" 
#include "ns3/time-step-tag.h" 
#include "ns3/TStep-sequence-number-tag.h"


#include "CAMmanagement.h"

NS_LOG_COMPONENT_DEFINE ("CAMmanage");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CAMmanage);

TypeId
CAMmanage::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CAMmanage")
    .SetParent<Application> ()
    .AddConstructor<CAMmanage> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0.0),
    		   MakeDoubleAccessor  (&CAMmanage::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&CAMmanage::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Port", "The port of the CAM Application",
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&CAMmanage::m_port),
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use.",
               TypeIdValue (c2cl4TSocketFactory::GetTypeId ()),
               MakeTypeIdAccessor (&CAMmanage::m_tid),
               MakeTypeIdChecker ())
    ;
  return tid;
}

CAMmanage::CAMmanage ()
{
  m_socket = 0;
  m_port = 0;
  m_packetSize = 0;
  m_dataRate = 0;
  m_frequency = 0;
  m_running = false;
  m_packetsSent = 0;
  m_sendCount = 0;
  m_recvCount = 0;
  m_currentTimeStep=0;
  m_previousTimeStep=0;
  m_stepSequenceNumber=0;
  m_rndOffset = UniformVariable(0.0,0.999); 
}

CAMmanage::~CAMmanage()
{
}

void
CAMmanage::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_socket = 0;
  Application::DoDispose ();
}


void 
CAMmanage::StartTransmitting (Ptr<c2cAddress> address)            
{
  SetC2CAddress(address);
  m_running = true;
  m_packetsSent = 0;

  InitializeINCIvariables();

  if (!m_socket)
    {
      NS_LOG_INFO("[ns3][CAMmanage]******************************* ready to start transmitting CAMs *****************************\n");
      NS_LOG_INFO("[ns3][CAMmanage] Creating c2c Socket of Type = "<<m_tid<<" | Binds at PORT number = "<<m_port<<"\n");;
      m_socket = Socketc2c::CreateSocket (GetNode(), m_tid);
      m_socket->Bind (m_port);
      m_socket->SetRecvCallback(MakeCallback (&CAMmanage::Receive, this));
    }
  Simulator::Schedule (Seconds (m_rndOffset.GetValue()), &CAMmanage::SendPacket,this); 
}

void
CAMmanage::StartApplication (void)
{
}

void 
CAMmanage::StopTransmitting(void)       
{
  m_running = false;

  NS_LOG_INFO("[ns3][CAMmanage]*******************************  STOP APPLICATION ON NODE ********************************\n"); 

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
CAMmanage::StopApplication(void)
{
}

void
CAMmanage::Receive (Ptr<Socketc2c> socketc2c)
{
  Ptr<Packet> packet;
  NS_LOG_INFO("[ns3][CAMmanage] Start Receiving - Call Socketc2c -> Recv()\n");

  m_recvCount++;

  packet = socketc2c->Recv();

  NS_LOG_INFO("[ns3][CAMmanage]================================== SUCCESS : APP RECV  ===============================\n");

  RetrieveInciPacketTags (packet);

  NS_LOG_INFO("[ns3][CAMmanage] SUCCESS: Receiving CAM no. "<<m_recvCount<<" at "<<Simulator::Now ().GetSeconds ()<<  " seconds | CAM size  = "<<packet->GetSize()<<" Bytes\n");
}


void 
CAMmanage::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  NS_LOG_INFO("[ns3][CAMmanage]=================================== START : APP SEND ================================\n");

  AddInciPacketTags(packet);

  m_sendCount ++;
  NS_LOG_INFO("[ns3][CAMmanage] SENDING CAM no. "<<m_sendCount<<" at "<<Simulator::Now ().GetSeconds ()<< " seconds | CAM size = "<<packet->GetSize()<<"\n");
  m_socket->DoSendTo(packet, m_c2cAddress, m_port,1,1,m_sendCount);

  ScheduleTx ();
}

void 
CAMmanage::ScheduleTx (void)
{
  if (m_running)
    {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &CAMmanage::SendPacket, this);
    }
}

void 
CAMmanage::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
CAMmanage::SetFrequency (double frequency)
{
  m_frequency = frequency;
}

void 
CAMmanage::SetC2CAddress(Ptr<c2cAddress> address)
{
  m_c2cAddress = address;
}

void
CAMmanage::SetChTag(ChannelTag ch_tag)
{
  m_channeltag = ch_tag;
}

} // Namespace ns3
