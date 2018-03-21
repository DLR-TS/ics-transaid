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

#include "ns3/c2c-route.h"

#include "c2c-transport.h"
#include "c2cTransport-header.h"
#include "c2cl4T-socket-factory-impl.h"
#include "c2c-end-point-demux.h"
#include "c2c-end-point.h"
#include "c2cl4T-socket-impl.h"
#include "c2c-l3-protocol.h"

NS_LOG_COMPONENT_DEFINE ("c2cTransport");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cTransport);

//iTETRIS Assumption: c2c stack Transport Protocol Number = 0x0808
const uint8_t c2cTransport::PROT_NUMBER = 1;

TypeId
c2cTransport::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cTransport")
	.SetParent<c2cL4Protocol> ()
	.AddConstructor<c2cTransport> ()
    ;
  return tid;
}

c2cTransport::c2cTransport ()
  : m_endPoints (new c2cEndPointDemux ())
{
  NS_LOG_FUNCTION_NOARGS ();
}

c2cTransport::~c2cTransport ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
c2cTransport::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node>
c2cTransport::GetNode (void)
{
  return m_node;
}

/*
 * This method is called by AddAgregate and completes the aggregation
 * by setting the node in the c2c transport stack and link it to the c2c object
 * present in the node along with the c2c socket factory
 */

void
c2cTransport::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node> node = this->GetObject<Node> ();
      if (node != 0)
        {
          Ptr<c2cL3Protocol> c2c = this->GetObject<c2cL3Protocol> ();
          if (c2c != 0)
            {
              this->SetNode (node);
              c2c->Insert (this);
              Ptr<c2cl4TSocketFactoryImpl> c2cTransportFactory = CreateObject<c2cl4TSocketFactoryImpl> ();
              c2cTransportFactory->Setc2cTransport  (this);
              node->AggregateObject (c2cTransportFactory);
            }
        }
    }
  Object::NotifyNewAggregate ();
}

int
c2cTransport::GetProtocolNumber (void) const
{
  return PROT_NUMBER;
}

void
c2cTransport::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_endPoints != 0)
    {
      delete m_endPoints;
      m_endPoints = 0;
    }
  m_node = 0;
  c2cL4Protocol::DoDispose ();
}

Ptr<Socketc2c>
c2cTransport::CreateSocket (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<c2cl4TSocketImpl> socket = CreateObject<c2cl4TSocketImpl> ();
  socket->SetNode (m_node);
  socket->Setc2cTransport (this);
  return socket;
}

c2cEndPoint *
c2cTransport::Allocate (uint16_t port)
{
  NS_LOG_FUNCTION (this << port);
  return m_endPoints->Allocate (port);
}

void
c2cTransport::DeAllocate (c2cEndPoint *endPoint)
{
  NS_LOG_FUNCTION (this << endPoint);
  m_endPoints->DeAllocate (endPoint);
}

enum c2cL4Protocol::RxStatus
c2cTransport::Receive (Ptr<Packet> packet, Ptr<const c2cAddress> saddr, Ptr<const c2cAddress> daddr, Ptr<c2cInterface> interface)
{
	NS_LOG_FUNCTION (this << packet << interface);

        c2cTransportHeader c2ctransportheader;
        packet->RemoveHeader (c2ctransportheader);
	//---------------- check iTETRIS ----------------------------
	NS_LOG_INFO ("c2cTransport : Receiving Packet From c2cL3Protocol");
	NS_LOG_INFO ("c2cTransport : READING c2c Transport Header");

	//------- iTETRIS: Reading Transport Header Here-------
	NS_LOG_INFO ("c2cTransport : Destination Port = "<<c2ctransportheader.GetDestinationPort());
        NS_LOG_INFO ("c2cTransport : Source Port = "<<c2ctransportheader.GetSourcePort());
	//------- iTETRIS: Reading Transport Header Here-------

	NS_LOG_INFO ("c2cTransport : Do Lookup into the available EndPoints");
	NS_LOG_INFO ("c2cTransport : Call c2c End Point Demux");
	//---------------- check iTETRIS ----------------------------

   c2cEndPointDemux::EndPoints endPoints =
		    m_endPoints->Lookupc2c(daddr, c2ctransportheader.GetDestinationPort(), saddr, c2ctransportheader.GetSourcePort());
   if (endPoints.empty ())
     {
       NS_LOG_LOGIC ("RX_ENDPOINT_UNREACH");
       return c2cL4Protocol::RX_ENDPOINT_UNREACH;
     }

   for (c2cEndPointDemux::EndPointsI endPoint = endPoints.begin ();
        endPoint != endPoints.end (); endPoint++)
     {

       (*endPoint)->ForwardUp (packet->Copy (), saddr, c2ctransportheader.GetSourcePort());
     }
  return c2cL4Protocol::RX_OK;
}

void
c2cTransport::Send (Ptr<Packet> packet, c2cCommonHeader::LongPositionVector sourcePosVector,
                    Ptr<c2cAddress>  daddr, uint16_t sport, 
                    uint16_t dport, uint8_t lt, uint8_t tc, uint16_t sn)
{
  NS_LOG_FUNCTION (this << packet << sport << dport);
  uint16_t ht;

  c2cTransportHeader c2ctransportheader;
  c2ctransportheader.SetDestinationPort (dport);
  c2ctransportheader.SetSourcePort (sport);

  packet->AddHeader (c2ctransportheader);

	NS_LOG_INFO ("c2cTransport : Destination Port = "<<c2ctransportheader.GetDestinationPort());
        NS_LOG_INFO ("c2cTransport : Source Port = "<<c2ctransportheader.GetSourcePort());
  Ptr<c2cL3Protocol> c2c = m_node->GetObject<c2cL3Protocol> ();
   if (c2c != 0)
      {
        Socketc2c::SocketErrno errno_;
        struct c2cRoutingProtocol::output routeresult;
        Ptr<NetDevice> oif (0); //specify non-zero if bound to a source address
        if (c2c->GetRoutingProtocol () != 0)
        {
          routeresult = c2c->GetRoutingProtocol ()->RouteOutput (packet, sourcePosVector, daddr, lt, sn, oif, errno_);
        }
        else
        {
            NS_LOG_ERROR ("No c2c Routing Protocol");
            routeresult.packet = 0;
            routeresult.route = 0;
        }
        //std::cout<<"c2cTransport::Send!!!!!!!"<<Node::C2C_GEOBCAST<<std::endl;
        if (daddr->IsGeoBroadcast())
          ht = Node::C2C_GEOBCAST;
        if (daddr->IsGeoUnicast())
          ht = Node::C2C_UNICAST;
        if (daddr->IsGeoAnycast())
          ht = Node::C2C_GEOANYCAST;
        if (daddr->IsTopoBroadcast())
          ht = Node::C2C_TSB;

        NS_LOG_LOGIC ("Sending to c2c");
        //---------------- check iTETRIS ----------------------------
        NS_LOG_INFO ("c2cTransport : Sending Packet to c2cL3Protocol");
        //---------------- check iTETRIS ----------------------------

        //hst = 0 is hard coded 
        Ptr<c2cRoute> route = routeresult.route;

        if(route->GetGateway() != NULL){
           c2c->Send (routeresult, PROT_NUMBER, ht, 0, tc);
        }
      }
}

}; // namespace ns3

