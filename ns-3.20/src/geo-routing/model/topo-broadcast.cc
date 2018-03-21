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


/*
 *
 * Edited by Ramon Bauza
 *
 */

#include "topo-broadcast.h"
#include "topoBroadcast-header.h"
#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("topoBroadcast");

namespace ns3
{

topoBroadcast::topoBroadcast ()
{
}

TypeId
topoBroadcast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::geo-routing::topoBroadcast")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<topoBroadcast> ()
  ;
  return tid;
}

topoBroadcast::~topoBroadcast ()
{
}

void
topoBroadcast::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);

  m_c2c = c2c;
}

void
topoBroadcast::DoDispose ()
{
  m_c2c = 0;
  c2cRoutingProtocol::DoDispose ();
}

struct c2cRoutingProtocol::output
topoBroadcast::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                            Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn, Ptr<NetDevice> oif,
                            Socketc2c::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p->GetUid());

  sockerr = Socketc2c::ERROR_NOTERROR;
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  NS_LOG_INFO ("TopoBroadcast: RouteOutput at Node "<<(m_c2c->GetObject<Node> ())->GetId());

  //Geobroadcast extended header
  topoBroadcastHeader tpheader;
  tpheader.SetLifeTime (lt);
  tpheader.SetSeqNb(sn);
  tpheader.SetSourPosVector (sourcePosVector);

  p->AddHeader (tpheader);
  result.packet = p;

  //C2C route: source and destination address
  Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
  saddr->Set (sourcePosVector.gnAddr, sourcePosVector.Lat, sourcePosVector.Long);
  result.route->SetSource (saddr);

  result.route->SetGateway (daddr);
  result.packet->Print(std::cout);
  NS_LOG_INFO ("");
  return result;
}

bool
topoBroadcast::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                           Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                           LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p->GetUid());
  NS_ASSERT (m_c2c != 0);

  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0);
  int32_t iif = m_c2c->GetInterfaceForDevice (idev);
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  NS_LOG_INFO ("TopoBroadcast: RouteInput at Node "<<(m_c2c->GetObject<Node> ())->GetId());


  if (checkReception (p,header) == false) // check whether this packet has been received before
  {
    Ptr<Packet> packetlcb = p->Copy ();
    topoBroadcastHeader tpheader;
    packetlcb->RemoveHeader (tpheader);

    //Update location table
    Ptr <LocationTable> ltable = (m_c2c->GetObject<Node> ())->GetObject<LocationTable> ();
    if (ltable != 0)
    ltable ->AddPosEntry (tpheader.GetSourPosVector());

    //C2C route: source and destination address
    Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
    saddr->Set (tpheader.GetSourPosVector().gnAddr, tpheader.GetSourPosVector().Lat, tpheader.GetSourPosVector().Long);
    Ptr<c2cAddress> daddr = CreateObject<c2cAddress> ();
    daddr->Set (c2cAddress::BROAD, header.GetHopLimit());

    //Local deliver
    lcb (packetlcb, header, saddr, daddr, iif);
    if (header.GetHopLimit()-1 != 0)
    {
     result.packet = p->Copy ();
     result.route->SetSource (saddr);
     result.route->SetDestination (daddr);
     result.route->SetGateway (daddr);
     ucb (result, header);
     return true;
    }
    else
    {
      return false;
    }
  }
  else
  return false; //Packet processed before

}


bool
topoBroadcast::checkReception (Ptr<const Packet> p, const c2cCommonHeader &commonHeader) 
{
  bool result = false;
  Ptr<Packet> packet = p->Copy ();
  topoBroadcastHeader tpheader;
  packet->RemoveHeader (tpheader);

   AppIndexTag appindexTag;
   bool found;
   found = packet->PeekPacketTag (appindexTag);
   NS_ASSERT (found);
   uint32_t appindex = appindexTag.Get ();

   NS_LOG_INFO ("TopoBroadcast: checkreception: appindex tag= "<< appindex );

  if (m_c2c->GetObject<Node> ()->GetId() != tpheader.GetSourPosVector().gnAddr)
  {
     if ((!m_packetReceived.empty()))  // local node did receive packets before
     {
         packetReceived::iterator iter = m_packetReceived.find (tpheader.GetSourPosVector().gnAddr);
         if (iter != m_packetReceived.end())  // local node did receive packets from this source before
         {
              AppPackets::iterator it =  iter->second.find (appindex);
              if (it != iter->second.end())  // local node did receive packets belonging to this application from this node before
              {
                  for (PacketSequenceNumber::iterator i = it->second.begin (); i != it->second.end (); i++) // scan all the packets belonging to this application received from this node
                  {
                       if (*(i) == tpheader.GetSeqNb()) //the local node has received a packet belonging to this application from this sender with the same SN as this one (the local node has not to retransmit this packet)
                       {
                          result = true;
                          break;
                       }
                  }
                  if (result == false) //the local node has received a packet belonging to this application from this sender but not with the same SN as this one
                  {
                      it->second.push_back (tpheader.GetSeqNb());
                  }
              }
              else //local node did not receive packets belonging to this application from this node before
              {
                  result = false;
                  PacketSequenceNumber v;
                  v.push_back (tpheader.GetSeqNb());
                  AppPackets AppPkts;
                  AppPkts [appindex] = v;
                  m_packetReceived [tpheader.GetSourPosVector().gnAddr] = AppPkts;
              }
           }
           else  // local node did not receive a packet from this source before
             {
                 result = false;
                 PacketSequenceNumber v;
                 v.push_back (tpheader.GetSeqNb());
                 AppPackets AppPkts;
                 AppPkts [appindex] = v;
                 m_packetReceived [tpheader.GetSourPosVector().gnAddr] = AppPkts;
             }
     }
     else  // local node did not receive any packet before
     {
        PacketSequenceNumber v;
        v.push_back (tpheader.GetSeqNb());
        AppPackets AppPkts;
        AppPkts [appindex] = v;
        m_packetReceived [tpheader.GetSourPosVector().gnAddr] = AppPkts;
        result = false;
     }
  }
  else // local node is the packet source (the local node has not to retransmit this packet)
  result = true;
  return result;
}

}
