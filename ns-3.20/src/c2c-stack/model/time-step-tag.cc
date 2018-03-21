/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Michele Rondinone <mrondinone@umh.es>
 */
#include "time-step-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
TimeStepTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TimeStepTag")
    .SetParent<Tag> ()
    .AddConstructor<TimeStepTag> ()
    .AddAttribute ("TimeStep", "iTETRIS Time step of an ns-3 action that wants to be tracked in the iCS",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TimeStepTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
TimeStepTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TimeStepTag::TimeStepTag()
{}

uint32_t 
TimeStepTag::GetSerializedSize (void) const
{
  return 4;
}

void
TimeStepTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_timeStep);
}

void
TimeStepTag::Deserialize (TagBuffer i)
{
  m_timeStep = i.ReadU32 ();
}

void
TimeStepTag::Set (uint32_t timeStep)
{
  m_timeStep = timeStep;
}

uint32_t
TimeStepTag::Get () const
{
  return m_timeStep;
}

void
TimeStepTag::Print (std::ostream &os) const
{
  os << "Time Step=" << m_timeStep;
}

} //namespace ns3
