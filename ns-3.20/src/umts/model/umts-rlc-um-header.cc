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

#include "umts-rlc-um-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRlcUmHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRlcUmHeader);



TypeId
UmtsRlcUmHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRlcUmHeader")
    .SetParent<Header> ()
    .AddConstructor<UmtsRlcUmHeader> ()
  ;
  return tid;
}

TypeId
UmtsRlcUmHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsRlcUmHeader::UmtsRlcUmHeader ()
{
  m_reserved=0;
  m_lastNumber=0;
  m_resegmentation=0;
  m_sequenceNumber=0;
  m_lengthIndicator=0;
  m_extension2=0;
}

uint32_t
UmtsRlcUmHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 5;
}

void
UmtsRlcUmHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_reserved;
  value = value << 13;
  value = value | (m_lastNumber<< 11);
  value = value | (m_resegmentation<< 10);
  value=value|m_sequenceNumber;


  uint8_t value2 = m_lengthIndicator;
  value2 = value2 << 1;
  value2 = value2 | m_extension2;

  Buffer::Iterator i = start;
  
  i.WriteHtonU16 (value);
  i.WriteU8 (value2);
  
  i.WriteU8(m_lastFragment);
  i.WriteU8(m_firstFragment);

}

uint32_t
UmtsRlcUmHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint16_t value = i.ReadNtohU16 ();
  m_reserved = (value >> 13) & 7;
  m_lastNumber = (value >> 11) & 3;
  m_resegmentation = (value >> 10) & 1;  
  m_sequenceNumber = value & 1023;
  
  uint8_t value2 = i.ReadU8 ();
  m_lengthIndicator = (value2 >> 1) & 127;
  m_extension2 = value2 & 1;
  
    m_lastFragment=i.ReadU8 ();
  m_firstFragment=i.ReadU8 ();

  return GetSerializedSize (); // The number of bytes consumed.
}

void
UmtsRlcUmHeader::Print (std::ostream &os) const
{
  os << "****RLC AM Header**** "
     << "Last Number: " << m_lastNumber<< " "     
     << "Sequence Number: " << m_sequenceNumber;
}

void 
UmtsRlcUmHeader::SetLastNumber (uint16_t value)
{
  m_lastNumber=value;
}

uint16_t 
UmtsRlcUmHeader::GetLastNumber()
{
  return m_lastNumber;
}
 
void 
UmtsRlcUmHeader::SetResegmentation (uint16_t value)
{
  m_resegmentation=value;
}
  
uint16_t 
UmtsRlcUmHeader::GetResegmentation ()
{
  return m_resegmentation;
}

void 
UmtsRlcUmHeader::SetSequenceNumber(uint16_t value)
{
  m_sequenceNumber=value;
}

uint16_t 
UmtsRlcUmHeader::GetSequenceNumber()
{
  return m_sequenceNumber;
}

void 
UmtsRlcUmHeader::SetLengthIndicator(uint16_t value)
{
  m_lengthIndicator=value;
}
  
uint16_t 
UmtsRlcUmHeader::GetLengthIndicator()
{
  return m_lengthIndicator;
}

void 
UmtsRlcUmHeader::SetFirstFragment(uint8_t value)
{
  m_firstFragment=value;
}

uint16_t 
UmtsRlcUmHeader::GetFirstFragment()
{
  return m_firstFragment;
}

void 
UmtsRlcUmHeader::SetLastFragment(uint8_t value)
{
  m_lastFragment=value;
}

uint8_t 
UmtsRlcUmHeader::GetLastFragment()
{
  return m_lastFragment;
}

} // namespace ns3

