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

#include "pes-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("PesHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PesHeader);

TypeId
PesHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PesHeader")
    .SetParent<Header> ()

  ;
  return tid;
}

TypeId
PesHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

PesHeader::PesHeader ()
{
  m_startCode = 0x000001;
  m_sid = 0;
  m_packetLength = 0;
}

uint32_t
PesHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a PES Header
  */

  return 6;
}

void
PesHeader::Serialize (Buffer::Iterator start) const
{

  uint32_t value = m_startCode;
  value = value << 24;
  value = value | m_sid;

  Buffer::Iterator i = start;

  i.WriteU32 (value);
  i.WriteU16 (m_packetLength);

}

uint32_t
PesHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint32_t value = i.ReadU32 ();
  m_startCode = (value >> 24) &255 ;
  m_sid = value & 16777215;

  m_packetLength = i.ReadU16 ();
  return GetSerializedSize (); // The number of bytes consumed.
}

void
PesHeader::Print (std::ostream &os) const
{
  os << "****PES Header**** "
     << "SID: " << m_sid << " "
     << "Packet Length: " << m_packetLength;
}

void
PesHeader::SetSid (uint32_t sid)
{
  m_sid = sid;
}

uint32_t
PesHeader::GetSid ()
{
  return m_sid;
}

void
PesHeader::SetPacketLength (uint16_t value)
{
  m_packetLength = value;
}

uint16_t
PesHeader::GetPacketLength ()
{
  return m_packetLength;
}




} // namespace ns3

