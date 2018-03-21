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

#include "dvbh-queue.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "dvbh-manager.h"
#include "dvbh-phy-layer-BaseStation.h"


NS_LOG_COMPONENT_DEFINE ("DVBHQueue");


namespace ns3 {

TypeId
DVBHQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHQueue");

  return tid;
}

DVBHQueue::DVBHQueue ()
{
  m_queueSize = 0;
  m_pesQueueSize = 0;
  m_tsQueueSize=0;
  m_limit = 100000;
}

void
DVBHQueue::SetQueueLimit (int limit_)
{
  m_limit = limit_;
}

int
DVBHQueue::GetQueueLimit ()
{
  return m_limit;
}

int32_t
DVBHQueue::GetPesSize ()
{
  return m_pesQueueSize;
}

int32_t
DVBHQueue::GetTsSize ()
{
  return m_tsQueueSize;
}

void
DVBHQueue:: Enqueue (Ptr<Packet> packet)
{
  if (m_queueSize < m_limit)
    {
      m_packetQueue.push_back (packet->Copy ());
      m_queueSize++;
    }
}

void
DVBHQueue::EnqueuePES (Ptr<Packet> packet)
{
  if (m_pesQueueSize < m_limit)
    {
      m_pesPacketQueue.push_back (packet);
      m_pesQueueSize++;
    }
}

void
DVBHQueue::EnqueueTS (struct TSPacket packet)
{
  if (m_tsQueueSize < m_limit)
    {
      m_tsPacketQueue.push_back (packet);
      m_tsQueueSize++;
    }
}

Ptr<Packet>
DVBHQueue::DequeuePES ()
{
  Ptr<Packet> i;
  if (!m_pesPacketQueue.empty ())
    {
      m_pesQueueSize--;

      return m_pesPacketQueue.front ();
    }
  i = 0;
  return i ;
}

struct TSPacket
DVBHQueue::DequeueTS ()
{
  TSPacket i;
  if (!m_tsPacketQueue.empty ())
    {
      i = m_tsPacketQueue.front ();
      m_tsPacketQueue.pop_front ();
      m_tsQueueSize--;

      return i;
    }
  i.id = -1;
  i.size = -1;
  return i ;
}

Ptr<Packet>
DVBHQueue::Dequeue ()
{

  if (!m_packetQueue.empty ())
    {
      Ptr<Packet> packet = m_packetQueue.front ()->Copy ();
      m_packetQueue.pop_front ();
      m_queueSize--;
      return packet;
    }

  return 0;
}


void
DVBHQueue::RemovePacket (Ptr<Packet> packet)
{

  if (!m_packetQueue.empty ())
    {
      for (PacketQueueI i = m_packetQueue.begin (); i != m_packetQueue.end (); i++)
        {

          if (*i == packet)
            {
              m_packetQueue.erase (i);
              m_queueSize--;
            }
        }
    }

}

void
DVBHQueue::RemoveAll ()
{

  for (PacketQueueI i = m_packetQueue.begin (); i != m_packetQueue.end ();)
    {
      if (!m_packetQueue.empty ())
        {

          i = m_packetQueue.erase (i);
          m_queueSize--;

        }
    }
}

int32_t
DVBHQueue::GetSize ()
{
  return m_queueSize;
}

Ptr<DVBHQueue>
DVBHQueue::Copy ()
{
  Ptr<DVBHQueue> newQueue = Create<DVBHQueue> ();

  for (PacketQueueI iterator = m_packetQueue.begin (); iterator != m_packetQueue.end (); iterator++)
    {
      newQueue->Enqueue ((*iterator));
    }

  return newQueue;
}



NS_LOG_COMPONENT_DEFINE ("DVBHTsQueue");
TypeId
DVBHTsQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHTsQueue");

  return tid;
}

DVBHTsQueue::DVBHTsQueue ()
{
  m_size = 0;
  m_limit = 10000;
}

void
DVBHTsQueue::Enqueue (struct TSPacket packet)
{
  if (m_size < m_limit)
    {
      m_tsQueue.push_back (packet);
      m_size++;
    }
}

struct TSPacket
DVBHTsQueue::Dequeue ()
{
  TSPacket i;
  if (!m_tsQueue.empty ())
    {
      i = m_tsQueue.front ();
      m_tsQueue.pop_front ();
      m_size--;

      return i;
    }
  i.id = -1;
  i.size = -1;
  return i ;
}

void
DVBHTsQueue::SetStreamId (int id)
{
  m_streamId = id;
}

Ptr<DVBHTsQueue>
DVBHTsQueue::Copy ()
{
  Ptr<DVBHTsQueue> newQueue = Create<DVBHTsQueue> ();

  for (TSQueueI iterator = m_tsQueue.begin (); iterator != m_tsQueue.end (); iterator++)
    {
      newQueue->Enqueue ((*iterator));
    }

  return newQueue;
}

int
DVBHTsQueue::GetStreamId ()
{
  return m_streamId;
}

int
DVBHTsQueue::IsEmpty ()
{
  return m_tsQueue.empty ();
}

int
DVBHTsQueue::GetSize ()
{
  return m_size;
}

int32_t
DVBHTsQueue::GetContentSize ()
{
  int32_t sum = 0;
  for (TSQueueI iterator = m_tsQueue.begin (); iterator != m_tsQueue.end (); iterator++)
    {      
      sum += 188;
    }

  return sum;
}



} // namespace ns3

