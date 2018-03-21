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
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "umts-rrc-layer-ue.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "controlpacket.h"
#include "umts-rlc-layer.h"
#include "umts-queue.h"
#include "umts-phy-layer-ue.h"
#include "umts-manager.h"
#include "umts-ifq-layer.h"
#include "ns3/random-variable.h"
#include "umts-tags.h"
#include "umts-userequipment-manager.h"
#include "ns3/node-id-tag.h" 
#include "ns3/app-index-tag.h"
#include "umts-rlc-layer.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRrcLayerUE");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRrcLayerUE);

TypeId
UmtsRrcLayerUE::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRrcLayerUE")
    .SetParent<Object> ()
    .AddConstructor<UmtsRrcLayerUE> ()
  ;
  return tid;
}
void
UmtsRrcLayerUE::DoDispose ()
{

}

UmtsRrcLayerUE::UmtsRrcLayerUE ()
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      RemoveFlowFromRegistry (i);
    }

  m_handover = 0;
  m_uplinkFrequency = 0;  
  m_myIp.Set ("0");

  m_nodeIdentifier = (uint32_t)-1;
  m_nodeBIdentifier=(uint32_t)-1;

}


void
UmtsRrcLayerUE::SetIpAddress (Ipv4Address address)
{
  m_myIp = address;
  m_rlc->SetIpAddress (address);
  m_phyNodeUE->SetMyIpAddress (address);
}


void
UmtsRrcLayerUE::SetIpAddress (Ipv6Address address)
{
  m_myIpv6 = address;
  m_rlc->SetIpv6Address (address);
  m_phyNodeUE->SetMyIpAddress (address);
}


Ipv4Address
UmtsRrcLayerUE::GetIpAddress ()
{
  return m_myIp;
}

Ipv6Address
UmtsRrcLayerUE::GetIpv6Address ()
{
  return m_myIpv6;
}

void
UmtsRrcLayerUE::SetPhyLayer (Ptr<UmtsPhyLayerUE> phyLayer)
{
  m_phyNodeUE = phyLayer;
  m_phyNodeUE->SetForwardToRRCCallback (MakeCallback (&UmtsRrcLayerUE::PrimitiveFromPhy, this));  
}

void
UmtsRrcLayerUE::SetRlc (Ptr<UmtsRlcLayer> rlc)
{
  m_rlc = rlc;
  m_rlc->SetForwardUpCallback (MakeCallback (&UmtsRrcLayerUE::PacketReceivedFromBelow, this));
}

void
UmtsRrcLayerUE::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
  m_rlc->SetNodeIdentifier(identifier);
}

uint32_t
UmtsRrcLayerUE::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

void
UmtsRrcLayerUE::SetNodeBIdentifier(uint32_t identifier)
{
  m_nodeBIdentifier=identifier;

}

void
UmtsRrcLayerUE::SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_forwardUp = upCallback;
}


void
UmtsRrcLayerUE::ForwardUp (Ptr<Packet> packet)
{
  m_forwardUp (packet);
}

void
UmtsRrcLayerUE::SetIFQQueue (Ptr<UmtsIfqLayer> ifq)
{
  m_ifqQueue = ifq;
  m_ifqQueue->SetRLCLayer (m_rlc);
  m_ifqQueue->StartUmtsIfqLayer ();
}

void
UmtsRrcLayerUE::SetUplinkFrequency (int freq)
{
  m_uplinkFrequency = freq;
}


void
UmtsRrcLayerUE::SetHandoverState (int value)
{
  m_handover = value;  
}

void
UmtsRrcLayerUE::ReceiveFromAbove(Ptr<Packet> packet,Ipv4Address address)
{
  int i;
  
  
  NodeIdTag tag;
  packet->PeekPacketTag(tag);
  
  AppIndexTag idTag;
  packet->PeekPacketTag(idTag);
    
  // look for the flow

  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (m_applications[i].flowid_ == idTag.Get())	
        {
	  UmtsCodeTag codeTag;
	  codeTag.Set(m_applications[i].c_coding_);
	  packet->AddPacketTag(codeTag);
	  		  
	  m_applications[i].dest_ = address;
	  
          if (m_applications[i].wait_ == 1)
            {            
              // resources already allocated              
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);
              SendDown(packet->Copy (),m_applications[i].txType);                          // send packet to IFQ
              
            }

          else
            {
              // is the first packet of that flow
              // calculate the physical rate that is going to send over the air interface
              m_applications[i].rate_ = GetRate (m_applications[i].user_rate_, m_applications[i].c_coding_, packet->GetSize ());
              // calculate the interval between two packets
              m_applications[i].interval_ = (packet->GetSize () * 8) / m_applications[i].user_rate_;

              m_applications[i].dest_ = address;
              	      
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);
	      
              m_applications[i].sf_ = GetSpreadingFactor (m_applications[i].rate_,m_applications[i].txType);

              if (m_applications[i].sf_ != 0)
                {                                                     
                  if (m_applications[i].txType == Common)
                    {
                      CalculateRachSlotBytes (m_applications[i].rate_,m_applications[i].sf_);	      
                      SendDown (packet,m_applications[i].txType);                      
                    }
                  else
                    {
		      m_applications[i].wait_ = 1;                     // put wait = 0 to prevent sending messages
		      SendDown (packet,m_applications[i].txType);                      
                    }
                }
              else
                {
                  NS_LOG_DEBUG ("Node UE. The Common Tx has a Tx Rate greater than the allowed maximum");
                  
                }
            }
            
            break;
        }
    }
  
}


void
UmtsRrcLayerUE::ReceiveFromAbove(Ptr<Packet> packet,Ipv6Address address)
{
 //std::cout<<"RRC LAYER 1!"<<std::endl;
  int i;
  
  NodeIdTag tag;
  packet->PeekPacketTag(tag);
  
  AppIndexTag idTag;
  packet->PeekPacketTag(idTag);
 //std::cout<<"idTag.Get()"<<idTag.Get()<<std::endl;
  // look for the flow
 
  //REMOVE THIS WHEN YOU ARE DONE!!!
  //SendDown (packet,m_applications[0].txType); 
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
     //std::cout<<"m_applications[i].flowid_"<<m_applications[i].flowid_<<std::endl;
      if (m_applications[i].flowid_ == idTag.Get())	
        {
          //std::cout<<"RRC LAYER 2!"<<std::endl;
          // flowid already registered
	  
	  UmtsCodeTag codeTag;
	  codeTag.Set(m_applications[i].c_coding_);
	  packet->AddPacketTag(codeTag);
	  		  
	  m_applications[i].destv6_ = address;
	 //std::cout<<"m_applications[i].wait_"<<m_applications[i].wait_<<std::endl;
          if (m_applications[i].wait_ == 1)
            {            
              // resources already allocated              
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);
              SendDown(packet->Copy (),m_applications[i].txType);                          // send packet to IFQ
              
            }

          else
            {
              // is the first packet of that flow
              // calculate the physical rate that is going to send over the air interface
              m_applications[i].rate_ = GetRate (m_applications[i].user_rate_, m_applications[i].c_coding_, packet->GetSize ());
              // calculate the interval between two packets
              m_applications[i].interval_ = (packet->GetSize () * 8) / m_applications[i].user_rate_;

              m_applications[i].destv6_ = address;
              	      
	      UmtsTxRateTag rateTag;
	      rateTag.Set(m_applications[i].user_rate_);
	      packet->AddPacketTag(rateTag);
	      
              m_applications[i].sf_ = GetSpreadingFactor (m_applications[i].rate_,m_applications[i].txType);

              if (m_applications[i].sf_ != 0)
                {                                                     
                  if (m_applications[i].txType == Common)
                    {
                      CalculateRachSlotBytes (m_applications[i].rate_,m_applications[i].sf_);	      
          
                      SendDown (packet,m_applications[i].txType);                      
                    }
                  else
                    {
		      m_applications[i].wait_ = 1;                     // put wait = 0 to prevent sending 
                      SendDown (packet,m_applications[i].txType);                      
                    }
                }
              else
                {
                  NS_LOG_DEBUG ("Node UE. The Common Tx has a Tx Rate greater than the allowed maximum");
                  
                }
            }
            
            break;
        }
    }
  
}

void
UmtsRrcLayerUE::SendDown(Ptr<Packet> packet,enum TxType txType)
{
  UmtsReceiverIdTag receiverId;
  receiverId.Set(m_nodeBIdentifier);
  packet->AddPacketTag(receiverId);
   
  if (txType == Dedicated)
    {
      
      UmtsPacketTypeTag tag;
      tag.Set(0);
      
      packet->AddPacketTag(tag);
      packet->Print(std::cout);      
      m_ifqQueue->ReceivePacketFromRRC (packet);
    }
  else
    { 
      UmtsPacketTypeTag tag;
      tag.Set(1);
      
      packet->AddPacketTag(tag);   
      //packet->Print(std::cout);
      m_rlc->ReceiveFromAbove (packet,receiverId.Get());
    }

}

void
UmtsRrcLayerUE::PacketReceivedFromBelow (Ptr<Packet> packet)
{
 //std::cout<<"!!! Packet Received from RRC Layer!!!"<<std::endl;    
  UmtsPacketTypeTag typeTag;  
  packet->PeekPacketTag(typeTag);
  
  UmtsReceiverIdTag receiverTag;
  packet->PeekPacketTag(receiverTag);
  
  NodeIdTag senderTag;
  packet->PeekPacketTag(senderTag);
      
  if(typeTag.Get()==CommonUnicastData)
  {    
      if (receiverTag.Get()== m_nodeIdentifier)
	{	  
	  ForwardUp (packet);
	}       
  }
  else if(typeTag.Get()==MulticastData||typeTag.Get()==BroadcastData)
  {      
      ForwardUp (packet);
  }
  else if(typeTag.Get()==DedicatedUnicastData)
  {    
      NS_LOG_DEBUG ("Node UE " << m_nodeIdentifier << " RRC //Dedicated Data Packet has arrived for me from NodeB " << senderTag.Get()<< " with size"<<packet->GetSize());
      ForwardUp (packet);                   // send the packet up through the scheduler
  }
}

//It is called from lower layer (i.e. UMTS PHY)
void
UmtsRrcLayerUE::PrimitiveFromPhy(uint32_t applicationId)
{
  NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " RRC//RRC Failure Packet received from below.Time " << Simulator::Now ());
  
  // It has been impossible to send the RACH message because
  //	- we have not received response to the preamble or
  //	- we have not found available resources to send the message
  
  RemoveFlow(applicationId);
}

void
UmtsRrcLayerUE::SetupReplyReceived (Ptr<ControlPacket> controlpacket)
{  
  m_uplinkFrequency = controlpacket->GetFrequency ();     // take the new frequence
  m_nodeBIdentifier= controlpacket->GetSourceNodeIdentifier();   // take the Node B physical address
  m_nodeBIp = controlpacket->GetSource ();           // take the Node B IP address

}


/**-------------------------------------------Management of the Signalling Messages--------------------------*/

void
UmtsRrcLayerUE::ResourceReplyReceived (uint32_t applicationId,uint32_t size)
{  
  for (int i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if ((applicationId == m_applications[i].flowid_) && (m_applications[i].wait_ == 0))
        {
          // update fields
          m_applications[i].wait_ = 1;
	  
	    // is the first packet of that flow
              // calculate the physical rate that is going to send over the air interface
              m_applications[i].rate_ = GetRate (m_applications[i].user_rate_, m_applications[i].c_coding_, size);
              // calculate the interval between two packets
              m_applications[i].interval_ = (size * 8) / m_applications[i].user_rate_;                                           
              // spreading factor needed in physical layer
              m_applications[i].sf_ = GetSpreadingFactor (m_applications[i].rate_,m_applications[i].txType);
	  
          double bytes = GetBytes (i);                  // get bytes for IFQ
          NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " RRC// Registering flowid " << applicationId<< " and bytes " << bytes << ".Freq es " << m_uplinkFrequency << " rate "<<m_applications[i].rate_<<" i "<<m_applications[i].sf_<<"            Time " << Simulator::Now ());
          m_ifqQueue->RegistrationNodeUE (applicationId, bytes);                  // register the flow in IFQ
          UpdatePhyBytesInSlot ();              // calculate bytes in physical layer
                  
          break;
        }
    }
}

//Paging Messages are received only when the Base Station is trying to set a dedicated connection with the NodeUE

int
UmtsRrcLayerUE::PagingReceived(std::string type,uint32_t applicationId,uint32_t address)
{
  int acked, fraged;
  // variabile dichiarata e non usata
  //uint32_t code;
    
  //code=CONV_HALF;
  fraged=1;
  acked=1;
      
  return UpdateFlow (acked,fraged,applicationId,Dedicated,address);
}

// give resources in RLC layer for the flow
int
UmtsRrcLayerUE::UpdateFlow (int acked, int fraged, int flowid,enum TxType txType,uint32_t address)
{
  int value=m_rlc->store_flow (acked, fraged, flowid,address,0,txType);               
  return value;
}

// configure a new flow in RRC layer
int
UmtsRrcLayerUE::ConfigureNewFlow (std::string type, double rate,uint32_t applicationId,uint32_t address)
{

  int acked, fraged;
  uint32_t code;
  enum TxType txType;
 
  // depending on the type of application, chose the mode (ACK or non-ACK),
  // the channel coding and if is necessary fragmentation (default fragmentation)

  if(type=="UM-NON_FRAG")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
  }
  else if(type=="UM-FRAG")
  {
      code = CONV_THIRD;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 0; 
      txType=Common;
  }
  else if(type=="AM")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 1;                       // fragmentation
      acked = 1; 
      txType=Dedicated;
  }

  else if(type=="MULTICAST")
  {
      code = CONV_HALF;                 // convolutional half
      fraged = 0;                       // fragmentation
      acked = 0; 
      txType=Common;
  }

  // register the new flow with its characteristics in the m_applications[] array
  int i = InsertFlow (applicationId, rate, code,txType);
  //std::cout<<"INSERT FLOW!!!"<<i<<std::endl;
  if (i == -1)
    {
	 //std::cout<<"UmtsRrcLayerUE::ConfigureNewFlow too many apps"<<std::endl;
      // too many applications in this terminal

      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " RRC// Too many application in this NodeUE, not registring the new Flow. Time " << Simulator::Now () << "\n");
      return -1;
    }
  else
    {
      // register the new flow in RLC layer
      //std::cout<<"Before Update!!!"<<std::endl;
      UpdateFlow (acked, fraged, applicationId,txType,address);
      return 1;
    }

}

// register a new application in RRC layer
int
UmtsRrcLayerUE::InsertFlow (int flow, double rate, uint32_t code,enum TxType txType)
{
  int i;
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      // looks for a free position
      //std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!! UINT!!!!!!!!!!!!!!!!!"<<(uint32_t)-1<<std::endl;
      //we have initialized all the flowids = (uint32_t) -1. Thus we can add our new flowid in the 
      //first place that we find that has this value.
	  // faccio il cast di flow a uint64_t, stesso tipo di flowid
	  if (m_applications[i].flowid_ == (uint64_t)flow)
		  return(i);
      if (m_applications[i].flowid_ == (uint32_t)-1)
        {          
          m_applications[i].flowid_ = flow;
          m_applications[i].wait_ = 0;
          m_applications[i].user_rate_ = rate;
          m_applications[i].c_coding_ = code;
          m_applications[i].txType = txType;
          // NS_LOG_DEBUG("UE Node New application REGISTERED with i"<<i<<" flowid "<<m_applications[i].flowid_<< " rate "<<rate);
          return(i);
        }
    }
  // there is not free position, ERROR
  return (-1);
}

void
UmtsRrcLayerUE::RemoveDownLinkFlow(uint32_t applicationId)
{
  m_rlc->remove_flow (applicationId,m_nodeBIdentifier);            // remove resources from RLC
}

// remove resources allocated for a flow
void
UmtsRrcLayerUE::RemoveFlow (uint32_t flowId)
{
  int i;
  int isFlowActive = 0;
  // looks for the flow in the array
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (m_applications[i].flowid_ == flowId)
        {
          isFlowActive = 1;
          break;
        }
    }

  if (isFlowActive)
    {
      m_rlc->remove_flow (m_applications[i].flowid_,m_nodeBIdentifier);            // remove resources from RLC
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier<< " RRC// Removing Flow " << m_applications[i].flowid_ << ".Time " << Simulator::Now ());
      if (m_applications[i].txType == Dedicated)
        {
          m_ifqQueue->RegistrationNodeUE (m_applications[i].flowid_,0.0);                // remove resources from IFQ
        }
      RemoveFlowFromRegistry (i);


      UpdatePhyBytesInSlot ();                          // remove resources from PHY
    }

}

void
UmtsRrcLayerUE::RemoveFlowFromRegistry (int position)
{
  //m_applications[position].flowid_ = 10;
  m_applications[position].flowid_ = (uint32_t)-1;
  m_applications[position].dest_.Set ("0");
  m_applications[position].serviceId="NULL";
  //m_applications[position].wait_ = 1;
  m_applications[position].wait_ = -1;
  m_applications[position].sf_ = -1;
  m_applications[position].rate_ = -1;
  m_applications[position].user_rate_ = -1;
  m_applications[position].interval_ = -1;
  m_applications[position].c_coding_ = CONV_HALF;
  m_applications[position].txType = Common;
}

void
UmtsRrcLayerUE::RemoveAllFlows ()
{
  for (int i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (m_applications[i].flowid_ != (uint32_t)-1)
        {
          m_rlc->remove_flow (m_applications[i].flowid_,m_nodeBIdentifier);                // remove resources from RLC
          NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " RRC// Removing Flow " << m_applications[i].flowid_ << ".Time " << Simulator::Now () << "\n");
          if (m_applications[i].txType == Dedicated)
            {
              m_ifqQueue->RegistrationNodeUE (m_applications[i].flowid_,0.0);                    // remove resources from IFQ

            }
          RemoveFlowFromRegistry (i);
          UpdatePhyBytesInSlot ();                              // remove resources from PHY
        }
    }
}

/**---------------------------------------------Necessary Calculations--------------------------------*/

// calculate the physical rate
double
UmtsRrcLayerUE::GetRate (double rate, uint32_t code, int psize)
{
  double rt, time;
  int rlcfsize = m_rlc->GetFragmentationSize ();                // rlc fragment size
  int rlchdrsize = RLC_HDR_SZ;                          // rlc-umts header size
  int machdrsize = MAC_HDR_SZ;                          // mac-umts header size
  // int phyhdrsize = PHY_HDR_SZ;			// phy-umts header size

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

// calculate the needed spreading factor for a given rate
int
UmtsRrcLayerUE::GetSpreadingFactor (double rate,enum TxType txType)
{
  double index = rate * UMTS_SlotTime / 8;       // bytes per slot in physical layer
  if (index > 0)
    {
      index = (320 / index);
      index = (int)(log (index) / log (2));
      int i, sf_ = 2;      
      if (index > 9)
        {
          index = 9;                            // the highest spreading factor is 256
        }
      if (index < 2)
        {
          index = 2;                            // the lowest spreading factor is 4
        }
      for (i = 0; i < ((int)index - 1); i++)
        {
          sf_ *= 2;
        }
      
      if (txType == Dedicated)
        {	  
          return(sf_);
        }
      else if (sf_ < 32)
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


// calculate bytes per slot in physical layer and update them
void
UmtsRrcLayerUE::UpdatePhyBytesInSlot (void)
{
  NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " RRC// Updating Resources in the PHY Layer. Time " << Simulator::Now ());
  int i;
  double bytes_slot, sum = 0.0;

  // add the physical rates of all the applications currently running
  for (i = 0; i < MAX_NUM_FLOWS; i++)
    {
      if (m_applications[i].flowid_ !=(uint32_t)-1)
        {
          sum += m_applications[i].rate_;
        }
    }

 
  if (sum > 0)
    {
      // gets the spreading factor needed for supporting the total rate
      if(2560 / GetSpreadingFactor (sum,Dedicated)<=8)
      {
	bytes_slot=1;
      }
      else
      {
	 bytes_slot = (2560 / GetSpreadingFactor (sum,Dedicated)) / 8;             // calculate bytes in physical layer
       
	if (sum > 960000)                         // if more than one dpdch is requiered
	  {
	    bytes_slot = bytes_slot * ((int)(sum / 960000) + 1);
	  }
      }
     
      // NS_LOG_DEBUG("UE %d at %f RRC: ~~~~~~~~~~~~~~~~~~~~~ bytes_slot in PHY "<<bytes_slot<<" ~~~~~~~~~~~~~~\n");
    }
  else
    {
      // no applications running in that moment
      bytes_slot = 0.0;
    }
 
  m_phyNodeUE->StoreSlotBytes (bytes_slot);       // update bytes per slot in physical layer
  return;
}

void
UmtsRrcLayerUE::CalculateRachSlotBytes (double txRate,double sf)
{
  double bytes_slot;

  if (txRate > 0)
    {
      // bytes per slot in physical layer for the actual SF for that UE
      bytes_slot = ((2560 / (double)sf) / 8);
    }
  else
    {
      bytes_slot = 0.0;         // none application for that UE
    }

  m_phyNodeUE->StoreRachSlotBytes (bytes_slot);   // pass the result to phy layer
}

// bytes in ifq per tti
double
UmtsRrcLayerUE::GetBytes (int pos)
{
  double bits, sum;
  int rlcfsize = m_rlc->GetFragmentationSize ();        // rlc fragment size
  int rlchdrsize = RLC_HDR_SZ;                          // rlc-umts header size
  int machdrsize = MAC_HDR_SZ;                          // mac-umts header size
  int phyhdrsize = PHY_HDR_SZ;                          // phy-umts header size

  bits = (2560 / m_applications[pos].sf_) * (m_ifqQueue->GetTTI () / UMTS_SlotTime);       // bits per tti
  sum = m_applications[pos].rate_;
  if (sum > 0)
    {
      if (sum > 960000)           // more than one dpdch is required
        {
          bits = bits * ((int)(sum / 960000) + 1);
        }
    }
  bits = (bits * (rlcfsize + rlchdrsize + machdrsize)) / (rlcfsize + rlchdrsize + machdrsize + phyhdrsize);

  // apply decoding
  switch (m_applications[pos].c_coding_)
    {
    case TURBO:              // turbo coding
      bits = bits / 3;
      break;
    case CONV_HALF:              // convolutional half
      bits = bits / 2;
      break;
    case CONV_THIRD:               // convolutional third
      bits = bits / 3;
      break;
    default:
      break;
    }
  // bytes in IFQ for an application per TTI
  bits = (bits * rlcfsize) / (rlcfsize + rlchdrsize + machdrsize);

  return(bits / 8);      // bits in IFQ for that application per TTI
}




} // namespace ns3
