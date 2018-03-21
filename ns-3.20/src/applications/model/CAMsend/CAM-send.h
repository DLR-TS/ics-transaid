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

/****************************************************************************************
 *                          CAMSend.h
 * This is a DEPRECATED METHOD. Please use CAMManage.cc instead
 *
***************************************************************************************/


#ifndef __CAM_send__h__
#define __CAM_send_h__

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/channel-tag.h"

namespace ns3 {

class Socketc2c;

/**
 * \ingroup applications 
 * \defgroup CAMsend CAMsend
 *
 */
 /**
 * \ingroup CAMsend
 *
 * \brief  iTETRIS [WP600] - Initiates packet transmission (of defined packet size) at a defined frequency.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class CAMsend : public Application
{
public:
  static TypeId GetTypeId (void);

  CAMsend ();

  virtual ~CAMsend();

protected:
  virtual void DoDispose (void);
private:
  // inherited from Application base class.
  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  //helpers
  void SendPacket();

  Ptr<Socketc2c>  m_socket;       // Associated socket
  uint64_t        m_peer;         // Peer NodeID
  uint64_t        m_sendCount;
  uint16_t        m_port;         // Port Number
  uint32_t        m_pktSize;      // Size of packets
  EventId         m_sendEvent;    // Eventid of pending "send packet" event
  bool            m_sending;      // True if currently in sending state
  TypeId          m_tid;
  TracedCallback<Ptr<const Packet> > m_txTrace;
  uint16_t        m_frequency;    // Packet sending frequency
  DataRate        m_dataRate;
  ChannelTag      m_channeltag;
  
private:
  void ScheduleNextTx();
};

} // namespace ns3

#endif

