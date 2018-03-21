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

#ifndef UMTS_NET_DEVICE_H
#define UMTS_NET_DEVICE_H

#include <vector>
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/mac48-address.h"
#include "ns3/net-device.h"
#include "ns3/mobility-model.h"

namespace ns3 {

class UMTSChannel;
class UmtsMacLayer;
class UmtsRlcLayer;
class UmtsIfqLayer;
class ControlPacket;
class UmtsPhyLayerBS;
class UmtsRrcLayerBS;
class UmtsBaseStationManager;
class UmtsRlcLayer;
class UmtsPhyLayerUE;
class UmtsRrcLayerUE;
class UmtsUserEquipmentManager;


class UMTSNetDevice : public NetDevice
{
public:
  UMTSNetDevice ();
  ~UMTSNetDevice ();
  static TypeId GetTypeId (void);
  void CompleteConfig (uint32_t uid);
  void SetPhy (Ptr<Object> phy,Ptr<UMTSChannel> broadcastChannel);
  void SetMac (Ptr<Object> mac);
  void SetRlc (Ptr<Object> rlc);
  void SetRrc (Ptr<Object> rrc);
  void SetIFQ ();
  void SetManager ();
  void EnableIpv6(void);
  Ptr<Object> GetManager ();
  Ptr<Object> GetMac (void) const;
  Ptr<Object> GetPhy (void) const;
  Ptr<Object> GetRlc (void) const;
  Ptr<Object> GetRrc (void) const;
  void DoDispose (void);
  
  void ForwardUp (Ptr<Packet> packet);
  void SetNodeType(std::string type_);
  std::string GetNodeType();
  
  void AddNodeB (Ptr<UmtsBaseStationManager> manager_);
  void SetMobility(Ptr<MobilityModel> mobility);

  // inherited from NetDevice base class.
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void SetLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetName (std::string name_);
  virtual std::string GetName (void) const;
  void SetIpAddress();
  //NEW
  void SetIpv6Address();
  //NEW
  void SetIpv6Address(Ipv6Address address);
  virtual void SetAddress (Address address);  
  virtual void AddLinkChangeCallback (Callback<void> callback);  
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);

  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;

  uint32_t GetNodeIdentifier();

private:
  uint32_t m_ifIndex;
  Ptr<UMTSChannel> DoGetChannel (void) const;
  
  uint16_t mtuSize;
  bool m_linkUp;
  Ptr<UmtsPhyLayerBS> m_phyB;
  Ptr<UmtsMacLayer> m_mac;
  Ptr<UmtsRlcLayer> m_rlc;
  Ptr<UmtsIfqLayer> m_ifq;
  Ptr<UmtsRrcLayerBS> m_rrcB;
  Ptr<UmtsBaseStationManager> m_managerB;
  
  Ptr<UmtsPhyLayerUE> m_phyUE;
  Ptr<UmtsRrcLayerUE> m_rrcUE;
  Ptr<UmtsUserEquipmentManager> m_managerUE;

  std::string m_nodeType;
  uint32_t m_nodeIdentifier;
  
  Ptr<Node> m_node;
  NetDevice::ReceiveCallback m_forwardUp;
  std::string m_name;
  //bool m_EnableIpv6;  
  TracedCallback<> m_linkChanges;    

};

} // namespace ns3

#endif /* UMTS_NET_DEVICE_H */
