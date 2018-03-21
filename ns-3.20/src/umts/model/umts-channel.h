/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) YEAR COPYRIGHTHOLDER
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
 * Author: Sendoa Vaz
 */


#ifndef UMTS_CHANNEL_H
#define UMTS_CHANNEL_H

#include <vector>
#include "ns3/packet.h"
#include "ns3/channel.h"
#include <list>
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "umts-manager.h"

#include "cost231_model.h"

namespace ns3 {

class UmtsPhyLayerUE;
class UmtsPhyLayerBS;
class ControlPacket;

class UMTSChannel : public Channel
{
public:
  UMTSChannel ();
  ~UMTSChannel ();
  static TypeId GetTypeId (void);
  void SetUmtsPhyLayerBS (Ptr<UmtsPhyLayerBS> phyNodeB_);
  Ptr<UMTSChannel> GetChannel ();
  void SendToNodeB (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void ReceiveFromNodeUE (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void SendToUE (Ptr<UmtsPhyLayerUE> phyNodeUE,Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void ReceiveUnicastFromNodeB (uint32_t destinationPhyAddress, Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void ReceiveMulticastFromNodeB (std::list<uint32_t > destinationNodeIdentifiers, Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void ReceiveBroadcastFromNodeB (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void Add (Ptr<UmtsPhyLayerUE> phy,uint32_t phyAddress);
  void Dispose ();
  void RemoveNode (Ptr<UmtsPhyLayerUE> phyNodeUE);
  uint32_t GetNDevices (void) const { return 0; }
  Ptr<NetDevice> GetDevice (uint32_t i) const { return 0; }

  double CalculateRxPowerFromNodeB (Ptr<UmtsPhyLayerUE> phy,enum TxType type);
  double CalculateRxPowerFromNodeUE (Ptr<UmtsPhyLayerUE> phy,enum TxType type);

  void SetDedicatedChannelPeer (Ptr<UmtsPhyLayerUE> phy);
  double ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                            double nodeBHeight,double nodeUEHeight);

  void PowerChangeNotificationFromNodeB (int sign);


private:
  struct PhyNode
  {
    Ptr<UmtsPhyLayerUE> phyNodeUE;
    uint32_t phyAddress;
  };

  typedef std::list<struct PhyNode> UMTSPhyList;
  UMTSPhyList m_PhyList;

  Ptr<UmtsPhyLayerBS> m_phyNodeB;
  int m_associatedNodes;
  
  Ptr<Cost231Propagation> m_costPropagationModel;
  Ptr<UmtsPhyLayerUE> m_phyNodeUEDedicated;
    
};

} // namespace ns3

#endif /* UMTSCHANNEL_H */
