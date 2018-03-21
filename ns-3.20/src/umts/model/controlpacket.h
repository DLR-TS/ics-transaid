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

#ifndef CONTROL_PACKET_H
#define CONTROL_PACKET_H

#include <stdint.h>
#include <list>
#include <string.h>
#include "ns3/callback.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "umts-manager.h"

namespace ns3 {

class UmtsPhyLayerUE;
class UmtsPhyLayerBS;

class ControlPacket : public Object
{
public:
  ControlPacket ();
  ~ControlPacket ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  
  void SetSource (Ipv4Address address);
  //NEW
  void SetSourcev6 (Ipv6Address address);
  Ipv4Address GetSource ();
  //NEW
  Ipv6Address GetSourcev6 ();
  void SetDestination (Ipv4Address address);
  Ipv4Address GetDestination ();
  enum LLType GetLLType ();
  void SetLLType (enum LLType type);
  enum UmtsChannelType GetChannel ();
  void SetChannel (enum UmtsChannelType channel_);  
  void SetTxRate (float txRate_);
  float GetTxRate ();
  int GetSize ();
  void SetSize (int size_);
  uint32_t GetSourceNodeIdentifier ();
  void SetSourceNodeIdentifier (uint32_t address);
  uint32_t GetDestinationNodeIdentifier ();
  void SetDestinationNodeIdentifier (uint32_t address);
  void SetPacketType (enum PacketType type);
  enum PacketType GetPacketType ();
  void SetPrimaryScramblingCode (int sc);
  int GetPrimaryScramblingCode ();
  void SetScramblingCode (int sc);
  int GetScramblingCode ();
  void SetUsedRach (int usedRach_);
  int GetUsedRach ();
  void SetSignature (int signature_);
  int GetSignature ();
  void SetSpreadingFactor (int sf);
  int GetSpreadingFactor ();
  void SetChannelisationCode (int k);
  int GetChannelisationCode ();
  void SetBeginOfSequence (int start);
  int GetBeginOfSequence ();
  void SetEndOfSequence (int end);
  int GetEndOfSequence ();
  void SetInitialAccess (int access);
  int GetInitialAccess ();
  void SetRxPower (double pw);
  double GetRxPower ();
  void SetUmtsPhyLayerUEPointer (Ptr<UmtsPhyLayerUE> phyNodeUE);
  Ptr<UmtsPhyLayerUE> GetUmtsPhyLayerUEPointer ();
  void SetUmtsPhyLayerBSPointer (Ptr<UmtsPhyLayerBS> phyNodeB);
  Ptr<UmtsPhyLayerBS> GetUmtsPhyLayerBSPointer ();
  void SetPagingIndicator (int pi);
  int GetPagingIndicator ();
  int GetFreeResources (int index1,int index2);
  void SetFreeResources (int index1,int index2,int value);
  void SetAvailableRach (int index1,int value);
  int GetAvailableRach (int index1);
  void SetFrequency (int freq);
  int GetFrequency ();
  int GetNumberOfDpdch ();
  void SetNumberOfDpdch (int number);
  double GetDpdchRate ();
  void SetDpdchRate (double rate);
  void InsertInDpdchQueue (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo);
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > GetDpdchPacket ();
  int GetDpdchQueueSize ();
  int GetSequenceNumber ();
  void SetSequenceNumber (int number);
  int GetPagingOk ();
  void SetPagingOk (int value);
  double GetDataSize ();
  void SetDataSize (double value);
  void SetPagingGroup (int value);
  int GetPagingGroup ();
  void SetEndOfPacket(int end);
  int GetEndOfPacket();
  void SetPhyInformation (PhyInformation newInfo);
  void SetCommonInformation (CommonInformation newInfo);  
  void SetUmtsUserEquipmentManager (Ptr<UmtsUserEquipmentManager> manager_);
  Ptr<UmtsUserEquipmentManager> GetUmtsUserEquipmentManager (); 
  void SetTxType (enum TxType type);
  enum TxType GetTxType ();

  Ptr<ControlPacket> Copy ();
  PhyInformation phyInfo;
  CommonInformation commonInfo;  
  Ptr<UmtsUserEquipmentManager> manager;  
  enum TxType txType;  
  int m_endOfPacket;

};

} // namespace ns3


#endif
