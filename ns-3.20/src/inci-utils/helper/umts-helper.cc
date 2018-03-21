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

#include "umts-helper.h"
#include "ns3/umts-phy-layer-bs.h"
#include "ns3/umts-rrc-layer-bs.h"
#include "ns3/umts-userequipment-manager.h"
#include "ns3/umts-phy-layer-ue.h"
#include "ns3/umts-rlc-layer.h"
#include "ns3/umts-mac-layer.h"
#include "ns3/umts-rrc-layer-ue.h"
#include "ns3/umts-basestation-manager.h"
#include "ns3/umts-net-device.h"
#include "ns3/log.h"
#include "ns3/pcap-writer.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("UMTSHelper");

namespace ns3 {

UMTSChannelHelper::UMTSChannelHelper ()
{}

UMTSChannelHelper 
UMTSChannelHelper::Default (void)
{
  UMTSChannelHelper helper;
  return helper;
}

Ptr<UMTSChannel> 
UMTSChannelHelper::Create (void) const
{
  Ptr<UMTSChannel> channel = CreateObject<UMTSChannel> ();

  return channel;
}

UMTSPhyHelper::~UMTSPhyHelper ()
{}

UMTSPhyHelper
UMTSPhyHelper::Default (void)
{
  UMTSPhyHelper helper;
 
  return helper;
}

void 
UMTSPhyHelper::Set (std::string name, const AttributeValue &v)
{
  
  m_phy.Set (name, v);
}

void
UMTSPhyHelper::SetNodeType(std::string type)
{
  if(type=="NodeUE")
  {
    m_phy.SetTypeId ("ns3::UmtsPhyLayerUE"); 
  }
  else if(type=="NodeB")
  {
    m_phy.SetTypeId ("ns3::UmtsPhyLayerBS"); 
  }
   
}


Ptr<Object> 
UMTSPhyHelper::Create (std::string type) const
{
  if(type=="NodeUE")
  {      
      Ptr<UmtsPhyLayerUE> phy = m_phy.Create<UmtsPhyLayerUE> ();      
      return phy;
  }
  else
  {
    Ptr<UmtsPhyLayerBS> phy = m_phy.Create<UmtsPhyLayerBS> ();
    return phy;
  }
 
}

void 
UMTSPhyHelper::SetChannel (Ptr<UMTSChannel> channel)
{
  m_channel = channel;
}


UMTSMacHelper::~UMTSMacHelper ()
{}

UMTSMacHelper
UMTSMacHelper::Default (void)
{
  UMTSMacHelper helper;
 
  return helper;
}

Ptr<Object> 
UMTSMacHelper::Create (std::string type) const
{
     Ptr<UmtsMacLayer> mac = CreateObject<UmtsMacLayer> ();  
     return mac;
 
}
UMTSRlcHelper::UMTSRlcHelper()
{
  
}

UMTSRlcHelper::~UMTSRlcHelper ()
{}

UMTSRlcHelper
UMTSRlcHelper::Default (void)
{
  UMTSRlcHelper helper;
 
  return helper;
}

Ptr<Object> 
UMTSRlcHelper::Create (std::string type) const
{
    Ptr<UmtsRlcLayer> rlc =CreateObject<UmtsRlcLayer> ();
    return rlc;  
}

UMTSRrcHelper::~UMTSRrcHelper ()
{}

UMTSRrcHelper
UMTSRrcHelper::Default (void)
{
  UMTSRrcHelper helper;
 
  return helper;
}

Ptr<Object> 
UMTSRrcHelper::Create (std::string type) const
{
  if(type=="NodeUE")
  {
    Ptr<UmtsRrcLayerUE> rrc = CreateObject<UmtsRrcLayerUE> ();
    return rrc;
  }
  else
  {
    Ptr<UmtsRrcLayerBS> rrc = CreateObject<UmtsRrcLayerBS> ();
    return rrc;
  }
  
}



UMTSHelper::UMTSHelper ()
{}

UMTSHelper
UMTSHelper::Default (void)
{
  UMTSHelper helper;
 
  return helper;
}
//At each node of the Node Container install the UMTS device???
NetDeviceContainer 
UMTSHelper::Install (const UMTSPhyHelper &phyHelper,std::string nodeType,NodeContainer c) const
{
  NetDeviceContainer devices;
  
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<UMTSNetDevice> device = CreateObject<UMTSNetDevice> ();
      device->SetNodeType(nodeType);
      
      Ptr<UMTSChannel> broadcastChannel=Create<UMTSChannel>();	  
      Ptr<Object> phy = phyHelper.Create (nodeType);
            
      UMTSMacHelper macHelper;
      Ptr<Object> mac=macHelper.Create(nodeType);

      UMTSRlcHelper rlcHelper;
      Ptr<Object> rlc=rlcHelper.Create(nodeType);
      
      UMTSRrcHelper rrcHelper;
      Ptr<Object> rrc=rrcHelper.Create(nodeType);
      
      device->SetPhy(phy,broadcastChannel);
      device->SetMac(mac);
      device->SetRlc(rlc);
      device->SetIFQ();
      device->SetRrc(rrc);
      device->SetManager();
      device->SetAddress (Mac48Address::Allocate ());

      (*i)->AddDevice (device);
      devices.Add (device);
            
    }
  return devices;
}

NetDeviceContainer 
UMTSHelper::Install (const UMTSPhyHelper &phy,std::string nodeType,Ptr<Node> node) const
{
  return Install (phy,nodeType, NodeContainer (node));
}

NetDeviceContainer 
UMTSHelper::Install (const UMTSPhyHelper &phy,
                     std::string nodeType, std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (phy, nodeType, NodeContainer (node));
}

Ptr<UMTSNetDevice> 
UMTSHelper::NetDeviceInstall (NodeContainer c,Ptr<UMTSChannel> broadcastChannel,std::string nodeType)
{
    
    Ptr<Node> node = *c.Begin();

    Ptr<UMTSNetDevice> device = CreateObject<UMTSNetDevice> ();
    device->SetNodeType(nodeType);
        
    UMTSPhyHelper phyHelper;
    phyHelper.SetNodeType(nodeType);
    Ptr<Object> phy = phyHelper.Create (nodeType);
        
    UMTSMacHelper macHelper;
    Ptr<Object> mac=macHelper.Create(nodeType);

    UMTSRlcHelper rlcHelper;
    Ptr<Object> rlc=rlcHelper.Create(nodeType);
    
    UMTSRrcHelper rrcHelper;
    Ptr<Object> rrc=rrcHelper.Create(nodeType);
  
    device->SetPhy(phy,broadcastChannel);
    device->SetMac(mac);
    device->SetRlc(rlc);
    device->SetIFQ();
    device->SetRrc(rrc);
    device->SetManager();
    device->SetAddress (Mac48Address::Allocate ());    
    node->AddDevice (device);    
      
  return device;
}




} // namespace ns3
