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

#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "c2cl4T-socket.h"

NS_LOG_COMPONENT_DEFINE ("c2cl4TSocket");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cl4TSocket);

TypeId
c2cl4TSocket::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cl4TSocket")
    .SetParent<Socketc2c> ()
    .AddAttribute ("RcvBufSize",
                   "c2cl4TSocket maximum receive buffer size (bytes)",
                   UintegerValue (0xffffffffl),
                   MakeUintegerAccessor (&c2cl4TSocket::GetRcvBufSize,
                                         &c2cl4TSocket::SetRcvBufSize),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

c2cl4TSocket::c2cl4TSocket ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

c2cl4TSocket::~c2cl4TSocket()
{
  NS_LOG_FUNCTION_NOARGS ();
}

}; // namespace ns3
