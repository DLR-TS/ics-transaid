/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"

#include "c2c-interface.h"
#include "ns3/net-device.h"
#include "ns3/pointer.h"
#include "c2c-l3-protocol.h"

NS_LOG_COMPONENT_DEFINE ("c2cInterface");

using namespace std;

namespace ns3 {

TypeId 
c2cInterface::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cInterface")
    .SetParent<Object> ()
    ;
  return tid;
}

c2cInterface::c2cInterface ()
  //: m_ifup(false), // Modified by Ramon Bauza
  : m_ifup(true), // Added by Ramon Bauza
	m_forwarding (true),
    m_node (0),
    //m_device (0)
    m_netdevicerouter(0)
{
  NS_LOG_FUNCTION (this);
}

c2cInterface::~c2cInterface ()
{
  NS_LOG_FUNCTION (this);
}

void
c2cInterface::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  //m_device = 0;
  m_netdevicerouter = 0;
  Object::DoDispose ();
}

void 
c2cInterface::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node>
c2cInterface::GetNode (void)
{
  return m_node;
}

void 
c2cInterface::SetDeviceRouter (Ptr<NetDeviceRouter> DeviceRouter)
{
 //std::cout<<"c2cInterface::SetDeviceRouter"<<std::endl;
  m_netdevicerouter = DeviceRouter;
}


void 
c2cInterface::SetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}


void
c2cInterface::DoSetup (void)
{
  if (m_node == 0 || m_device == 0)
    {
      return;
    }
}

Ptr<NetDeviceRouter>
c2cInterface::GetDeviceRouter (void) const
{
  return m_netdevicerouter;
}


Ptr<NetDevice>
c2cInterface::GetDevice (void) const
{
  return m_device;
}


void
c2cInterface::SetDeviceType (NetDeviceType deviceType)
{
  m_deviceType = deviceType;
}

NetDeviceType
c2cInterface::GetDeviceType (void) const
{
  return m_deviceType;
}

bool 
c2cInterface::IsUp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ifup;
}

bool 
c2cInterface::IsDown (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return !m_ifup;
}

void 
c2cInterface::SetUp (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_ifup = true;
}

void 
c2cInterface::SetDown (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_ifup = false;
}

bool
c2cInterface::IsForwarding (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_forwarding;
}

void
c2cInterface::SetForwarding (bool val)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_forwarding = val;
}

void 
c2cInterface::Send(Ptr<Packet> p)
{
 //std::cout<<"c2cInterface::Send 1"<<std::endl;
  NS_LOG_FUNCTION_NOARGS ();

  //----------------  check iTETRIS --------------------------------------------
  NS_LOG_INFO ("c2cInterface : Sending To  = "<< GetDevice ()->GetBroadcast());
  
  //----------------  check iTETRIS --------------------------------------------

  //iTETRIS: Broadcasting
  
  //m_netdevicerouter->Send(p, m_device->GetBroadcast ());
  NS_LOG_INFO ("-----------------C2C-Interface----------------" << "\n");
  NS_LOG_INFO ("c2cInterface: Device Type( 0-CCH/ 1-SCH ) = " << this->GetDeviceType() << "\n");
  m_device ->Send (p, m_device->GetBroadcast (),
		           c2cL3Protocol::PROT_NUMBER);
}

// Added by Ramon Bauza
void 
c2cInterface::Send(Ptr<Packet> p, const Address &dest)
{
 //std::cout<<"c2cInterface::Send"<<std::endl;
 //std::cout<<"c2cInterface::Send "<<m_deviceType<<std::endl;
  NS_LOG_FUNCTION_NOARGS ();

  //----------------  check iTETRIS --------------------------------------------
  NS_LOG_INFO ("c2cInterface : Sending To  = "<< dest );
  
  //----------------  check iTETRIS --------------------------------------------
  m_device ->Send (p, dest,
  		           c2cL3Protocol::PROT_NUMBER);
  //m_netdevicerouter ->Send (p, dest);
}

}//namespace ns3
