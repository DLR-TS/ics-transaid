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
#include "tx-power-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
TxPowerTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TxPowerTag")
    .SetParent<Tag> ()
    .AddConstructor<TxPowerTag> ()
    .AddAttribute ("txPower", "The transmission power to be used for transmitting the packet",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TxPowerTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
TxPowerTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TxPowerTag::TxPowerTag()
{}

uint32_t 
TxPowerTag::GetSerializedSize (void) const
{
  return 1;
}

void
TxPowerTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_txPower);
}

void
TxPowerTag::Deserialize (TagBuffer i)
{
  m_txPower = i.ReadU8 ();
}

void
TxPowerTag::Set (uint8_t txPower)
{
  m_txPower = txPower;
}

uint8_t
TxPowerTag::Get () const
{
  return m_txPower;
}

void
TxPowerTag::Print (std::ostream &os) const
{
  os << "TxPower=" << m_txPower;
}

} //namespace ns3
