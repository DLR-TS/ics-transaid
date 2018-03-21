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

#ifndef UmtsIfqLayer_H
#define UmtsIfqLayer_H

#include <list>
#include <utility>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/timer.h"
#include "ns3/ipv4-address.h"
#include <string>
#include "umts-manager.h"
#include "ns3/event-id.h"


namespace ns3 {

class ControlPacket;
class Umts;
class UmtsRlcLayer;
class UMTSQueue;

struct user
{
  uint32_t dest_;    // UE address
  double len_;  // support for bsfcHandler
  double bytes_per_tti_;        // bytes allowed to be transmitted in each tti_ for that UE
};

// structure for transmission control for each flow
struct ifq_control
{
  uint32_t flowid_;          // flow id
  double bytes_per_tti_;        // bytes allowed to be transmitted in each tti_
  double len;                           // support for fcHandler
};


class UmtsIfqLayer : public Object
{
public:
  static TypeId GetTypeId (void);
  UmtsIfqLayer (); 
  void ReceivePacketFromRRC (Ptr<Packet> packet);
  void RegistrationNodeB (uint32_t dir, double l);
  void RegistrationNodeUE (uint32_t flow, double l);  
  int Find (uint32_t dir);
  int GetPosition (uint32_t flow);
  int AnyFlowActive ();  
  void IfqHandlerNodeB ();
  void IfqHandlerNodeUE ();
  void SetRLCLayer (Ptr<Object> rlcLayer_);
  void SetTTI (double value);
  double GetTTI ();
  void StartUmtsIfqLayer ();
  
  void SetNodeType(std::string nodeType);
  std::string GetNodeType();
  
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();

private:
  Ptr<UMTSQueue> m_ifqQueue;
  
  double m_time;        // time for TTI
  user users_[MAX_NUM_UE];      // information about UEs
  ifq_control ifqc[MAX_NUM_FLOWS];
  EventId m_ifqTimer;  
  Ptr<UmtsRlcLayer> m_rlc;
  
  uint32_t m_nodeIdentifier;
  std::string m_nodeType;

};
}
#endif
