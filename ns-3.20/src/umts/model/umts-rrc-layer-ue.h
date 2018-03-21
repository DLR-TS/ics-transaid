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

#ifndef UMTS_RRCLAYER_UE_H
#define UMTS_RRCLAYER_UE_H

#include <stdint.h>

#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"
#include "ns3/object.h"
#include "umts-manager.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"


namespace ns3 {

class ControlPacket;
class UmtsPhyLayerUE;
class UmtsIfqLayer;
class RLCNodeUE;
class UmtsRlcLayer;


class UmtsRrcLayerUE : public Object
{
public:
  UmtsRrcLayerUE ();
  void DoDispose ();
  static TypeId GetTypeId (void);

  void SetIpAddress (Ipv4Address address);
  //NEW
  void SetIpAddress (Ipv6Address address);
  void SetPhyLayer (Ptr<UmtsPhyLayerUE> phyLayer);  
  void SetIFQQueue (Ptr<UmtsIfqLayer> ifq);
  Ipv4Address GetIpAddress ();
  //NEW
  Ipv6Address GetIpv6Address ();
  void SetRlc (Ptr<UmtsRlcLayer> rlc);
  void ForwardUp (Ptr<Packet> packet);
  
  // Receive a packet from below, either control and data packet
  void PacketReceivedFromBelow (Ptr<Packet> packet);

  void SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback);
  
  void SendPagingOk (Ptr<ControlPacket> controlpacket);
  int PagingReceived(std::string type,uint32_t applicationId,uint32_t address);

  // Other Functions
  int GetSpreadingFactor (double rate,enum TxType txType);
  void UpdatePhyBytesInSlot ();
  void RemoveFlow (uint32_t flowId);
  double GetBytes (int pos);
  int UpdateFlow (int acked, int fraged, int flowidm,enum TxType txType,uint32_t address);
     
  double GetRate (double rate, uint32_t code, int psize);
  void SetHandoverState (int value);
  int InsertFlow (int flow, double rate, uint32_t code,enum TxType txType);
  int ConfigureNewFlow (std::string type, double rate,uint32_t applicationId,uint32_t address);

  void RemoveAllFlows ();
  void ResourceReplyReceived (uint32_t applicationId,uint32_t size);
  void SetupReplyReceived (Ptr<ControlPacket> controlpacket);
  void SetUplinkFrequency (int freq);  
  void RemoveFlowFromRegistry (int position);
  void CalculateRachSlotBytes (double txRate,double sf);
  
  void ReceiveFromAbove(Ptr<Packet> packet,Ipv4Address address);
  //NEW
  void ReceiveFromAbove(Ptr<Packet> packet,Ipv6Address address);
  void SendDown(Ptr<Packet> packet,enum TxType txType);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();
  void PrimitiveFromPhy(uint32_t applicationId);
  void SetNodeBIdentifier(uint32_t identifier);
  void RemoveDownLinkFlow(uint32_t applicationId);

private:
  
  apps_info m_applications[MAX_NUM_FLOWS];       // information about applications in the UE
  
  Ipv4Address m_myIp;
  //NEW
  Ipv6Address m_myIpv6;
  Ipv4Address m_nodeBIp;
  //NEW
  Ipv6Address m_nodeBIpv6;
  Ptr<UmtsPhyLayerUE> m_phyNodeUE;
    
  int m_handover;                                // 1, UE in handover procedure
  int m_uplinkFrequency;                                  // uplink frequence assigned to the UE
  
  Ptr<UmtsIfqLayer> m_ifqQueue;
  Callback<void, Ptr<Packet> > m_forwardUp;  
  Ptr<UmtsRlcLayer> m_rlc;

  uint32_t m_nodeIdentifier;
  uint32_t m_nodeBIdentifier;



};

} // namespace ns3


#endif /*RRC_NODEUE_H */
