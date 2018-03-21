/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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

#ifndef C2C_END_POINT_H
#define C2C_END_POINT_H

#include <stdint.h>
#include "ns3/callback.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class Header;
class Packet;

/**
 * \brief  iTETRIS [WP600] - A representation of a c2c endpoint/connection
 *
 * This class provides a c2c n-tuple (currently only destination and/or source ports).
 * These are used in the ns3::c2cEndPointDemux as targets
 * of lookups.  The class also has a callback for notification to higher
 * layers that a packet from a lower layer was received.  In the ns3
 * c2c-stack, these notifications are automatically registered to be
 * received by the corresponding socket.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

class c2cEndPoint {
public:
  c2cEndPoint (uint16_t port);
  ~c2cEndPoint ();

  uint64_t GetLocalNodeId (void);
  void SetLocalNodeId (uint64_t nodeId);
  uint16_t GetLocalPort (void);
  uint64_t GetPeerNodeId (void);
  uint16_t GetPeerPort (void);

  void SetPeer (uint64_t nodeId, uint16_t port);

  // Called from socket implementations to get notified about important events.
  void SetRxCallback (Callback<void,Ptr<Packet>, Ptr<const c2cAddress>, uint16_t> callback);
  void SetDestroyCallback (Callback<void> callback);

  // Called from an L4Protocol implementation to notify an endpoint of a
  // packet reception.
  void ForwardUp (Ptr<Packet> p, Ptr<const c2cAddress> saddr, uint16_t sport);

private:
  void DoForwardUp (Ptr<Packet> p, Ptr<const c2cAddress> saddr, uint16_t sport);
  uint64_t m_localNodeId;
  uint16_t m_localPort;
  uint64_t m_peerNodeId;
  uint16_t m_peerPort;
  Callback<void,Ptr<Packet>, Ptr<const c2cAddress>, uint16_t> m_rxCallback;
  Callback<void> m_destroyCallback;
};

}; // namespace ns3


#endif /* C2C_END_POINT_H */
