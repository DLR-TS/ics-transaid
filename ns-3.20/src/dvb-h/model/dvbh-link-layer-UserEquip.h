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


#ifndef DVBH_LINK_LAYER_USEREQUIP_H
#define DVBH_LINK_LAYER_USEREQUIP_H


#include "ns3/object.h"
#include "dvbh-manager.h"
#include "dvbh-controlpacket.h"
#include "ns3/mobility-model.h"
#include <list>


namespace ns3 {

class DVBHPhyLayerUserEquip;

class DVBHLinkLayerUserEquip : public Object
{
public:
  DVBHLinkLayerUserEquip ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetMacAddress (Address address);
  Address GetMacAddress ();

  void SetPhyLayer (Ptr<DVBHPhyLayerUserEquip> phyLayer_);
  void SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback);
  void ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);

  void SetScanningCallback (Callback<void,bool> scanningCallback);
  void InformAboutScan (bool switchedOn);

  void ReceiveFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);

  void AddPidToDesiredList (int value,int id);
  void EnableMpeFec ();

  void DisableMpeFec ();
  void ProcessMPEPacket (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket);
  void ProcessFecStream ();
  void SetFecProcessTime (Time time);
  void ReceiverAwake ();
  void SetNodeIdentifier (uint32_t nodeIdentifier);
  uint32_t GetNodeIdentifier ();
  Time GetFecProcessTime ();


private:
  Callback<void, Ptr<Packet>,Ptr<DVBHControlPacket> > m_forwardUp;
  Callback<void, bool> m_scanning;
  Ptr<DVBHPhyLayerUserEquip> m_phyLayer;

  struct MyPacket
  {
    Ptr<DVBHControlPacket> controlpacket;
    Ptr<Packet> packet;
  };

  typedef std::list<struct MyPacket> MyPacketList;
  typedef std::list<struct MyPacket>::iterator MyPacketListI;
  MyPacketList m_myPacketList;

  Address m_myMacAddress;
  int m_nodeIdentifier;
  bool m_mpeFecEnabled;
  Time m_fecProcessTime;
};
}

#endif /* __dvbh_link_layer_UserEquip_ */
