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
#include "time-stamp-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
TimeStampTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TimeStampTag")
    .SetParent<Tag> ()
    .AddConstructor<TimeStampTag> ()
    .AddAttribute ("timeStamp", "The timestamp when the common header was generated",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TimeStampTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
TimeStampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TimeStampTag::TimeStampTag()
{}

uint32_t 
TimeStampTag::GetSerializedSize (void) const
{
  return 8;
}

void
TimeStampTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_timeStamp);
}

void
TimeStampTag::Deserialize (TagBuffer i)
{
  m_timeStamp = i.ReadU32 ();
}

void
TimeStampTag::Set (uint32_t timeStamp)
{
  m_timeStamp = timeStamp;
}

uint32_t
TimeStampTag::Get () const
{
  return m_timeStamp;
}

void
TimeStampTag::Print (std::ostream &os) const
{
  os << "TimeStamp=" << m_timeStamp;
}

} //namespace ns3
