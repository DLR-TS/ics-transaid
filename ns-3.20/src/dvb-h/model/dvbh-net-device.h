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

#ifndef DVBH_NET_DEVICE_H
#define DVBH_NET_DEVICE_H

#include <vector>
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/mac48-address.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"


namespace ns3 {

class DVBHControlPacket;
class DVBHLinkLayerBaseStation;
class DVBHPhyLayerBaseStation;
class DVBHLinkLayerUserEquip;
class DVBHPhyLayerUserEquip;
class DVBHManagerUserEquip;
class DVBHManagerBaseStation;
class DVBHOfdmLayer;
class DVBHChannel;

class DvbhNetDevice : public NetDevice
{
public:
  DvbhNetDevice ();
  ~DvbhNetDevice ();
  static TypeId GetTypeId (void);
  void CompleteConfig (uint32_t id);

  void DoDispose (void);

  void ForwardUp (Ptr<Packet> packet);
  void SetPhyLayer (Ptr<Object> phy);
  void SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm);
  void SetLinkLayer (Ptr<Object> linkLayer);
  void SetManager ();
  Ptr<Object> GetManager ();
  Ptr<Object> GetPhyLayer ();
  Ptr<Object> GetLinkLayer ();
  Ptr<DVBHOfdmLayer> GetOfdmLayer ();

  // inherited from NetDevice base class.
  void SetIfIndex (const uint32_t index);
  uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  Address GetAddress (void) const;
  bool SetMtu (const uint16_t mtu);
  uint16_t GetMtu (void) const;
  bool IsLinkUp (void) const;
  virtual void SetLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  void SetIpAddress();
  void SetName (std::string name_){}
  std::string GetName (void) const { return NULL; }
  
  void SetNodeType(std::string type_);
  std::string GetNodeType();  
  void SetMobility(Ptr<MobilityModel> mobility);
  
  void SetAddress (Address address);

  void AddLinkChangeCallback (Callback<void> callback)
  {
  }
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);

  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;

private:
  uint32_t m_ifIndex;
  Ptr<DVBHChannel> DoGetChannel (void) const;
  Ptr<DVBHPhyLayerBaseStation> m_phyB;
  Ptr<DVBHPhyLayerUserEquip> m_phyUE;
  Ptr<DVBHOfdmLayer> m_ofdm;
  Ptr<DVBHManagerBaseStation> m_managerB;
  Ptr<DVBHManagerUserEquip> m_managerUE;
  Ptr<DVBHLinkLayerBaseStation> m_linkLayerB;
  Ptr<DVBHLinkLayerUserEquip> m_linkLayerUE;
  
  int mtuSize;
  std::string m_nodeType;
  Address m_address;  
  Ptr<Node> m_node;
  NetDevice::ReceiveCallback m_forwardUp;
  
  uint32_t m_nodeIdentifier;


};

} // namespace ns3

#endif /* UMTS_NET_DEVICE_BaseStation_H */
