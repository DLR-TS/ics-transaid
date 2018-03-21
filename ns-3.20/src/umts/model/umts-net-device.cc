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
#include "umts-net-device.h"
#include "umts-channel.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/mac48-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "controlpacket.h"
#include "ns3/mobility-model.h"
#include "umts-phy-layer-bs.h"
#include "ns3/llc-snap-header.h"
#include "umts-rrc-layer-bs.h"
#include "umts-basestation-manager.h"
#include "umts-phy-layer-ue.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "umts-rrc-layer-ue.h"
#include "umts-userequipment-manager.h"

#include "umts-rlc-layer.h"
#include "umts-mac-layer.h"
#include "umts-ifq-layer.h"

#include "ns3/ip-interface-list.h"

NS_LOG_COMPONENT_DEFINE ("UMTSNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UMTSNetDevice);

// non so dove trovarla, la dichiaro qui
bool m_EnableIpv6;

TypeId
UMTSNetDevice::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::UMTSNetDevice")

    .SetParent<NetDevice> ()
    .AddConstructor<UMTSNetDevice> ()
  ;

  return tid;
}

UMTSNetDevice::UMTSNetDevice ()
{
  mtuSize = 1024;
  m_EnableIpv6 = true;  
}

UMTSNetDevice::~UMTSNetDevice ()
{

}

void
UMTSNetDevice::DoDispose (void)
{
  if(m_nodeType=="NodeB")
  {
    m_phyB->DoDispose ();
    m_mac->DoDispose ();
    m_rlc->DoDispose ();
    m_rrcB->DoDispose ();
    m_mac = 0;
    m_phyB = 0;
    m_rlc = 0;
    m_rrcB = 0;
    m_ifq = 0;
  }
  else
  {
    m_phyUE->DoDispose ();   
    m_rrcUE->DoDispose ();    
    m_phyUE = 0;    
    m_rrcUE = 0;
    
  }
 
  NetDevice::DoDispose ();
}

void 
UMTSNetDevice::SetNodeType(std::string type_)
{
 m_nodeType=type_; 
}

std::string 
UMTSNetDevice::GetNodeType()
{
  return m_nodeType;
}

void
UMTSNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}
uint32_t
UMTSNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}

void
UMTSNetDevice::EnableIpv6(void) 
{
  m_EnableIpv6 = true;
}


Ptr<Channel>
UMTSNetDevice::GetChannel (void) const
{
   if(m_nodeType=="NodeUE")
  {
    return m_phyUE->GetChannel ();
  }
  else
  {
    return m_phyB->GetChannel ();
  }  
}

Ptr<UMTSChannel>
UMTSNetDevice::DoGetChannel (void) const
{
  if(m_nodeType=="NodeUE")
  {
    return m_phyUE->GetChannel ();
  }
  else
  {
    return m_phyB->GetChannel ();
  }
    
  
}

void
UMTSNetDevice::AddNodeB (Ptr<UmtsBaseStationManager> manager_)
{
  if(m_nodeType=="NodeUE")
  {
      m_managerUE->AddNodeBStation(manager_);
  }
  
}

void
UMTSNetDevice::SetMobility(Ptr<MobilityModel> mobility)
{    
  if(m_nodeType=="NodeUE")
  {
    m_phyUE->SetMobility(mobility);
  }
  else
  {
    m_phyB->SetMobility(mobility);
  }
  
}

bool
UMTSNetDevice::SetMtu (const uint16_t mtu)
{
  mtuSize = mtu;
  return true;
}
uint16_t
UMTSNetDevice::GetMtu (void) const
{

  return mtuSize;
}

Ptr<Node>
UMTSNetDevice::GetNode (void) const
{
  return m_node;
}
void
UMTSNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
  m_nodeIdentifier=node->GetId();     
  CompleteConfig (node->GetId());
}

void 
UMTSNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  m_linkChanges.ConnectWithoutContext (callback);
}

bool 
UMTSNetDevice::IsLinkUp (void) const
{  
  return true;
}

bool
UMTSNetDevice::IsBridge (void) const
{
  return false;
}


bool
UMTSNetDevice::NeedsArp (void) const
{
  return false;
}

bool
UMTSNetDevice::IsPointToPoint (void) const
{
  return false;
}

void
UMTSNetDevice::SetLinkChangeCallback (Callback<void> callback)
{

}
bool
UMTSNetDevice::IsBroadcast (void) const
{
  return false;
}

bool
UMTSNetDevice::SupportsSendFrom (void) const
{  
  return false;
}

Address
UMTSNetDevice::GetBroadcast (void) const
{
  return Mac48Address::GetBroadcast ();
}
bool
UMTSNetDevice::IsMulticast (void) const
{
  return true;
}
Address
UMTSNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address::GetMulticast (multicastGroup);
}
Address UMTSNetDevice::GetMulticast (Ipv6Address addr) const
{
  return Mac48Address::GetMulticast (addr);
}

void
UMTSNetDevice::CompleteConfig (uint32_t uid)
{
 
  if(m_nodeType=="NodeUE")
  {
    m_mac->SetPhy (m_phyUE);
    m_rlc->SetMac (m_mac);
    m_rrcUE->SetRlc (m_rlc);
    m_rrcUE->SetPhyLayer (m_phyUE);
    m_rrcUE->SetIFQQueue (m_ifq);
    m_rrcUE->SetForwardUpCallback (MakeCallback (&UMTSNetDevice::ForwardUp, this));   
    m_managerUE->SetPhy (m_phyUE);
    m_managerUE->SetRRC (m_rrcUE);
    m_managerUE->SetNodeIdentifier(uid);

  }
  else
  {    
    m_mac->SetPhy (m_phyB);
    m_rlc->SetMac (m_mac);
    m_rrcB->SetRlc (m_rlc);
    m_rrcB->SetUmtsPhyLayerBS (m_phyB);
    m_rrcB->SetIFQQueue (m_ifq);
    m_rrcB->SetForwardUpCallback (MakeCallback (&UMTSNetDevice::ForwardUp, this));
    m_managerB->SetPhy (m_phyB);
    m_managerB->SetRRC (m_rrcB);
    m_managerB->SetNodeIdentifier(uid);

  }
  
  SetMobility(m_node->GetObject<MobilityModel> ());
            
}

uint32_t
UMTSNetDevice::GetNodeIdentifier()
{
 return m_node->GetId(); 
}

void
UMTSNetDevice::SetIpAddress()
{    
  if(m_nodeType=="NodeUE")
  {
    m_managerUE->SetIpAddress(m_node->GetObject<IpInterfaceList>()->GetIpAddress("Umts"));
  }
  else
  {
    m_managerB->SetIpAddress(m_node->GetObject<IpInterfaceList>()->GetIpAddress("Umts"));
  }
}

void
UMTSNetDevice::SetIpv6Address(Ipv6Address address)
{    
  if(m_nodeType=="NodeUE")
  {
	 //std::cout<<"UMTSNetDevice::SetIpv6Address: "<<address<<std::endl;
    m_managerUE->SetIpv6Address(address);
  }
  else
  {
    return;
  }
}

//NEW FUNCTION
void
UMTSNetDevice::SetIpv6Address()
{    
  if(m_nodeType=="NodeUE")
  {
   //std::cout<<"UMTSNetDevice::SetIpv6Address()"<<std::endl;
    m_managerUE->SetIpv6Address(m_node->GetObject<IpInterfaceList>()->GetIpv6Address("Umts"));
  }
  else
  {
    //std::cout<<"UMTSNetDevice::SetIpv6Address()"<<std::endl;
    m_managerB->SetIpv6Address(m_node->GetObject<IpInterfaceList>()->GetIpv6Address("Umts"));
  }
}




void
UMTSNetDevice::SetMac (Ptr<Object> mac)
{
     m_mac = DynamicCast<UmtsMacLayer>(mac);     
     m_mac->SetNodeType(m_nodeType);   
}

void
UMTSNetDevice::SetIFQ ()
{
     m_ifq = CreateObject<UmtsIfqLayer>();
     m_ifq->SetNodeType(m_nodeType); 
}

void
UMTSNetDevice::SetPhy (Ptr<Object> phy,Ptr<UMTSChannel> broadcastChannel)
{
  if(m_nodeType=="NodeUE")
  {
     m_phyUE = DynamicCast<UmtsPhyLayerUE>(phy);
  }
  else
  {
     m_phyB = DynamicCast<UmtsPhyLayerBS>(phy);
     m_phyB->SetSharedChannel(broadcastChannel);
  }
     
}

void
UMTSNetDevice::SetRlc (Ptr<Object> rlc)
{
     m_rlc = DynamicCast<UmtsRlcLayer>(rlc);
     m_rlc->SetNodeType(m_nodeType);     
}

void
UMTSNetDevice::SetRrc (Ptr<Object> rrc)
{
  if(m_nodeType=="NodeUE")
  {
     m_rrcUE = DynamicCast<UmtsRrcLayerUE>(rrc);
  }
  else
  {
     m_rrcB = DynamicCast<UmtsRrcLayerBS>(rrc);
  }
     
}

void
UMTSNetDevice::SetManager ()
{
  if(m_nodeType=="NodeUE")
  {       
     m_managerUE = CreateObject<UmtsUserEquipmentManager>(); 
  }
  else
  {     
     m_managerB = CreateObject<UmtsBaseStationManager>(); 
  }
  

}

Ptr<Object>
UMTSNetDevice::GetManager ()
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

void
UMTSNetDevice::SetName (std::string name_)
{
  m_name = name_;
}

std::string
UMTSNetDevice::GetName () const
{
  return m_name;
}

Ptr<Object>
UMTSNetDevice::GetPhy (void) const
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

Ptr<Object>
UMTSNetDevice::GetMac (void) const
{
    return m_mac;
}

Ptr<Object>
UMTSNetDevice::GetRlc (void) const
{
    return m_rlc;

}

Ptr<Object>
UMTSNetDevice::GetRrc (void) const
{
  if(m_nodeType=="NodeUE")
  {
    return m_rrcUE;
  }
  else
  {
    return m_rrcB;
  }
  
}

bool
UMTSNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
 if(m_EnableIpv6)
 {
   //std::cout<<"UMTSNetDevice::Send1111"<<m_nodeType<<std::endl;
   //std::cout<<"I enabled Ipv6 header!!!"<<std::endl;
   Ipv6Header header;
  
  packet->PeekHeader(header);
  
  // dichiarate ma non usate
  //Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  //Mac48Address realFrom = Mac48Address::Allocate ();

  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
     
  if(m_nodeType=="NodeUE")
  { 
    m_managerUE->PeriodicScanning(IPv6); //
    m_rrcUE->ReceiveFromAbove(packet,header.GetDestinationAddress());
    
  }
  else
  {
   //std::cout<<"UMTSNetDevice::Send, TriggerNodeUEScanning"<<std::endl;
    uint32_t id=m_managerB->GetNodeUEIdentifier(header.GetDestinationAddress());
    if(id!=(uint32_t)-1)
    {
	m_managerB->TriggerNodeUEScanning(id);
    }    
    m_rrcB->ReceiveFromAbove(packet,header.GetDestinationAddress());
  }

  return true;
 }
 else
 {
  
  Ipv4Header header;
  
  packet->PeekHeader(header);
  
  // dichiarate ma non usate
  //Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  //Mac48Address realFrom = Mac48Address::Allocate ();

  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
     
  if(m_nodeType=="NodeUE")
  { 
    m_managerUE->PeriodicScanning();
    
    m_rrcUE->ReceiveFromAbove(packet,header.GetDestination());
    
  }
  else
  {
      
    uint32_t id=m_managerB->GetNodeUEIdentifier(header.GetDestination());
    if(id!=(uint32_t)-1)
    {
	m_managerB->TriggerNodeUEScanning(id);
    }    
    m_rrcB->ReceiveFromAbove(packet,header.GetDestination());
  }

  return true;
  }
}

Address
UMTSNetDevice::GetAddress (void) const
{
  return m_mac->GetAddress ();
}

bool
UMTSNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  return true;
}

void
UMTSNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{

}

void
UMTSNetDevice::SetAddress(Address address)
{ 
  m_mac->SetAddress (Mac48Address::ConvertFrom (address));
}

void
UMTSNetDevice::SetReceiveCallback ( NetDevice::ReceiveCallback cb)
{
  m_forwardUp = cb;
}

void
UMTSNetDevice::ForwardUp (Ptr<Packet> packet)
{ 
   //std::cout<<"!!! Packet received from UMTS Net Device !!!"<<std::endl; 
    LlcSnapHeader llc;    
    packet->RemoveHeader (llc);
        
    if(m_EnableIpv6)
    {
      Ipv6Header header;
      packet->PeekHeader(header);
    }
    else
    {
      Ipv4Header header;
      packet->PeekHeader(header);
    }
    
  Ptr<Node> node = GetNode ();
  Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
  //std::cout << "[UMTSNetDevice::ForwardUp] Distance=" << mob->GetPosition().x <<std::endl;

    m_forwardUp(this,packet,llc.GetType(),Mac48Address::Allocate());
    //std::cout<<"UMTSNetDevice::ForwardUp"<<std::endl;

}

} // namespace ns3
