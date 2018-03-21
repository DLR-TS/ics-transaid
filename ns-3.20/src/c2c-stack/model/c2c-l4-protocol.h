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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */


#ifndef C2C_L4_PROTOCOL_H
#define C2C_L4_PROTOCOL_H

#include "ns3/object.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class Packet;
class c2cInterface;

/**
 * \brief  iTETRIS [WP600] - The c2c L4 Protocol abstract base class
 *
 * This is an abstract base class for layer four protocols which use c2c as
 * the network layer.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class c2cL4Protocol : public Object
{
public:
  enum RxStatus {
    RX_OK,
    RX_CSUM_FAILED,
    RX_ENDPOINT_CLOSED,
    RX_ENDPOINT_UNREACH
  };

  static TypeId GetTypeId (void);

  virtual ~c2cL4Protocol();

  /**
   * \returns the protocol number of this protocol.
   */
  virtual int GetProtocolNumber (void) const = 0;

  /**
   * \param p packet to forward up
   * \param snodeId source node ID of packet received
   * \param dnodeId destination node ID of packet received
   * \param incomingInterface the c2cInterface on which the packet arrived
   *
   * Called from lower-level layers to send the packet up
   * in the stack.
   */

  virtual enum RxStatus Receive(Ptr<Packet> p,
                                Ptr<const c2cAddress> saddr,
                                Ptr<const c2cAddress> daddr,
                                Ptr<c2cInterface> incomingInterface) = 0;

};

} // Namespace ns3

#endif

