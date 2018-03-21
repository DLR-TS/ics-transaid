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

#include "umts-manager.h"
#include "umts-mac-layer.h"
#include "umts-mac-header.h"
#include "ns3/log.h"
#include "controlpacket.h"
#include "ns3/random-variable.h"
#include "umts-phy-layer-bs.h"
#include "umts-phy-layer-ue.h"
#include "umts-tags.h"

#include "ns3/ipv4-header.h"

NS_LOG_COMPONENT_DEFINE ("UmtsMacLayer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsMacLayer);

TypeId
UmtsMacLayer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsMacLayer")
    .SetParent<Object> ()
    .AddConstructor<UmtsMacLayer> ()
  ;
  return tid;
}

UmtsMacLayer::UmtsMacLayer ()
{

}

void
UmtsMacLayer:: DoDispose ()
{
  m_phyB = 0;
  m_phyUE = 0;
}

void
UmtsMacLayer::SetForwardUpCallback (Callback<void,Ptr<Packet>,uint8_t > upCallback)
{
  m_forwardUp = upCallback;
}

void 
UmtsMacLayer::SetNodeType(std::string nodeType)
{
   m_nodeType=nodeType;
}

std::string 
UmtsMacLayer::GetNodeType()
{
 return m_nodeType; 
}

void
UmtsMacLayer::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
      
  if(m_nodeType=="NodeUE")
  {    
    m_phyUE->SetNodeIdentifier(identifier);
  }
  else
  {   
     m_phyB->SetNodeIdentifier(identifier);
  }
}

uint32_t
UmtsMacLayer::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

void
UmtsMacLayer::SetPhy (Ptr<Object> phy)
{
  if(m_nodeType=="NodeUE")
  {
     m_phyUE = DynamicCast<UmtsPhyLayerUE>(phy);     
     m_phyUE->SetRxCallback (MakeCallback (&UmtsMacLayer::ReceiveFromBelow, this));
  }
  else
  {
    m_phyB = DynamicCast<UmtsPhyLayerBS>(phy);    
    m_phyB->SetRxCallback (MakeCallback (&UmtsMacLayer::ReceiveFromBelow, this));
  }
 
}

/* Receive packets from above and pass them to the PHY Layer**/

void UmtsMacLayer::ReceiveFromAbove (Ptr<Packet> packet)
{
  //packet->Print(std::cout);
  UmtsPacketTypeTag tag;
  packet->PeekPacketTag(tag);
  UmtsMacHeader macHeader;
            
  if (tag.Get() == DedicatedUnicastData)       // Dedicated Data Channel
    {
      
      if(m_nodeType=="NodeUE")
      {
	macHeader.SetTctf((uint8_t)0x0100);	
      }
      else
      {
	macHeader.SetTctf((uint8_t)0x11);
      }
      
      packet->AddHeader(macHeader);
      SendDown (packet,tag.Get());
    }
  else if (tag.Get()== CommonUnicastData) // Common Data Channel
    {

      if(m_nodeType=="NodeUE")
      {
	macHeader.SetTctf((uint8_t)0x00);
      }
      else
      {
	macHeader.SetTctf((uint8_t)0x01000000);
      }
      
      packet->AddHeader(macHeader);
                  
      SendDown (packet,tag.Get());
    }
  else if (tag.Get()== BroadcastData||tag.Get()== MulticastData)     // MBMS Packets
    {

      if(m_nodeType=="NodeUE")
      {
	macHeader.SetTctf((uint8_t)0x00);
      }
      else
      {
	macHeader.SetTctf((uint8_t)0x01000000);
      }
      packet->AddHeader(macHeader);
      
      SendDown (packet,tag.Get());
    }
  else if (tag.Get()== Ack)  // ACK Packet through the Common Channel
    {       
      if(m_nodeType=="NodeUE")
      {
	macHeader.SetTctf((uint8_t)0x00);
      }
      else
      {
	macHeader.SetTctf((uint8_t)0x01000000);
      }
      
      packet->AddHeader(macHeader);
      SendDown (packet,tag.Get());
    }
}

/**Pass the packets to the PHY Layer**/

void
UmtsMacLayer::SendDown (Ptr<Packet> packet,uint8_t packetType)
{
  //packet->Print(std::cout);  
 //std::cout<<"MAC LAYER send!"<<std::endl;
  if(m_nodeType=="NodeUE")
  {
    
     m_phyUE->ReceivedDataPacketFromAbove (packet,packetType);
    //std::cout<<"UmtsMacLayer::SendDown"<<std::endl;
  }
  else
  {
     m_phyB->ReceivedDataPacketFromAbove (packet,packetType);
  }
 
}

/*Receive Packets from the PHY Layer and pass them to the RLC Layer**/

void UmtsMacLayer::ReceiveFromBelow (Ptr<Packet> packet,uint8_t packetType)
{
  UmtsMacHeader header;
  
  UmtsReceiverIdTag idTag;
  
  packet->PeekPacketTag(idTag);
 //std::cout<<"!!!Packet Receiving at Umts MAC Layer22!!! "<<packet->GetSize()<<std::endl;

  packet->Print(std::cout);
  //std::cout<<"!!!Packet Receiving at Umts MAC Layer23!!!"<<std::endl;
  if (packetType==DedicatedUnicastData) // Dedicated Channel
    {
     //std::cout<<"UmtsMacLayer::ReceiveFromBelow PacketType 1"<<std::endl;
      packet->RemoveHeader(header);            
      ForwardUp (packet,packetType);
      
    }
  else 
    {
	 //std::cout<<"UmtsMacLayer::ReceiveFromBelow PacketType 2"<<std::endl;
      packet->RemoveHeader(header);      
      ForwardUp (packet,packetType);
    }

}

void
UmtsMacLayer::ForwardUp (Ptr<Packet> packet,uint8_t packetType)
{
  m_forwardUp (packet,packetType);
}

Mac48Address
UmtsMacLayer::GetAddress (void) const
{
  return m_self;
}

void
UmtsMacLayer::SetAddress (Mac48Address ad)
{
  m_self = ad;
}

}

