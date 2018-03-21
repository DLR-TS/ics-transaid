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


#ifndef DVBH_PHY_LAYER_USEREQUIP_H
#define DVBH_PHY_LAYER_USEREQUIP_H


#include "ns3/object.h"
#include "ns3/packet.h"
#include "dvbh-manager.h"
#include "dvbh-controlpacket.h"
#include "ns3/mobility-model.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include <list>


namespace ns3 {

class DVBHOfdmLayer;
class DVBHQueue;
class DVBHTsQueue;

class DVBHPhyLayerUserEquip : public Object
{
public:
  DVBHPhyLayerUserEquip ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback);
  void ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_);
  uint32_t GetNodeIdentifier ();
  int CheckTsPacketPid (int pid);
  void ReceivePsiInfoFromBelow (struct TSPacket packet,bool sleep);
  void AddPidToDesiredList (int value,int pid);
  void RemovePidFromDesiredList (int value,int pid);
  void CreateNewTSQueue (uint32_t streamId);
  void DemultiplexStream (Ptr<Packet> packet,uint32_t serviceId);

  void RebuildStreams ();
  void CreatePESPackets (Ptr<DVBHQueue> streamTsQueue);
  void ExtractMPEPackets ();

  void Sleep (Time sleepTime);
  void WakeUp ();

  void EnableMpeFec ();
  void DisableMpeFec ();
  void SetWakeUpCallback (Callback<void> upCallback);
  void ReceiveFromBelow (Ptr<Packet> packet,bool sleep);

  void SendWakeUp ();
  void SetNodeIdentifier(uint32_t nodeIdentifier);


private:
  uint32_t m_nodeIdentifier;
  Ptr<DVBHQueue> m_PESQueue;

  typedef std::list< int> DesiredStreamList;
  DesiredStreamList m_desiredStreamList;

  typedef std::list< int> DesiredPMTList;
  DesiredPMTList m_desiredPmtList;


  typedef std::list<std::pair<Ptr<DVBHQueue>,uint32_t> > TsQueueList;
  typedef std::list<std::pair<Ptr<DVBHQueue>,uint32_t> >::iterator TsQueueListI;
  TsQueueList m_tsQueueList;
  
  Callback<void, Ptr<Packet>,Ptr<DVBHControlPacket> > m_forwardUp;
  Callback<void> m_wakeUp;
  Ptr<DVBHOfdmLayer> m_ofdmLayer;

  bool m_switchedOff;
  bool m_mpeFecEnabled;
  bool m_multipleServices;








};
}

#endif /* __dvbh_phy_layer__ */
