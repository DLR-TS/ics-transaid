/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/assert.h" 
#include "ns3/node.h" 
#include "netdevice-router.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/net-device.h"
#include "ns3/channel-tag.h"

NS_LOG_COMPONENT_DEFINE ("NetDeviceRouter");

namespace ns3 {

const uint16_t NetDeviceRouter::PROT_NUMBER = 0x88DC;

NS_OBJECT_ENSURE_REGISTERED (NetDeviceRouter);

TypeId 
NetDeviceRouter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NetDeviceRouter")
    .SetParent<Object> ()
    .AddConstructor<NetDeviceRouter> ()
    ;
  return tid;
}

NetDeviceRouter::NetDeviceRouter ()
  : m_nInterfaces (0)
{}

NetDeviceRouter::~NetDeviceRouter ()
{
  NS_LOG_FUNCTION (this);
}

void
NetDeviceRouter::SetNode (Ptr<Node> node)
{
 //std::cout<<"NetDeviceRouter::SetNode"<<std::endl;
  m_node = node;
}

void 
NetDeviceRouter::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
 
  for (NetDeviceList::iterator i = m_interfaces.begin (); i != m_interfaces.end (); ++i)
    {
      i->second = 0;
    }
  m_interfaces.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void 
NetDeviceRouter::Receive( Ptr<NetDevice> device, Ptr<const Packet> p, uint16_t protocol, const Address &from,
                         const Address &to, NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION (this << &device << p << protocol <<  from);

  NS_LOG_LOGIC ("Packet from " << from << " received on node " << m_node->GetId ());

  uint32_t index = 0;
  Ptr<NetDevice> Device;
  for (NetDeviceList::const_iterator i = m_interfaces.begin (); 
       i != m_interfaces.end (); 
       i++)
    {
      Device = i->second;
      index++;
    }
}

void
NetDeviceRouter::Send (Ptr<Packet> packet, Address hardwareDestination)
{
  NS_LOG_FUNCTION (this << packet);
  NS_LOG_LOGIC ("Send via interface " << 1);
  ChannelTag channel;
  Ptr<NetDevice> outInterface;
  if (packet->PeekPacketTag (channel))
    {
      if (channel.Get () == CCH)
        {
    	 //std::cout<<"NetDeviceRouter::Send: CCH 1111"<<std::endl;
    	 //std::cout<<"NetDeviceRouter::Send, list size: "<<GetNInterfaces()<<std::endl;
          outInterface = m_interfaces[ITS_CCH];
         //std::cout<<"NetDeviceRouter::Send: CCH 2"<<std::endl;
        }
      else 
        {
    	 //std::cout<<"NetDeviceRouter::Send: SCH"<<std::endl;
          outInterface = m_interfaces[ITS_SCH];
        }
    }
  else
    {
	 //std::cout<<"NetDeviceRouter::Send: else"<<std::endl;
      outInterface = m_interfaces[ITS_CCH];
     //std::cout<<"NetDeviceRouter::Send: else 1"<<std::endl;
    }
  NS_ASSERT_MSG (outInterface, "NetDeviceRouter: Bad Out Interface");
  //NS_ASSERT (outInterface);
 //std::cout<<"NetDeviceRouter::Send: else 2"<<std::endl;
  outInterface->Send(packet,hardwareDestination,NetDeviceRouter::PROT_NUMBER);
}

uint32_t 
NetDeviceRouter::AddInterface (NetDeviceType deviceType, Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << &device);

  Ptr<Node> node = GetObject<Node> ();
  node->RegisterProtocolHandler (MakeCallback (&NetDeviceRouter::Receive, this), 
                                 NetDeviceRouter::PROT_NUMBER, device);
  uint32_t index = m_nInterfaces;
 //std::cout<<"NetDeviceRouter::AddInterface!!!"<<std::endl;
  m_interfaces.insert (std::make_pair(deviceType, device));
  m_nInterfaces++;
  return index;
}

uint32_t 
NetDeviceRouter::GetNInterfaces (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nInterfaces;
}

} // namespace ns3
