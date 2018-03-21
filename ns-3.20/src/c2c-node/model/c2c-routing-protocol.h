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

#ifndef C2C_ROUTING_PROTOCOL_H
#define C2C_ROUTING_PROTOCOL_H

#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/object.h"
#include "ns3/socketc2c.h"
#include "c2c.h"
#include "ns3/c2c-address.h"
#include "c2c-common-header.h"
#include "ns3/c2c-interface.h"
#include "c2c-route.h"

namespace ns3 {

class c2cRoute;
class NetDevice;

/**
 * \ingroup node 
 * \defgroup c2cRouting c2cRoutingProtocol
 */

/**
 * \ingroup c2cRouting
 * \brief  iTETRIS [WP600] - Abstract base class for c2c routing protocols
 * 
 * Defines two virtual functions for packet routing and forwarding.  The first, 
 * RouteOutput(), is used for locally originated packets, and the second,
 * RouteInput(), is used for forwarding and/or delivering received packets. 
 * Also defines the signatures of four callbacks used in RouteInput().
 *
 */
class c2cRoutingProtocol : public Object
{
public:
  static TypeId GetTypeId (void);

  struct output
  {
  	  Ptr<c2cRoute> route;
  	  Ptr<Packet> packet;
  };

  typedef Callback<void, struct output, const c2cCommonHeader &> UnicastForwardCallback;
  typedef Callback<void, Ptr<const Packet>, const c2cCommonHeader &, Ptr<const c2cAddress>, Ptr<const c2cAddress> , uint32_t > LocalDeliverCallback;
  typedef Callback<void, Ptr<const Packet>, const c2cCommonHeader &, Socketc2c::SocketErrno > ErrorCallback;
  
  /**
   * \brief Query routing cache for an existing route, for an outbound packet
   *
   * This lookup is used by transport protocols.  It does not cause any
   * packet to be forwarded, and is synchronous.  Can be used for
   * multicast or unicast.  The Linux equivalent is ip_route_output()
   *
   * \param p packet to be routed.  Note that this method may modify the packet (add extended headers).
   *          Callers may also pass in a null pointer. 
   * \param header input parameter (used to form key to search for the route)
   * \param oif Output interface Netdevice.  May be zero, or may be bound via
   *            socket options to a particular output interface.
   * \param sockerr Output parameter; socket errno 
   *
   * \returns a code that indicates what happened in the lookup
   */
  virtual struct output RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector, Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn, Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr) = 0;

  /**
   * \brief Route an input packet (to be forwarded or locally delivered)
   *
   * This lookup is used in the forwarding process.  The packet is
   * handed over to the c2cRoutingProtocol, and will get forwarded onward
   * by one of the callbacks.  The Linux equivalent is ip_route_input().
   * There are four valid outcomes, and a matching callbacks to handle each.
   *
   * \param p received packet
   * \param header input parameter used to form a search key for a route
   * \param idev Pointer to ingress network device
   * \param ucb Callback for the case in which the packet is to be forwarded
   *            as unicast
   * \param lcb Callback for the case in which the packet is to be locally
   *            delivered
   * \param ecb Callback to call if there is an error in forwarding
   * \returns true if the c2cRoutingProtocol takes responsibility for
   *          forwarding or delivering the packet, false otherwise
   */ 
  virtual bool RouteInput  (Ptr<const Packet> p, const c2cCommonHeader &header, Ptr<const NetDevice> idev, UnicastForwardCallback ucb, LocalDeliverCallback lcb, ErrorCallback ecb) = 0;

  /**
   * \param interface the index of the interface we are being notified about
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   */
//   virtual void NotifyInterfaceUp (uint32_t interface) = 0;
  /**
   * \param interface the index of the interface we are being notified about
   *
   * Protocols are expected to implement this method to be notified of the state change of
   * an interface in a node.
   */
//   virtual void NotifyInterfaceDown (uint32_t interface) = 0;

  /**
   * \param c2c the c2c object this routing protocol is being associated with
   * 
   * Typically, invoked directly or indirectly from ns3::c2c::SetRoutingProtocol
   */
  virtual void Setc2c (Ptr<c2c> c2c) = 0;
};

} //namespace ns3

#endif /* C2C_ROUTING_PROTOCOL_H */
