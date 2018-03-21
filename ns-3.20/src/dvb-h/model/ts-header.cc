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

#include "ts-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("TsHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TsHeader);

TypeId
TsHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TsHeader")
    .SetParent<Header> ()

  ;
  return tid;
}

TypeId
TsHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TsHeader::TsHeader ()
{
  m_synchronisationByte = 0x47;
  m_error_indicator = 0;
  m_payloadUnitStart = 0;
  m_priority = 0;
  m_pid = 0;
  m_scrambling_control = 0;
  m_adaptation_field_control = 0;
  m_counter = 0;
}

uint32_t
TsHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 4;
}

void
TsHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_error_indicator;
  value = value << 15;
  value = value | (m_payloadUnitStart << 14);
  value = value | (m_priority << 13);
  value = value | m_pid;

  uint8_t value2 = m_scrambling_control;
  value2 = value2 << 6;
  value2 = value2 | (m_adaptation_field_control << 4);
  value2 = value2 | m_counter;

  Buffer::Iterator i = start;

  i.WriteU8 (m_synchronisationByte);
  i.WriteU16 (value);
  i.WriteU8 (value2);
}

uint32_t
TsHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_synchronisationByte = i.ReadU8 ();
  uint16_t value = i.ReadU16 ();
  m_error_indicator = (value >> 15) & 1;
  m_payloadUnitStart = (value >> 14) & 1;
  m_priority = (value >> 13) & 1;
  m_pid = value & 8191;

  uint8_t value2 = i.ReadU8 ();
  m_scrambling_control = (value2 >> 6) & 3;
  m_adaptation_field_control = (value2 >> 4) & 3;
  m_counter = value2 & 15;

  return GetSerializedSize (); // The number of bytes consumed.
}

void
TsHeader::Print (std::ostream &os) const
{
  os << "****TS Header**** "
     << "Payload Start: " << m_payloadUnitStart << " "
     << "PID: " << m_pid << " "
     << "Counter: " << m_counter;
}

void
TsHeader::SetErrorIndicator (uint16_t indicator)
{
  m_error_indicator = indicator;
}

uint8_t
TsHeader::GetErrorIndicator ()
{
  return m_error_indicator;
}

void
TsHeader::SetPayloadUnitStart (uint16_t value)
{
  m_payloadUnitStart = value;
}

uint16_t
TsHeader::GetPayloadUnitStart ()
{
  return m_payloadUnitStart;
}

void
TsHeader::SetPid (uint16_t pid)
{
  m_pid = pid;
}

uint16_t
TsHeader::GetPid ()
{
  return m_pid;
}

void
TsHeader::SetCounter (uint8_t count)
{
  m_counter = count;
}

uint8_t
TsHeader::GetCounter ()
{
  return m_counter;
}


} // namespace ns3

