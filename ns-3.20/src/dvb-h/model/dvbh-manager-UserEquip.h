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

#ifndef DVBH_MANAGER_NODEUE_H
#define DVBH_MANAGER_NODEUE_H


#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "dvbh-controlpacket.h"
#include "ns3/mobility-model.h"
#include <list>


namespace ns3 {

class DVBHLinkLayerUserEquip;
class DVBHOfdmLayer;
class DVBHManagerBaseStation;
class DVBHChannel;
class DvbhIpPer;

class DVBHManagerUserEquip : public Object
{
public:
  DVBHManagerUserEquip ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetLinkLayer (Ptr<DVBHLinkLayerUserEquip> linkLayer_);

  void SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback);
  void ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket);
  void ForwardUp (Ptr<Packet> packet);
  void SetIpAddress (Ipv4Address address);
  //NEW
  void SetIpv6Address (Ipv6Address address);
  
  Ipv4Address GetIpAddress ();
  //NEW
  Ipv6Address GetIpv6Address ();
  bool IsInCoverageArea (Ptr<DVBHManagerBaseStation> manager_);

  Ptr<DVBHManagerBaseStation> GetBaseStationManager (Ipv4Address ipAddress);
  void SetupConnection ();
  void PeriodicScanning (void);
  Ptr<DVBHManagerBaseStation> ScanningBaseStation (void);
  void SetMobility (Ptr<MobilityModel> mobility);
  void SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_);

  Ptr<DVBHOfdmLayer> GetOfdmLayer ();

  Ptr<MobilityModel> GetMobility ();
  void ServiceStopNotice (struct DVBHService service);
  void ServiceStartNotice (struct DVBHService service);
  void AddBaseStation(Ptr<DVBHManagerBaseStation> manager_);

  void HandoverProcedure ();

  void AddPATTableSection (struct PATTable table);
  void AddPMTTableSection (struct PMTTable table);
  void AddSDTTableSection (struct SDTTable table);
  void ReceivePATTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void ReceiveSDTTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void ReceivePMTTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void ReceiveDataFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);

  void DecideToJoinService ();
  void DecisionMaker ();
  void ReadPidValuesFromPMT (struct PMTTable table);

  void EnableMpeFec ();
  void DisableMpeFec ();
  void SwitchOffReceiver (bool switchedOff);
  void SetSimultaneousServiceNumber (int number);

  void PresetDesiredServiceType (std::string type);
  void InformBaseStationAboutSubscription (int serviceId);

  void SetNodeIdentifier (uint32_t nodeIdentifier);
  uint32_t GetNodeIdentifier ();
  void JoinService(uint32_t serviceId,std::string type);
  
  bool CheckIpPER(double receivedPw);

private:

  typedef std::list<struct PATTable> PatSectionList;
  typedef std::list<struct PATTable>::iterator PatSectionListI;
  PatSectionList m_patSectionList;

  typedef std::list<struct PMTTable> PmtSectionList;
  typedef std::list<struct PMTTable>::iterator PmtSectionListI;
  PmtSectionList m_pmtSectionList;

  typedef std::list<struct SDTTable> SdtSectionList;
  typedef std::list<struct SDTTable>::iterator SdtSectionListI;
  SdtSectionList m_sdtSectionList;

  typedef std::list< Ptr<DVBHManagerBaseStation> > BaseStationList;
  typedef std::list< Ptr<DVBHManagerBaseStation> >::iterator BaseStationListI;
  BaseStationList m_nodeBList;

  typedef std::list< struct PATEntry > AttachedStreams;
  AttachedStreams m_attachedStreams;

  Callback<void, Ptr<Packet> > m_forwardUp;
  Ptr<MobilityModel> m_mobility;

  Ptr<DVBHLinkLayerUserEquip> m_linkLayer;
  Ptr<DVBHManagerBaseStation> m_nodeBManager;

  Ipv4Address m_nodeBIp;

  Ptr<DVBHOfdmLayer> m_ofdm;
  Ptr<DvbhIpPer> m_perCalculator;

  Ipv4Address m_myIp;
  //NEW
  Ipv6Address m_myIpv6;
  bool m_mpeFecEnabled;
  bool m_scanningEnabled;
  uint32_t m_simultaneousServiceNumber;
  int m_nodeIdentifier;
  std::string m_desiredServicetype;

  int m_size;
};
}

#endif /* __dvbh_manager_UserEquip__ */
