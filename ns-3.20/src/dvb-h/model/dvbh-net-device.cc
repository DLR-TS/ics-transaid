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

#include "ns3/packet.h"
#include "dvbh-net-device.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/mac48-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "dvbh-phy-layer-BaseStation.h"
#include "dvbh-phy-layer-UserEquip.h"
#include "dvbh-link-layer-BaseStation.h"
#include "dvbh-link-layer-UserEquip.h"
#include "dvbh-ofdm-layer.h"
#include "dvbh-manager-BaseStation.h"
#include "dvbh-manager-UserEquip.h"
#include "dvbh-controlpacket.h"
#include "ns3/net-device.h"
#include "dvbh-channel.h"
#include "ns3/ip-interface-list.h"
#include "ns3/dvbh-tags.h"
#include "ns3/llc-snap-header.h"
#include "ns3/ipv4-header.h"

#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("DvbhNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DvbhNetDevice);

TypeId
DvbhNetDevice::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::DvbhNetDevice")

    .SetParent<NetDevice> ()
    .AddConstructor<DvbhNetDevice> ()
  ;

  return tid;
}

DvbhNetDevice::DvbhNetDevice ()
{  
  mtuSize=2000;

}

DvbhNetDevice::~DvbhNetDevice ()
{

}

void
DvbhNetDevice::DoDispose (void)
{
  if(m_nodeType=="NodeUE")
  {
      m_phyUE->DoDispose ();
      m_ofdm->DoDispose ();
      m_linkLayerUE->DoDispose ();
      m_managerUE->DoDispose ();
      m_ofdm = 0;
      m_phyUE = 0;
      m_linkLayerUE = 0;
      m_managerUE = 0;
  }
  else
  {
      m_phyB->DoDispose ();
      m_ofdm->DoDispose ();
      m_linkLayerB->DoDispose ();
      m_managerB->DoDispose ();
      m_ofdm = 0;
      m_phyB = 0;
      m_linkLayerB = 0;
      m_managerB = 0;
  }

  NetDevice::DoDispose ();

}

void
DvbhNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}
uint32_t
DvbhNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}

  void 
DvbhNetDevice::SetAddress (Address address)
{
  m_address = Mac48Address::ConvertFrom (address);
}

Address 
DvbhNetDevice::GetAddress (void) const
{
  return m_address;
}

void
DvbhNetDevice::SetIpAddress()
{    
  if(m_nodeType=="NodeUE")
  {
    m_managerUE->SetIpAddress(m_node->GetObject<IpInterfaceList>()->GetIpAddress("Dvbh"));
  }
  else
  {
    m_managerB->SetIpAddress(m_node->GetObject<IpInterfaceList>()->GetIpAddress("Dvbh"));
  }
}


Ptr<Channel>
DvbhNetDevice::GetChannel (void) const
{
   return m_ofdm->GetChannel ();  
}

Ptr<DVBHChannel>
DvbhNetDevice::DoGetChannel (void) const
{
  return m_ofdm->GetChannel ();  
}

bool
DvbhNetDevice::SetMtu (const uint16_t mtu)
{
  mtuSize = mtu;
  return true;
}
uint16_t
DvbhNetDevice::GetMtu (void) const
{

  return mtuSize;
}

Ptr<Node>
DvbhNetDevice::GetNode (void) const
{
  return m_node;
}
void
DvbhNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
  m_nodeIdentifier=node->GetId();    
  CompleteConfig (node->GetId());
}

void 
DvbhNetDevice::SetNodeType(std::string type_)
{
 m_nodeType=type_; 
   
}

std::string 
DvbhNetDevice::GetNodeType()
{
  return m_nodeType;
}

bool
DvbhNetDevice::IsBridge (void) const
{
  return false;
}

bool
DvbhNetDevice::IsLinkUp (void) const
{
  return false;
}

void
DvbhNetDevice::SetMobility(Ptr<MobilityModel> mobility)
{      
  if(m_nodeType=="NodeUE")
  {   
    m_managerUE->SetMobility(mobility);
  }
  else
  {
    m_managerB->SetMobility(mobility);
  }
    
}

bool
DvbhNetDevice::NeedsArp (void) const
{
  return false;
}

bool
DvbhNetDevice::IsPointToPoint (void) const
{
  return false;
}

void
DvbhNetDevice::SetLinkChangeCallback (Callback<void> callback)
{
}
bool
DvbhNetDevice::IsBroadcast (void) const
{
  return true;
}

bool
DvbhNetDevice::SupportsSendFrom (void) const
{
  return false;
}


bool 
DvbhNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{

  DvbhDeltaTTag deltaTag;
  packet->PeekPacketTag(deltaTag);
  
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
    
  if(m_nodeType=="NodeUE")
  {    
      /*Not Implemented*/
  }
  else
  {
    m_managerB->ReceiveFromAbove(packet);
  }

 
  return true;
}

Address
DvbhNetDevice::GetBroadcast (void) const
{
  return Mac48Address::GetBroadcast ();
}
bool
DvbhNetDevice::IsMulticast (void) const
{
  return true;
}
Address
DvbhNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address::GetMulticast (multicastGroup);
}
Address DvbhNetDevice::GetMulticast (Ipv6Address addr) const
{
  return Mac48Address::GetMulticast (addr);
}

void
DvbhNetDevice::CompleteConfig (uint32_t id)
{

  if(m_nodeType=="NodeUE")
  {  
    m_phyUE->SetOfdmLayer (m_ofdm);
    m_linkLayerUE->SetPhyLayer (m_phyUE);
    m_managerUE->SetOfdmLayer (m_ofdm);    
    m_managerUE->SetLinkLayer (m_linkLayerUE);
    m_managerUE->SetForwardUpCallback (MakeCallback (&DvbhNetDevice::ForwardUp,this));
    m_managerUE->SetNodeIdentifier(m_node->GetId());
  
  }
  else
  {
    m_phyB->SetOfdmLayer (m_ofdm);
    m_linkLayerB->SetPhyLayer (m_phyB);
    m_managerB->SetOfdmLayer (m_ofdm);
    m_managerB->SetLinkLayer (m_linkLayerB);
    m_managerB->SetForwardUpCallback (MakeCallback (&DvbhNetDevice::ForwardUp,this));
    m_managerB->SetNodeIdentifier(m_node->GetId());
  }
  
  SetMobility(m_node->GetObject<MobilityModel> ());

}

void
DvbhNetDevice::SetManager ()
{
  if(m_nodeType=="NodeUE")
  {
    m_managerUE = CreateObject<DVBHManagerUserEquip>();
  }
  else
  {
    m_managerB = CreateObject<DVBHManagerBaseStation>();
  }

}

void
DvbhNetDevice::SetPhyLayer (Ptr<Object> phy)
{
  if(m_nodeType=="NodeUE")
  {
    m_phyUE = DynamicCast<DVBHPhyLayerUserEquip>(phy);
  }
  else
  {
    m_phyB= DynamicCast<DVBHPhyLayerBaseStation>(phy);
  }

}

void
DvbhNetDevice::SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm)
{
  m_ofdm = ofdm;

}

void
DvbhNetDevice::SetLinkLayer (Ptr<Object> linkLayer)
{
  if(m_nodeType=="NodeUE")
  {
    m_linkLayerUE = DynamicCast<DVBHLinkLayerUserEquip>(linkLayer);
  }
  else
  {
    m_linkLayerB= DynamicCast<DVBHLinkLayerBaseStation>(linkLayer);
  }
  
}


Ptr<Object>
DvbhNetDevice::GetManager ()
{
  if(m_nodeType=="NodeUE")
  {
    return m_managerUE;
  }
  else
  {
    return m_managerB;
  }
  
}

Ptr<Object>
DvbhNetDevice::GetPhyLayer ()
{
  if(m_nodeType=="NodeUE")
  {
    return m_phyUE;
  }
  else
  {
    return m_phyB;
  }
}

Ptr<DVBHOfdmLayer>
DvbhNetDevice::GetOfdmLayer ()
{
  return m_ofdm;
}

Ptr<Object>
DvbhNetDevice::GetLinkLayer ()
{
  if(m_nodeType=="NodeUE")
  {
    return m_linkLayerUE;
  }
  else
  {
    return m_linkLayerB;
  }
  
  
}

bool
DvbhNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{

  return true;
}

void
DvbhNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
}


void
DvbhNetDevice::SetReceiveCallback ( NetDevice::ReceiveCallback cb)
{
  m_forwardUp = cb;
}

void
DvbhNetDevice::ForwardUp (Ptr<Packet> packet)
{
  LlcSnapHeader llc;
    
  packet->RemoveHeader (llc);
      
  Ipv4Header header;
  packet->PeekHeader(header);
      
  m_forwardUp(this,packet,llc.GetType(),Mac48Address::Allocate());

}

} // namespace ns3
