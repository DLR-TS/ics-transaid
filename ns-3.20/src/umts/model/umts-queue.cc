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


#include "umts-queue.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "umts-rlc-am-header.h"


NS_LOG_COMPONENT_DEFINE ("UMTSQueue");

namespace ns3 {

TypeId
UMTSQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UMTSQueue");

  return tid;
}

UMTSQueue::UMTSQueue ()
{
  m_queueSize = 0;
  m_phyQueueSize = 0;
  m_limit = 10000;	//Maximum number of packets per queue statically set to 10000
}

void
UMTSQueue::SetQueueLimit (uint32_t m_limit_)
{
  m_limit = m_limit_;
}

uint32_t
UMTSQueue::GetQueueLimit ()
{
  return m_limit;
}


// Insert the packet in back of the queue

void
UMTSQueue:: PhyEnqueue (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  if (m_phyQueueSize < m_limit)
    {
      m_phyPacketQueue.push_back (std::make_pair(packet,controlpacket));
      m_phyQueueSize++;
    }
}

void
UMTSQueue:: PhyEnqueueHead (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  if (m_phyQueueSize < m_limit)
    {
       m_phyPacketQueue.insert (m_phyPacketQueue.begin (),std::make_pair(packet,controlpacket));
       m_phyQueueSize++;
    }


}

void
UMTSQueue:: Enqueue (Ptr<Packet> packet)
{
  if (m_queueSize < m_limit)
    {   
      m_packetQueue.push_back (packet);
      m_queueSize++;
    }
}

// Insert the packet in the head of the queue

void
UMTSQueue:: EnqueueHead (Ptr<Packet> packet)
{
  if (m_queueSize < m_limit)
    {
      m_packetQueue.insert (m_packetQueue.begin (),packet);
      m_queueSize++;
    }


}


// Removes all packets with a seqno less to a certain seqno from the queue and free the packets belonging to the queued pointers.

void
UMTSQueue::DropTill (int32_t seqno)
{

  if (m_queueSize > 0)
    {
      for (PacketQueueI i = m_packetQueue.begin (); i != m_packetQueue.end (); )
        {
	  UmtsRlcAmHeader amHeader;
	  (*i)->PeekHeader(amHeader);
	  
          if (amHeader.GetSequenceNumber () < seqno && amHeader.GetSequenceNumber () >= 0)
            {
              i = m_packetQueue.erase (i);
              m_queueSize--;
            }
          else
            {
              i++;
            }


        }
    }
}

std::pair<Ptr<Packet>, Ptr<ControlPacket> >
UMTSQueue::PhyDequeue ()
{
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > pair;
  if (!m_phyPacketQueue.empty ())
    {
      pair = m_phyPacketQueue.front ();
      m_phyPacketQueue.pop_front ();
      m_phyQueueSize--;
     
      return pair;
    }
  return pair ;
}

Ptr<Packet>
UMTSQueue::Dequeue ()
{
  Ptr<Packet> i;
  if (!m_packetQueue.empty ())
    {
      i = m_packetQueue.front ();
      m_packetQueue.pop_front ();
      m_queueSize--;
      
      return i;
    }

  return 0 ;
}


Ptr<Packet>
UMTSQueue::GetHead ()
{
  PacketQueueI iterator = m_packetQueue.begin ();
  if (m_packetQueue.empty ())
    {      
      return 0;
    }
  else
    {

      return (*iterator);
    }

}

Ptr<Packet>
UMTSQueue::GetNext (Ptr<Packet> packet)
{
  bool found = 0;
  if (!m_packetQueue.empty ())
    {
      for (PacketQueueI i = m_packetQueue.begin (); i != m_packetQueue.end (); i++)
        {
          if (found == 1)
            {
              found = 0;
              if ((*i)!= 0)
                {
                  return (*i);
                }
            }
          if (*i == packet)
            {
              found = 1;
            }

        }
    }
 
  return 0;
}

void
UMTSQueue::RemovePacket (Ptr<Packet> packet)
{

  if (!m_packetQueue.empty ())
    {
      for (PacketQueueI i = m_packetQueue.begin (); i != m_packetQueue.end (); i++)
        {

          if (*i== packet)
            {
              i = m_packetQueue.erase (i);
              m_queueSize--;
            }
        }
    }

}

void
UMTSQueue::RemoveAll ()
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

void
UMTSQueue::PhyRemoveAll ()
{

  for (PhyPacketQueueI i = m_phyPacketQueue.begin (); i != m_phyPacketQueue.end ();)
    {
      if (!m_phyPacketQueue.empty ())
        {

          i = m_phyPacketQueue.erase (i);
          m_phyQueueSize--;

        }
    }

}

uint32_t
UMTSQueue::GetSize ()
{
  return m_queueSize;
}

uint32_t
UMTSQueue::GetPhySize ()
{
  return m_phyQueueSize;
}

} // namespace ns3

