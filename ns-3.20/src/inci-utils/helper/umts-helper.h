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

#ifndef UMTS_HELPER_H
#define UMTS_HELPER_H

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/umts-channel.h"

namespace ns3 {

class UMTSNetDevice;
class UmtsPhyLayerUE;
class UmtsRlcLayer;
class UmtsMacLayer;
class UmtsRrcLayerUE;
class Node;

class UMTSChannelHelper
{
public:

  UMTSChannelHelper ();
  static UMTSChannelHelper Default (void);  
  Ptr<UMTSChannel> Create (void) const;

};


class UMTSPhyHelper
{
public:
  ~UMTSPhyHelper ();
  void SetNodeType(std::string type);
  Ptr<Object> Create (std::string type) const;
   static UMTSPhyHelper Default ();
  void SetChannel (Ptr<UMTSChannel> channel);
  void Set (std::string name, const AttributeValue &v);

private:

  ObjectFactory m_phy;
  Ptr<UMTSChannel> m_channel;
};


class UMTSMacHelper
{
public:
  ~UMTSMacHelper ();
  Ptr<Object> Create (std::string type) const;
   static UMTSMacHelper Default (void);
private:

  ObjectFactory m_mac;
};

class UMTSRlcHelper
{
public:
  UMTSRlcHelper();
  ~UMTSRlcHelper ();
  Ptr<Object> Create (std::string type) const;
   static UMTSRlcHelper Default (void);
private:

  ObjectFactory m_rlc;

};

class UMTSRrcHelper
{
public:
  ~UMTSRrcHelper ();
  Ptr<Object> Create (std::string type) const;
   static UMTSRrcHelper Default (void);
private:

  ObjectFactory m_rrc;

};



class UMTSHelper
{
public:

  UMTSHelper ();

  static UMTSHelper Default (void);
  
  NetDeviceContainer Install (const UMTSPhyHelper &phyHelper,std::string nodeType,NodeContainer c) const;
  NetDeviceContainer Install (const UMTSPhyHelper &phy, std::string nodeType, Ptr<Node> node) const;
  NetDeviceContainer Install (const UMTSPhyHelper &phy,std::string nodeType, std::string nodeName) const;
  NetDeviceContainer Install (NodeContainer c,std::string nodeType,Ptr<UMTSChannel> broadcastChannel) ;

  Ptr<UMTSNetDevice>  NetDeviceInstall(NodeContainer c,Ptr<UMTSChannel> broadcastChannel,std::string nodeType);
  
  
};

} // namespace ns3

#endif /* UMTS_HELPER_H */
