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

#ifndef C2C_L4T_SOCKET_IMPL_H
#define C2C_L4T_SOCKET_IMPL_H

#include <stdint.h>
#include <queue>
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include "ns3/socketc2c.h"
#include "ns3/ptr.h"
#include "ns3/c2cl4T-socket.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class c2cEndPoint;
class Node;
class Packet;
class c2cTransport;

/**
 * \ingroup c2cTransport
 * \brief  iTETRIS [WP600] - A socket interface to c2c
 * 
 * This class subclasses ns3::c2cl4TSocket, and provides a socket interface
 * to ns3's implementation of c2c.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

class c2cl4TSocketImpl : public c2cl4TSocket
{
public:
  static TypeId GetTypeId (void);
  /**
   * Create an unbound c2cTransport socket.
   */
  c2cl4TSocketImpl ();
  virtual ~c2cl4TSocketImpl ();

  void SetNode (Ptr<Node> node);
  void Setc2cTransport (Ptr<c2cTransport> c2cTransport);
  virtual Ptr<Node> GetNode (void) const;
  virtual int Bind (const uint16_t port);
  virtual int Send (Ptr<Packet> p, uint32_t flags);
  int DoSendTo (Ptr<Packet> p, Ptr <c2cAddress> daddr, uint16_t dport, uint8_t lt, uint8_t tc, uint16_t sn);

  virtual int Close (void);

  virtual uint32_t GetTxAvailable (void) const;
  virtual uint32_t GetRxAvailable (void) const;
  virtual Ptr<Packet> Recv (void);
  virtual Ptr<Packet> Recv (uint32_t maxSize, uint32_t flags);
  virtual Ptr<Packet> RecvFrom (uint32_t maxSize, uint32_t flags,
    uint64_t fromNodeId);
  virtual int GetSockName (uint64_t nodeId) const;

private:
  // Attributes set through c2cl4TSocket base class
  virtual void SetRcvBufSize (uint32_t size);
  virtual uint32_t GetRcvBufSize (void) const;

private:

  friend class c2cl4TSocketFactory;
  int FinishBind (void);
  void ForwardUp (Ptr<Packet> p, Ptr<const c2cAddress> saddr, uint16_t port);
  void Destroy (void);
  int DoSend (Ptr<Packet> p);

  int DoSendTo (Ptr<Packet> p, uint64_t dnodeId, uint16_t dport);


  c2cEndPoint *m_endPoint;
  Ptr<Node> m_node;
  Ptr<c2cTransport> m_c2cTransport;
  uint64_t m_defaultNodeId;
  uint16_t m_defaultPort;
  TracedCallback<Ptr<const Packet> > m_dropTrace;

  bool m_shutdownSend;
  bool m_shutdownRecv;

  std::queue<Ptr<Packet> > m_deliveryQueue;
  uint32_t m_rxAvailable;
  
  // c2c Socket attributes
  uint32_t m_rcvBufSize;
};

}//namespace ns3

#endif /* C2C_L4T_SOCKET_IMPL_H */
