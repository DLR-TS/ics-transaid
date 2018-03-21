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

#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "umts-rlc-layer.h"
#include "ns3/simulator.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "umts-queue.h"
#include "umts-manager.h"
#include "ns3/timer.h"
#include "ns3/log.h"
#include "umts-mac-layer.h"
#include "ns3/integer.h"
#include "umts-rlc-am-header.h"
#include "umts-rlc-um-header.h"
#include "umts-rlc-ack-header.h"
#include "umts-tags.h"
#include "ns3/node-id-tag.h"
#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRlcLayer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRlcLayer);

TypeId
UmtsRlcLayer ::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRlcLayer")
    .SetParent<Object> ().AddConstructor<UmtsRlcLayer> ()
    .AddAttribute ("RlcPacketSize",
                   "RLC Packet Size (bytes)",
                   IntegerValue (200),
                   MakeIntegerAccessor (&UmtsRlcLayer::SetFragmentationSize,
                                        &UmtsRlcLayer::GetFragmentationSize),
                   MakeIntegerChecker<int> ())
  ;
  return tid;
}

UmtsRlcLayer::UmtsRlcLayer ()
{

  m_bufferLength = 0;
  int i;
  for (i = 0; i < MAX_LOAD; i++)
    {
      info_[i].Txbuf_ = Create<UMTSQueue> ();
      info_[i].Rxbuf_ = Create<UMTSQueue> ();
      info_[i].flow_ = (uint32_t)-1;      
      info_[i].destination=(uint32_t)-1;
      info_[i].fraged_ = -1;
      info_[i].acked_ = -1;
      info_[i].lastFragment = -1;
      info_[i].seqno_ = 0;                                      // link-layer sequence number
      info_[i].ackno_ = 0;                                      // ACK received so far
      info_[i].rackno_ = -1;                            // seq no of left most pkt
      info_[i].window_ = 500;                           // window size for sack
      info_[i].inseq_ = 1;
      info_[i].unackseqno_ = 0;
      info_[i].numdups_ = 0;
      info_[i].handover_ = 0;
      info_[i].lastRx_=Create<Packet>();
    }

  m_myIp.Set ("0");
  m_delay = Seconds (0.000025);
  m_rlcLayerFragmentSize = 150;
  m_packetList = Create<UMTSQueue> ();

}

UmtsRlcLayer::~UmtsRlcLayer ()
{
  m_timerQueue.clear ();
}

void
UmtsRlcLayer::DoDispose ()
{
  m_mac = 0;
  m_timerQueue.clear ();

}

void
UmtsRlcLayer::SetDelay (Time delay)
{
  m_delay = delay;
}

void
UmtsRlcLayer::SetFragmentationSize (int size)
{
  m_rlcLayerFragmentSize = size;
}

int
UmtsRlcLayer::GetFragmentationSize () const
{
  return m_rlcLayerFragmentSize;
}

void
UmtsRlcLayer ::SetMac (Ptr<UmtsMacLayer> mac)
{
  m_mac = mac;  
  m_mac->SetForwardUpCallback (MakeCallback (&UmtsRlcLayer::ReceiveFromBelow, this));
}

void
UmtsRlcLayer ::ForwardUp (Ptr<Packet> packet)
{  
  m_forwardUp (packet);
}

void
UmtsRlcLayer::SetIpAddress (Ipv4Address address)
{
  m_myIp = address;
}

//!!! NEW FUNCTION !!!
void
UmtsRlcLayer::SetIpv6Address (Ipv6Address address)
{
  m_myIpv6 = address;
}

void
UmtsRlcLayer::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
  m_mac->SetNodeIdentifier(m_nodeIdentifier);
}

uint32_t
UmtsRlcLayer::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

void
UmtsRlcLayer::SetNodeType(std::string type)
{
 m_nodeType=type; 
}

std::string
UmtsRlcLayer::GetNodeType()
{
 return m_nodeType; 
}

void
UmtsRlcLayer::SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_forwardUp = upCallback;
}

void
UmtsRlcLayer ::SendDown (Ptr<Packet> packet)
{
  //packet->Print(std::cout);
  m_mac->ReceiveFromAbove (packet);

}

int
UmtsRlcLayer::look_for (uint32_t flow, uint32_t destinationId)
{
	std::cout<<"UmtsRlcLayer::look_for flow"<<flow<<std::endl;
	std::cout<<"UmtsRlcLayer::look_for destinationId"<<destinationId<<std::endl;
  int i;
  for (i = 0; i < MAX_LOAD; i++)
    {
      if ((info_[i].flow_ == flow) && (info_[i].destination == destinationId))
        {
    	 //std::cout<<"UmtsRlcLayer::look_for 2"<<std::endl;
          return(i);
        }
    }
  return(-1);
}

void
UmtsRlcLayer::remove_flow (uint32_t flowid, uint32_t destinationId)
{
  int i;

  if (flowid != (uint32_t)-1)
    {
      for (i = 0; i < MAX_LOAD; i++)
        {
          if ((info_[i].flow_ == flowid) && (info_[i].destination == destinationId))
            {
              info_[i].destination=(uint32_t)-1;
              info_[i].flow_ = (uint32_t)-1;
              info_[i].acked_ = -1;
              info_[i].lastFragment = -1;
              info_[i].fraged_ = -1;
              info_[i].Txbuf_->RemoveAll();
              info_[i].Rxbuf_->RemoveAll();              
              info_[i].seqno_ = 0;                                      // link-layer sequence number
              info_[i].ackno_ = 0;                                      // ACK received so far
              info_[i].rackno_ = -1;                            // seq no of left most pkt
              info_[i].window_ = 500;                           // window size for sack
              info_[i].inseq_ = 1;
              info_[i].unackseqno_ = 0;
              info_[i].numdups_ = 0;
              info_[i].handover_ = 0;
              DeleteANode (destinationId, flowid);
            }
        }
    }
  else
    {
      for (i = 0; i < MAX_LOAD; i++)
        {
          if (info_[i].destination == destinationId)
            {

              info_[i].destination=(uint32_t)-1;
              info_[i].flow_ = (uint32_t)-1;
              info_[i].acked_ = -1;
              info_[i].lastFragment = -1;
              info_[i].fraged_ = -1;
              info_[i].Txbuf_->RemoveAll();
              info_[i].Rxbuf_->RemoveAll();              
              info_[i].seqno_ = 0;                                      // link-layer sequence number
              info_[i].ackno_ = 0;                                      // ACK received so far
              info_[i].rackno_ = -1;                            // seq no of left most pkt
              info_[i].window_ = 500;                           // window size for sack
              info_[i].inseq_ = 1;
              info_[i].unackseqno_ = 0;
              info_[i].numdups_ = 0;
              info_[i].handover_ = 0;
              DeleteANode (destinationId, flowid);
            }
        }
    }

  return;
}

void
UmtsRlcLayer::AddANode (uint32_t destinationId,int flow)
{
  TimerList timerList;
  timerList.flowId = flow;
  timerList.address = destinationId;

  Timer *timer = new Timer;
  timer->SetFunction (&UmtsRlcLayer::RTxHandler,this);
  timer->SetArguments (destinationId,flow);
  timerList.timer = timer;
  m_timerQueue.push_back (timerList);

}

void
UmtsRlcLayer::DeleteANode (uint32_t destinationId,int flow)
{
  for (TimerQueueI i = m_timerQueue.begin (); i != m_timerQueue.end (); i++)
    {
      if (i->flowId == flow &&i->address == destinationId)
        {
          i->timer->Cancel ();
          i->timer->Remove ();
          i->timer = 0;
          m_timerQueue.erase (i);
          break;

        }
    }
}

void
UmtsRlcLayer::start (uint32_t destinationId, int flow, Time time)
{
  for (TimerQueueI i = m_timerQueue.begin (); i != m_timerQueue.end (); i++)
    {
      if (i->address == destinationId && i->flowId == flow)
        {
          i->timer->Cancel ();
          i->timer->Schedule (time);

        }
    }

}

void
UmtsRlcLayer::stop (uint32_t destinationId, int flow)
{

  for (TimerQueueI i = m_timerQueue.begin (); i != m_timerQueue.end (); i++)
    {
      if (i->address == destinationId && i->flowId == flow)
        {          
          i->timer->Cancel ();
        }
    }
}



int
UmtsRlcLayer::store_flow (int acked, int fraged, uint32_t flowid, uint32_t destinationId, int handover,enum TxType txType)
{
  int i;

  //That's probably a way to check if the flow with the specific flowid already exists
  for (i = 0; i < MAX_LOAD; i++)
    {
      if ((info_[i].flow_ == flowid) && (info_[i].destination == destinationId))
        {
    	 //std::cout<<"UmtsRlcLayer::store_flow flow already there!"<<std::endl;
          return 1;
        }
    }
  for (i = 0; i < MAX_LOAD; i++)
    {
      if (info_[i].flow_==(uint32_t)-1)
        {
          info_[i].destination = destinationId;
          info_[i].flow_ = flowid;
         //std::cout<<"UmtsRlcLayer::store_flow dest"<<destinationId<<std::endl;
         //std::cout<<"UmtsRlcLayer::store_flow flow"<<flowid<<std::endl;
          info_[i].acked_ = acked;
          info_[i].fraged_ = fraged;
          info_[i].handover_ = handover;
          info_[i].txType = txType;
         //std::cout<<"UmtsRlcLayer::store_flow flow"<<txType<<std::endl;
         
          AddANode (destinationId, flowid);
          return 1;

        }
    }
    
    NS_LOG_DEBUG(m_nodeType<<" "<<m_nodeIdentifier<<" RLC. It is not possible to allocate Resources for the flow \n");
    
    return -1;

}

/**
** The following function receives packets from the MAC Layer and process them, distinguising between data packets and control packets
**/

void UmtsRlcLayer::ReceiveFromBelow (Ptr<Packet> packet,uint8_t packetType)
{
 //std::cout<<"!!!Receive from RLC Layer 1!!!"<<std::endl;
  int i;
  
  AppIndexTag idTag;
  packet->PeekPacketTag(idTag);
        
  NodeIdTag senderTag;
  packet->PeekPacketTag(senderTag);
  
  UmtsReceiverIdTag receiverTag;
  packet->PeekPacketTag(receiverTag);
  
  if (packetType != Ack)
    {
      
      i = look_for (idTag.Get(), senderTag.Get());
      
      
	if (i!=-1&&((info_[i].fraged_) == 1 || (info_[i].acked_) == 1))
        {
          SendUpDATA (packet, i);
        }
        else
        {         
	  UmtsRlcUmHeader umHeader;
	  packet->RemoveHeader(umHeader);
          ForwardUp(packet);
        }
                  
    }
  else
    {            
      RecvACK (packet);
    }
 //std::cout<<"UmtsRlcLayer::ReceiveFromBelow"<<std::endl;
  return;
}

void 
UmtsRlcLayer::ReceiveFromAbove (Ptr<Packet> packet,uint32_t destinationId)
{
	std::cout<<"UmtsRlcLayer::ReceiveFromAbove"<<std::endl;
     //std::cout<<"RLC LAYER receive from above"<<std::endl;
     // packet->Print(std::cout);
      int i;
      AppIndexTag tag;
      packet->PeekPacketTag(tag);
     //std::cout<<"UmtsRlcLayer::ReceiveFromAbove: "<<destinationId<<std::endl;
                         
      i = look_for (tag.Get(), destinationId);
     //std::cout<<"LOOK FOR!!!"<<i<<std::endl;

      if(i!=-1)
      {
         //std::cout<<"RLC LAYER 2"<<std::endl;
	 if ((info_[i].fraged_) || (info_[i].acked_))
        {
         //std::cout<<"!!!Enqueue!!!"<<std::endl;
          EnqueueDATA (packet,i);
        }
      else
        {                         	  
	  UmtsRlcUmHeader umHeader;	  
	  umHeader.SetSequenceNumber(0);
	  umHeader.SetResegmentation(0);
	  umHeader.SetLastFragment(0);
	  umHeader.SetFirstFragment(0);
	  
	  packet->AddHeader(umHeader);
          //std::cout<<"RLC LAYER 2"<<std::endl;
          SendDown (packet);
        }
      }    
}


void
UmtsRlcLayer::EnqueueDATA (Ptr<Packet> packet,int pos)
{    
  UmtsRlcAmHeader amHeader;
  UmtsRlcUmHeader umHeader;
    
  int psize;
  
  if (info_[pos].fraged_ == 1)
    {
      psize = m_rlcLayerFragmentSize;
    }
  else
    {
      psize = packet->GetSize ();
    }

  uint16_t bopno = info_[pos].seqno_;
  uint16_t eopno = bopno + (int)((packet->GetSize () - 1) / psize);

  uint16_t i;

  for ( i = bopno; i <= eopno; i++)
    {

      info_[pos].lastFragment = eopno;
                  
      Ptr<Packet> newFragment=Create<Packet>();
      
      if (i == eopno )
        {
	  newFragment=packet->Copy();          
        }
      else
        {
	  newFragment=packet->CreateFragment(0,psize);
	  packet->RemoveAtStart(psize);
          
        }

      if ((info_[pos].acked_ == 0))
        {	  	  
	  umHeader.SetSequenceNumber(i);
	  umHeader.SetResegmentation(1);
	  umHeader.SetLastFragment(eopno);
	  umHeader.SetFirstFragment(bopno);
	            
          info_[pos].seqno_++;          	  	 	  
	  newFragment->AddHeader(umHeader);
          
          SendDown (newFragment->Copy());
        }
      else
        {
	  amHeader.SetSequenceNumber(i);
	  amHeader.SetResegmentation(1);
	  amHeader.SetLastFragment(eopno);
	  amHeader.SetFirstFragment(bopno);         
	  
	  newFragment->AddHeader(amHeader);
          info_[pos].seqno_++;          
	  m_packetList->Enqueue(newFragment);
        }
    }
  if ((info_[pos].acked_ == 1))
    {
      SendDownDATA (pos);
    }

  return;
}

void UmtsRlcLayer::SendDownDATA (int pos)
{  NS_LOG_FUNCTION(this);
  if (m_bufferLength >= m_packetList->GetSize())
    {      
      m_bufferLength = 0;
      return;
    }
    
  Ptr<Packet> packet= m_packetList->Dequeue();
      
  if (packet== 0)
    {
      m_bufferLength = 0;
      return;
    }
  m_bufferLength++;
      
  AppIndexTag tag;
  packet->PeekPacketTag(tag);
  
  UmtsRlcAmHeader header;
  packet->PeekHeader(header);

  if (tag.Get() != info_[pos].flow_)
    {
      m_packetList->Enqueue(packet);
      SendDownDATA (pos);
    }
  else
    {      
      if (info_[pos].Txbuf_->GetSize() < info_[pos].window_)
        {
          info_[pos].Txbuf_->Enqueue(packet->Copy ());
          
          Simulator::Schedule (m_delay,&UmtsRlcLayer::SendDown,this,packet->Copy ());
          m_bufferLength--;
          SendDownDATA (pos);
        }
      else
        {
          m_bufferLength = 0;
          m_packetList->EnqueueHead(packet);
        }
    }
  return;
}

void UmtsRlcLayer::RecvACK (Ptr<Packet> packet)
{
  int i, first, last, temp;
  
  UmtsRlcAckHeader ackHeader;
  packet->PeekHeader(ackHeader);
  
  NodeIdTag senderTag;
  packet->PeekPacketTag(senderTag);
  
  AppIndexTag flowTag;
  packet->PeekPacketTag(flowTag);

  i = look_for (flowTag.Get(),senderTag.Get());
 
  if (ackHeader.GetAckNumber() == info_[i].lastFragment + 1)
    {
      NS_LOG_DEBUG(m_nodeType<<" "<< m_nodeIdentifier << " RLC// ACK for the last Fragment arrived. Packet Sent Correctly!.\n");
      return;
    }

  if (ackHeader.GetAckNumber() == info_[i].ackno_)
    {
      
      first = ackHeader.GetFirstFragment ();
      last = ackHeader.GetLastFragment ();      
      for (temp = first; temp < last; temp++)
        {
          SendDownDATAonACK (i, temp);
        }

      // Notify drop
      return;
    }
  else if (info_[i].ackno_ < ackHeader.GetAckNumber())
    {
      NS_LOG_DEBUG(m_nodeType <<" "<< m_nodeIdentifier << " RLC//Correct Ack Received. Next SeqNum "<<ackHeader.GetAckNumber()<<". \n");
      info_[i].ackno_ = ackHeader.GetAckNumber();
    }
  else
    {
      // Notify Drop
      return;
    }

  // delete pkts with seqno < ackno
 
  info_[i].Txbuf_->DropTill (info_[i].ackno_);

  if ((info_[i].Txbuf_->GetSize() > 0) && (info_[i].Txbuf_->GetSize()< info_[i].window_))
    {      
      SendDownDATA (i);
    }
}


void UmtsRlcLayer::SendDownDATAonACK (int pos, int seqno)
{  
  Ptr<Packet> packet= info_[pos].Txbuf_->GetHead();
  UmtsRlcAmHeader amHeader;
  packet->PeekHeader(amHeader);
    
  int Txlen_ = info_[pos].Txbuf_->GetSize();
    
  for (int i = 0; i < Txlen_; i++)
    {

      if (amHeader.GetSequenceNumber () == seqno)
        {          
	  NS_LOG_DEBUG(m_nodeType <<" "<< m_nodeIdentifier << " RLC//ACK Requested the Retransmission of Packet "<<seqno<<".Sending down data.Time " << Simulator::Now () << "\n");
	  SendDown(packet);
          return;
        }
        packet=info_[pos].Txbuf_->GetNext (packet);

    }
}

/**The following function passes the received packet to the higher level but first we need to process it in case the
** Fragmentation or the ACK mode are enabled
**/

void
UmtsRlcLayer::SendUpDATA (Ptr<Packet> packet,int pos)
{      
  UmtsReceiverIdTag receiverTag;            
  NodeIdTag senderTag;
  UmtsPacketTypeTag typeTag;
  AppIndexTag flowTag;      
  UmtsCodeTag codeTag;
	  
  if (info_[pos].acked_ == 0)
    {
      /** Non ACK Mode **/
      
      UmtsRlcUmHeader umHeader;
      packet->RemoveHeader(umHeader);
      
      PacketTagIterator i = packet->GetPacketTagIterator ();	        
		     
      packet->PeekPacketTag(receiverTag);

      if (umHeader.GetSequenceNumber()== info_[pos].unackseqno_)
        {
	  
	  packet->RemoveAllPacketTags();
	  info_[pos].lastRx_->AddAtEnd(packet);	  
          info_[pos].unackseqno_++;
	  
        }
      else if ((umHeader.GetSequenceNumber() == umHeader.GetFirstFragment()) && (umHeader.GetSequenceNumber() == 0))
        {
	  packet->RemoveAllPacketTags();
	  info_[pos].lastRx_->AddAtEnd(packet->Copy());
          info_[pos].unackseqno_ = 1;
        }
      else
        {
          info_[pos].inseq_ = 0;
          info_[pos].unackseqno_ = umHeader.GetSequenceNumber() + 1;
	  ResetReceiverValues(pos);
        }

      if (umHeader.GetSequenceNumber() == umHeader.GetLastFragment())
        {
          /**We have received the last fragment of an SDU and we are going to check if we have received them in order**/

          if (info_[pos].inseq_)
            {
              /**The packet has arrived in sequence, we ressemble the fragments and send the SDU to the upper layer**/
	     	  	     	      
	      while (i.HasNext ())
		{
		  PacketTagIterator::Item item = i.Next ();
		  NS_ASSERT (item.GetTypeId ().HasConstructor ());
		  Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
		  NS_ASSERT (!constructor.IsNull ());
		  ObjectBase *instance = constructor ();
		  Tag *tag = dynamic_cast<Tag *> (instance);
		  NS_ASSERT (tag != 0);
		  item.GetTag (*tag);
		  
		  if(!info_[pos].lastRx_->PeekPacketTag(*tag))
		  {
		    info_[pos].lastRx_->AddPacketTag(*tag);
		  }
			  
		}
	      
              ForwardUp(info_[pos].lastRx_->Copy());
	      ResetReceiverValues(pos);
            }
          else
            {
              ResetReceiverValues(pos);
            }
        }
      else
        {
          // Free the packet
        }
      return;
    }
  else
    {
                 
      UmtsRlcAmHeader amHeader;
      packet->PeekHeader(amHeader);
      
      if ((info_[pos].handover_ == 1) && (info_[pos].rackno_ + 1 == 0))
        {
          info_[pos].handover_ = 0;
          info_[pos].rackno_ = amHeader.GetSequenceNumber ();
          goto GO;
        }
      else if (amHeader.GetSequenceNumber () == (info_[pos].rackno_ + 1))
        {
          info_[pos].rackno_++;

GO:                    
	  info_[pos].numdups_ = 0;
          
	   packet->RemoveHeader(amHeader);
	  	  	  
	  packet->PeekPacketTag(receiverTag);
	  packet->PeekPacketTag(senderTag);
	  packet->PeekPacketTag(typeTag);
	  packet->PeekPacketTag(flowTag);
	  packet->PeekPacketTag(codeTag);
	  	  	  
	  info_[pos].lastRx_->AddAtEnd(packet->Copy());
	  
	  PacketTagIterator i = packet->GetPacketTagIterator ();
	  
	  while (i.HasNext ())
	    {
	      PacketTagIterator::Item item = i.Next ();
	      NS_ASSERT (item.GetTypeId ().HasConstructor ());
	      Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
	      NS_ASSERT (!constructor.IsNull ());
	      ObjectBase *instance = constructor ();
	      Tag *tag = dynamic_cast<Tag *> (instance);
	      NS_ASSERT (tag != 0);
	      item.GetTag (*tag);
	      
	      if(!info_[pos].lastRx_->PeekPacketTag(*tag))
	      {
		info_[pos].lastRx_->AddPacketTag(*tag);
	      }
	      	      
	    }
	    
	  info_[pos].lastRx_->PeekPacketTag(receiverTag);
	  info_[pos].lastRx_->PeekPacketTag(senderTag);
	 	  	  
          if (amHeader.GetSequenceNumber() == amHeader.GetLastFragment ())
            {             	      
              SendACK (pos);
              stop (info_[pos].destination, info_[pos].flow_);                         /**Esto es mio**/
	      ForwardUp(info_[pos].lastRx_->Copy());	      
	      ResetReceiverValues(pos);
	      
              return;
            }
          else
            {
              NS_LOG_DEBUG(m_nodeType <<" "<< m_nodeIdentifier << " RLC//ACK Mode. There are still some fragments non received.Time " << Simulator::Now ());
            }

          int flag = 1;
          while (flag)
            {
              flag = 0;
              if (info_[pos].Rxbuf_->GetSize () != 0)
                {                  
		  Ptr<Packet> newPacket=info_[pos].Rxbuf_->GetHead(); 
                  int Rxlen_ = info_[pos].Rxbuf_->GetSize ();
                  for (int i = 1; i <= Rxlen_; i++)
                    {
                      if (newPacket== 0)
                        {
                          flag = 0;
                          break;
                        }
                        
                        UmtsRlcAmHeader amHeader;
		      newPacket->PeekHeader(amHeader);
		      
                      if (amHeader.GetSequenceNumber() == (info_[pos].rackno_ + 1))
                        {
                          info_[pos].rackno_++;
			  newPacket->RemoveHeader(amHeader);
			  info_[pos].lastRx_->AddAtEnd( newPacket->Copy ());
			                            
                          flag = 1;

                          if (amHeader.GetSequenceNumber() == amHeader.GetLastFragment () )
                            {                             
                              SendACK (pos);
			      newPacket->RemoveHeader(amHeader);

			      ForwardUp(info_[pos].lastRx_->Copy());
                              stop (info_[pos].destination, info_[pos].flow_);                                         
			      ResetReceiverValues(pos);
                              return;
                            }
                                                  
			    info_[pos].Rxbuf_->RemovePacket(newPacket);
                        }
                                                
                        newPacket=info_[pos].Rxbuf_->GetNext (newPacket);
                      
                    }
                }
            }

          SendACK (pos);
        }
      else if (amHeader.GetSequenceNumber () > (info_[pos].rackno_ + 1))
        {          
          info_[pos].Rxbuf_->Enqueue(packet);
          if (info_[pos].numdups_ < 1)
            {
              SendACK (pos);
              info_[pos].numdups_++;
            }
        }
    }
}

void
UmtsRlcLayer::ResetReceiverValues(int pos)
{

  info_[pos].inseq_=1;
  info_[pos].numdups_=0;
  info_[pos].lastRx_->RemoveAllPacketTags();
  info_[pos].lastRx_=Create<Packet>();                         // last packet received in sequence  

}

void UmtsRlcLayer::SendACK (int pos)
{
  Ptr<Packet> ackPacket = Create<Packet> (5);
  
  UmtsPacketTypeTag typeTag;
  typeTag.Set(Ack);
         
  UmtsReceiverIdTag receiverTag;
  receiverTag.Set(info_[pos].destination);
  
  NodeIdTag senderTag;
  senderTag.Set(m_nodeIdentifier);
  
  UmtsCodeTag codeTag;
  codeTag.Set(1);
  
  AppIndexTag idTag;
  idTag.Set(info_[pos].flow_);
   
  ackPacket->AddPacketTag(typeTag);
  
  UmtsRlcAckHeader ackHeader;
  ackHeader.SetAckNumber(info_[pos].rackno_ + 1);
  ackHeader.SetFirstFragment(info_[pos].rackno_ + 1);
  
  UmtsRlcAmHeader amHeader;
  
  Ptr<ControlPacket> newControlInfo = Create<ControlPacket> ();

  int len, vt;
  
  len = info_[pos].Rxbuf_->GetSize();
  
  Ptr<Packet> newPacket=info_[pos].Rxbuf_->GetHead();

  if (newPacket != 0)
    {
      newPacket->PeekHeader(amHeader);
      vt = amHeader.GetSequenceNumber ();                       
      newPacket = info_[pos].Rxbuf_->GetNext (newPacket);
      
      if(newPacket!=0)
      {
	newPacket->PeekHeader(amHeader);
      
	for (int j = 1; j < len; j++)
        {
          if (newPacket != 0&&amHeader.GetSequenceNumber() < vt)
            {
              vt = amHeader.GetSequenceNumber();
            }                                    
          newPacket = info_[pos].Rxbuf_->GetNext (newPacket);
        }
      }
      
        ackHeader.SetLastFragment(vt);     
    }
  else
    {      
      ackHeader.SetLastFragment(ackHeader.GetFirstFragment()+1);
    }
              
  senderTag.Set(m_nodeIdentifier);
  
  ackPacket->AddPacketTag(receiverTag);
  ackPacket->AddPacketTag(senderTag);
  ackPacket->AddPacketTag(idTag);
  ackPacket->AddPacketTag(codeTag);
  
  NS_LOG_DEBUG(m_nodeType <<" "<< m_nodeIdentifier << " RLC//Sending ACK Packet to the MAC Layer. ACK Number is " << ackHeader.GetAckNumber () << " To Node " << info_[pos].destination<<".Time " << Simulator::Now ());
  
  ackPacket->AddHeader(ackHeader);
    
  SendDown(ackPacket);
  start (info_[pos].destination, info_[pos].flow_,Seconds (0.2));

}

void
UmtsRlcLayer::RTxHandler (uint32_t address, int flow)
{
  int pos = look_for (flow, address);
  if (pos != -1)
    {      
	SendACK (pos);      
    }

}

}
