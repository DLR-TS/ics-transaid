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

#include "ns3/beaconing-protocol.h"
#include "c2c-l3-protocol.h"
#include "ns3/geo-broadcast.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("BeaconingProtocol");

namespace ns3
{
const uint8_t BeaconingProtocol::TRAFFIC_CLASS = 0;
const float BeaconingProtocol::BEACON_INTERVAL = 0.5;

NS_OBJECT_ENSURE_REGISTERED (BeaconingProtocol);

TypeId 
BeaconingProtocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BeaconingProtocol")
    .SetParent<Object> ()
    .AddConstructor<BeaconingProtocol> ()
    ;
  return tid;
}

BeaconingProtocol::BeaconingProtocol ()
{
  NS_LOG_FUNCTION(this);
  //DISABLE
  Simulator::Schedule (Seconds(2.0), &BeaconingProtocol::ScheduleBeaconSending, this); //activated by TTN (22/04/2015)
}

BeaconingProtocol::~BeaconingProtocol ()
{
  Simulator::Cancel (m_sendBEvent);
}

void 
BeaconingProtocol::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
BeaconingProtocol::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

void 
BeaconingProtocol::ScheduleBeaconSending ()
{
  double timeb;

  timeb = ((double)m_node->GetId ()/1000);
  m_sendBEvent = Simulator::Schedule(Seconds (/*1.0 */timeb), 
                                   &BeaconingProtocol::SendBeacon, this);
}

void
BeaconingProtocol::SendBeacon ()
{
  NS_LOG_FUNCTION(this);
	//std::cout<<"BeaconingProtocol::SendBeacon ()"<<std::endl;
  //Create a dummy payload
  Ptr<Packet> packet = Create<Packet> ();

  // Use Layer 3 protocol to send beacons
  Ptr<c2cL3Protocol> c2cl3p = m_node->GetObject<c2cL3Protocol> ();
  struct c2cRoutingProtocol::output routeresult;
  routeresult.route = CreateObject<c2cRoute> ();
  c2cCommonHeader::geoAreaPos* p;
  p = NULL;
  Ptr<c2cAddress> add = CreateObject<c2cAddress> ();
  add->Set (c2cAddress::BROAD, p);
  routeresult.route->SetGateway (add);
  routeresult.packet = packet;
  if (c2cl3p != 0)
    {
      NS_LOG_LOGIC ("Sending Beacon");
      //---------------- check iTETRIS ----------------------------
      NS_LOG_DEBUG ("BeaconingProtocol: Sending Beacon");
      //---------------- check iTETRIS ----------------------------
      c2cl3p->Send (routeresult, 0, Node::C2C_BEACON, Node::C2C_GEOBROADCAST_CIRCLE, TRAFFIC_CLASS);
    }

  //Send periodically the beacon (each 0.5 second)
  m_sendBEvent = Simulator::Schedule (Seconds (BeaconingProtocol::BEACON_INTERVAL),
                                   &BeaconingProtocol::SendBeacon, this);
}

} //namespace
