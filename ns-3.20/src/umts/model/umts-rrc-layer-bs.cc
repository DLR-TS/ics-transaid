/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
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
 * Author:Sendoa Vaz
 */


#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "umts-tags.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "umts-rrc-layer-bs.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "controlpacket.h"
#include "umts-phy-layer-bs.h"
#include "umts-rlc-layer.h"
#include "umts-manager.h"
#include "umts-ifq-layer.h"
#include "ovsfTrees.h"
#include "ns3/random-variable.h"
#include "umts-phy-layer-ue.h"
#include "umts-tags.h"
#include "ns3/node-id-tag.h"
#include "ns3/app-index-tag.h"
#include "ns3/itetris-types.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRrcLayerBS");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRrcLayerBS);

TypeId
UmtsRrcLayerBS::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRrcLayerBS")
    .SetParent<Object> ()
    .AddConstructor<UmtsRrcLayerBS> ()
  ;
  return tid;
}

Ipv4Address address;
struct location UmtsRrcLayerBS::registry_[MAX_NUM_NODES] = {};

UmtsRrcLayerBS::UmtsRrcLayerBS ()
{
  int i;
  for (i = 0; i < MAX_NUM_NODES; i++)
    {
      registry_[i].ue_=(uint32_t)-1;
      registry_[i].nodeb_=(uint32_t)-1;
    }

  for (i = 0; i < MAX_NUM_UE; i++)
    {
      m_nodeUEInformation[i].ipaddr_.Set ("0");
      int j;
      for (j=0; j<MAX_NUM_IPv6; j++)
      {
    	  m_nodeUEInformation[i].ipv6addr_[j].Set ("0");
      }
      m_nodeUEInformation[i].identifier = (uint32_t)-1;
      m_nodeUEInformation[i].dltempsf_ = -1;
      m_nodeUEInformation[i].dlsf_ = -1;
      m_nodeUEInformation[i].dlfreq_ = -1;
      m_nodeUEInformation[i].dltotalr_ = 0;
    }

  for (i = 0; i < 13 /*MAX_NUM_FREQ*/; i++)
    {
      m_downlinkSFTree[i] = Create<OvsfTree> (512);
    }

  for (i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {
      m_applications[i].flowid_ = (uint32_t)-1;
      m_applications[i].dest_.Set ("0");
      m_applications[i].wait_ = -1;
      m_applications[i].sf_ = -1;
      m_applications[i].rate_ = -1;
      m_applications[i].user_rate_ = -1;
      m_applications[i].interval_ = -1;
      m_applications[i].c_coding_ = CONV_HALF;
      m_applications[i].isBroadcast=false;
      m_applications[i].isMulticast=false;
    }
    
}

void
UmtsRrcLayerBS::DoDispose ()
{

}

Ipv4Address
UmtsRrcLayerBS::GetIpAddress ()
{
  return m_myIp;
}

void
UmtsRrcLayerBS::SetIpAddress (Ipv4Address address)
{
  m_myIp = address;
  m_rlc->SetIpAddress (address);
  m_phyNodeB->SetMyIpAddress (address);
}

void
UmtsRrcLayerBS::SetIpv6Address (Ipv6Address address)
{
  //std::cout<<"Check 10:"<<std::endl;
  m_myIpv6 = address;
  m_rlc->SetIpv6Address (address);
  m_phyNodeB->SetMyIpv6Address (address);
}

void
UmtsRrcLayerBS::SetUmtsPhyLayerBS (Ptr<UmtsPhyLayerBS> phyLayer)
{
  m_phyNodeB = phyLayer;    
}

Ptr<UmtsPhyLayerBS>
UmtsRrcLayerBS::GetUmtsPhyLayerBS ()
{
  return m_phyNodeB;
}

void
UmtsRrcLayerBS::SetRlc (Ptr<UmtsRlcLayer> rlc)
{
  m_rlc = rlc;
  
  m_rlc->SetForwardUpCallback (MakeCallback (&UmtsRrcLayerBS::PacketReceivedFromBelow, this));
}

void
UmtsRrcLayerBS::SetDownlinkFrequency (int freq)
{
  m_downlinkFrequency = freq;
}

void
UmtsRrcLayerBS::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
  m_rlc->SetNodeIdentifier(m_nodeIdentifier);
  m_ifqQueue->SetNodeIdentifier(m_nodeIdentifier);
}

uint32_t
UmtsRrcLayerBS::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

uint32_t
UmtsRrcLayerBS::FindNodeIdentifier(Ipv4Address address)
{
  for(int i=0;i<MAX_NUM_UE;i++)
  {
    if(m_nodeUEInformation[i].ipaddr_==address)
    {
     return  m_nodeUEInformation[i].identifier;
    }
  }
  
  return (uint32_t)-1;
}

uint32_t
UmtsRrcLayerBS::FindNodeIdentifier(Ipv6Address address)
{
	int j;
  //bool found = false;
  for(int i=0;i<MAX_NUM_UE;i++)
  {
	  for (j=0; j<MAX_NUM_IPv6; j++)
	  {
		  if(m_nodeUEInformation[i].ipv6addr_[j]==address)
		  {
			  return  m_nodeUEInformation[i].identifier;
		  }
	  }
  }
  
  return (uint32_t)-1;
}


void
UmtsRrcLayerBS::ReceiveFromAbove(Ptr<Packet> packet,Ipv4Address address)
{

  AppIndexTag tag;
  packet->PeekPacketTag(tag);
      
  UmtsReceiverIdTag idTag;
  uint32_t id=FindNodeIdentifier(address);
  bool found=false;
    
  if(id!=(uint32_t)-1)
  {
      idTag.Set(id);
      
  }
  else
  {
     for (int i = 0; i < MAX_NUM_FLOWS_DL; i++)
      {
	
	if(m_applications[i].dest_==address&&m_applications[i].isBroadcast==1)
	{
	  idTag.Set(ID_BROADCAST);	  
	  found=true;
	  break;
	}
	else if(m_applications[i].dest_==address&&m_applications[i].isMulticast==1)
	{
	  idTag.Set(ID_MULTICAST);	  
	  found=true;
	  break;
	}	  
      }
      
      if(!found)
      {
	return;
      }
      
  }
  
  packet->AddPacketTag(idTag);
    
  
  for (int i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {
      if (m_applications[i].flowid_ == tag.Get())
        {
	  
	  UmtsCodeTag codeTag;
	  codeTag.Set(m_applications[i].c_coding_);
	  packet->AddPacketTag(codeTag);	
	  
	  // flowid already registered
          if (m_applications[i].wait_ == 1)
            {              
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);	      
              SendDown (packet->Copy (),m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                              // send packet to IFQ
              
            }

          else
            {
              // is the first packet of that flow
              // calculate the physical rate that is going to send over the air interface
              m_applications[i].rate_ = GetRate (m_applications[i].user_rate_, m_applications[i].c_coding_, packet->GetSize ());
              // calculate the interval between two packets
              m_applications[i].interval_ = (packet->GetSize () * 8) / m_applications[i].user_rate_;              
              m_applications[i].dest_ = address;
              // spreading factor needed in physical layer

              m_applications[i].sf_ = CalculateSpreadingFactor (m_applications[i].rate_,m_applications[i].txType);
	      
              if (m_applications[i].sf_ != 0)
                {
                  UmtsTxRateTag rateTag;
		  rateTag.Set(m_applications[i].user_rate_);
		  packet->AddPacketTag(rateTag);
		  
                  if (m_applications[i].txType == Common)
                    {
                      CalculateFachSlotBytes (m_applications[i].rate_,m_applications[i].sf_);                      
                      SendDown (packet,m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                     
                    }
                  else
                    {
		      m_applications[i].wait_ = 1;                     // put wait = 0 to prevent sending messages                      
 		      SendDown (packet->Copy (),m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                              // send packet to IFQ                     
                    }

                }
              else
                {
                  NS_LOG_DEBUG (" Node B. The Common Tx has a Tx Rate greater than the allowed maximum");
                  
                }
	}
	break;
    }
    }   
}


void
UmtsRrcLayerBS::ReceiveFromAbove(Ptr<Packet> packet,Ipv6Address address)
{

  //std::cout<<"Inside RRC2"<<std::endl;
  AppIndexTag tag;
  packet->PeekPacketTag(tag);
      
  UmtsReceiverIdTag idTag;

  //Problem is here
  uint32_t id=FindNodeIdentifier(address);

 //std::cout<<"UmtsRrcLayerBS::ReceiveFromAbove id: "<<id<<std::endl;
  bool found=false;
    
  if(id!=(uint32_t)-1)
  {
      idTag.Set(id);
      found = true;
      
  }
  else
  {
     for (int i = 0; i < MAX_NUM_FLOWS_DL; i++)
      {
   //std::cout<<"UmtsRrcLayerBS::ReceiveFromAbove 1: "<<m_applications[i].destv6_<<std::endl;
	if(m_applications[i].destv6_==address&&m_applications[i].isBroadcast==1)
	{
	  idTag.Set(ID_BROADCAST);	  
	  found=true;
	  break;
	}
	else if(m_applications[i].destv6_==address&&m_applications[i].isMulticast==1)
	{
	  idTag.Set(ID_MULTICAST);
	 //std::cout<<"UmtsRrcLayerBS::ReceiveFromAbove 2"<<std::endl;
	  found=true;
	  break;
	}

	//!!! NEW Addition here!!!
	/*else if(m_applications[i].destv6_==address)
	{
		idTag.Set(id);
		std::cout<<"UmtsRrcLayerBS::ReceiveFromAbove 3, id: "<<id<<std::endl;
		found=true;
		break;
	}*/

	}
  }
  if(!found)
  {
	  return;
  }
      
  
  packet->AddPacketTag(idTag);


  //REMOVE THIS WHEN YOU ARE DONE!!!    
  //SendDown (packet,m_applications[0].txType,m_applications[0].isMulticast,m_applications[0].isBroadcast); 
  
  for (int i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {
      if (m_applications[i].flowid_ == tag.Get())
        {
    	 //std::cout<<"UmtsRrcLayerBS::ReceiveFromAbove 4"<<std::endl;
	  UmtsCodeTag codeTag;
	  codeTag.Set(m_applications[i].c_coding_);
	  packet->AddPacketTag(codeTag);	
	  
	  // flowid already registered
          if (m_applications[i].wait_ == 1)
            {              
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);	      
              SendDown (packet->Copy (),m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                              // send packet to IFQ
              
            }

          else
            {
              // is the first packet of that flow
              // calculate the physical rate that is going to send over the air interface
              m_applications[i].rate_ = GetRate (m_applications[i].user_rate_, m_applications[i].c_coding_, packet->GetSize ());
              // calculate the interval between two packets
              m_applications[i].interval_ = (packet->GetSize () * 8) / m_applications[i].user_rate_;              
              m_applications[i].destv6_ = address;

              // spreading factor needed in physical layer

              m_applications[i].sf_ = CalculateSpreadingFactor (m_applications[i].rate_,m_applications[i].txType);
	      
              if (m_applications[i].sf_ != 0)
                {
                  UmtsTxRateTag rateTag;
		  rateTag.Set(m_applications[i].user_rate_);
		  packet->AddPacketTag(rateTag);
		  
                  if (m_applications[i].txType == Common)
                    {
                      CalculateFachSlotBytes (m_applications[i].rate_,m_applications[i].sf_);                      
                      SendDown (packet,m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                     
                    }
                  else
                    {
		      m_applications[i].wait_ = 1;                     // put wait = 0 to prevent sending messages                      
 		      SendDown (packet->Copy (),m_applications[i].txType,m_applications[i].isMulticast,m_applications[i].isBroadcast);                              // send packet to IFQ                     
                    }

                }
              else
                {
                  NS_LOG_DEBUG (" Node B. The Common Tx has a Tx Rate greater than the allowed maximum");
                  
                }
	}
	break;
    }
    }   
}

void
UmtsRrcLayerBS::SendDown(Ptr<Packet> packet,enum TxType txType,bool isMulticast,bool isBroadcast)
{
	std::cout<<"UmtsRrcLayerBS::SendDown"<<std::endl;
  UmtsReceiverIdTag receiverId;
  packet->PeekPacketTag(receiverId);
 //std::cout<<"UmtsRrcLayerBS::SendDown, receiver tag: "<<receiverId.Get()<<std::endl;
      
  if(isBroadcast)
  {      
      UmtsPacketTypeTag tag;
      tag.Set(2);      
      packet->AddPacketTag(tag);      
     //std::cout<<"Before RLC 1"<<std::endl;
      m_rlc->ReceiveFromAbove(packet,receiverId.Get());
      
      return;
  }
  
  if(isMulticast)
  {
      UmtsPacketTypeTag tag;
      tag.Set(4);      
      packet->AddPacketTag(tag);   
     //std::cout<<"Before RLC 2"<<std::endl;   
      m_rlc->ReceiveFromAbove(packet,receiverId.Get());
      
      return;
  }
  
  if (txType == Dedicated)
    {
      UmtsPacketTypeTag tag;
      tag.Set(0);      
      packet->AddPacketTag(tag);
     //std::cout<<"Before RLC 3"<<std::endl;      
      m_ifqQueue->ReceivePacketFromRRC (packet);
    }
  else
    {                        
      UmtsPacketTypeTag tag;
      tag.Set(1);      
      packet->AddPacketTag(tag);
     //std::cout<<"Before RLC 4"<<std::endl;                              
      m_rlc->ReceiveFromAbove (packet,receiverId.Get());
    }

}

// configure a new flow in RRC layer
int
UmtsRrcLayerBS::ConfigureNewFlow (std::string type, double rate,uint32_t destinationId,uint32_t applicationId,Ipv4Address address)
{

  int acked, fraged;
  uint32_t code;
  enum TxType txType;
  bool isMulticast=false;
  bool isBroadcast=false;

  if(type=="MULTICAST")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=true;
      isBroadcast=false;
  }
  else if(type=="BROADCAST")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=true;
  }
  else if(type=="UM-NON_FRAG")
  {

      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=false;
  }
  else if(type=="UM-FRAG")
  {
      code = CONV_THIRD;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=false;
  }
  else if(type=="AM")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 1; 
      txType=Dedicated;
      isMulticast=false;
      isBroadcast=false;
  }

  // register the new flow with its characteristics in the m_applications[] array
  int i = InsertFlow (applicationId, rate, code,address,isMulticast,isBroadcast,txType);
  if (i == -1)
    {
      // too many applications in this terminal
      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC// Too many application in this NodeUE, not registring the new Flow. Time " << Simulator::Now ());
      return -1;
    }
  else
    {
      // register the new flow in RLC layer      
      UpdateFlow (acked, fraged, applicationId,destinationId,0,txType);
      return 1;
    }

}


int
UmtsRrcLayerBS::InsertFlow (int flow, double rate, uint32_t code,Ipv4Address address,int isMulticast,int isBroadcast,enum TxType txType)
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {
      // looks for a free position
      if (m_applications[i].flowid_ ==(uint32_t)-1)
        {          
          m_applications[i].flowid_ = flow;
          m_applications[i].dest_= address;

	  m_applications[i].wait_ = -1;
          m_applications[i].user_rate_ = rate;
          m_applications[i].c_coding_ = code;
          m_applications[i].isMulticast = isMulticast;
	  m_applications[i].isBroadcast= isBroadcast;
	  m_applications[i].txType=txType;
	  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC// Inserting a new Flow with DestIP "<<address<<" . Time " << Simulator::Now ());
          return(i);
        }
    }
  // there is not free position, ERROR
  return (-1);
}

void
UmtsRrcLayerBS::RemoveFlowFromRegistry (int position)
{
  m_applications[position].flowid_ = (uint32_t)-1;
  m_applications[position].dest_.Set ("0");
  m_applications[position].wait_ =-1;
  m_applications[position].sf_ = -1;
  m_applications[position].rate_ = -1;
  m_applications[position].user_rate_ = -1;
  m_applications[position].interval_ = -1;
  m_applications[position].isMulticast = false;
  m_applications[position].isBroadcast = false;
  m_applications[position].c_coding_ = CONV_HALF;
}

int
UmtsRrcLayerBS::InsertFlow (int flow, double rate, uint32_t code,Ipv6Address address,int isMulticast,int isBroadcast,enum TxType txType)
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {
      // looks for a free position
      if (m_applications[i].flowid_ ==(uint32_t)-1)
        {          
          m_applications[i].flowid_ = flow;
          m_applications[i].destv6_= address;
         //std::cout<<"UmtsRrcLayerBS::InsertFlow: "<<address<<std::endl;
	  m_applications[i].wait_ = -1;
          m_applications[i].user_rate_ = rate;
          m_applications[i].c_coding_ = code;
          m_applications[i].isMulticast = isMulticast;
	  m_applications[i].isBroadcast= isBroadcast;
	  m_applications[i].txType=txType;
	  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC// Inserting a new Flow with DestIP "<<address<<" . Time " << Simulator::Now ());
          return(i);
        }
    }
  // there is not free position, ERROR
  return (-1);
}


// configure a new flow in RRC layer
int
UmtsRrcLayerBS::ConfigureNewFlow (std::string type, double rate,uint32_t destinationId,uint32_t applicationId,Ipv6Address address)
{
 //std::cout<<"UmtsRrcLayerBS::ConfigureNewFlow adress: "<<address<<std::endl;
  int acked, fraged;
  uint32_t code;
  enum TxType txType;
  bool isMulticast=false;
  bool isBroadcast=false;

  if(type=="MULTICAST")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=true;
      isBroadcast=false;
  }
  else if(type=="BROADCAST")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=true;
  }
  else if(type=="UM-NON_FRAG")
  {
	 //std::cout<<"UmtsRrcLayerBS::ConfigureNewFlow UM-NON_FRAG"<<std::endl;
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=false;
  }
  else if(type=="UM-FRAG")
  {
      code = CONV_THIRD;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 0; 
      txType=Common;
      isMulticast=false;
      isBroadcast=false;
  }
  else if(type=="AM")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 1; 
      txType=Dedicated;
      isMulticast=false;
      isBroadcast=false;
  }

  // register the new flow with its characteristics in the m_applications[] array
  int i = InsertFlow (applicationId, rate, code,address,isMulticast,isBroadcast,txType);
  if (i == -1)
    {
      // too many applications in this terminal
      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC// Too many application in this NodeUE, not registring the new Flow. Time " << Simulator::Now ());
      return -1;
    }
  else
    {
      // register the new flow in RLC layer      
      UpdateFlow (acked, fraged, applicationId,destinationId,0,txType);
      return 1;
    }

}


void
UmtsRrcLayerBS::SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_forwardUp = upCallback;
}

void
UmtsRrcLayerBS::ForwardUp (Ptr<Packet> packet)
{
  m_forwardUp (packet);
}

void
UmtsRrcLayerBS::SetIFQQueue (Ptr<UmtsIfqLayer> ifq)
{  
  m_ifqQueue = ifq;  
  m_ifqQueue->SetRLCLayer (m_rlc);
  m_ifqQueue->StartUmtsIfqLayer ();
}

void
UmtsRrcLayerBS::PacketReceivedFromBelow (Ptr<Packet> packet)
{
  
  UmtsPacketTypeTag typeTag;  
  packet->PeekPacketTag(typeTag);  
  UmtsReceiverIdTag receiverTag;
  packet->PeekPacketTag(receiverTag);
  
  NodeIdTag senderTag;
  packet->PeekPacketTag(senderTag);
  
  if(typeTag.Get()==CommonUnicastData||typeTag.Get()==MulticastData||typeTag.Get()==BroadcastData)
  {
    
      if (receiverTag.Get()== m_nodeIdentifier)
	{
	  NS_LOG_DEBUG ("Node B " << m_nodeIdentifier << " RRC /Common Data Packet has arrived for me from NodeUE " << senderTag.Get());	  

	  ForwardUp (packet);
          //std::cout<<"UmtsRrcLayerBS::PacketReceivedFromBelow 1"<<std::endl;
	}       
  }
  else if(typeTag.Get()==DedicatedUnicastData)
  {
    if (receiverTag.Get()== m_nodeIdentifier)
      {
	NS_LOG_DEBUG ("Node B " << m_nodeIdentifier << " RRC /Dedicated Data Packet has arrived for me from NodeUE " << senderTag.Get());	
	ForwardUp (packet);
        //std::cout<<"UmtsRrcLayerBS::PacketReceivedFromBelow 2"<<std::endl;
      }
  }
  
}

int
UmtsRrcLayerBS::PagingOkReceived (uint32_t address,uint32_t applicationId)
{
  int i = 0;
  
  for (int k = 0; k < MAX_NUM_FLOWS; k++)
    {      
      if ((applicationId== m_applications[k].flowid_) && (m_applications[k].wait_ == -1))
        {	  
          if (AvailableResources (address,k))
            {
              i = LookForUE (address);
              m_ifqQueue->RegistrationNodeB (address, CalculateBytes (i));          // registring in IFQ
              CalculateAndUpdateBytesInPhy (i);                 // updating physical bytes
	      	      
	      while(m_packetQueue[k].size()>0)
	      {
		Ptr<Packet> packet=m_packetQueue[k].front();
		m_packetQueue[k].pop_front();
		SendDown(packet,Dedicated,false,false);
		
	      }

              m_nodeUEInformation[i].dltempsf_ = -1;
	      
	      return 1;
            }
          break;
        }
        return 0;
    }
    return 0;


}

void
UmtsRrcLayerBS::CalculateFachSlotBytes (double txRate,double sf)
{
  double bytes_slot;

  if (txRate > 0)
    {
      // bytes per slot in physical layer for the actual SF for that UE
      bytes_slot = (2 * (2560 / (double)sf) / 8);

    }
  else
    {
      bytes_slot = 0.0;         // none application for that UE
    }
//   NS_LOG_DEBUG ("Storing a slot bytes value of " << bytes_slot);

  m_phyNodeB->StoreFachSlotBytes (bytes_slot);    // pass the result to phy layer
}

void
UmtsRrcLayerBS::CalculateAndUpdateBytesInPhy (int pos)
{
  double bytes_slot;

  if (m_nodeUEInformation[pos].dltotalr_ > 0)
    {
      // bytes per slot in physical layer for the actual SF for that UE
      bytes_slot = (2 * (2560 / (double)m_nodeUEInformation[pos].dlsf_) / 8);
      if (m_nodeUEInformation[pos].dltotalr_ > 1920000)
        {
          // case of several DPDCHs
          bytes_slot = bytes_slot * ((int)(m_nodeUEInformation[pos].dltotalr_ / 1920000) + 1);
        }
    }
  else
    {
      bytes_slot = 0.0;         // none application for that UE
    }  
  
  m_phyNodeB->StoreSlotBytes (m_nodeUEInformation[pos].identifier, bytes_slot);        // pass the result to phy layer

}

// gets bytes per tti for a specific user in IFQ
double
UmtsRrcLayerBS::CalculateBytes (int pos)
{
  double bits, sum;
  int rlcfsize = m_rlc->GetFragmentationSize ();                // rlc fragment size
  int rlchdrsize = RLC_HDR_SZ;                          // rlc-umts header size
  int machdrsize = MAC_HDR_SZ;                          // mac-umts header size
  // int phyhdrsize = PHY_HDR_SZ;			// phy-umts header size
  
  if (m_nodeUEInformation[pos].dlsf_ > 0)
    {
      bits = 2 * (2560 / m_nodeUEInformation[pos].dlsf_) * (m_ifqQueue->GetTTI () / UMTS_SlotTime);           // bits per tti
      sum = m_nodeUEInformation[pos].dltotalr_;

      if (sum > 0)
        {
          if (sum > 1920000)                // more than one dpdch is needed
            {
              bits = bits * ((int)(sum / 1920000) + 1);
            }
        }
      bits = bits / 2;          // apply Convolutional Half Rate coding
      bits = (bits * rlcfsize) / (rlcfsize + rlchdrsize + machdrsize);

      return(bits / 8);
    }
  else
    {
      return (0.0);
    }
}

// allocate downlink resources in this layer and OSVF trees if there are enough available (returns 1)
// and returns 0 if not.
int
UmtsRrcLayerBS::AvailableResources (uint32_t address,int k)
{
  int sf, pos;
  double temp_rate;

  pos = LookForUE (address);

  if (IsUEFixedNode (address))
    {
      temp_rate = m_applications[k].user_rate_ ;                 // is a UE, take the physical rate needed from the LL header
    }
        
  if (m_nodeUEInformation[pos].dlsf_ > 0)
    {     // flows already allocated for that node
      double tr = GetRate (m_nodeUEInformation[pos].dlsf_);                // get maximum physical rate allowed for the
      // allocated SF for that UE in downlink
      if (tr >= (m_nodeUEInformation[pos].dltotalr_ + temp_rate))                  // is able to support new flow

        {
          m_nodeUEInformation[pos].dltotalr_ += temp_rate;                 // update registers

          return(1);                    // OK
        }
      else
        {
          // is not able to support new flow, new resources has to be allocated
          m_nodeUEInformation[pos].dltempsf_ = m_nodeUEInformation[pos].dlsf_;
          RemoveResources (pos);                // remove resources from OSVF trees
          sf = CalculateSpreadingFactor (temp_rate + m_nodeUEInformation[pos].dltotalr_,Dedicated);                // calculate needed SF

          if (AllocateResources (address,sf))                     // allocate resources in OSVF downlink trees

            {
              m_nodeUEInformation[pos].dltotalr_ += temp_rate;                     // update registers

              return(1);                        // OK
            }
          else                 // not possible to allocate resuorces in OSVF trees
            {
              m_nodeUEInformation[pos].dlsf_ = m_nodeUEInformation[pos].dltempsf_;
              m_nodeUEInformation[pos].dltempsf_ = -1;
              AllocateResources (address,m_nodeUEInformation[pos].dlsf_);                    // give the previous resources
              return(0);                        // not possible
            }
        }
    }
  else                  // no flows already allocated for that node
    {
      sf = CalculateSpreadingFactor (address,k);          // get the needed SF for the flow
      m_nodeUEInformation[pos].dltempsf_ = -1;
      if (AllocateResources (address,sf))                 // allocate resources in OSVF trees

        {
          m_nodeUEInformation[pos].dltotalr_ += temp_rate;                 // update resgisters

          return(1);                    // OK
        }
    }
  return (0);           // not possible
}

// calculates the spreading factor for a given packet
int
UmtsRrcLayerBS::CalculateSpreadingFactor (uint32_t address,int k)
{
  double index;         // bits per slot

  if (IsUEFixedNode (address))
    {
      index = m_applications[k].user_rate_ * UMTS_SlotTime / 8;           // bytes per slot in physical layer
    }
  else
    {
      // takes the information from the packet
      index = m_applications[k].user_rate_  * UMTS_SlotTime / 8;           // bytes per slot in physical layer
    }

  if (index > 0)         // calculate the SF for that physical rate
    {
      index = (2 * 320 / index);
      index = (int)(log (index) / log (2));
      int i, sf_ = 2;
      if (index > 9)
        {
          index = 9;
        }
      if (index < 2)
        {
          index = 2;
        }
      for (i = 0; i < ((int)index - 1); i++)
        {
          sf_ *= 2;
        }

      return(sf_);
    }
  else
    {
      return(0);
    }
}

// calculate the physical rate
double
UmtsRrcLayerBS::GetRate (double rate, uint32_t code, int psize)
{
  double rt, time;
  int rlcfsize = m_rlc->GetFragmentationSize ();                // rlc fragment size
  int rlchdrsize = RLC_HDR_SZ;                          // rlc-umts header size
  int machdrsize = MAC_HDR_SZ;                          // mac-umts header size
  
  time = (psize * 8) / rate;                    // time between two user packets
  int npkts = (int)(psize / rlcfsize);          // number of rlc fragments per user packet
  if ((psize % rlcfsize) > 0)
    {
      npkts++;
    }
  rt = (npkts * rlcfsize * 8) / time;           // effective user rate
  rt = (rt * (rlcfsize + rlchdrsize + machdrsize)) / rlcfsize;          // user rate before channel coding
  // apply channel coding
  switch (code)
    {
    case 0:             // turbo coding
      rt = 3 * rt;
      break;
    case 1:             // convolutional half
      rt = 2 * rt;
      break;
    case 2:             // convolutional third
      rt = 3 * rt;
      break;
    default:
      break;
    }

  return(rt);
}


// ---------------------------------------------------------------------------------------------------------------//
// ------------------------Setup and Handover Procedures----------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------------------//


int
UmtsRrcLayerBS::RegisterNewUE (Ptr<ControlPacket> controlpacket)
{
  int i;

  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_nodeUEInformation[i].identifier == (uint32_t)-1)
        {
          // register the new UE in array m_nodeUEInformation[]
          m_nodeUEInformation[i].identifier = controlpacket->GetSourceNodeIdentifier ();
         //std::cout<<"UmtsRrcLayerBS::RegisterNewUE 1, :"<<controlpacket->GetSourceNodeIdentifier ()<<std::endl;
          m_nodeUEInformation[i].ipaddr_ = controlpacket->GetSource();
          //!!! Change! We now have a vector of Ipv6 addresses for each node.
          //(Difference from IPv4).
          m_nodeUEInformation[i].ipv6addr_[0] =  controlpacket->GetSourcev6();
         //std::cout<<"UmtsRrcLayerBS::RegisterNewUE 2, :"<<controlpacket->GetSourcev6()<<std::endl;
          return (1);
        }
    }
  
  return (0);
}

//The purpose of this function is to add the global ipv6
//address into m_nodeUEInformation vector so that we don't have
//problems during the retrieval of the node identifier
//(function FindNodeIdentifier()).

int
UmtsRrcLayerBS::AddUEIpv6Address (uint32_t NodeId, Ipv6Address Address)
{
	std::cout<<"UmtsRrcLayerBS::AddUEIpv6Address 1"<<std::endl;
	int i;
	int j;
	for (i = 0; i < MAX_NUM_UE; i++)
	    {
	      if (m_nodeUEInformation[i].identifier == NodeId)
	        {
	    	 //std::cout<<"UmtsRrcLayerBS::AddUEIpv6Address 2"<<std::endl;
	    	  for (j =0; j<MAX_NUM_IPv6; j++)
	    	  {
	    		 //std::cout<<"UmtsRrcLayerBS::AddUEIpv6Address 3, IPv6:"<<m_nodeUEInformation[i].ipv6addr_[j]<<std::endl;
	    		  if(m_nodeUEInformation[i].ipv6addr_[j] == Address)
	    		  {
	    			 //std::cout<<"UmtsRrcLayerBS::AddUEIpv6Address 3"<<std::endl;
	    			  break;
	    		  }
	    		  else if (m_nodeUEInformation[i].ipv6addr_[j] == "::")
	    		  {
	    			  m_nodeUEInformation[i].ipv6addr_[j] = Address;
	    			 //std::cout<<"UmtsRrcLayerBS::AddUEIpv6Address 4"<<m_nodeUEInformation[i].ipv6addr_[j]<<std::endl;
	    			  return 1;
	    		  }
	    	  }
	        }
	    }
	return 0;
}

// update the static structure registry_
void UmtsRrcLayerBS::UpdateNodeRegistry (uint32_t address)
{
  int i;
  Ipv4Address address2;

  // UE already registered (handover procedure)
  for (i = 0; i < MAX_NUM_NODES; i++)
    {
      if (registry_[i].ue_ == address)
        {          
          registry_[i].nodeb_ = m_nodeIdentifier;                   // update the registry_
          break;
        }
    }

  // UE not registered in the system (turn on procedure)
  if (i == MAX_NUM_NODES)
    {
      for (i = 0; i < MAX_NUM_NODES; i++)
        {
          if (registry_[i].ue_ == (uint32_t)-1)                          // update registry_
            {
              registry_[i].ue_ = address;
              registry_[i].nodeb_ = m_nodeIdentifier;
              break;
            }
        }
    }
  return;
}

int
UmtsRrcLayerBS::ResourceRequestReceived(std::string type,uint32_t applicationId,uint32_t destinationId)
{
  int acked, fraged;
  // variabile non usata
  //uint32_t code;
  //code = CONV_HALF;                 // convolutional half
  fraged = 1;                       // fragmentation
  acked = 1;     
  int value=UpdateFlow (acked,fraged,applicationId,destinationId,0,Dedicated); 
  
  return value;
}

// give resources in RLC layer for the flow
int
UmtsRrcLayerBS::UpdateFlow (int acked, int fraged, int flowid, uint32_t destinationId, int handover,enum TxType txType)
{
  int value=m_rlc->store_flow (acked, fraged, flowid,destinationId, handover,txType);      // update RLC layer
  //std::cout<<"UmtsRrcLayerBS::UpdateFlow: "<<value<<std::endl;
  return value;
}

/** Returns the maximum rate allowed for the SF*/

double
UmtsRrcLayerBS::GetRate (int sf)
{
  return((2 * 2560 / sf) / UMTS_SlotTime);
}

// remove the uplink resources allocated for that flow
void
UmtsRrcLayerBS::RemoveUpLinkFlow (uint32_t flowId, int pos)
{
  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier<< " RRC  **********  Removing UL Resources for UE " << m_nodeUEInformation[pos].identifier << ".           Time " << Simulator::Now () << "\n ");
  m_rlc->remove_flow (flowId,m_nodeUEInformation[pos].identifier);       // remove resources in RLC layer

}

void
UmtsRrcLayerBS::NodeUEConnectionLoss(uint32_t nodeIdentifier,Ipv4Address address)
{
  int i = LookForUE (nodeIdentifier);
  
  RemoveUpLinkFlow((uint32_t)-1,i);
  
  for (i = 0; i < MAX_NUM_FLOWS_DL; i++)
    {      
      if (m_applications[i].dest_ ==address)
        {          
          RemoveDownLinkFlow(nodeIdentifier,m_applications[i].flowid_);	  	            
        }
    }
  
  
}


/** Remove downlink resources allocated for that flow**/

void
UmtsRrcLayerBS::RemoveDownLinkFlow (uint32_t destinationId,uint32_t applicationId)
{
  int i = LookForUE (destinationId);
  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC  ********** Removing Downlink Flow for one UE with i " << i << " .Time " << Simulator::Now ());
  int k = 0;
  
   for (k = 0; k < MAX_NUM_FLOWS; k++)
    {
      if (m_applications[k].flowid_ == applicationId)
	{
	  RemoveFlowFromRegistry (k);
	  break;
	}
    }
  
  if (m_nodeUEInformation[i].dlsf_ > 0)
    {           // flows already allocated for that node
      RemoveResources (i);              // remove downlink resources from OSVF trees
      m_rlc->remove_flow (applicationId,destinationId);             // remove resources in RLC layer
      m_nodeUEInformation[i].dltotalr_ -= m_applications[k].user_rate_;             // decrease physical rate for that UE

      if (m_nodeUEInformation[i].dltotalr_ > 0)
        {
          // there are still active flows for that UE; reallocate resources
          
          int sf = CalculateSpreadingFactor (m_nodeUEInformation[i].dltotalr_,Dedicated);                  // recalculate the needed SF          
          AllocateResources (destinationId,sf);                 // reallocate resources in OSVF trees
          
        }
      else
        {
          // that UE has not got any other application running
          m_nodeUEInformation[i].dltotalr_ = 0;
          m_nodeUEInformation[i].dlsf_ = -1;
          m_phyNodeB->RemoveDedicatedChannel (destinationId);
         }
    
      m_ifqQueue->RegistrationNodeB (destinationId, CalculateBytes (i));         // update IFQ layer for that UE
      CalculateAndUpdateBytesInPhy (i);                         // update physical rate needed for that UE
    }
  return;
}

/** Allocate resources in the OSVF trees**/

int
UmtsRrcLayerBS::AllocateResources (uint32_t address, int sf)
{
  int  r;

  int j = RandomVariable (UniformVariable (0,MAX_NUM_FREQ - 1)).GetInteger ();

  for (int i = 0; i < MAX_NUM_FREQ; i++)        // for any frequencies
    {      
      if (m_downlinkSFTree[j]->InsertSf (address, sf))          // allocate resources
        {
          NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC// Allocated Resources in OVSF Trees \n");
          r = LookForUE (address);
          m_nodeUEInformation[r].dlsf_ = sf;                       // update SF for that UE
          m_nodeUEInformation[r].dlfreq_ = m_downlinkFrequency;               // update freq for that UE (downlink)

          return(1);
        }
      j++;
      if (j == MAX_NUM_FREQ)
        {
          j = 0;
        }
    }

  return(0);
}

/** Remove resources in the OSVF trees**/

int
UmtsRrcLayerBS::RemoveResources (int pos)
{
  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " RRC  **********  Removing UE with from OSVF            Time " << Simulator::Now () << "\n ");

  if (m_downlinkSFTree[m_nodeUEInformation[pos].dlfreq_ - 12]->RemoveSf (m_nodeUEInformation[pos].identifier, m_nodeUEInformation[pos].dlsf_))
    {
      // resources removed successfully
      return(1);
    }
  // resuorces not removed successfully
  return(0);
}


/** Calculates the spreading factor for a given physical rate*/

int
UmtsRrcLayerBS::CalculateSpreadingFactor (double rate,enum TxType txType)
{
  double index;         // bits per slot

  index = rate * UMTS_SlotTime / 8;       // bytes per slot in physical layer
  if (index > 0)         // calculate the SF for that physical rate
    {
      index = (2 * 320 / index);
      index = (int)(log (index) / log (2));
      int i, sf_ = 2;
      if (index > 9)
        {
          index = 9;
        }
      if (index < 2)
        {
          index = 2;
        }
      for (i = 0; i < ((int)index - 1); i++)
        {
          sf_ *= 2;
        }

      if (txType == Dedicated)
        {
          return(sf_);
        }
      else if (sf_ < 4)
        {
          return 0;
        }
      else
        {
          return(sf_);
        }

    }
  else
    {
      return(0);
    }
}

/**Look for the UE stored in the system according to a concrete Ipv4*/

int
UmtsRrcLayerBS::LookForUE (uint32_t address)
{
  int i;
  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_nodeUEInformation[i].identifier== address)
        {
          return(i);                    // found it
        }
    }
  return(-1);           // not found it
}

int
UmtsRrcLayerBS::IsUEFixedNode (uint32_t source)
{
  for (int i = 0; i < MAX_NUM_NODES; i++)
    {
      if (registry_[i].ue_ == source)           // UE
        {
          return(1);
        }
    }
  return(0);    // fixed node
}

} // namespace ns3
