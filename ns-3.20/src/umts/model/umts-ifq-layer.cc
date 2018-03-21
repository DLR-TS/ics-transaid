/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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


#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "umts-queue.h"
#include "controlpacket.h"
#include "umts-manager.h"
#include "umts-rlc-layer.h"
#include "umts-ifq-layer.h"
#include "umts-tags.h"
#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("UmtsIfqLayer");

namespace ns3 {

TypeId
UmtsIfqLayer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsIfqLayer")
  .SetParent<Object> ()
    .AddConstructor<UmtsIfqLayer> ();
  return tid;
}

UmtsIfqLayer::UmtsIfqLayer ()
{
  m_ifqQueue = Create<UMTSQueue> ();
    
  m_time = 0.01;
  
    for (int i = 0; i < MAX_NUM_UE; i++)
    {
      users_[i].bytes_per_tti_ = 0;
      users_[i].dest_=(uint32_t)-1;
      users_[i].len_ = 0;
    }
    
    for (int i = 0; i < MAX_NUM_FLOWS; i++)
    {
      ifqc[i].flowid_ = 0;
      ifqc[i].bytes_per_tti_ = 0;
      ifqc[i].len = 0;
    }

}

void
UmtsIfqLayer::StartUmtsIfqLayer ()
{
  if(m_nodeType=="NodeUE")
  {
     m_ifqTimer = Simulator::ScheduleNow (&UmtsIfqLayer::IfqHandlerNodeUE,this);
  }
  else
  {
     
     m_ifqTimer= Simulator::ScheduleNow (&UmtsIfqLayer::IfqHandlerNodeB,this);
  }
  
}

void
UmtsIfqLayer::SetTTI (double value)
{

  m_time = value;
}

double
UmtsIfqLayer::GetTTI ()
{
  return m_time;
}

void 
UmtsIfqLayer::SetNodeType(std::string nodeType)
{
   m_nodeType=nodeType;
}

std::string 
UmtsIfqLayer::GetNodeType()
{
 return m_nodeType; 
}


void
UmtsIfqLayer::ReceivePacketFromRRC (Ptr<Packet> packet)
{
   //std::cout<<"UMTS-IFQ"<<std::endl;
    if (m_ifqQueue->GetSize () < m_ifqQueue->GetQueueLimit ())    // check if the queue is full
      {	
	m_ifqQueue->Enqueue(packet); //Enqueue the Packet
      }

  return;

}

void
UmtsIfqLayer::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
}

uint32_t
UmtsIfqLayer::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

void
UmtsIfqLayer::SetRLCLayer (Ptr<Object> rlcLayer_)
{
      m_rlc= DynamicCast<UmtsRlcLayer>(rlcLayer_);
 
}

// register and de-register for a UE
void
UmtsIfqLayer::RegistrationNodeB (uint32_t dir, double l)
{
  int id, len;  
  id = Find (dir);
  if (l == 0)         // send upwards all the packets stored in the queue belonging to the UE
    {      
      len=m_ifqQueue->GetSize();
      
      while (len > 0)
        {

	  Ptr<Packet> packet=m_ifqQueue->Dequeue();	  
	  UmtsReceiverIdTag receiverTag;
	  packet->PeekPacketTag(receiverTag);
	  
          if (receiverTag.Get() != users_[id].dest_)
            {
              m_ifqQueue->Enqueue(packet);
            }

          --len;
        }
      users_[id].bytes_per_tti_ = 0;
      users_[id].dest_ = (uint32_t)-1;
      users_[id].len_ = 0;
      return;
    }
  if (id < 0)         // UE not registered, proceed
    {
      for (id = 0; id < MAX_NUM_UE; id++)
        {
          if (users_[id].dest_ == (uint32_t)-1)
            {              
              users_[id].dest_ = dir;
              break;
            }
        }
    }
  // update users_[] fields for the UE
  users_[id].bytes_per_tti_ = l;
  users_[id].len_ = 0;
  return;
}

// register and de-register for a flow
void
UmtsIfqLayer::RegistrationNodeUE (uint32_t flow, double l)
{
  int id, len;
  id = GetPosition (flow);
  
  if (l == 0)         // drop all the packets stored in the queue belonging to the flow
    {
      len = m_ifqQueue->GetSize ();

      while (len > 0)
        {
           
          Ptr<Packet> packet = m_ifqQueue->Dequeue ();
          AppIndexTag tag;
	  packet->PeekPacketTag(tag);
	  
          if (tag.Get()!= ifqc[id].flowid_)
            {
              m_ifqQueue->Enqueue (packet);

            }

          --len;
        }
      ifqc[id].bytes_per_tti_ = 0;
      ifqc[id].flowid_ = 0;
      ifqc[id].len = 0;
      if (AnyFlowActive () == 0)
        {
          m_ifqTimer.Cancel ();

        }
      return;
    }
  if (id < 0)         // flow not registered, proceed to register
    {
      for (id = 0; id < MAX_NUM_UE; id++)
        {
          if (ifqc[id].flowid_ < 1)
            {
              ifqc[id].flowid_ = flow;
              break;
            }
        }
    }
  // update ifqc[] fields for the flow
  ifqc[id].bytes_per_tti_ = l;
  ifqc[id].len = 0;
  return;
}

// return position in ifqc[] for flow
int
UmtsIfqLayer::GetPosition (uint32_t flow)
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (ifqc[i].flowid_ == flow)
        {
          return (i);                   // flow found
        }
    }
  return (-1);          // not found
}

// returns position in users_[] for a UE
int
UmtsIfqLayer::Find (uint32_t dir)
{
  int id;
  for (id = 0; id < MAX_NUM_UE; id++)
    {
      if (users_[id].dest_ == dir)
        {
          return (id);                  // UE found
        }
    }
  return(-1);           // UE not found
}

int
UmtsIfqLayer::AnyFlowActive ()
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (ifqc[i].flowid_ >0)
        {
          return (1);                   // flow found
        }
    }
  return 0;
}

// handler of BsFCTimer, executed each tti_
void
UmtsIfqLayer::IfqHandlerNodeB ()
{NS_LOG_FUNCTION(this);
  int l, id;
  Ipv4Address address;
  // Restart timer for next tti.
  m_ifqTimer = Simulator::Schedule (Seconds (m_time),&UmtsIfqLayer::IfqHandlerNodeB,this);

  l =m_ifqQueue->GetSize(); //ifqQueue->GetSize ();
  

  while (l > 0)         // fo through all the queue
    {
    
      Ptr<Packet> packet=m_ifqQueue->Dequeue();     
      UmtsReceiverIdTag receiverTag;
      packet->PeekPacketTag(receiverTag);

      id = Find (receiverTag.Get());               // find the UE for the packet
      users_[id].len_ -= packet->GetSize ();          // update len

      if (users_[id].len_ < users_[id].bytes_per_tti_)             // allow to tx this packet
        {
          users_[id].len_ += packet->GetSize ();                // update len
          m_rlc->ReceiveFromAbove (packet,receiverTag.Get());
                    
        }
      else               // enqueue again the packet
        {
          m_ifqQueue->Enqueue(packet);
        }
      --l;
    }

  // update len in each tti.
  for (id = 0; id < MAX_NUM_UE; id++)
    {
      if ((users_[id].dest_ != (uint32_t)-1) && (users_[id].len_ > 0))
        {
          users_[id].len_ -= users_[id].bytes_per_tti_;
          if (users_[id].len_ < 0)
            {
              users_[id].len_ = 0;
            }
        }
    }

}

// IFQ Queue handler. Executed every TTI time
void
UmtsIfqLayer::IfqHandlerNodeUE ()
{
NS_LOG_FUNCTION(this);
  int l, pos;
  // Restart timer for next tti.
  m_ifqTimer = Simulator::Schedule (Seconds (m_time),&UmtsIfqLayer::IfqHandlerNodeUE,this);
  
  l = m_ifqQueue->GetSize ();             // take length of the queue

  while (l > 0)
    {           // go through all the queue      
      Ptr<Packet> packet = m_ifqQueue->Dequeue ();
            
      AppIndexTag tag;
      packet->PeekPacketTag(tag);
      
      UmtsReceiverIdTag receiverTag;
      packet->PeekPacketTag(receiverTag);
      
      pos = GetPosition (tag.Get());

      if (ifqc[pos].len < ifqc[pos].bytes_per_tti_)             // allow to tx this packet
        {          
          ifqc[pos].len += packet->GetSize ();                     // update len
          m_rlc->ReceiveFromAbove (packet,receiverTag.Get());                      // transmit the packet downwards
        }
      else                      // enqueue again the packet
        {
          m_ifqQueue->Enqueue (packet);
        }
      --l;
    }

  // update len in each tti.
  for (pos = 0; pos < MAX_NUM_FLOWS; pos++)
    {
      if ((ifqc[pos].flowid_ != 0) && (ifqc[pos].len > 0))
        {
          ifqc[pos].len -= ifqc[pos].bytes_per_tti_;
          if (ifqc[pos].len < 0)
            {
              ifqc[pos].len = 0;
            }
        }
    }
  return;
}



} // namespace ns3

