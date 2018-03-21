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

#ifndef DVBH_CHANNEL_H
#define DVBH_CHANNEL_H

#include <vector>
#include "ns3/packet.h"
#include "ns3/channel.h"
#include <list>
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "dvbh-manager.h"
#include "dvbh-phy-layer-BaseStation.h"
#include "ns3/net-device.h"
#include "ns3/cost231_model.h"

namespace ns3 {

class DVBHControlPacket;
class DVBHOfdmLayer;

struct ServiceRegistry
{
  int serviceId;
  std::string serviceType;
  std::list<Ptr<DVBHOfdmLayer> > destinationList;
};

class DVBHChannel : public Channel
{
public:
  DVBHChannel ();
  ~DVBHChannel ();
  static TypeId GetTypeId (void);
  Ptr<DVBHChannel> GetChannel ();
  void SendToBaseStation (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void ReceiveFromUserEquip (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void SendToUE (struct OFDMBlock block, bool isFirstBlock, uint64_t frequency,Ptr<DVBHQueue> burst,double txPower);
  void ReceiveFromBaseStation (struct TSPacket tspacket);

  void Dispose ();

  uint32_t GetNDevices (void) const
  {
    return 0;
  }
  Ptr<NetDevice> GetDevice (uint32_t i) const
  {
    return 0;
  }

  void RemoveUserEquip (Ptr<DVBHOfdmLayer> phyNode);
  void AddNewUserEquip (Ptr<DVBHOfdmLayer> phyNode);
  void AddNewService (int serviceId,std::string type);

  void AddNewDestination (int serviceId,Ptr<DVBHOfdmLayer> phyAddress);
  void RemoveUserFromService (int serviceId,Ptr<DVBHOfdmLayer> phyAddress);
  void RemoveService (int serviceId);
  double ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                            double nodeBHeight,double nodeUEHeight);
  void SetOfdmOfBaseStation (Ptr<DVBHOfdmLayer> ofdmLayer);
  double CalculateRxPowerFromBaseStation (Ptr<DVBHOfdmLayer> destinationOfdmLayer);



private:
  Ptr<Cost231Propagation> m_costPropagationModel;
  typedef std::list< Ptr<DVBHOfdmLayer> > UserEquipList;
  UserEquipList m_nodeUEList;
  
  ServiceRegistry m_registry;
  typedef std::list< struct ServiceRegistry> RegistryList;
  typedef std::list< struct ServiceRegistry>::iterator RegistryListI;
  RegistryList m_registryList;

  Ptr<DVBHOfdmLayer> m_ofdmLayer;

};

} // namespace ns3

#endif /* DVBHCHANNEL_H */
