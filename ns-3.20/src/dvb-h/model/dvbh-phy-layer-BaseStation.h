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

#ifndef DVBH_PHY_LAYER_BASESTATION_H
#define DVBH_PHY_LAYER_BASESTATION_H


#include "ns3/object.h"
#include "ns3/packet.h"
#include "dvbh-manager.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"

#include <list>
#include "dvbh-controlpacket.h"

namespace ns3 {


class DVBHQueue;
class DVBHTsQueue;
class DVBHOfdmLayer;
class DVBHChannel;
class PesHeader;



class DVBHPhyLayerBaseStation : public Object
{
public:
  DVBHPhyLayerBaseStation ();
  static TypeId GetTypeId (void);
  void DoDispose ();

  void SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback);
  void ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdmLayer_);
  void SetChannel (Ptr<DVBHChannel> channel_);

  void ReceiveFromAbove (Ptr<Packet> packet);
  int CreatePESPacket (Ptr<Packet> mpePacket,Ptr<Packet> packet,PesHeader pesHeader,uint32_t serviceId);
  int CreateTSPacket (Ptr<Packet> pesPacket,Ptr<DVBHQueue> streamTsQueue,uint32_t sid,int count,int first);
  void CreateTSPacketFromPMT (struct PMTTable pmtTable);
  void CreateTSPacketFromPAT (struct PATTable patTable);
  void CreateTSPacketFromSDT (struct SDTTable sdtTable);
  void SplitPESPackets ();

  void SendDownPackets (struct TSPacket packet);
  uint32_t GetNodeIdentifier ();
  void DownStreamHandler (uint32_t pid);
  void ReceiveFromBelow (struct TSPacket packet,bool sleep);
  void CreateNewTSQueue (int streamId);
  Ptr<DVBHQueue> GetTSQueue (uint32_t streamId);

  void SetOffTime (Time time);
  Time GetOffTime ();
  void EnableMpeFec ();
  void DisableMpeFec ();
  void SetNodeIdentifier(uint32_t nodeIdentifier);

  void SendDownBurst (Ptr<DVBHQueue> queue,uint32_t serviceId);

private:
  Ptr<DVBHQueue> m_PESQueue;
  Ptr<DVBHQueue> m_TSQueue;
  uint32_t m_nodeIdentifier;
  
  Ptr<DVBHOfdmLayer> m_ofdmLayer;
  
  Callback<void, Ptr<Packet>,Ptr<DVBHControlPacket> > m_forwardUp;
  typedef std::list<std::pair<Ptr<DVBHQueue>,uint32_t> > TsQueueList;
  typedef std::list<std::pair<Ptr<DVBHQueue>,uint32_t> >::iterator TsQueueListI;
  TsQueueList m_tsQueueList;
  
  Ptr<Packet> m_usingPesPacket;
  Ptr<MobilityModel> m_mobility;

  uint32_t m_fragmentCount;

  Time m_transmissionTime;
  Time m_offTime;
  bool m_mpeFecEnabled;



};
}

#endif /* __dvbh_phy_layer__ */
