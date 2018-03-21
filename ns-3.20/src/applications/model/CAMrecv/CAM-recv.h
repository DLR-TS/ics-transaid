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
 *                          CAMReceive.h
 * This is a DEPRECATED METHOD. Please use CAMManage.cc instead
 *
***************************************************************************************/


#ifndef __CAM_recv_h__
#define __CAM_recv_h__

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Socketc2c;
class Packet;

/**
 * \ingroup applications 
 * \defgroup CAMrecv CAMrecv
 *
 * The CAMrecv APP Receives CAM packets at application layer
 *
 */

/**
 * \ingroup CAMrecv
 *
 * \brief  iTETRIS [WP600] - Receive traffic generated to a port
 *
 * The CAMrecv APP Receives CAM packets at ns-3's Facility layer
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class CAMrecv : public Application
{
public:
  static TypeId GetTypeId (void);
  CAMrecv ();

  virtual ~CAMrecv ();
  
protected:
  virtual void DoDispose (void);
private:
  // inherited from Application base class.
  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  void HandleRead (Ptr<Socketc2c>);
  
  Ptr<Socketc2c>     m_socket;
  std::list<Ptr<Socketc2c> > m_socketList;

  uint64_t        m_local;         // Local node to bind to
  uint16_t        m_port;          // Port to bind to
  TypeId          m_tid;           // Protocol TypeId
  uint64_t         m_recvCount;    // Counting Receive
  TracedCallback<Ptr<const Packet>, const uint64_t> m_rxTrace;
};

} // namespace ns3

#endif

