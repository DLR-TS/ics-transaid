/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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

#include "ns3/log.h"
#include "vehicle-scan-mngr.h"
#include "ns3/node.h"

NS_LOG_COMPONENT_DEFINE ("VehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (VehicleScanMngr);

TypeId VehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VehicleScanMngr")
    .SetParent<Object> ()                  
    ;
  return tid;
}

VehicleScanMngr::~VehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
VehicleScanMngr::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void 
VehicleScanMngr::SetNetDevice (Ptr<NetDevice> device)
{
  m_netDevice = device;
}

} // namespace ns3