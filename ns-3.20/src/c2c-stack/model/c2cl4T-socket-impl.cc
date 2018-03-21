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

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/c2c.h"
#include "ns3/c2cl4T-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "c2cl4T-socket-impl.h"
#include "c2c-transport.h"
#include "c2c-end-point.h"
#include <limits>
#include "ns3/mobility-model.h"
#include "ns3/simulator.h"
#include "ns3/c2c-address.h"

NS_LOG_COMPONENT_DEFINE ("c2cl4TSocketImpl");

namespace ns3 {

static const uint32_t MAX_C2C_L4T_DATAGRAM_SIZE = 1550;

// Add attributes generic to all c2cTransport Sockets to base class c2cl4TSocket
TypeId
c2cl4TSocketImpl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cl4TSocketImpl")
    .SetParent<c2cl4TSocket> ()
    .AddConstructor<c2cl4TSocketImpl> ()
    ;
  return tid;
}

c2cl4TSocketImpl::c2cl4TSocketImpl ()
: m_endPoint (0),
  m_node (0),
  m_c2cTransport (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

c2cl4TSocketImpl::~c2cl4TSocketImpl ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_node = 0;
  if (m_endPoint != 0)
    {
      NS_ASSERT (m_c2cTransport != 0);
      /**
       * Note that this piece of code is a bit tricky:
       * when DeAllocate is called, it will call into
       * c2cEndPointDemux::Deallocate which triggers
       * a delete of the associated endPoint which triggers
       * in turn a call to the method ::Destroy below
       * will will zero the m_endPoint field.
       */
      NS_ASSERT (m_endPoint != 0);
      m_c2cTransport->DeAllocate (m_endPoint);
      NS_ASSERT (m_endPoint == 0);
    }
  m_c2cTransport = 0;
}

void 
c2cl4TSocketImpl::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = node;
}
void 
c2cl4TSocketImpl::Setc2cTransport(Ptr<c2cTransport> c2cTransport )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_c2cTransport = c2cTransport ;
}

Ptr<Node>
c2cl4TSocketImpl::GetNode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}

void 
c2cl4TSocketImpl::Destroy (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = 0;
  m_endPoint = 0;
  m_c2cTransport = 0;
}


int
c2cl4TSocketImpl::FinishBind (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_endPoint == 0)
    {
      return -1;
    }
  m_endPoint->SetRxCallback (MakeCallback (&c2cl4TSocketImpl::ForwardUp, Ptr<c2cl4TSocketImpl> (this)));
  m_endPoint->SetDestroyCallback (MakeCallback (&c2cl4TSocketImpl::Destroy, Ptr<c2cl4TSocketImpl> (this)));
  return 0;
}

int
c2cl4TSocketImpl::Bind (const uint16_t port)
{
  NS_LOG_FUNCTION_NOARGS ();
  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("     * c2cl4TsocketImpl - NodeID = "<<m_node->GetId());
  NS_LOG_INFO ("     * c2cl4TsocketImpl - c2c Socket Bind(port) - DONE at port = "<<port);
  //---------------- check iTETRIS ----------------------------
  m_endPoint = m_c2cTransport->Allocate (port);
  return FinishBind ();
}

int
c2cl4TSocketImpl::Close (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_shutdownRecv == true && m_shutdownSend == true)
    {
     // m_errno = Socketc2c::ERROR_BADF;
      return -1;
    }
  m_shutdownRecv = true;
  m_shutdownSend = true;
  return 0;
}

int 
c2cl4TSocketImpl::Send (Ptr<Packet> p, uint32_t flags)
{
  NS_LOG_FUNCTION (this << p << flags);
/*
  if (!m_connected)
    {
      m_errno = ERROR_NOTCONN;
      return -1;
    }*/
  return DoSend (p);
}



int 
c2cl4TSocketImpl::DoSend (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  if (m_endPoint == 0)
    {
      NS_ASSERT (m_endPoint != 0);
    }
  return DoSendTo (p, 9999, 8080);
}

int
c2cl4TSocketImpl::DoSendTo (Ptr<Packet> p, Ptr <c2cAddress> daddr, uint16_t dport, uint8_t lt, uint8_t tc, uint16_t sn)
{
  // Source position vector
  Ptr<MobilityModel> model = m_node->GetObject<MobilityModel> ();
  c2cCommonHeader::LongPositionVector sourcePosVector;
  sourcePosVector.gnAddr = m_node->GetId ();
  sourcePosVector.Lat = (uint32_t) model->GetPosition().x;
  sourcePosVector.Long = (uint32_t) model->GetPosition().y;
  sourcePosVector.Alt = 0;
  sourcePosVector.Speed = (uint16_t) model->GetVelocity().x;
  sourcePosVector.Ts = (static_cast<uint32_t>(Simulator::Now ().GetSeconds()));
  //TODO retrieve the rest of source position vector data from the mobility model.

  if (m_endPoint == 0)
    {
      NS_ASSERT (m_endPoint != 0);
    }
  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("     * c2cTl4TSocketImpl - SENDER NODE ID = "<<m_node->GetId ());
  NS_LOG_INFO ("     * c2cTl4TSocketImpl - APP PORT       = "<<m_endPoint->GetLocalPort ());
  //---------------- check iTETRIS ----------------------------

  m_c2cTransport->Send (p, sourcePosVector, daddr, m_endPoint->GetLocalPort (), dport, lt, tc, sn);

  if (p->GetSize () > GetTxAvailable ())
   {
     // m_errno = ERROR_MSGSIZE;
      return -1;
   }

  NotifyDataSent (p->GetSize ());
  NotifySend (GetTxAvailable ());
  return p->GetSize();
}

int
c2cl4TSocketImpl::DoSendTo (Ptr<Packet> p, uint64_t dnodeId, uint16_t dport)
{
  NS_LOG_FUNCTION (this << p << dnodeId << dport);

  uint8_t lt = 10;
  uint8_t tc = 10;
  uint16_t sn = 99;

  //**daddr, sn, lt and tc are hard coded**//
  //daddr has the same position as the node 0
  Ptr<Node> ns3node = GetObject<Node> ();
  ns3node = NodeList::GetNode (0);

  Ptr<MobilityModel> model1 = ns3node->GetObject<MobilityModel> ();
  uint32_t ttl = 5;
  Ptr<c2cAddress> daddr = CreateObject<c2cAddress> ();
  daddr->Set (c2cAddress::BROAD, ttl);

  // Source position vector
  Ptr<MobilityModel> model = m_node->GetObject<MobilityModel> ();
  c2cCommonHeader::LongPositionVector sourcePosVector;
  sourcePosVector.gnAddr = m_node->GetId ();
  sourcePosVector.Lat = (uint32_t) model->GetPosition().x;
  sourcePosVector.Long = (uint32_t) model->GetPosition().y;
  sourcePosVector.Alt = 0;
  sourcePosVector.Speed = (uint16_t) model->GetVelocity().x;
  sourcePosVector.Ts = (static_cast<uint32_t>(Simulator::Now ().GetSeconds()));
  //TODO retrieve the rest of source position vector data from the mobility model.
  //**daddr, sn, lt and tc are hard coded just to test**//

  if (m_endPoint == 0)
    {
      NS_ASSERT (m_endPoint != 0);
    }
  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("     * c2cTl4TSocketImpl - SENDER NODE ID = "<<m_node->GetId ());
  NS_LOG_INFO ("     * c2cTl4TSocketImpl - APP PORT       = "<<m_endPoint->GetLocalPort ());
  //---------------- check iTETRIS ----------------------------


   m_c2cTransport->Send (p, sourcePosVector, daddr, m_endPoint->GetLocalPort (), dport, lt, tc, sn);

  if (p->GetSize () > GetTxAvailable ())
   {
     // m_errno = ERROR_MSGSIZE;
      return -1;
   }

  NotifyDataSent (p->GetSize ());
  NotifySend (GetTxAvailable ());
  return p->GetSize();
}

uint32_t
c2cl4TSocketImpl::GetTxAvailable (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  // No finite send buffer is modelled, but we must respect
  // the maximum size of an c2c datagram (1600 bytes - headers).
  return MAX_C2C_L4T_DATAGRAM_SIZE;
}

uint32_t
c2cl4TSocketImpl::GetRxAvailable (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  // We separately maintain this state to avoid walking the queue 
  // every time this might be called
  return m_rxAvailable;
}


Ptr<Packet>
c2cl4TSocketImpl::Recv ()
{

  if (m_deliveryQueue.empty() )
    {
      //m_errno = ERROR_AGAIN;
      return 0;
    }

  Ptr<Packet> p = m_deliveryQueue.front ();
  m_deliveryQueue.pop ();
  m_rxAvailable -= p->GetSize ();

  //--------------  CHECK iTETRIS -----------------------------
  NS_LOG_INFO ("     * c2cTl4TSocketImpl->Recv() : Receiving");
  NS_LOG_INFO ("     * c2cTl4TSocketImpl : FORWARD UP to APPLICATION - RX PACKET size (Bytes) = "<<p->GetSize());
  NS_LOG_INFO ("                         : Recv Port                                          = "<<m_endPoint->GetLocalPort ());
  NS_LOG_INFO ("                         : Recv NodeID                                        = "<<m_node->GetId ());
  //--------------  CHECK iTETRIS -----------------------------
  return p;
}

Ptr<Packet>
c2cl4TSocketImpl::Recv (uint32_t maxSize, uint32_t flags)
{
  NS_LOG_FUNCTION (this << maxSize << flags);

  if (m_deliveryQueue.empty() )
    {
      //m_errno = ERROR_AGAIN;
      return 0;
    }

  Ptr<Packet> p = m_deliveryQueue.front ();
  if (p->GetSize () <= maxSize) 
    {
      m_deliveryQueue.pop ();
      m_rxAvailable -= p->GetSize ();
    }
  else
    {
      p = 0; 
    }
  return p;
}

Ptr<Packet>
c2cl4TSocketImpl::RecvFrom (uint32_t maxSize, uint32_t flags,
  uint64_t fromNodeId)
{
  NS_LOG_FUNCTION (this << maxSize << flags);
  Ptr<Packet> packet = Recv (maxSize, flags);
  return packet;
}


int
c2cl4TSocketImpl::GetSockName (uint64_t nodeId) const
{
  return 0;
}

void
c2cl4TSocketImpl::ForwardUp (Ptr<Packet> packet, Ptr<const c2cAddress> saddr, uint16_t port)
{
  NS_LOG_FUNCTION (this << packet << /*nodeId <<*/ port);

  if ((m_rxAvailable + packet->GetSize ()) <= m_rcvBufSize)
    {
      m_deliveryQueue.push (packet);
      m_rxAvailable += packet->GetSize ();
      NotifyDataRecv ();
    }
  else
    {
      // In general, this case should not occur unless the
      // receiving application reads data from this socket slowly
      // in comparison to the arrival rate
      //
      // drop and trace packet

	  //---------------- check iTETRIS ----------------------------
   	  NS_LOG_INFO ("c2cT Sockets : No receive buffer space available.  Drop.");
   	  //---------------- check iTETRIS ----------------------------

      NS_LOG_WARN ("No receive buffer space available.  Drop.");
      m_dropTrace (packet);
    }
}

void
c2cl4TSocketImpl::SetRcvBufSize (uint32_t size)
{
  m_rcvBufSize = size;
}

uint32_t
c2cl4TSocketImpl::GetRcvBufSize (void) const
{
  return m_rcvBufSize;
}

} //namespace ns3
