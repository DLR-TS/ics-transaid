/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Michele Rondinone <mrondinone@umh.es>
 */
#include "TStep-sequence-number-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
TStepSequenceNumberTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TStepSequenceNumberTag")
    .SetParent<Tag> ()
    .AddConstructor<TStepSequenceNumberTag> ()
    .AddAttribute ("TStepSequenceNumber", "iTETRIS Time step sequence number of an ns-3 action that wants to be tracked in the iCS",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TStepSequenceNumberTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
TStepSequenceNumberTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TStepSequenceNumberTag::TStepSequenceNumberTag()
{
}

uint32_t 
TStepSequenceNumberTag::GetSerializedSize (void) const
{
  return 4;
}

void
TStepSequenceNumberTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_TStepSequenceNumber);
}

void
TStepSequenceNumberTag::Deserialize (TagBuffer i)
{
  m_TStepSequenceNumber = i.ReadU32 ();
}

void
TStepSequenceNumberTag::Set (uint32_t timeStep)
{
  m_TStepSequenceNumber = timeStep;
}

uint32_t
TStepSequenceNumberTag::Get () const
{
  return m_TStepSequenceNumber;
}

void
TStepSequenceNumberTag::Print (std::ostream &os) const
{
  os << "TStep Sequence Number = " << m_TStepSequenceNumber;
}

} //namespace ns3
