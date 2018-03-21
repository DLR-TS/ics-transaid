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

#include "umts-mac-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("UmtsMacHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsMacHeader);

TypeId
UmtsMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsMacHeader")
    .SetParent<Header> ()
    .AddConstructor<UmtsMacHeader> ()
  ;
  return tid;
}

TypeId
UmtsMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsMacHeader::UmtsMacHeader ()
{
  m_tctf = 0;
  m_ct=0;
  m_ueId=0;

}

uint32_t
UmtsMacHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 2;
}

void
UmtsMacHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_ct;
  value = value << 4;
  value = value | m_ueId;

  Buffer::Iterator i = start;

  i.WriteU8 (m_tctf);
  i.WriteU8 (value);
 
}

uint32_t
UmtsMacHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_tctf = i.ReadU8 ();   
  uint16_t value = i.ReadU8();
  m_ct= (value >> 4) & 15;
  m_ueId = value & 15;
 
  return GetSerializedSize (); // The number of bytes consumed.
}

void
UmtsMacHeader::Print (std::ostream &os) const
{
  os << "****MAC Header**** "
     << "TCTF: " << m_tctf;
}

void 
UmtsMacHeader::SetTctf (uint8_t value)
{
  m_tctf=value;
}
        
uint8_t 
UmtsMacHeader::GetTctf ()
{
  return m_tctf;
}
  
} // namespace ns3

