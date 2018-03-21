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
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/c2c.h"
#include "ns3/net-device.h"
#include "c2c-interface-helper.h"

NS_LOG_COMPONENT_DEFINE("c2cInterfaceHelper");

namespace ns3 {

c2cInterfaceHelper::c2cInterfaceHelper ()
{
  NS_LOG_FUNCTION_NOARGS ();
}


c2cInterfaceContainer
c2cInterfaceHelper::Assign (const NetDeviceType deviceType, const NetDeviceContainer &c)
{
   NS_LOG_FUNCTION_NOARGS ();
   c2cInterfaceContainer retval;
   for (uint32_t i = 0; i < c.GetN (); ++i) {
     Ptr<NetDevice> device = c.Get (i);

     Ptr<Node> node = device->GetNode ();
        NS_ASSERT_MSG (node, "c2cInterfaceHelper::Allocate(): Bad node");

     Ptr<c2c> C2C = node->GetObject<c2c> ();
        NS_ASSERT_MSG (C2C, "c2cInterfaceHelper::Allocate(): Bad c2c");

     uint32_t interface = C2C->AddInterface(deviceType, device);
      NS_ASSERT_MSG (interface >= 0, "c2cInterfaceHelper::Allocate(): "
        "Interface index not found");

     retval.Add(C2C,interface);
   }
   return retval;
}


c2cInterfaceContainer
c2cInterfaceHelper::Assign (const NetDeviceContainer &c)
{
   NS_LOG_FUNCTION_NOARGS ();
   c2cInterfaceContainer retval;
   for (uint32_t i = 0; i < c.GetN (); ++i) {
     Ptr<NetDevice> device = c.Get (i);

     Ptr<Node> node = device->GetNode ();
        NS_ASSERT_MSG (node, "c2cInterfaceHelper::Allocate(): Bad c2c");

     Ptr<c2c> C2C = node->GetObject<c2c> ();
        NS_ASSERT_MSG (C2C, "c2cInterfaceHelper::Allocate(): Bad c2c");

     uint32_t interface = C2C->AddInterface(device);
     NS_ASSERT_MSG (interface >= 0, "c2cInterfaceHelper::Allocate(): "
        "Interface index not found");

     retval.Add(C2C,interface);
   }
   return retval;
}
}; // namespace ns3
