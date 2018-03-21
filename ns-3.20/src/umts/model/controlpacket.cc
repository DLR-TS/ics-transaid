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
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/mac48-address.h"
#include "controlpacket.h"
#include "umts-queue.h"
#include "umts-phy-layer-ue.h"
#include "umts-phy-layer-bs.h"
#include "umts-userequipment-manager.h"


NS_LOG_COMPONENT_DEFINE ("ControlPacket");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ControlPacket);

TypeId
ControlPacket::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ControlPacket")
    .SetParent<Object> ()
    .AddConstructor<ControlPacket> ()
  ;
  return tid;
}

ControlPacket::ControlPacket ()
{

  phyInfo.dpdch_control_ = Create<UMTSQueue> ();
  SetRxPower (0);
}

ControlPacket::~ControlPacket ()
{

}

void
ControlPacket:: DoDispose ()
{


}

void
ControlPacket::SetSource (Ipv4Address address)
{
  commonInfo.source = address;
}

void
ControlPacket::SetSourcev6 (Ipv6Address address)
{
  commonInfo.sourcev6 = address;
}

Ipv4Address
ControlPacket::GetSource ()
{
  return commonInfo.source;
}

//NEW FUNCTION
Ipv6Address
ControlPacket::GetSourcev6 ()
{
  return commonInfo.sourcev6;
}

void
ControlPacket::SetDestination (Ipv4Address address)
{
  commonInfo.destination = address;
}

Ipv4Address
ControlPacket::GetDestination ()
{
  return commonInfo.destination;
}

void 
ControlPacket::SetEndOfPacket(int end)
{
 m_endOfPacket=end; 
}

int 
ControlPacket::GetEndOfPacket()
{
 return m_endOfPacket;
}

enum LLType
ControlPacket::GetLLType ()
{
  return commonInfo.lltype;
}

void
ControlPacket::SetLLType (enum LLType type)
{
  commonInfo.lltype = type;
}

enum UmtsChannelType
ControlPacket::GetChannel ()
{ 
 /*if(commonInfo.channel==SCCPCH)
  {
    //std::cout<<"ControlPacket::GetChannel:"<<commonInfo.channel<<std::endl;
  }*/
  return commonInfo.channel;
}

void
ControlPacket::SetChannel (enum UmtsChannelType channel_)
{
  commonInfo.channel = channel_;
}

void
ControlPacket::SetTxRate (float txRate_)
{
  phyInfo.txRate = txRate_;
}

float
ControlPacket::GetTxRate ()
{
  return phyInfo.txRate;
}

int
ControlPacket::GetSize ()
{
  return commonInfo.size;
}

void
ControlPacket::SetSize (int size_)
{
  commonInfo.size = size_;
}

uint32_t
ControlPacket::GetSourceNodeIdentifier ()
{
  return phyInfo.phySourceAddress;
}

void
ControlPacket::SetSourceNodeIdentifier (uint32_t address)
{
  phyInfo.phySourceAddress = address;
}

uint32_t
ControlPacket::GetDestinationNodeIdentifier ()
{
  return phyInfo.phyDestinationAddress;
}

void
ControlPacket::SetDestinationNodeIdentifier (uint32_t address)
{
  phyInfo.phyDestinationAddress = address;
}

void
ControlPacket::SetPacketType (enum PacketType type)
{
  commonInfo.ptype = type;
}

enum PacketType
ControlPacket::GetPacketType ()
{
  return commonInfo.ptype;
}

void
ControlPacket::SetPrimaryScramblingCode (int sc)
{
  phyInfo.primaryScramblingCode = sc;
}

int
ControlPacket::GetPrimaryScramblingCode ()
{
  return phyInfo.primaryScramblingCode;
}

void
ControlPacket::SetScramblingCode (int sc)
{
  phyInfo.scramblingCode = sc;
}

int
ControlPacket::GetScramblingCode ()
{
  return phyInfo.scramblingCode;
}

void
ControlPacket::SetUsedRach (int usedRach_)
{
  phyInfo.usedRach = usedRach_;
}

int
ControlPacket::GetUsedRach ()
{
  return phyInfo.usedRach;
}

void
ControlPacket::SetSpreadingFactor (int sf)
{
  phyInfo.spreadingFactor = sf;
}
int
ControlPacket::GetSpreadingFactor ()
{
  return phyInfo.spreadingFactor;
}

void
ControlPacket::SetChannelisationCode (int k)
{
  phyInfo.channelisationCode = k;
}

int
ControlPacket::GetChannelisationCode ()
{
  return phyInfo.channelisationCode;
}

void
ControlPacket::SetBeginOfSequence (int start)
{
  phyInfo.beginOfSequence = start;
}

void
ControlPacket::SetUmtsPhyLayerUEPointer (Ptr<UmtsPhyLayerUE> phyNodeUE)
{
  commonInfo.phyNodeUE = phyNodeUE;
}

Ptr<UmtsPhyLayerUE>
ControlPacket::GetUmtsPhyLayerUEPointer ()
{
  return commonInfo.phyNodeUE;
}


void
ControlPacket::SetUmtsPhyLayerBSPointer (Ptr<UmtsPhyLayerBS> phyNodeB)
{
  commonInfo.phyNodeB = phyNodeB;
}

Ptr<UmtsPhyLayerBS>
ControlPacket::GetUmtsPhyLayerBSPointer ()
{
  return commonInfo.phyNodeB;
}

int
ControlPacket::GetBeginOfSequence ()
{
  return phyInfo.beginOfSequence;
}

void
ControlPacket::SetEndOfSequence (int end)
{
  phyInfo.endOfSequence = end;
}

int
ControlPacket::GetEndOfSequence ()
{
  return phyInfo.endOfSequence;
}

void
ControlPacket::SetInitialAccess (int access)
{
  phyInfo.firstAccess = access;
}

int
ControlPacket::GetInitialAccess ()
{
  return phyInfo.firstAccess;
}

void
ControlPacket::SetSignature (int signature_)
{
  phyInfo.signature = signature_;
}

int
ControlPacket::GetSignature ()
{
  return phyInfo.signature;
}

void
ControlPacket::SetRxPower (double pw)
{
  phyInfo.rxPower = pw;
}

double
ControlPacket::GetRxPower ()
{
  return phyInfo.rxPower;
}

void
ControlPacket::SetPagingIndicator (int pagingIndicator)
{
  phyInfo.pagingIndicator = pagingIndicator;
}

int
ControlPacket::GetPagingIndicator ()
{
  return phyInfo.pagingIndicator;
}

int
ControlPacket::GetFreeResources (int index1,int index2)
{
  return phyInfo.freeResources[index1][index2];
}

void
ControlPacket::SetFreeResources (int index1,int index2,int value)
{
  phyInfo.freeResources[index1][index2] = value;
}

void
ControlPacket::SetAvailableRach (int index1,int value)
{
  phyInfo.rach[index1] = value;
}

int
ControlPacket::GetAvailableRach (int index1)
{
  return phyInfo.rach[index1];
}

void
ControlPacket::SetFrequency (int freq)
{
  phyInfo.Frequency = freq;
}

int
ControlPacket::GetFrequency ()
{
  return phyInfo.Frequency;
}

int
ControlPacket::GetNumberOfDpdch ()
{
  return phyInfo.dpcch_control_.numberOfDpdch;
}

void
ControlPacket::SetNumberOfDpdch (int number)
{
  phyInfo.dpcch_control_.numberOfDpdch = number;
}

double
ControlPacket::GetDpdchRate ()
{
  return phyInfo.dpcch_control_.dpdchRate;
}

void
ControlPacket::SetDpdchRate (double rate)
{
  phyInfo.dpcch_control_.dpdchRate = rate;
}

void
ControlPacket::InsertInDpdchQueue (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo)
{
  phyInfo.dpdch_control_->PhyEnqueue (packet,controlInfo);
}

std::pair<Ptr<Packet>, Ptr<ControlPacket> >
ControlPacket::GetDpdchPacket ()
{
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > packetStorage = phyInfo.dpdch_control_->PhyDequeue ();
  return packetStorage;
}

int
ControlPacket::GetDpdchQueueSize ()
{
  return phyInfo.dpdch_control_->GetPhySize ();
}

int
ControlPacket::GetSequenceNumber ()
{
  return phyInfo.sequenceNumber;
}

void
ControlPacket::SetSequenceNumber (int number)
{
  phyInfo.sequenceNumber = number;
}

void
ControlPacket::SetPagingOk (int value)
{
  phyInfo.paging_ok = value;
}

int
ControlPacket::GetPagingOk ()
{
  return phyInfo.paging_ok;
}

double
ControlPacket::GetDataSize ()
{
  return phyInfo.dataSize;
}
void
ControlPacket::SetDataSize (double value)
{
  phyInfo.dataSize = value;
}

void
ControlPacket::SetPagingGroup (int value)
{
  phyInfo.pagingGroup = value;
}

int
ControlPacket::GetPagingGroup ()
{
  return phyInfo.pagingGroup;
}

void
ControlPacket::SetPhyInformation (PhyInformation newInfo)
{
  phyInfo = newInfo;
}

void
ControlPacket::SetCommonInformation (CommonInformation newInfo)
{
  commonInfo = newInfo;

}

void
ControlPacket::SetTxType (enum TxType type)
{
  txType = type;
}

enum TxType
ControlPacket::GetTxType ()
{
  return txType;

}

void
ControlPacket::SetUmtsUserEquipmentManager (Ptr<UmtsUserEquipmentManager> manager_)
{
  manager = manager_;
}


Ptr<UmtsUserEquipmentManager>
ControlPacket::GetUmtsUserEquipmentManager ()
{
  return manager;
}

Ptr<ControlPacket>
ControlPacket::Copy ()
{
  Ptr<ControlPacket> newControlPacket = Create<ControlPacket> ();
  newControlPacket->SetPhyInformation (phyInfo);
  newControlPacket->SetCommonInformation (commonInfo);  
  newControlPacket->SetUmtsUserEquipmentManager (manager);
  newControlPacket->SetTxType (this->GetTxType ());    
  newControlPacket->SetEndOfPacket(this->GetEndOfPacket());
  return newControlPacket;

}

} // namespace ns3
