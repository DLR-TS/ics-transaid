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
 *                          CAMReceive.cc
 * This is a DEPRECATED METHOD. Please use CAMManage.cc instead
 *
***************************************************************************************/


#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/socketc2c.h"
#include "ns3/simulator.h"
#include "ns3/socket-factoryc2c.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "CAM-recv.h"
#include "ns3/uinteger.h"
#include "ns3/c2cl4T-socket-factory.h" // Modified by Ramon Bauza

NS_LOG_COMPONENT_DEFINE ("CAMrecv"); // Modified by Ramon Bauza

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CAMrecv);

TypeId 
CAMrecv::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CAMrecv")
    .SetParent<Application> ()
    .AddConstructor<CAMrecv> ()
	.AddAttribute ("Local", "The NodeID to bind to",
			UintegerValue (1),
			MakeUintegerAccessor (&CAMrecv::m_local),
			MakeUintegerChecker<uint64_t> ())
	.AddAttribute ("Port", "port for CAM Recv",
			UintegerValue (0),
			MakeUintegerAccessor (&CAMrecv::m_port),
			MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Protocol", "The type id of the protocol to use for the rx socket.",
            TypeIdValue (c2cl4TSocketFactory::GetTypeId ()), // Modified by Ramon Bauza
            MakeTypeIdAccessor (&CAMrecv::m_tid),
            MakeTypeIdChecker ())
    .AddTraceSource ("Rx", "A packet has been received",
            MakeTraceSourceAccessor (&CAMrecv::m_rxTrace))
    ;
  return tid;
}

CAMrecv::CAMrecv ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_recvCount = 0;
}

CAMrecv::~CAMrecv()
{
  NS_LOG_FUNCTION (this);
}

void CAMrecv::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void CAMrecv::StartApplication()    // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socketc2c::CreateSocket (GetNode(), m_tid);
      m_socket->Bind(m_port);
    }

  m_socket->SetRecvCallback (MakeCallback(&CAMrecv::HandleRead, this));
}

void CAMrecv::StopApplication()     // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
  while(!m_socketList.empty())
  {
    Ptr<Socketc2c> acceptedSocket = m_socketList.front();
    m_socketList.pop_front();
    acceptedSocket->Close();
  }
  if (m_socket) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socketc2c> > ());
    }
  //---------------- check iTETRIS ----------------------------
 //std::cout<<std::endl;
 //std::cout<<"*******************************     STOP CAM-Recv APP    *********************************"<<std::endl;
 //std::cout<<std::endl;
  //---------------- check iTETRIS ----------------------------

}

void CAMrecv::HandleRead (Ptr<Socketc2c> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  packet=socket->Recv();
  m_recvCount++;

  //---------------- check iTETRIS ----------------------------
 //std::cout<<"CAM-recv APP : SUCCESS: Receiving CAM no. "<<m_recvCount<<" at "<<Simulator::Now ().GetSeconds ()<<				  " seconds | CAM size     = "<<packet->GetSize()<<" Bytes"<<std::endl;
 //std::cout<<"================================== SUCCESS : APP RECV  ==============================="<<std::endl;
  //sleep(1);
 //std::cout<<std::endl;
 //std::cout<<std::endl;
  //---------------- check iTETRIS ----------------------------
}

} // Namespace ns3
