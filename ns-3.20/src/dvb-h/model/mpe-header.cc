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

#include "mpe-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("MpeHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MpeHeader);

TypeId
MpeHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpeHeader")
    .SetParent<Header> ()

  ;
  return tid;
}

TypeId
MpeHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

MpeHeader::MpeHeader ()
{
  m_tableId = 0x3E;
  m_section_syntax_indicator = 0;
  m_private = 0;
  m_reserved = 0;
  m_section_length = 0;
  m_mac_address6 = 0;
  m_mac_address5 = 0;
  m_reserved2 = 0;
  m_payload_scrambling_control = 0;
  m_address_scrambling_control = 0;
  m_Llc_Snap_flag = 0;
  m_current_next_indicator = 0;
  m_section_number = 0;
  m_last_section_number = 0;
  m_mac_address4 = 0;
  m_mac_address3 = 0;
  m_mac_address2 = 0;
  m_mac_address1 = 0;
}

uint32_t
MpeHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 12;
}

void
MpeHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_section_syntax_indicator;
  value = value << 15;
  value = value | (m_private << 14);
  value = value | (m_reserved << 12);
  value = value | m_section_length;


  uint8_t value2 = m_reserved2;
  value2 = value2 << 6;
  value2 = value2 | (m_payload_scrambling_control << 4);
  value2 = value2 | (m_address_scrambling_control << 2);
  value2 = value2 | (m_Llc_Snap_flag << 1);
  value2 = value2 | m_current_next_indicator;

  Buffer::Iterator i = start;

  i.WriteU8 (m_tableId);
  i.WriteHtonU16 (value);
  i.WriteU8 (m_mac_address6);
  i.WriteU8 (m_mac_address5);
  i.WriteU8 (value2);
  i.WriteU8 (m_section_number);
  i.WriteU8 (m_last_section_number);
  i.WriteU8 (m_mac_address4);
  i.WriteU8 (m_mac_address3);
  i.WriteU8 (m_mac_address2);
  i.WriteU8 (m_mac_address1);

}

uint32_t
MpeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_tableId = i.ReadU8 ();   // this byte includes Version (4bit) and nhtype (4bit)
  uint16_t value = i.ReadNtohU16 ();
  m_section_syntax_indicator = (value >> 15) & 1;
  m_private = (value >> 14) & 1;
  m_reserved = (value >> 12) & 3;
  m_section_length = value & 4095;
  m_mac_address6 = i.ReadU8 ();
  m_mac_address5 = i.ReadU8 ();

  uint8_t value2 = i.ReadU8 ();
  m_reserved2 = (value2 >> 6) & 3;
  m_payload_scrambling_control = (value2 >> 4) & 3;
  m_address_scrambling_control = (value2 >> 2) & 3;
  m_Llc_Snap_flag = (value2 >> 1) & 1;
  m_current_next_indicator = value2 & 1;

  m_section_number = i.ReadU8 ();
  m_last_section_number = i.ReadU8 ();
  m_mac_address4 = i.ReadU8 ();
  m_mac_address3 = i.ReadU8 ();
  m_mac_address2 = i.ReadU8 ();
  m_mac_address1 = i.ReadU8 ();

  return GetSerializedSize (); // The number of bytes consumed.
}

void
MpeHeader::Print (std::ostream &os) const
{
  os << "****MPE Header**** "
     << "Table ID: " << m_tableId << " "
     << "Section Number: " << m_section_number << " "
     << "Last Section Number: " << m_last_section_number;
}

void
MpeHeader::SetTableId (uint8_t id)
{
  m_tableId = id;
}

uint8_t
MpeHeader::GetTableId ()
{
  return m_tableId;
}

void
MpeHeader::SetSectionSyntaxIndicator (uint16_t indicator)
{
  m_section_syntax_indicator = indicator;
}

uint16_t
MpeHeader::GetSectionSyntaxIndicator ()
{
  return m_section_syntax_indicator;
}

void
MpeHeader::SetSectionLength (uint16_t length)
{  
  m_section_length = length;
}

uint16_t
MpeHeader::GetSectionLength ()
{
  return m_section_length;
}

void
MpeHeader::SetMac6 (uint8_t mac)
{
  m_mac_address6 = mac;
}
uint8_t
MpeHeader::GetMac6 ()
{
  return m_mac_address6;
}

void
MpeHeader::SetMac5 (uint8_t mac)
{
  m_mac_address5 = mac;
}

uint8_t
MpeHeader::GetMac5 ()
{
  return m_mac_address5;
}

void
MpeHeader::SetPayloadScramblingControl (uint8_t value)
{
  m_payload_scrambling_control = value;
}

uint8_t
MpeHeader::GetPayloadScramblingControl ()
{
  return m_payload_scrambling_control;
}

void
MpeHeader::SetAddressScramblingControl (uint8_t value)
{
  m_address_scrambling_control = value;
}

uint8_t
MpeHeader::GetAddressScramblingControl ()
{
  return m_address_scrambling_control;
}

void
MpeHeader::SetSnapFlag (uint8_t flag)
{
  m_Llc_Snap_flag = flag;
}

uint8_t
MpeHeader::GetSnapFlag ()
{
  return m_Llc_Snap_flag;
}

void
MpeHeader::SetCurrentNextIndicator (uint8_t next)
{
  m_current_next_indicator = next;
}

uint8_t
MpeHeader::GetCurrentNextIndicator ()
{
  return m_current_next_indicator;
}

void
MpeHeader::SetSectionNumber (uint8_t number)
{
  m_section_number = number;
}

uint8_t
MpeHeader::GetSectionNumber (void) const
{

  return m_section_number;
}

void
MpeHeader::SetLastSectionNumber (uint8_t number)
{
  m_last_section_number = number;
}

uint8_t
MpeHeader::GetLastSectionNumber (void) const
{
  return m_last_section_number;
}

void
MpeHeader::SetMac4 (uint8_t mac)
{
  m_mac_address4 = mac;
}

uint8_t
MpeHeader::GetMac4 ()
{
  return m_mac_address4;
}

void
MpeHeader::SetMac3 (uint8_t mac)
{
  m_mac_address3 = mac;
}

uint8_t
MpeHeader::GetMac3 ()
{
  return m_mac_address3;
}

void
MpeHeader::SetMac2 (uint8_t mac)
{
  m_mac_address2 = mac;
}

uint8_t
MpeHeader::GetMac2 ()
{
  return m_mac_address2;
}

void
MpeHeader::SetMac1 (uint8_t mac)
{
  m_mac_address1 = mac;
}

uint8_t
MpeHeader::GetMac1 ()
{
  return m_mac_address1;
}



} // namespace ns3

