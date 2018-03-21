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


#ifndef UMTS_RRCLAYER_BS_H
#define UMTS_RRCLAYER_BS_H

#include <stdint.h>
#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "umts-manager.h"

namespace ns3 {

class ControlPacket;
class UmtsPhyLayerBS;
class UmtsIfqLayer;
class OvsfTree;
class UmtsRlcLayer;

class UmtsRrcLayerBS : public Object
{
public:
  UmtsRrcLayerBS ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetIpAddress (Ipv4Address address);
  //NEW
  void SetIpv6Address (Ipv6Address address);
  Ipv4Address GetIpAddress ();
  //NEW
  Ipv6Address GetIpv6Address ();
  Ptr<UmtsPhyLayerBS> GetUmtsPhyLayerBS ();
  void SetUmtsPhyLayerBS (Ptr<UmtsPhyLayerBS> phyLayer);
  void SetRlc (Ptr<UmtsRlcLayer> rlc);
  void ForwardUp (Ptr<Packet> packet);
  void SetIFQQueue (Ptr<UmtsIfqLayer> ifq);
  void SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback);  
  void PacketReceivedFromBelow (Ptr<Packet> packet);
  uint32_t FindNodeIdentifier(Ipv4Address address);
  //NEW
  uint32_t FindNodeIdentifier(Ipv6Address address);

  void SendDown(Ptr<Packet> packet);
  int AvailableResources (uint32_t address,int k);
  int LookForUE (uint32_t address);
  int CalculateSpreadingFactor (double rate,enum TxType txType);
  int CalculateSpreadingFactor (uint32_t address,int k);
  double GetRate (double rate, uint32_t code, int psize);
  int RegisterNewUE (Ptr<ControlPacket> controlpacket);
  void UpdateNodeRegistry (uint32_t address);
  int AllocateResources (uint32_t address, int sf);
  int RemoveResources (int pos);
  int IsUEFixedNode (uint32_t source);
  double CalculateBytes (int pos);
  void CalculateAndUpdateBytesInPhy (int pos);
  double GetRate (int sf);

  // Flow Management

  int UpdateFlow (int acked, int fraged, int flowid,uint32_t destinationId, int handover,enum TxType txType);
  void RemoveUpLinkFlow (uint32_t flowId, int pos);
  void RemoveDownLinkFlow (uint32_t destinationId,uint32_t applicationId);
  int DetectFlow (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket,int pos);
  void ReceiveDataPacketsFromAbove (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket,int pos);
  int ConfigureNewFlow (std::string type, double rate,uint32_t destinationId,uint32_t applicationId,Ipv4Address address);
  
  //NEW
  int ConfigureNewFlow (std::string type, double rate,uint32_t destinationId,uint32_t applicationId,Ipv6Address address);
  int InsertFlow (int flow, double rate, uint32_t code,Ipv4Address address,int isMulticast,int isBroadcast,enum TxType txType);
  
  //NEW
  int InsertFlow (int flow, double rate, uint32_t code,Ipv6Address address,int isMulticast,int isBroadcast,enum TxType txType);
  int PagingOkReceived (uint32_t address,uint32_t applicationId);
  void SetDownlinkFrequency (int freq);  
  void RemoveFlowFromRegistry (int position);
  void CalculateFachSlotBytes (double txRate,double sf);  
  void ReceiveFromAbove(Ptr<Packet> packet,Ipv4Address address);
  //NEW
  void ReceiveFromAbove(Ptr<Packet> packet,Ipv6Address address);
  int ResourceRequestReceived(std::string type,uint32_t applicationId,uint32_t destinationId);
  void SendDown(Ptr<Packet> packet,enum TxType txType,bool isMulticast,bool isBroadcast);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();
  void NodeUEConnectionLoss(uint32_t nodeIdentifier,Ipv4Address address);
  int AddUEIpv6Address (uint32_t NodeId, Ipv6Address Address);

private:

  Callback<void, Ptr<Packet> > m_forwardUp;
  Ptr<UmtsPhyLayerBS> m_phyNodeB;
  //NEW
  //Ptr<UmtsPhyLayerBS> m_phyNodeBv6;
  Ipv4Address m_myIp;
  //NEW
  Ipv6Address m_myIpv6;  
  Ptr<UmtsIfqLayer> m_ifqQueue;
  Ptr<OvsfTree> m_downlinkSFTree[MAX_NUM_FREQ];        // OVSF trees  
  apps_info m_applications[MAX_NUM_FLOWS_DL];    // information about applications in the NodeB
      
  std::list<Ptr<Packet> > m_packetQueue[MAX_NUM_FLOWS_DL];  
  NodeUEInformation m_nodeUEInformation[MAX_NUM_UE];       // information about the UEs in our cell  
  static location registry_[MAX_NUM_NODES];     // It stores the collection of pairs NodeUE-Attached BaseStation
  Ptr<UmtsRlcLayer> m_rlc;
  //NEW
  //Ptr<UmtsRlcLayer> m_rlcv6;
  int m_downlinkFrequency;  
  uint32_t m_nodeIdentifier;


};

} // namespace ns3


#endif /*RRC_NODEB_H */
