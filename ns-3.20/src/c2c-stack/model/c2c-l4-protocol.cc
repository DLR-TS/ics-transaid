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
 * ns-3 - c2c Layer 4 Protocol base class
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#include "c2c-l4-protocol.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cL4Protocol);

TypeId
c2cL4Protocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cL4Protocol")
    .SetParent<Object> ()
    .AddAttribute ("ProtocolNumber", "The c2c protocol number.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&c2cL4Protocol::GetProtocolNumber),
                   MakeUintegerChecker<int> ())
    ;
  return tid;
}

c2cL4Protocol::~c2cL4Protocol()
{}

}//namespace ns3
