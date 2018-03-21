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
#include "traffic-class-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
TrafficClassTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TrafficClassTag")
    .SetParent<Tag> ()
    .AddConstructor<TrafficClassTag> ()
    .AddAttribute ("Traffic_Class", "The Traffic class to be used for transmitting the packet",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TrafficClassTag::Get),
                   MakeUintegerChecker<uint16_t> ())
    ;
  return tid;
}

TypeId
TrafficClassTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TrafficClassTag::TrafficClassTag()
{}

uint32_t
TrafficClassTag::GetSerializedSize (void) const
{
  return 2;
}

void
TrafficClassTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_trafficclass);
}

void
TrafficClassTag::Deserialize (TagBuffer i)
{
	m_trafficclass = i.ReadU16 ();
}

void
TrafficClassTag::Set (uint16_t trafficclass)
{
	m_trafficclass = trafficclass;
}

uint16_t
TrafficClassTag::Get () const
{
  return m_trafficclass;
}

void
TrafficClassTag::Print (std::ostream &os) const
{
  os << "Traffic Class=" << m_trafficclass;
}

} //namespace ns3
