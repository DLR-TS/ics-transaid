/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#ifndef DVBHQueue_H
#define DVBHQueue_H

#include <list>
#include <utility>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "dvbh-controlpacket.h"
#include "dvbh-manager.h"
#include "dvbh-phy-layer-BaseStation.h"
#include <string>


namespace ns3 {

struct DVBHPacket
{
  Ptr<Packet> packet;
  Ptr<DVBHControlPacket> controlInfo;
};

struct TSPacket
{
  int id;
  int size;
  int payloadUnitStart;
  int counter;
  Ptr<DVBHControlPacket> controlpacket;
  Ptr<Packet> packet;
};

class DVBHQueue : public Object
{
public:
  static TypeId GetTypeId (void);
  DVBHQueue ();
  void SetQueueLimit (int limit_);
  int GetQueueLimit ();
  void Enqueue (Ptr<Packet> packet);
  void EnqueuePES (Ptr<Packet> packet);
  Ptr<Packet> DequeuePES ();
  struct TSPacket DequeueTS ();
  void RemoveAll ();

  int32_t GetSize ();
  int32_t GetPesSize ();
  int32_t GetTsSize ();
  void SetStreamId (int id);
  int GetStreamId ();
  Ptr<Packet> Dequeue ();
  void EnqueueTS (struct TSPacket packet);
  Ptr<DVBHQueue> Copy ();
  void RemovePacket (Ptr<Packet> packet);
  
private:
  typedef std::list<Ptr<Packet> > PacketQueue;
  typedef std::list<Ptr<Packet> >::iterator PacketQueueI;

  PacketQueue m_packetQueue;

  typedef std::list<Ptr<Packet> > PESPacketQueue;
  PESPacketQueue m_pesPacketQueue;

  typedef std::list<struct TSPacket> TSPacketQueue;
  TSPacketQueue m_tsPacketQueue;

  int32_t m_limit;
  int32_t m_queueSize;
  int32_t m_pesQueueSize;
  int32_t m_tsQueueSize;

  uint32_t m_streamId;

};

class DVBHTsQueue : public Object
{
public:
  static TypeId GetTypeId (void);
  DVBHTsQueue ();
  void SetStreamId (int id);
  int GetStreamId ();
  void Enqueue (struct TSPacket packet);
  struct TSPacket Dequeue ();
  int IsEmpty ();
  int GetSize ();
  Ptr<DVBHTsQueue> Copy ();
  int32_t GetContentSize ();


private:
  typedef std::list<struct TSPacket> TSQueue;
  typedef std::list<struct TSPacket>::iterator TSQueueI;
  TSQueue m_tsQueue;

  int m_streamId;
  int m_size;
  int m_limit;
};

}
#endif
