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
 * Author: Ramon Bauza <rbauza@umh.es>
 */
#include "channel-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
ChannelTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ChannelTag")
    .SetParent<Tag> ()
    .AddConstructor<ChannelTag> ()
    .AddAttribute ("channel", "The Channel to be used for transmitting the packet",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ChannelTag::Get),
                   MakeUintegerChecker<uint16_t> ())
    ;
  return tid;
}

TypeId
ChannelTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

ChannelTag::ChannelTag()
{}

uint32_t
ChannelTag::GetSerializedSize (void) const
{
  return 2;
}

void
ChannelTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_channel);
}

void
ChannelTag::Deserialize (TagBuffer i)
{
  m_channel = i.ReadU16 ();
}

void
ChannelTag::Set (uint16_t channel)
{
  m_channel = channel;
}

uint16_t
ChannelTag::Get () const
{
  return m_channel;
}

void
ChannelTag::Print (std::ostream &os) const
{
  os << "Channel=" << m_channel;
}

} //namespace ns3
