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
#include "switching-manager-helper.h"

#include "ns3/wifi-net-device.h"
#include "ns3/log.h"
#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("SwitchingManagerHelper");

namespace ns3 {

SwitchingManagerHelper::SwitchingManagerHelper ()
{}

SwitchingManagerContainer 
SwitchingManagerHelper::Install (NetDeviceContainer c) const
{
  SwitchingManagerContainer managers;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<SwitchingManager> deviceManager = CreateObject<SwitchingManager> ();
      deviceManager->SetNetDevice (*i);
      (*i)->AggregateObject (deviceManager);
      managers.Add(deviceManager);
    }
  return managers;
}

SwitchingManagerContainer 
SwitchingManagerHelper::Install (Ptr<NetDevice> device) const
{
  return Install (NetDeviceContainer (device));
}

SwitchingManagerContainer 
SwitchingManagerHelper::Install (std::string deviceName) const
{
  Ptr<NetDevice> device = Names::Find<NetDevice> (deviceName);
  return Install (NetDeviceContainer (device));
}

} // namespace ns3
