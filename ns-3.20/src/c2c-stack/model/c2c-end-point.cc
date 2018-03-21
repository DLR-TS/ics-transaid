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

#include "c2c-end-point.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("c2cEndPoint");

namespace ns3 {

c2cEndPoint::c2cEndPoint (uint16_t port)
  :m_localPort (port)
{
	NS_LOG_FUNCTION_NOARGS ();
}

c2cEndPoint::~c2cEndPoint ()
{
	NS_LOG_FUNCTION_NOARGS ();
	if (!m_destroyCallback.IsNull ())
    {
		m_destroyCallback ();
    }
}

uint64_t
c2cEndPoint::GetLocalNodeId  (void)
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_localNodeId;
}

void 
c2cEndPoint::SetLocalNodeId (uint64_t nodeId)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_localNodeId = nodeId;
}

uint16_t 
c2cEndPoint::GetLocalPort (void)
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_localPort;
}
uint64_t
c2cEndPoint::GetPeerNodeId (void)
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_peerNodeId;
}
uint16_t 
c2cEndPoint::GetPeerPort (void)
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_peerPort;
}
void 
c2cEndPoint::SetPeer (uint64_t nodeId, uint16_t port)
{
	NS_LOG_FUNCTION_NOARGS ();
 	m_peerNodeId = nodeId;
 	m_peerPort = port;
}

void 
c2cEndPoint::SetRxCallback (Callback<void,Ptr<Packet>, Ptr<const c2cAddress>, uint16_t> callback)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_rxCallback = callback;
}

void 
c2cEndPoint::SetDestroyCallback (Callback<void> callback)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_destroyCallback = callback;
}

void 
c2cEndPoint::ForwardUp (Ptr<Packet> p, Ptr<const c2cAddress> saddr, uint16_t sport)
{
	NS_LOG_FUNCTION_NOARGS ();
	if (!m_rxCallback.IsNull ())
    {

		//----------------  check iTETRIS --------------------------------------------
		NS_LOG_INFO ("     * c2cEndPoint      - OK - Respond To Callback - Forward Up");
		//----------------  check iTETRIS --------------------------------------------

		Simulator::ScheduleNow (&c2cEndPoint::DoForwardUp, this, p, saddr, sport);
    }
}
void 
c2cEndPoint::DoForwardUp (Ptr<Packet> p, Ptr<const c2cAddress> saddr, uint16_t sport)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_rxCallback (p, saddr, sport);
}

}; // namespace ns3
