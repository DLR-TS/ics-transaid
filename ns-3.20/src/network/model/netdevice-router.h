/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */
#ifndef NetDeviceRouter_H
#define NetDeviceRouter_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/callback.h"
#include <map>
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/itetris-technologies.h"
#include "ns3/net-device.h"

namespace ns3 {

class Node;
class NetDevice;
class Packet;

class NetDeviceRouter : public Object
{
public:
  static TypeId GetTypeId (void);
  static const uint16_t PROT_NUMBER;
  NetDeviceRouter ();
  virtual ~NetDeviceRouter ();
  void SetNode (Ptr<Node> node);
  void Send (Ptr<Packet> packet,Address hardwareDestination);
  void Receive( Ptr<NetDevice> device, Ptr<const Packet> p, uint16_t protocol, const Address &from,
                const Address &to, NetDevice::PacketType packetType);
  uint32_t AddInterface (NetDeviceType deviceType, Ptr<NetDevice> device);
  uint32_t GetNInterfaces (void) const;

protected:
  virtual void DoDispose (void);

private:
  typedef std::map<NetDeviceType, Ptr<NetDevice> > NetDeviceList;
  NetDeviceList m_interfaces;
  uint32_t m_nInterfaces;
  Ptr<Node> m_node;


};

} // namespace ns3 

#endif /* NetDeviceRouter_H */
