/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */


#ifndef C2C_LIST_ROUTING_H
#define C2C_LIST_ROUTING_H

#include <list>

#include "ns3/c2c-common-header.h"
#include "ns3/c2c-routing-protocol.h"

namespace ns3 {

/**
 * \ingroup routing 
 * \defgroup c2cListRouting c2c List Routing
 */
/**
 * \ingroup c2cListRouting
 *
 * This class is a specialization of c2cRoutingProtocol that allows 
 * other instances of c2cRoutingProtocol to be inserted in a 
 * prioritized list.  Routing protocols in the list are consulted one
 * by one, from highest to lowest priority, until a routing protocol
 * is found that will take the packet (this corresponds to a non-zero
 * return value to RouteOutput, or a return value of true to RouteInput).
 * The order by which routing protocols with the same priority value 
 * are consulted is undefined.
 * 
 */
class c2cListRouting : public c2cRoutingProtocol
{
public:
  static TypeId GetTypeId (void);

  c2cListRouting ();
  virtual ~c2cListRouting ();

  /**
   * \brief Register a new routing protocol to be used in this c2c stack
   *
   * \param routingProtocol new routing protocol implementation object
   * \param priority priority to give to this routing protocol.
   * Values may range between -32768 and +32767.  
   */
  virtual void AddRoutingProtocol (Ptr<c2cRoutingProtocol> routingProtocol, int16_t priority);

  /**
   * \return number of routing protocols in the list
   */
  virtual uint32_t GetNRoutingProtocols (void) const;

  /**
   * Return pointer to routing protocol stored at index, with the
   * first protocol (index 0) the highest priority, the next one (index 1)
   * the second highest priority, and so on.  The priority parameter is an
   * output parameter and it returns the integer priority of the protocol.
   * 
   * \return pointer to routing protocol indexed by 
   * \param index index of protocol to return
   * \param priority output parameter, set to the priority of the protocol
            being returned
   */
  virtual Ptr<c2cRoutingProtocol> GetRoutingProtocol (uint32_t index, int16_t& priority) const;


  virtual struct c2cRoutingProtocol::output RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                                                         Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                                                         Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr);

  virtual bool RouteInput (Ptr<const Packet> p, const c2cCommonHeader &cheader,
                           Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);

  virtual void Setc2c (Ptr<c2c> c2c);

protected:
  void DoDispose (void);
  void DoInitialize (void);
private:
  typedef std::pair<int16_t, Ptr<c2cRoutingProtocol> > c2cRoutingProtocolEntry;
  typedef std::list<c2cRoutingProtocolEntry> c2cRoutingProtocolList;
  c2cRoutingProtocolList m_routingProtocols;
  static bool Compare (const c2cRoutingProtocolEntry& a, const c2cRoutingProtocolEntry& b);
  Ptr<c2c> m_c2c;
};

} //namespace ns3

#endif /* C2C_LIST_ROUTING_H */
