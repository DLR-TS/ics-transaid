/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "dvbh-tags.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
DvbhServiceIdTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhServiceIdTag")
    .SetParent<Tag> ()
    .AddConstructor<DvbhServiceIdTag> ()

  ;
  return tid;
}

TypeId
DvbhServiceIdTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

DvbhServiceIdTag::DvbhServiceIdTag ()
{
}

uint32_t
DvbhServiceIdTag::GetSerializedSize (void) const
{
  return 4;
}

void
DvbhServiceIdTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_serviceId);
}

void
DvbhServiceIdTag::Deserialize (TagBuffer i)
{
  m_serviceId = i.ReadU32 ();
}

void
DvbhServiceIdTag::Set (uint32_t id)
{
  m_serviceId = id;
}

uint32_t
DvbhServiceIdTag::Get () const
{
  return m_serviceId;
}

void
DvbhServiceIdTag::Print (std::ostream &os) const
{
  os << "Service Id=" << m_serviceId;
}



TypeId
DvbhDeltaTTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhDeltaTTag")
    .SetParent<Tag> ()
    .AddConstructor<DvbhDeltaTTag> ()
  ;
  return tid;
}

TypeId
DvbhDeltaTTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

DvbhDeltaTTag::DvbhDeltaTTag ()
{
}

uint32_t
DvbhDeltaTTag::GetSerializedSize (void) const
{
  return 8;
}

void
DvbhDeltaTTag::Serialize (TagBuffer i) const
{
  i.WriteDouble (m_time);
}

void
DvbhDeltaTTag::Deserialize (TagBuffer i)
{
  m_time= i.ReadDouble ();
}

void
DvbhDeltaTTag::Set (double time)
{
  m_time = time;
}

double
DvbhDeltaTTag::Get () const
{
  return m_time;
}

void
DvbhDeltaTTag::Print (std::ostream &os) const
{
  os << "Delta T=" << m_time;
}


TypeId
RxPowerTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RxPowerTag")
    .SetParent<Tag> ()
    .AddConstructor<RxPowerTag> ()
  ;
  return tid;
}

TypeId
RxPowerTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

RxPowerTag::RxPowerTag ()
{
}

uint32_t
RxPowerTag::GetSerializedSize (void) const
{
  return 8;
}

void
RxPowerTag::Serialize (TagBuffer i) const
{
  i.WriteDouble (m_power);
}

void
RxPowerTag::Deserialize (TagBuffer i)
{
  m_power = i.ReadDouble ();
}

void
RxPowerTag::Set (double power)
{
  m_power = power;
}

double
RxPowerTag::Get () const
{
  return m_power;
}

void
RxPowerTag::Print (std::ostream &os) const
{
  os << "Rx Power=" << m_power;
}


} // namespace ns3
