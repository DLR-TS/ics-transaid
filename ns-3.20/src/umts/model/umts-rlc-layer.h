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

#ifndef UMTS_RLC_LAYER_H
#define UMTS_RLC_LAYER_H

#include "ns3/nstime.h"
#include "ns3/ipv4-address.h"
#include "ns3/mac48-address.h"
#include "umts-queue.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "umts-manager.h"
#include "ns3/timer.h"
#include <list>
#include <utility>


namespace ns3 {

class UmtsMacLayer;

class UmtsRlcLayer : public Object
{

public:
  UmtsRlcLayer ();
  ~UmtsRlcLayer ();
  static TypeId GetTypeId (void);
  void SetIpAddress (Ipv4Address address);
  void SetIpv6Address (Ipv6Address address);
  void DoDispose ();
  void SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback);

  void ResetReceiverValues(int pos);
  void ReceiveFromBelow (Ptr<Packet> packet,uint8_t packetType);
  void ReceiveFromAbove (Ptr<Packet> packet,uint32_t destinationId);
  void ForwardUp (Ptr<Packet> packet);
  void SetMac (Ptr<UmtsMacLayer> mac);
  void SendDown (Ptr<Packet> packet);
  void RecvACK (Ptr<Packet> packet);
  void SendUpDATA (Ptr<Packet> packet,int pos);
  void EnqueueDATA (Ptr<Packet> packet,int pos);
  void SendDownDATA (int pos);
  void SendACK (int pos);
  void SendDownDATAonACK (int pos, int seqno);
  int store_flow (int acked, int fraged, uint32_t flowid, uint32_t destinationId, int handover,enum TxType txType);
  void RTxHandler (uint32_t src, int flow);
  void SetDelay (Time delay);
  void SetFragmentationSize (int size);
  int GetFragmentationSize () const;
  void AddANode (uint32_t destinationId, int flow);
  void DeleteANode (uint32_t destinationId, int flow);
  void start (uint32_t destinationId, int flow, Time time);
  void stop (uint32_t destinationId, int flow);
  void remove_flow (uint32_t flowid,uint32_t destinationId);
  int look_for (uint32_t flow,uint32_t destinationId);
  flows_nodeb info_[MAX_LOAD];          // array with each flow information
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();
  
  void SetNodeType(std::string type);
  std::string GetNodeType();


protected:
                  
  Ptr<UMTSQueue> m_packetList;  
  Ipv4Address m_myIp;
  Ipv6Address m_myIpv6;
  Time m_delay;
  int m_rlcLayerFragmentSize;

  struct TimerList;
  typedef std::list<struct TimerList> TimerQueue;
  typedef std::list<struct TimerList >::iterator TimerQueueI;

  TimerQueue m_timerQueue;
  struct TimerList
  {

    Timer *timer;
    int flowId;
    uint32_t address;
  };
  Callback<void, Ptr<Packet> > m_forwardUp;
  uint32_t m_nodeIdentifier;



private:
  uint32_t m_bufferLength;                  // length for buf_
  Ptr<UmtsMacLayer> m_mac;
  std::string m_nodeType;
};



} // namespace ns3

#endif /*UMTS_RLC_LAYER*/
