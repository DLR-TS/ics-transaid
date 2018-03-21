/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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
 * Author: Ramon Bauza <rbauza@umh.es>, David Valiente <dvaliente@umh.es>
 */
#ifndef BsCommandManager_H
#define BsCommandManager_H

#include <stdint.h>
#include <list>
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "ns3/mobility-model.h"
#include "ns3/enum.h"
#include "wimax-phy.h"
#include "cid.h"
#include "ns3/ipv4-header.h"
#include "service-flow.h"
#include "ns3/node-id-tag.h" 
#include "ns3/time-step-tag.h" 
#include "ns3/TStep-sequence-number-tag.h"
#include "ns3/app-index-tag.h"
#include <map>

namespace ns3 {

class SsCommandManager;
class NetDevice;
class BaseStationNetDevice;
class ServiceFlow;

class WimaxPhy;
class Mac48Address;

class WimaxChannel;
class WimaxConnection;

class BsCommandManager : public Object
{
public:

  static TypeId GetTypeId (void);
  BsCommandManager ();
  virtual ~BsCommandManager ();

  void SetNetDevice (Ptr<NetDevice> device);
  void SetCoverageRange (double coverageRange);
  void SetBsId(uint32_t bsid);
  void BsON(void);
  void SetSsPhyOn (void);
  void SetSsOff (void);
  void SetSsPhyOff (void);
  void SetOnOff (bool valor);
  void ActivateNodes (Ipv4Address dest);

  Ptr<BaseStationNetDevice> GetBs (void);
  uint32_t GetBsId(void);
  Ptr<MobilityModel> GetMobility (void);
  Mac48Address GetMacAddress (void);
  double GetCoverageRange (void);
  Ptr<WimaxChannel> GetChannel();
  bool GetOnOff (void);
  Ipv4Address* GetVehicleIpAddress (uint32_t nodeId);
  //NEW
  Ipv6Address* GetVehicleIpv6Address (uint32_t nodeId);
  uint32_t GetNumberOfRegisteredSs (void);
  Ipv4Address* GetBsIpAddress (void);
  uint32_t GetNodeId (void);
  uint32_t GetLatitude (void);
  uint32_t GetLongitude (void);
  Ptr<SsCommandManager> GetSsManagerByIpAddress (Ipv4Address address);
  void TriggerVehiclesScanning (void);
  void AddSsManager (Ptr<SsCommandManager> ssManager);
  void AddNonRegisteredSsManager (Ptr<SsCommandManager> ssManager);
  void ClearSsCommandManager (Ptr<SsCommandManager> ssCommandManager);
  void ClearNonRegisteredSsCommandManager (Ptr<SsCommandManager> ssCommandManager);

  Ptr<SsCommandManager> GetNonRegisteredSsManager (uint32_t index);
  Ptr<SsCommandManager> GetNonRegisteredSsManager (Mac48Address ssAdress);
  Ptr<SsCommandManager> GetNonRegisteredSsManager (Ipv4Address ssAddress);

  void InitSfidIndex(void);
  void InitiateBroadcastConnection(void);
  void InitiateMulticastConnections(uint8_t numGroups);
  bool JoinMulticastGroup (uint8_t groupId);
  void LeaveMulticastGroup (uint8_t groupId);
  void RegisterSS (Ptr<SsCommandManager> ssCommandManager, Mac48Address ssAddress, WimaxPhy::ModulationType modulationType);
  ServiceFlow* ConnectSS (Mac48Address ssAddress, ServiceFlow* SSserviceFlow);
  ServiceFlow* ConnectSSIp (Mac48Address ssAddress, ServiceFlow* SSserviceFlow, Ipv4Address dest, ServiceFlow::Direction direction);
  void ProcessBwRequest(uint32_t bytesToRequest, uint32_t sfid);
  bool PrepareConnection (Ipv4Address dest);
  void PrepareBroadcastTxon (void);

  void DeleteRegister(Mac48Address ssAddress);
  void DeleteConnection (Cid cid, uint32_t sfid);
  void ResetOldRecords (Mac48Address ssAddress); 

  bool SendOnePacket (Mac48Address ssAddress);
  bool SendOnePacketIp (Mac48Address ssAddress);
  void SendBroadcastPacket(void);
  void SendBroadcastPacketIp (void);
  void SendMulticastPacket(uint8_t groupId);

  struct PacketTags {
    NodeIdTag nodeTag;
    AppIndexTag appIndexTag;
    TimeStepTag timeStepTag;
    TStepSequenceNumberTag tSSeqNTag;
  };

  void AddPacketTags (Ipv4Address address, PacketTags* packetTags);
  PacketTags* GetPacketTags (Ipv4Address address);

protected:

  virtual void DoDispose (void);

private:

  typedef std::map<Ipv4Address, PacketTags*> PacketTagList; 
  PacketTagList m_packetTagList;
  Ptr <BaseStationNetDevice> m_bsDevice;
  std::list< Ptr< SsCommandManager > > m_ssManagers;
  std::list< Ptr< SsCommandManager > > m_nonRegisteredSsManagers; 
  double m_coverageRange;
  bool m_bsPhyOn;
  uint32_t m_sfidIndex;
  uint32_t m_bsId;
  uint8_t m_nSSgroup[];
  uint8_t m_noGroups;

  Ptr<SsCommandManager> GetSsManager (uint32_t index);
  Ptr<SsCommandManager> GetSsManager (Mac48Address ssAdress);
  Ptr<SsCommandManager> GetSsManagerByNodeId (uint32_t nodeId);

  void RemoveOldPacketTags (void);
  ServiceFlow* CreateServiceFlowBroadcast (Ptr<WimaxConnection> conn);
  ServiceFlow* CreateServiceFlowUnicast (Ptr<WimaxConnection> conn, Ipv4Address ssIpAddress, ServiceFlow::Direction direction);
  bool GetAreServiceFlowsAllocatedDown (std::vector<ServiceFlow*> servFlows);

  void RefreshModulation(WimaxPhy::ModulationType modulationType,  Mac48Address ssAddress);
	
};

} // namespace ns3 

#endif /* BsCommandManager_H */
