/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
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
 * Author:Sendoa Vaz
 */

#ifndef UMTSQueue_H
#define UMTSQueue_H

#include <list>
#include <utility>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "controlpacket.h"
#include <string>


namespace ns3 {

class UMTSQueue : public Object
{
public:
  static TypeId GetTypeId (void);
  UMTSQueue ();
  void SetQueueLimit (uint32_t limit_);
  uint32_t GetQueueLimit ();
  void PhyEnqueue (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void DropTill (int32_t seqno);
  void RemoveAll ();
  void PhyEnqueueHead (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  uint32_t GetPhySize ();
  uint32_t GetSize();
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > PhyDequeue ();
  void PhyRemoveAll ();
  Ptr<Packet> GetNext (Ptr<Packet> packet);
  Ptr<Packet> GetHead ();
  void Enqueue(Ptr<Packet> packet);
  Ptr<Packet> Dequeue();
  void RemovePacket (Ptr<Packet> packet);
  void EnqueueHead (Ptr<Packet> packet);
  
private:
  typedef std::list<std::pair<Ptr<Packet>, Ptr<ControlPacket> > > PhyPacketQueue;
  typedef std::list<std::pair<Ptr<Packet>, Ptr<ControlPacket> > >::iterator PhyPacketQueueI;
  
  typedef std::list<Ptr<Packet> > PacketQueue;
  typedef std::list<Ptr<Packet> >::iterator PacketQueueI;
  
  uint32_t m_queueSize;
  uint32_t m_phyQueueSize;

  PhyPacketQueue m_phyPacketQueue;
  PacketQueue m_packetQueue;

  uint32_t m_limit;

};
}
#endif
