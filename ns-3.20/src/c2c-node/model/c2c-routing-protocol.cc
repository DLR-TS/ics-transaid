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
#include "c2c-route.h"
#include "c2c-routing-protocol.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cRoutingProtocol);

TypeId c2cRoutingProtocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cRoutingProtocol")
    .SetParent<Object> ()
    ;
  return tid;
}

}//namespace ns3
