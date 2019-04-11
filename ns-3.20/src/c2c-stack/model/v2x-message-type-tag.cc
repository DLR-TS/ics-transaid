/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Alejandro Correa <acorrea@umh.es>
 */
#include "v2x-message-type-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
V2XmessageTypeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::V2XmessageTypeTag")
    .SetParent<Tag> ()
    .AddConstructor<V2XmessageTypeTag> ()
    .AddAttribute ("V2XmessageType", "The V2X message type ",
                   UintegerValue (0),
                   MakeUintegerAccessor (&V2XmessageTypeTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
V2XmessageTypeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

V2XmessageTypeTag::V2XmessageTypeTag()
{}

uint32_t
V2XmessageTypeTag::GetSerializedSize (void) const
{
  return 8;
}

void
V2XmessageTypeTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_V2XmessageType);
}

void
V2XmessageTypeTag::Deserialize (TagBuffer i)
{
    m_V2XmessageType = i.ReadU32 ();
}

void
V2XmessageTypeTag::Set (uint32_t MessageType)
{
    m_V2XmessageType = MessageType;
}

uint32_t
V2XmessageTypeTag::Get () const
{
  return m_V2XmessageType;
}

void
V2XmessageTypeTag::Print (std::ostream &os) const
{
  os << "V2XmessageType=" << m_V2XmessageType;
}

} //namespace ns3
