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


#ifndef DVBH_LINK_LAYER_BASESTATION_H
#define DVBH_LINK_LAYER_BASESTATION_H

#include "ns3/object.h"
#include "dvbh-manager.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include <list>
#include "dvbh-controlpacket.h"
#include "ns3/mobility-model.h"

namespace ns3 {

class DVBHPhyLayerBaseStation;

class DVBHLinkLayerBaseStation : public Object
{
public:
  DVBHLinkLayerBaseStation ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetMacAddress (Address address);
  Address GetMacAddress ();

  void SetPhyLayer (Ptr<DVBHPhyLayerBaseStation> phyLayer_);
  void SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback);
  void ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void CreateMPEFrames (Ptr<Packet> packet,Time delta_t,uint8_t  lastSection);
  void ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket);
  void ReceiveFromAbove (Ptr<Packet> packet,Time delta_t,uint8_t lastSection);

  void SendDownMPEPacket (Ptr<Packet> packet);

  void PMTTableHandler ();
  void PATTableHandler ();
  void SDTTableHandler ();

  void CreateNewPMTTable (int programNumber,int tableId);

  void AddPatSection (int transportStreamId);
  void RemovePatSection (int transportStreamId);
  void AddNewTS (std::string name,double transmissionRate);
  void RemoveTS (std::string name);

  void AddEntrySDTTable (int programNumber,std::string type,int transportStreamId);
  void AddSdtSection (int transportStreamId);

  void AddEntryPATTable (int programNumber,int programMapPid,int transportStreamId);

  void AddEntryPMTTable (int programId,int elementaryStreamPID,std::string streamType);

  void RemovePMTTable (int tableId);
  void NewServiceStreamAdded (int streamId);
  void SetFecCalculationTime (Time time);
  void CalculateMPEFECTime (struct ElementaryStream stream,Time delta_t);
  void CreateMPEFECFrames (Ptr<Packet> packet,Time delta_t);
  void CreateMPEFECRsPackets (int pid,Time delta_t);
  void EnableMpeFec ();

  void DisableMpeFec ();

  void SetNodeIdentifier (uint32_t nodeIdentifier);
  uint32_t GetNodeIdentifier ();

  void ResetSectionCounter ();
  int GetSectionCounter ();

private:
  typedef std::list<struct TransportStream > StreamList;
  typedef std::list<struct TransportStream>::iterator StreamListI;
  StreamList streamList;

  typedef std::list<struct PATTable> PatSectionList;
  typedef std::list<struct PATTable>::iterator PatSectionListI;
  PatSectionList patSectionList;

  typedef std::list<struct SDTTable> SdtSectionList;
  typedef std::list<struct SDTTable>::iterator SdtSectionListI;
  SdtSectionList sdtSectionList;

  typedef std::list<struct PMTTable> PmtSectionList;
  typedef std::list<struct PMTTable>::iterator PmtSectionListI;
  PmtSectionList pmtSectionList;

  SDTTable m_sdtTable;
  PATTable m_patTable;
  //PMTTable m_pmtTable;

  EventId m_sdtTableTimer;
  EventId m_pmtTableTimer;
  EventId m_patTableTimer;

  Callback<void, Ptr<Packet>,Ptr<DVBHControlPacket> > m_forwardUp;
  Ptr<DVBHPhyLayerBaseStation> m_phyLayer;
  Address m_myMacAddress;

  Time m_pmtSendTime;
  Time m_patSendTime;
  Time m_sdtSendTime;
  bool mpeFecEnabled;
  Time m_fecCalculationTime;
  int m_nodeIdentifier;
  int m_fecTableRows;

  uint8_t m_sectionCounter;
};
}

#endif /* __dvbh_link_layer_BaseStation_ */
