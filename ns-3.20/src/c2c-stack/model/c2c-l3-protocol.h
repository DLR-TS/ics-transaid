/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EURECOM, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#ifndef C2C_L3_PROTOCOL_H
#define C2C_L3_PROTOCOL_H

#include <list>
#include <map>

#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/object.h"
#include "ns3/c2c.h"
#include "ns3/traced-callback.h"
#include "ns3/c2c-routing-protocol.h"
#include "ns3/c2c-common-header.h"
#include "ns3/location-table.h"

#include "itetris-technologies.h"

namespace ns3 {


class Packet;
class NetDevice;
class c2cInterface;
class c2cCommonHeader;
//class c2cRoutingTableEntry;
class c2cRoute;
class Node;
class Socketc2c;
class c2cL4Protocol;

/**
 * \brief  iTETRIS [WP600] - Implement of the c2c layer.
 *
 * This is the actual implementation of c2c. It contains APIs to send
 * and receive packets at the c2c layer.
 *
 * TODO APIs for c2c routing.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * @author Fatma Hrizi <fatma.hrizi@eurecom.fr>
 *
 */

class c2cL3Protocol : public c2c
{
public:

  static const uint16_t PROT_NUMBER;
  static TypeId GetTypeId (void);

  c2cL3Protocol ();
  virtual ~c2cL3Protocol ();

  enum DropReason
    {
      DROP_TTL_EXPIRED = 1, /**< Packet TTL has expired */
      DROP_NO_ROUTE, /**< No route to host */
      DROP_BAD_CHECKSUM, /**< Bad checksum */
      DROP_INTERFACE_DOWN, /**< Interface is down so can not send packet */
      DROP_ROUTE_ERROR, /**< Route error */
    };

  void SetNode (Ptr<Node> node);

  // functions defined in base class c2c

 void SetRoutingProtocol (Ptr<c2cRoutingProtocol> routingProtocol);
 Ptr<c2cRoutingProtocol> GetRoutingProtocol (void) const;

  /**
   * \param protocol a template for the protocol to add to this L4 Demux.
   * \returns the L4Protocol effectively added.
   *
   * Invoke Copy on the input template to get a copy of the input
   * protocol which can be used on the Node on which this L4 Demux
   * is running. The new L4Protocol is registered internally as
   * a working L4 Protocol and returned from this method.
   * The caller does not get ownership of the returned pointer.
   */
  void Insert(Ptr<c2cL4Protocol> protocol);
  /**
   * \param protocolNumber number of protocol to lookup
   *        in this L4 Demux
   * \returns a matching L4 Protocol
   *
   * This method is typically called by lower layers
   * to forward packets up the stack to the right protocol.
   */
  Ptr<c2cL4Protocol> GetProtocol(int protocolNumber) const;
  /**
   * \param protocol protocol to remove from this demux.
   *
   * The input value to this method should be the value
   * returned from the c2cL4Protocol::Insert method.
   */
  void Remove (Ptr<c2cL4Protocol> protocol);

  /**
   * \param ttl default ttl to use
   *
   * When we need to send a c2c packet, we use this default
   * ttl value.
   */
  void SetDefaultTtl (uint8_t ttl);

  /**
   * Receives Packet at c2cL3
   * \param device network device
   * \param p the packet
   * \param protocol protocol value
   * \param from address of the correspondent
   * \param to address of the destination
   * \param packetType type of the packet
   */
  void Receive (Ptr<NetDevice> device, Ptr<const Packet> p,
                uint16_t protocol, const Address &from,
                const Address &to, NetDevice::PacketType packetType);

  /**
   * Sends Packet down the stack
   * \param packet packet to send
   * \param snodeId source node ID of packet
   * \param dnodeId destination node ID of packet
   * \param protocol number of packet
   *
   * Higher-level layers call this method to send a packet
   * down the stack to the MAC and PHY layers.
   */
  void Send (struct c2cRoutingProtocol::output routeresult,
                    uint16_t protocol, uint16_t htype,
                    uint8_t hstype, uint8_t tc);
  void SendTo (struct c2cRoutingProtocol::output routeresult,
                    const c2cCommonHeader &header, uint8_t lt);

  uint32_t AddInterface (NetDeviceType deviceType, Ptr<NetDevice> device);
  uint32_t AddInterface (Ptr<NetDevice> device);
  Ptr<c2cInterface> GetInterface (uint32_t index) const;
  uint32_t GetNInterfaces (void) const;

  int32_t GetInterfaceForDevice (Ptr<const NetDevice> device) const;

  uint16_t GetMtu (uint32_t i) const;

  Ptr<NetDevice> GetNetDevice (uint32_t i);

protected:

  virtual void DoDispose (void);
  /**
   * This function will notify other components connected to the node that a new stack member is now connected
   * This will be used to notify Layer 3 protocol of layer 4 protocol stack to connect them together.
   */
  virtual void NotifyNewAggregate ();

private:

  c2cL3Protocol(const c2cL3Protocol &);
  c2cL3Protocol &operator = (const c2cL3Protocol &);

  void LocalDeliver (Ptr<const Packet> packet, c2cCommonHeader const&header, Ptr<const c2cAddress> saddr, Ptr<const c2cAddress> daddr, uint32_t iif);
  void L3Forward (struct c2cRoutingProtocol::output rtentry, const c2cCommonHeader &header);
  void RouteInputError (Ptr<const Packet> p, const c2cCommonHeader & header, Socketc2c::SocketErrno sockErrno);
  void AddPacketTags (Ptr<const Packet> p, c2cCommonHeader const&header); // Added by Ramon Bauza

  c2cCommonHeader BuildCommonHeader (Ptr<c2cAddress> addr, uint16_t protocol, uint16_t payloadSize, uint16_t htype, uint8_t hstype, uint8_t tc);

  uint32_t Addc2cInterface (Ptr<c2cInterface> interface);
  //void SetupLoopback (void);

  typedef std::list<Ptr<c2cInterface> > c2cInterfaceList;
  typedef std::list<Ptr<c2cL4Protocol> > L4List_t;
  typedef std::map<NetDeviceType, Ptr<NetDevice> > NetDeviceList;

  // Added by Ramon Bauza
  // Modifications to retrieve the MAC address of a peer station based on its nodeId 

  struct NodeIdAddressMap {
    uint32_t nodeId;
    Address address;
  };

  typedef std::vector<NodeIdAddressMap> NodeIdAddressMapList;
  NodeIdAddressMapList m_nodeIdAddressMap;
  void InsertNodeIdAddressMap (uint32_t nodeId, Address address);
  bool GetAddressByNodeId (uint32_t nodeId, Address &address);

  // End addition

  Ptr<Node> m_node;
  uint32_t m_nInterfaces;
  NetDeviceList m_devInterfaces;
  c2cInterfaceList m_interfaces;
  L4List_t m_protocols;

  TracedCallback<const c2cCommonHeader &, Ptr<const Packet>, DropReason, uint32_t> m_dropTrace;
  TracedCallback<Ptr<const Packet>, uint32_t> m_txTrace;
  TracedCallback<Ptr<const Packet>, uint32_t> m_rxTrace;
  TracedCallback<const c2cCommonHeader &, Ptr<const Packet>, uint32_t> m_unicastForwardTrace;
  TracedCallback<const c2cCommonHeader &, Ptr<const Packet>, uint32_t> m_localDeliverTrace;

  Ptr<c2cRoutingProtocol> m_routingProtocol;

};

}//namespace ns3


#endif /* C2C_L3_PROTOCOL_H */
