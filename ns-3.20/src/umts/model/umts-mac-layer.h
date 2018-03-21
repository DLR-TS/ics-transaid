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

#ifndef UMTS_MAC_LAYER_H
#define UMTS_MAC_LAYER_H

#include "umts-phy-layer-bs.h"
#include "umts-phy-layer-ue.h"

#include "umts-manager.h"


namespace ns3 {

/* Umts Mac layer in NodeB. */
class UmtsMacLayer : public Object
{

public:
  UmtsMacLayer ();
  static TypeId GetTypeId (void);
  void ReceiveFromAbove (Ptr<Packet> packet);
  void ReceiveFromBelow (Ptr<Packet> packet,uint8_t packetType);
  void ForwardUp (Ptr<Packet> packet,uint8_t packetType);
  void SetPhy (Ptr<Object> phy);
  void SetAddress (Mac48Address ad);
  Mac48Address GetAddress () const;
  void DoDispose ();
  void SendDown (Ptr<Packet> packet,uint8_t packetType);
  
  void SetNodeType(std::string nodeType);
  std::string GetNodeType();

  void SetForwardUpCallback (Callback<void,Ptr<Packet>,uint8_t > upCallback);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();

private:
  Callback<void, Ptr<Packet>,uint8_t > m_forwardUp;
  Ptr<UmtsPhyLayerBS> m_phyB;
  Ptr<UmtsPhyLayerUE> m_phyUE;

  Mac48Address m_self;
  std::string m_nodeType;
  uint32_t m_nodeIdentifier;

};

}

#endif
