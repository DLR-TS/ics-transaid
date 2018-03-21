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

#ifndef C2C_TRANSPORT_H_
#define C2C_TRANSPORT_H_

#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/c2c-address.h"

#include "c2c-l4-protocol.h"

namespace ns3 {

class Node;
class Socketc2c;
class c2cEndPointDemux;
class c2cEndPoint;

/**
 * \ingroup c2cTransport
 * \brief  iTETRIS [WP600] - Implementation of the c2c Transport Protocol
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * @author Fatma Hrizi <Fatma.Hrizi@eurecom.fr>
 *
 */
class c2cTransport : public c2cL4Protocol {
public:
  static TypeId GetTypeId (void);
  static const uint8_t PROT_NUMBER;

  c2cTransport ();
  virtual ~c2cTransport ();

  void SetNode (Ptr<Node> node);
  Ptr<Node> GetNode (void);

  virtual int GetProtocolNumber (void) const;

  /**
   * \return A smart Socket pointer to a c2c socket, allocated by this instance
   * of the c2c Transport protocol
   */
  Ptr<Socketc2c> CreateSocket (void);

  c2cEndPoint *Allocate (uint16_t port);

  void DeAllocate (c2cEndPoint *endPoint);

  // called by c2c socket.
  /**
   * \brief Send a packet via c2c transport
   * \param packet The packet to send
   * \param snodeId The source Node ID
   * \param dnodeId The destination Node ID
   * \param sport The source port number
   * \param dport The destination port number
   */
  void Send (Ptr<Packet> packet, c2cCommonHeader::LongPositionVector sourcePosVector,
             Ptr<c2cAddress> daddr, uint16_t sport,
             uint16_t dport, uint8_t lt, uint8_t tc, uint16_t sn);

  /**
   * \brief Receive a packet up the protocol stack
   * \param p The Packet to dump the contents into
   * \param snodeId The source NodeID
   * \param dnodeId The destination NodeID
   * \param interface the interface from which the packet is coming.
   */
  // inherited from c2cL4Protocol
  virtual enum c2cL4Protocol::RxStatus Receive(Ptr<Packet> packet, Ptr<const c2cAddress> saddr,
                                               Ptr<const c2cAddress> daddr, Ptr<c2cInterface> interface);

protected:
  virtual void DoDispose (void);
  /*
   * This function will notify other components connected to the node that a new stack member is now connected
   * This will be used to notify Layer 3 protocol of layer 4 protocol stack to connect them together.
   */
  virtual void NotifyNewAggregate ();
private:
  Ptr<Node> m_node;
  c2cEndPointDemux *m_endPoints;
};

}; // namespace ns3

#endif /* C2C_TRANSPORT_H_ */
