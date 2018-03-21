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
 *                          C2C-IP.cc
 * This is the iTETRIS C-ITS DEN Service sending DENM on the C2C stack
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
#include "ns3/socketc2c.h"
#include "ns3/simulator.h"
#include "ns3/socket-factoryc2c.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/random-variable.h" 
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/c2cl4T-socket-factory.h"
#include "ns3/address-utils.h"
#include "ns3/iTETRIS-Application.h"  

#include "DENM-app.h"


NS_LOG_COMPONENT_DEFINE ("DENMApp");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DENMApp);

TypeId
DENMApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DENMApp")
    .SetParent<Application> ()
    .AddConstructor<DENMApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&DENMApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("MsgLifeTime", "The time until when this message has to be forwarded.",
    		   UintegerValue (0),
    		   MakeUintegerAccessor  (&DENMApp::m_MsgLifeTime),
    		   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&DENMApp::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Port", "The port for c2c messages",     
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&DENMApp::m_portC2C),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SocketFactorytype1", "The type of the socket factory 1 to use.",   
               TypeIdValue (),
               MakeTypeIdAccessor (&DENMApp::m_C2CSocketfactory),
               MakeTypeIdChecker ())
    ;
  return tid;
}

DENMApp::DENMApp()
{
  m_C2Csocket = 0;
  m_portC2C = 0;
  m_packetSize = 0;
  m_dataRate = 0;
  m_frequency = 0;
  m_runningC2C = false;
  m_packetsSentC2C = 0;
  m_sendCountC2C = 0;
  m_recvCountC2C = 0;
  m_MessRegenerationTime = -1;
  m_MsgLifeTime=0;
  m_StartTime =0;
  m_rndOffset = UniformVariable(0.0,0.999); 
}

DENMApp::~DENMApp()
{
}

void
DENMApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_C2Csocket = 0;
  Application::DoDispose ();
}

void
DENMApp::StartApplication (void)
{
}

void
DENMApp::SetSockets (void)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][DENMApp]******************************* set sockets on node ****************************\n");

  if (!m_C2Csocket)
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][DENMApp] Creating c2c Socket of Type = "<<m_C2CSocketfactory<<" | Binds at PORT number = "<<m_portC2C<<"\n");

      m_C2Csocket = Socketc2c::CreateSocket (GetNode(), m_C2CSocketfactory);
      m_C2Csocket->Bind (m_portC2C);
      m_C2Csocket->SetRecvCallback(MakeCallback (&DENMApp::ReceiveC2C, this));
      NS_LOG_INFO("[ns3][DENMApp]*********** C2C socket created and binded ************\n");
    }
  else
    {
      NS_FATAL_ERROR ("[ns3][DENMApp] creation attempt of a c2c socket for DENMApp that has already a c2c socket active");
      return;
    }
}

void 
DENMApp::StopApplication(void)
{
}

void 
DENMApp::StopTransmitting(void)
{
  NS_LOG_INFO("[ns3][DENMApp] ********STOP DENM APP on node " << GetNode()->GetId() <<" ******\n");
  m_runningC2C = false;

  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][DENMApp] ********STOP C2C transmission on node " << GetNode()->GetId() <<"  while running ******\n");
    }
  if (m_C2Csocket)
    {
      m_C2Csocket->Close ();
    }
}


void 
DENMApp::StartTransmitting(Ptr<c2cAddress> address)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningC2C = true;
  m_c2cAddress = address;

  m_StartTime = (Simulator::Now()).GetSeconds ();

  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][DENMApp]****** STOP current c2c periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" **********\n");
    }
  InitializeINCIvariables();
  DoSendC2C(address,m_MessRegenerationTime ,m_StartTime);
}

void
DENMApp::DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  AddInciPacketTags(packet);
  m_sendCountC2C ++;
  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][DENMApp] SENDING C2C packet no. "<<m_sendCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"\n");

   m_C2Csocket->DoSendTo(packet, m_c2cAddress, m_portC2C,m_MsgLifeTime,1,m_sendCountC2C);
   ScheduleTxC2C (address,messRegenerationTime,startTime);
}

void
DENMApp::ReceiveC2C (Ptr<Socketc2c> socketc2c)
{
  Ptr<Packet> packet;
  NS_LOG_INFO("[ns3][DENMApp] Start Receiving - Call Socketc2c -> Recv()\n");
  packet=socketc2c->Recv();
      
  RetrieveInciPacketTags (packet);
  m_recvCountC2C++;

  NS_LOG_INFO("[ns3][DENMApp] SUCCESS: Receiving c2c message no. "<<m_recvCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<< " seconds | c2c message size  = "<<packet->GetSize()<<" Bytes\n");
  NS_LOG_INFO("[ns3][DENMApp]========= SUCCESS : C2C Reception on node " << GetNode()->GetId() <<"  =============\n");
}


void 
DENMApp::SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][DENMApp]********* STOP current C2C periodic transmission " << GetNode()->GetId() <<" *********\n");
    }
  if(messRegenerationTime < 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
    {
      DoSendC2C(address,messRegenerationTime,startTime);
    }
    else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
      {
        double now = (Simulator::Now()).GetSeconds();
        double lifetime =  now - startTime;
        if( lifetime <= messRegenerationTime )
          {
            DoSendC2C(address,messRegenerationTime,startTime);
          }
        else
         {
           messRegenerationTime = -1;  // set to default value
         } 
      } 
}

void 
DENMApp::ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  if (m_runningC2C)
    {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEventC2C = Simulator::Schedule (tNext, &DENMApp::SendPacketC2C, this,address,messRegenerationTime,startTime);
    }
  else
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][DENMApp]***************  STOP C2C transmission ON NODE  " << GetNode()->GetId() <<"**************\n");
    }
}


void 
DENMApp::SetMessRegenerationTime (double MessRegenerationTime)
{
  m_MessRegenerationTime=MessRegenerationTime;
}

void 
DENMApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
DENMApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
DENMApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
} 

void 
DENMApp::SetC2CAddress(Ptr<c2cAddress> address)
{
  m_c2cAddress = address;
}

void
DENMApp::SetChTag(ChannelTag ch_tag)
{
  m_channeltag = ch_tag;
}

} // Namespace ns3
