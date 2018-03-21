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
#include "mcs-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
McsTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McsTag")
    .SetParent<Tag> ()
    .AddConstructor<McsTag> ()
    .AddAttribute ("Mcs", "The modulation and coding scheme (MCS) to be used for transmitting the packet",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McsTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
McsTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

McsTag::McsTag()
{}

uint32_t 
McsTag::GetSerializedSize (void) const
{
  return 1;
}

void
McsTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_Mcs);
}

void
McsTag::Deserialize (TagBuffer i)
{
  m_Mcs = i.ReadU8 ();
}

void
McsTag::Set (uint8_t Mcs)
{
  m_Mcs = Mcs;
}

uint8_t
McsTag::Get () const
{
  return m_Mcs;
}

void
McsTag::Print (std::ostream &os) const
{
  os << "Mcs=" << m_Mcs;
}

} //namespace ns3
