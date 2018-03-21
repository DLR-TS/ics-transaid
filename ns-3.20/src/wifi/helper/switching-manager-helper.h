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
#ifndef SWITCHING_MANAGER_HELPER_H
#define SWITCHING_MANAGER_HELPER_H

#include <string>
#include "ns3/net-device-container.h"
#include "ns3/switching-manager-container.h"

namespace ns3 {

class WifiNetDevice;
class Node;

class SwitchingManagerHelper
{
public:
  
  SwitchingManagerHelper ();
  SwitchingManagerContainer Install (NetDeviceContainer c) const;
  SwitchingManagerContainer Install (Ptr<NetDevice> device) const;
  SwitchingManagerContainer Install (std::string deviceName) const;
};

} // namespace ns3

#endif /* SWITCHING_MANAGER_HELPER_H */
