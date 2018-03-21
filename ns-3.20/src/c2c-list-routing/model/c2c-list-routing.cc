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

#include "ns3/log.h"
#include "ns3/c2c.h"
#include "ns3/c2c-route.h"
#include "ns3/node.h"
#include "ns3/geo-routing.h"

#include "c2c-list-routing.h"

NS_LOG_COMPONENT_DEFINE ("c2cListRouting");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cListRouting);

TypeId
c2cListRouting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cListRouting")
    .SetParent<c2cRoutingProtocol> ()
    .AddConstructor<c2cListRouting> ()
    ;
  return tid;
}


c2cListRouting::c2cListRouting ()
 : m_c2c (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

c2cListRouting::~c2cListRouting () 
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
c2cListRouting::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  for (c2cRoutingProtocolList::iterator rprotoIter = m_routingProtocols.begin ();
       rprotoIter != m_routingProtocols.end (); rprotoIter++)
    {
      // Note:  Calling dispose on these protocols causes memory leak
      //        The routing protocols should not maintain a pointer to
      //        this object, so Dispose() shouldn't be necessary.
      (*rprotoIter).second = 0;
    }
  m_routingProtocols.clear ();
  m_c2c = 0;
}

void
c2cListRouting::DoInitialize (void)
{
  for (c2cRoutingProtocolList::iterator rprotoIter = m_routingProtocols.begin ();
       rprotoIter != m_routingProtocols.end (); rprotoIter++)
    {
      Ptr<c2cRoutingProtocol> protocol = (*rprotoIter).second;
      protocol->Initialize ();
    }
  c2cRoutingProtocol::DoInitialize ();
}


struct c2cRoutingProtocol::output
c2cListRouting::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                             Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                             Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr)
{

  struct c2cRoutingProtocol::output result;

  for (c2cRoutingProtocolList::const_iterator i = m_routingProtocols.begin ();
       i != m_routingProtocols.end (); i++)
    {
      NS_LOG_LOGIC ("Checking protocol " << (*i).second->GetInstanceTypeId () << " with priority " << (*i).first);
      result = (*i).second->RouteOutput (p, sourcePosVector, daddr, lt, sn, oif, sockerr);
      if (result.route != 0)
        {
          NS_LOG_LOGIC ("Found route " << result.route);
          sockerr = Socketc2c::ERROR_NOTERROR;
          return result;
        }
    }
  NS_LOG_LOGIC ("Done checking " << GetTypeId ());
  NS_LOG_LOGIC ("");
  sockerr = Socketc2c::ERROR_NOROUTETOHOST;
  return result;
}

// Patterned after Linux ip_route_input and ip_route_input_slow
bool
c2cListRouting::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &cheader,
                            Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                            LocalDeliverCallback lcb, ErrorCallback ecb)
{
  bool retVal = false;
  NS_LOG_FUNCTION (p << idev);
  NS_LOG_LOGIC ("RouteInput logic for node: " << m_c2c->GetObject<Node> ()->GetId ());

  NS_ASSERT (m_c2c != 0);
  // Check if input device supports IP 
  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0); 

  for (c2cRoutingProtocolList::const_iterator rprotoIter =
         m_routingProtocols.begin ();
       rprotoIter != m_routingProtocols.end ();
       rprotoIter++)
    {
      if ((*rprotoIter).second->RouteInput (p, cheader, idev, ucb, lcb, ecb))
        {
          NS_LOG_LOGIC ("Route found to forward packet in protocol " << (*rprotoIter).second->GetInstanceTypeId ().GetName ()); 
          return true;
        }
    }
  // No routing protocol has found a route..
  return retVal;
}

void 
c2cListRouting::Setc2c (Ptr<c2c> c2c)
{
  NS_LOG_FUNCTION(this << c2c);
  NS_ASSERT (m_c2c == 0);
  for (c2cRoutingProtocolList::const_iterator rprotoIter =
         m_routingProtocols.begin ();
       rprotoIter != m_routingProtocols.end ();
       rprotoIter++)
    {
      (*rprotoIter).second->Setc2c (c2c);
    }
  m_c2c = c2c;
}

void
c2cListRouting::AddRoutingProtocol (Ptr<c2cRoutingProtocol> routingProtocol, int16_t priority)
{
  NS_LOG_FUNCTION (this << routingProtocol->GetInstanceTypeId () << priority);
  m_routingProtocols.push_back (std::make_pair (priority, routingProtocol));
  m_routingProtocols.sort ( Compare );
  if (m_c2c != 0)
    {
      routingProtocol->Setc2c (m_c2c);
    }
}

uint32_t 
c2cListRouting::GetNRoutingProtocols (void) const
{
  NS_LOG_FUNCTION (this);
  return m_routingProtocols.size (); 
}

Ptr<c2cRoutingProtocol> 
c2cListRouting::GetRoutingProtocol (uint32_t index, int16_t& priority) const
{
  NS_LOG_FUNCTION (index);
  if (index > m_routingProtocols.size ())
    {
      NS_FATAL_ERROR ("c2cListRouting::GetRoutingProtocol():  index " << index << " out of range");
    }
  uint32_t i = 0;
  for (c2cRoutingProtocolList::const_iterator rprotoIter = m_routingProtocols.begin ();
       rprotoIter != m_routingProtocols.end (); rprotoIter++, i++)
    {
      if (i == index)
        {
          priority = (*rprotoIter).first;
          return (*rprotoIter).second;
        }
    }
  return 0;
}

bool 
c2cListRouting::Compare (const c2cRoutingProtocolEntry& a, const c2cRoutingProtocolEntry& b)
{
  return a.first > b.first;
}

} // namespace ns3

