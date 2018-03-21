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
#ifndef SsCommandManager_H
#define SsCommandManager_H

#include <stdint.h>
#include <list>
#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "ns3/node.h"
#include "wimax-phy.h"
#include "service-flow.h"
#include "ns3/enum.h"
#include "ns3/mobility-model.h"
#include "ns3/ipv4-header.h"
#include "ns3/node-id-tag.h" 
#include "ns3/time-step-tag.h" 
#include "ns3/TStep-sequence-number-tag.h"
#include "ns3/app-index-tag.h"
#include <map>

namespace ns3 {

class BsCommandManager;
class NetDevice;
class SubscriberStationNetDevice;
class BandwidthManager;
class ServiceFlow;
class WimaxPhy;
class Vector2D;
class WimaxChannel;
class WimaxConnection;
class Cid;

class SsCommandManager : public Object
{
public:
  static TypeId GetTypeId (void);
  SsCommandManager ();
  virtual ~SsCommandManager ();
  void SetNetDevice (Ptr<NetDevice> device);
  void SetIdRegisteredBs (uint32_t bsid);
  void SetScheduling (enum ServiceFlow::SchedulingType schedulingtype);
  void SetMyCid(Cid Cid);
  void SetMulticastGroupId (uint8_t groupId);
  void AddBsManager (Ptr<BsCommandManager> bsManager);

  Ptr<SubscriberStationNetDevice> GetSs(void);
  Ptr<BsCommandManager> GetBsManager (uint32_t index);
  uint32_t GetSfid(void);
  Mac48Address GetMacAddress (void);
  uint32_t GetNodeId (void);
  Ipv4Address* GetVehicleIpAddress (void);
  //NEW
  Ipv6Address* GetVehicleIpv6Address (void);
  Ptr<BsCommandManager> GetRegisteredBsManager (void);
  uint8_t  GetMulticastGroupId(void);

  void InitiateConnection (void);
  void InitiateConnectionIp (Ipv4Address ssIpAddress, ServiceFlow::Direction);
  void Bconnection (Cid c);
  void Pconnection (Cid c);
  void BandwidthRequest (Ptr<BsCommandManager> bsManager);
  bool PeriodicScanning (void);
  uint32_t ScanningBss(void);
  bool PrepareConnection (Ipv4Address dest);
  void InitiateBandwidthRequest (void); 
  void RegisterInBS (uint32_t bsId);

  void SendUnicastPacket (uint32_t size);
  void SendUnicastPacketIp (uint32_t size, Ipv4Address dest);

  void DeleteRegister(void);
  void DeleteConnection(void);

  bool IsInCoverageArea (Ptr<BsCommandManager> bsManager);
  void JoinMulticastGroup (uint8_t groupId);
  void LeaveMulticastGroup (void);

  void ActivateNode (void);
  void DeactivateNode (void);
  bool IsNodeActive (void);

  void SsON(void);
  bool GetOnOff(void);
  void SetOnOff(bool valor);
  void ResetNBytes (void);

  struct PacketTags {
    NodeIdTag nodeTag;
    AppIndexTag appIndexTag;
    TimeStepTag timeStepTag;
    TStepSequenceNumberTag tSSeqNTag;
  };

  void AddPacketTags (Ipv4Address address, PacketTags* packetTags);
  PacketTags* GetPacketTags (Ipv4Address address);

  Ptr<SubscriberStationNetDevice> m_ssDevice;

protected:

  virtual void DoDispose (void);

private:

  typedef std::map<Ipv4Address, PacketTags*> PacketTagList; 
  PacketTagList m_packetTagList;
  std::list< Ptr< BsCommandManager > > m_bsManagers;
  ServiceFlow::SchedulingType  m_schedulingType;
  Ptr<BsCommandManager> m_registeredBsManager;
  uint32_t m_idRegisteredBd;
  bool m_nodeActive;
  bool m_ssPhyOn;
  Cid m_cid;
  uint8_t m_groupId;
  uint32_t m_sfid;

  void RemoveOldPacketTags (void);
  Ptr<MobilityModel> GetMobility(void);
	
};

} // namespace ns3 

#endif /* SsCommandManager_H */
