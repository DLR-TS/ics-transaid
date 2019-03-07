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

#include <stdlib.h>
#include "ns3/mobility-model.h"
#include "geo-broadcast.h"
#include "geoBroadAnycast-header.h"
#include "utils.h"
#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("geoBroadcast");
namespace ns3
{

TypeId
geoBroadcast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::geo-routing::geoBroadcast")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<geoBroadcast> ()
  ;
  return tid;
}

geoBroadcast::geoBroadcast ()
{
}

geoBroadcast::~geoBroadcast ()
{
}

void
geoBroadcast::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);

  m_c2c = c2c;
}

void
geoBroadcast::DoDispose ()
{
  m_c2c = 0;
  c2cRoutingProtocol::DoDispose ();
}

struct c2cRoutingProtocol::output
geoBroadcast::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                           Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                           Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p->GetUid());

  sockerr = Socketc2c::ERROR_NOTERROR;
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  //Geobroadcast extended header
  GeoABcastHeader bheader;
  bheader.SetLifeTime (lt);
  bheader.SetSeqNb(sn);
  bheader.SetSourPosVector (sourcePosVector);
  bheader.SetGeoAreaPos1 (*(daddr->GetGeoAreaPos1 ()));
  if ((daddr->GetGeoAreaPos2 ()) != NULL)
  bheader.SetGeoAreaPos2 (*(daddr->GetGeoAreaPos2 ()));
  bheader.SetAreaSize (daddr->GetAreaSize ());

  p->AddHeader (bheader);
  result.packet = p;

  //C2C route: source and destination address
  Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
  saddr->Set (sourcePosVector.gnAddr, sourcePosVector.Lat, sourcePosVector.Long);
  result.route->SetSource (saddr);
  result.route->SetDestination (daddr);
//   result.route->SetOutputDevice ();

  if (CartesianDistance (sourcePosVector.Lat, sourcePosVector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon) < daddr->GetAreaSize())
  {
    result.route->SetGateway (daddr);
    result.packet->Print(std::cout);
  }
  else
  {

    if ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> () != 0)
    {
    struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
    if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId ())
    {
      //route:gateway
      Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
      gw->Set (vector.gnAddr, vector.Lat, vector.Long);
      result.route->SetGateway (gw);
    }
    }
  }
  return result;
}

bool
geoBroadcast::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                          Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                          LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p->GetUid());
  NS_ASSERT (m_c2c != 0);

  // Check if input device supports IP
  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0);
  int32_t iif = m_c2c->GetInterfaceForDevice (idev);
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  if (checkReception (p, header) == false)
  {
    Ptr<Packet> packetlcb = p->Copy ();
    GeoABcastHeader bheader;
    packetlcb->RemoveHeader (bheader);

    struct c2cCommonHeader::geoAreaPos* areapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
    struct c2cCommonHeader::geoAreaPos* areapos2 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
    if (areapos1 != NULL)
    *(areapos1) =  bheader.GetGeoAreaPos1 ();
    if (areapos2 != NULL)
    *(areapos2) =  bheader.GetGeoAreaPos2 ();


     Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
     saddr->Set (bheader.GetSourPosVector().gnAddr, bheader.GetSourPosVector().Lat, bheader.GetSourPosVector().Long);

     Ptr<c2cAddress> daddr = CreateObject<c2cAddress> ();
     daddr->Set (c2cAddress::BROAD, areapos1, areapos2, bheader.GetAreaSize());

     free (areapos1);
     free (areapos2);

     result.packet = p->Copy ();
     result.route->SetSource (saddr);
     result.route->SetDestination (daddr);

     //Update location table
     Ptr <LocationTable> ltable = (m_c2c->GetObject<Node> ())->GetObject<LocationTable> ();
     if (ltable != 0)
     ltable ->AddPosEntry (bheader.GetSourPosVector());

     //Position is retrieved from the mobility model
     c2cCommonHeader::ShortPositionVector m_posvector;
     Ptr<MobilityModel> model = m_c2c->GetObject<Node>()->GetObject<MobilityModel> ();
     m_posvector.gnAddr = m_c2c->GetObject<Node>()->GetId ();
     m_posvector.Lat = (uint32_t) (model->GetPosition().x<0?0:model->GetPosition().x); // Modified by acorrea to avoid overflow
     m_posvector.Long = (uint32_t) (model->GetPosition().y<0?0:model->GetPosition().y); // Modified by acorrea to avoid overflow


     double distanceReceiver = CartesianDistance(m_posvector.Lat, m_posvector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);
     if (distanceReceiver <= daddr->GetAreaSize())
     {
      //Local deliver
      lcb (packetlcb, header, saddr, daddr, iif);
      result.route->SetGateway (daddr);
      ucb (result, header);
      return true;
     }

     else
     {

     double distanceLastF = CartesianDistance(header.GetSourPosVector().Lat, header.GetSourPosVector().Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);

     if (distanceLastF <= daddr->GetAreaSize())
     {
        //drop packet coming from a forwarder located inside the destination area
        return false;
     }
     else
     {
       struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
       double distanceM = CartesianDistance (vector.Lat, vector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);
       if (distanceM < distanceLastF)
       {
         if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId())
         {
            Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
            gw->Set  (vector.gnAddr, vector.Lat, vector.Long);
            result.route->SetGateway (gw);

            ucb (result, header);
            return true;
         }
         else
         {
           return false;
         }
       }
       else
       {
         return false;
       }
     }
    }
  }
  else
  return false; //Packet processed before
}


bool
geoBroadcast::checkReception (Ptr<const Packet> p, const c2cCommonHeader &commonHeader)
{
  bool result = false;
  Ptr<Packet> packet = p->Copy ();
  GeoABcastHeader bheader;
  packet->RemoveHeader (bheader);

   AppIndexTag appindexTag;
   bool found;
   found = packet->PeekPacketTag (appindexTag);
   NS_ASSERT (found);
   uint32_t appindex = appindexTag.Get ();

   NS_LOG_INFO ("TopoBroadcast: checkreception: appindex tag= "<< appindex );

  if (m_c2c->GetObject<Node> ()->GetId() != bheader.GetSourPosVector().gnAddr)
  {
     if ((!m_packetReceived.empty()))  // local node did receive packets before
     {
         packetReceived::iterator iter = m_packetReceived.find (bheader.GetSourPosVector().gnAddr);
         if (iter != m_packetReceived.end())  // local node did receive packets from this source before
         {
              AppPackets::iterator it =  iter->second.find (appindex);
              if (it != iter->second.end())  // local node did receive packets belonging to this application from this node before
              {
                  for (PacketSequenceNumber::iterator i = it->second.begin (); i != it->second.end (); i++) // scan all the packets belonging to this application received from this node
                  {
                       if (*(i) == bheader.GetSeqNb()) //the local node has received a packet belonging to this application from this sender with the same SN as this one (the local node has not to retransmit this packet)
                       {
                          result = true;
                          break;
                       }
                  }
                  if (result == false) //the local node has received a packet belonging to this application from this sender but not with the same SN as this one
                  {
                      it->second.push_back (bheader.GetSeqNb());
                  }
              }
              else //local node did not receive packets belonging to this application from this node before
              {
                  result = false;
                  PacketSequenceNumber v;
                  v.push_back (bheader.GetSeqNb());
                  AppPackets AppPkts;
                  AppPkts [appindex] = v;
                  m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
              }
           }
           else  // local node did not receive a packet from this source before
             {
                 result = false;
                 PacketSequenceNumber v;
                 v.push_back (bheader.GetSeqNb());
                 AppPackets AppPkts;
                 AppPkts [appindex] = v;
                 m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
             }
     }
     else  // local node did not receive any packet before
     {
        PacketSequenceNumber v;
        v.push_back (bheader.GetSeqNb());
        AppPackets AppPkts;
        AppPkts [appindex] = v;
        m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
        result = false;
     }
  }
  else // local node is the packet source (the local node has not to retransmit this packet)
  result = true;
  return result;
}
}
