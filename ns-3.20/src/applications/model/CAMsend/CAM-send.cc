/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

/****************************************************************************************
 *                          CAMSend.cc
 * This is a DEPRECATED METHOD. Please use CAMManage.cc instead
 *
***************************************************************************************/

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
#include "ns3/trace-source-accessor.h"
#include "CAM-send.h"
#include "ns3/c2cl4T-socket-factory.h" // Modified by Ramon Bauza
#include "ns3/channel-tag.h"


NS_LOG_COMPONENT_DEFINE ("CAMsend"); // Modified by Ramon Bauza

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CAMsend);

TypeId
CAMsend::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CAMsend")
    .SetParent<Application> ()
    .AddConstructor<CAMsend> ()
    .AddAttribute ("Port", "port for CAM Send",
				   UintegerValue (0),
				   MakeUintegerAccessor (&CAMsend::m_port),
				   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Frequency", "The frequency in on state.",
		           UintegerValue (0),
		           MakeUintegerAccessor  (&CAMsend::m_frequency),
		           MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&CAMsend::m_pktSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Protocol", "The type of protocol to use.",
                   TypeIdValue (c2cl4TSocketFactory::GetTypeId ()), // Modified by Ramon Bauza
                   MakeTypeIdAccessor (&CAMsend::m_tid),
                   MakeTypeIdChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                   MakeTraceSourceAccessor (&CAMsend::m_txTrace))
    ;
  return tid;
}


CAMsend::CAMsend ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_socket = 0;
  m_port = 0;
  m_frequency = 0;
  m_sendCount = 0;
}

CAMsend::~CAMsend()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CAMsend::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_socket = 0;
  Application::DoDispose ();
}

// Application Methods
void CAMsend::StartApplication() // Called at time specified by Start
{
  NS_LOG_FUNCTION_NOARGS ();

  // Create the socket if not already
  if (!m_socket)
    {
	  //---------------- check iTETRIS ----------------------------
	 //std::cout<<std::endl;
	 //std::cout<<std::endl;
	  //sleep(1);
	 //std::cout<<"*********************** INSTALL CAMsend APPLICATION ON NODE *****************************"<<std::endl;
	 //std::cout<<"CAMsend      : Creating c2c Socket of Type = "<<m_tid<<" | Binds at PORT number = "<<m_port<<std::endl;
	  //---------------- check iTETRIS ----------------------------

	  m_socket = Socketc2c::CreateSocket (GetNode(), m_tid);
      m_socket->Bind (m_port);
    }
  SendPacket ();
}

void CAMsend::StopApplication() // Called at time specified by Stop
{
  NS_LOG_FUNCTION_NOARGS ();

  //CancelEvents ();
  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }
  if(m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN("CAMsend found null socket to close in StopApplication");
    }
}


// Private helpers
void CAMsend::ScheduleNextTx()
{
  NS_LOG_FUNCTION_NOARGS ();

  uint32_t bits = m_pktSize * 8 ;
  m_dataRate = bits * m_frequency;
  Time tNext (Seconds (bits / static_cast<double> (m_dataRate.GetBitRate ())));
  m_sendEvent = Simulator::Schedule (tNext, &CAMsend::SendPacket, this);
}

void CAMsend::SendPacket()
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("sending packet at " << Simulator::Now());
  NS_ASSERT (m_sendEvent.IsExpired ());
  Ptr<Packet> packet = Create<Packet> (m_pktSize);

  m_sendCount ++;

  //---------------- check iTETRIS ----------------------------
 //std::cout<<std::endl;
 //std::cout<<std::endl;
 //std::cout<<"=================================== START : APP SEND ================================"<<std::endl;
 //std::cout<<"CAMsend      : SENDING CAM no. "<<m_sendCount<<" at "<<Simulator::Now ().GetSeconds ()<<				  " seconds | CAM size = "<<packet->GetSize()<<" Bytes"<<std::endl;
  //---------------- check iTETRIS ----------------------------

  m_txTrace (packet);
  m_socket->Send (packet);
  ScheduleNextTx();
}



} // Namespace ns3
