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
#include "ns3/packet.h"
#include "ns3/node.h"
#include "socketc2c.h"
#include "socket-factoryc2c.h"
#include <limits>

NS_LOG_COMPONENT_DEFINE ("Socketc2c");

namespace ns3 {

Socketc2c::Socketc2c (void)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Socketc2c::~Socketc2c ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<Socketc2c>
Socketc2c::CreateSocket (Ptr<Node> node, TypeId tid)
{
  Ptr<Socketc2c> s;
  Ptr<SocketFactoryc2c> socketFactory = node->GetObject<SocketFactoryc2c> (tid);
  s = socketFactory->CreateSocket ();
  NS_ASSERT (s != 0);
  return s;
}


int
Socketc2c::Send (Ptr<Packet> p)
{
  NS_LOG_FUNCTION_NOARGS ();
  return Send (p, 0);
}

void
Socketc2c::SetRecvCallback (Callback<void, Ptr<Socketc2c> > receivedData)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_receivedData = receivedData;
}

/* ------ iTETRIS : Enable this, if working with constant maximum payload size ------
Ptr<Packet>
Socketc2c::Recv (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return Recv (std::numeric_limits<uint32_t>::max(), 0);
}
*/// --------------------------------------------------------------------------------

int
Socketc2c::Recv (uint8_t* buf, uint32_t size, uint32_t flags)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Packet> p = Recv (size, flags); // read up to "size" bytes
  if (p == 0)
    {
      return 0;
    }
  p->CopyData (buf, p->GetSize ());
  return p->GetSize ();
}

Ptr<Packet>
Socketc2c::RecvFrom (uint64_t fromNodeId)
{
  NS_LOG_FUNCTION_NOARGS ();
  return RecvFrom (std::numeric_limits<uint32_t>::max(), 0, fromNodeId);
}

int
Socketc2c::RecvFrom (uint8_t* buf, uint32_t size, uint32_t flags,
				uint64_t fromNodeId)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Packet> p = RecvFrom (size, flags, fromNodeId);
  if (p == 0)
    {
      return 0;
    }
  p->CopyData (buf, p->GetSize ());
  return p->GetSize ();
}

void
Socketc2c::NotifyDataSent (uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (!m_dataSent.IsNull ())
    {
      m_dataSent (this, size);
    }
}

void
Socketc2c::NotifySend (uint32_t spaceAvailable)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (!m_sendCb.IsNull ())
    {
      m_sendCb (this, spaceAvailable);
    }
}

void
Socketc2c::NotifyDataRecv (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (!m_receivedData.IsNull ())
    {
      m_receivedData (this);
    }
}

void
Socketc2c::DoDispose (void)
{
  m_dataSent = MakeNullCallback<void,Ptr<Socketc2c>, uint32_t> ();
  m_sendCb = MakeNullCallback<void,Ptr<Socketc2c>, uint32_t> ();
  m_receivedData = MakeNullCallback<void,Ptr<Socketc2c> > ();
}


/***********************Socketc2c Tags*************************
Socketc2cAddressTag::Socketc2cAddressTag ()
{
}
 ***************************************************************/

}//namespace ns3
