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


#ifndef DVBH_MANAGER_BASESTATION_H
#define DVBH_MANAGER_BASESTATION_H

#include "ns3/ipv4-address.h"
#include "ns3/object.h"
#include "dvbh-manager.h"
#include "dvbh-manager-UserEquip.h"
#include "ns3/mobility-model.h"
#include <list>


namespace ns3 {

class DVBHLinkLayerBaseStation;
class DVBHControlPacket;
class DVBHOfdmLayer;
class DVBHChannel;

struct DVBHService
{
  std::string serviceName;
  uint32_t serviceId;
  int packetSize;
  double startTime;
  double repetitionTime;
  std::string serviceType;
  bool joined;
  double duration;
  Ipv4Address ipAddress;
  std::list<struct ElementaryStream > streamList;
  std::list<Ptr<DVBHManagerUserEquip> > subscriberList;
  void ReceiveFromAbove(Ptr<Packet> packet);

};

class DVBHManagerBaseStation : public Object
{
public:
  DVBHManagerBaseStation ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetLinkLayer (Ptr<DVBHLinkLayerBaseStation> linkLayer_);
  void SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback);
  void ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket);
  void ForwardUp (Ptr<Packet> packet);
  void SetIpAddress (Ipv4Address address);
  Ipv4Address GetIpAddress ();
  void AddBaseStationManager (Ptr<DVBHManagerBaseStation> manager_);
  void AddVehicle(Ptr<DVBHManagerUserEquip> nodeUE);
  void SetMobility (Ptr<MobilityModel> mobility);
  void SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_);

  Ptr<DVBHChannel> GetSharedChannel (Ptr<DVBHManagerUserEquip> manager_);
  Ptr<DVBHOfdmLayer> GetOfdmLayer ();

  Ptr<MobilityModel> GetMobility ();
  double  GetCoverageRange (void);
  void ReceiveFromAbove(Ptr<Packet> packet);
  void SetCoverageRange (double range);
  int NewConnection (Ptr<DVBHManagerUserEquip> nodeUE);
  void RemoveUserEquip (Ptr<DVBHManagerUserEquip> nodeUE);
  
  void AddNewService (std::string serviceName,uint32_t serviceId,Ipv4Address address,uint32_t burstSize,std::string transmissionType,std::string type,int transportStreamId);
  void RemoveService (std::string serviceName);
  void RemoveServiceStream (int streamId);
  void RemoveUserEquipFromServices (Ptr<DVBHManagerUserEquip> manager_);
  void AddNewSubscriber (uint32_t serviceId,Ptr<DVBHManagerUserEquip> manager_);
  void AddElementaryStreamToService (uint32_t serviceId,uint32_t streamId,std::string streamName,int streamSize);
  void InformAboutSessionStop (struct DVBHService service);
  void InformAboutSessionStart (struct DVBHService service);

  void AddNewTransportStream (double transmissionRate,std::string name);
  void EnableMpeFec ();
  void DisableMpeFec ();

  void SetNodeIdentifier (uint32_t nodeIdentifier);
  uint32_t GetNodeIdentifier ();
  
  uint32_t getRegistedUsers();

  void TriggerVehicleScanning();
  Ipv4Address* GetVehicleIpAddress(uint32_t nodeIdentifier);
  Ipv6Address* GetVehicleIpv6Address(uint32_t nodeIdentifier);


private:
  typedef std::list<struct DVBHService > ServiceList;
  typedef std::list<struct DVBHService >::iterator ServiceListI;
  ServiceList m_serviceList;

  typedef std::list<struct TransportStream > TransportStreamList;
  typedef std::list<struct TransportStream >::iterator TransportStreamListI;
  TransportStreamList m_transportStreamList;

  typedef std::list< Ptr<DVBHManagerUserEquip> > UserEquipList;
  typedef std::list< Ptr<DVBHManagerUserEquip> >::iterator UserEquipListI;
  UserEquipList m_nodeUEList;
  UserEquipList m_registeredUEList;

  typedef std::list<Ptr<DVBHManagerBaseStation> > BaseStationList;
  typedef std::list<Ptr<DVBHManagerBaseStation> >::iterator BaseStationListI;
  BaseStationList m_nodeBList;

  Callback<void, Ptr<Packet> > m_forwardUp;
  Ptr<MobilityModel> m_mobility;

  Ptr<DVBHOfdmLayer> m_ofdm;

  Ptr<DVBHLinkLayerBaseStation> m_linkLayer;
  Ipv4Address m_myIp;
  double m_coverageRange;

  bool m_mpeFecEnabled;
  uint32_t m_nodeIdentifier;
};
}

#endif /* __dvbh_link_layer__ */
