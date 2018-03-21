/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 EU FP7 COLOMBO project
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
 * Author: Jerome Haerri <jerome.haerri@eurecom.fr>
 */
#include "rssi-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
RSSITag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RSSITag")
    .SetParent<Tag> ()
    .AddConstructor<RSSITag> ()
    .AddAttribute ("RSSI", "The RSSI of a RX packet",
                   UintegerValue (0),
                   MakeUintegerAccessor (&RSSITag::Get),
                   MakeUintegerChecker<uint16_t> ())
    ;
  return tid;
}

TypeId
RSSITag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

RSSITag::RSSITag()
{}

uint32_t
RSSITag::GetSerializedSize (void) const
{
  return 2;
}

void
RSSITag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_rssi);
}

void
RSSITag::Deserialize (TagBuffer i)
{
	m_rssi = i.ReadU16 ();
}

void
RSSITag::Set (uint16_t rssi)
{
	m_rssi = rssi;
}

uint16_t
RSSITag::Get () const
{
  return m_rssi;
}

void
RSSITag::Print (std::ostream &os) const
{
  os << "RSSI=" << m_rssi;
}

} //namespace ns3
