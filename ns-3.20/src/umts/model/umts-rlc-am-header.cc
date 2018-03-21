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

#include "umts-rlc-am-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRlcAmHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRlcAmHeader);

TypeId
UmtsRlcAmHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRlcAmHeader")
    .SetParent<Header> ()

  ;
  return tid;
}

TypeId
UmtsRlcAmHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsRlcAmHeader::UmtsRlcAmHeader ()
{
  m_dataControl=1; //0 Control 1 Data
  m_resegmentation=0;
  m_polling=0;
  m_lastNumber=0;
  m_sequenceNumber=0;
  m_lengthIndicator=0;
  m_extension2=0;
  
  m_lastFragment=0;
  m_firstFragment=0;
}

uint32_t
UmtsRlcAmHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 5;
}

void
UmtsRlcAmHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_dataControl;
  value = value << 15;
  value = value | (m_resegmentation << 14);
  value = value | (m_polling << 13);
  value = value | (m_lastNumber << 12);
  value = value | m_sequenceNumber;


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
UmtsRlcAmHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint16_t value = i.ReadNtohU16 ();
  m_dataControl = (value >> 15) & 1;
  m_resegmentation = (value >> 14) & 1;
  m_polling = (value >> 13) & 1;
  m_lastNumber = (value >> 12) & 1;
  m_sequenceNumber = value & 4095;
  
  uint8_t value2 = i.ReadU8 ();
  m_lengthIndicator = (value2 >> 1) & 127;
  m_extension2 = value2 & 1;
  
  m_lastFragment=i.ReadU8 ();
  m_firstFragment=i.ReadU8 ();

  return GetSerializedSize (); // The number of bytes consumed.
}

void
UmtsRlcAmHeader::Print (std::ostream &os) const
{
  os << "****RLC AM Header**** "
     << "Data-Control: " << m_dataControl<< " "
     << "ACK Mode: " << m_polling<< " "
     << "Sequence Number: " << m_sequenceNumber;
}

void 
UmtsRlcAmHeader::SetDataControl (uint16_t value)
{
 m_dataControl=value; 
}

uint16_t 
UmtsRlcAmHeader::GetDataControl ()
{
  return m_dataControl;
}

void 
UmtsRlcAmHeader::SetPolling (uint16_t value)
{
  m_polling=value;
}

uint16_t 
UmtsRlcAmHeader::GetPolling ()
{
  return m_polling;
}

void 
UmtsRlcAmHeader::SetResegmentation (uint16_t value)
{
  m_resegmentation=value;
}

uint16_t 
UmtsRlcAmHeader::GetResegmentation ()
{
  return m_resegmentation;
}

void 
UmtsRlcAmHeader::SetSequenceNumber(uint16_t value)
{
  m_sequenceNumber=value;
}

uint16_t 
UmtsRlcAmHeader::GetSequenceNumber()
{
  return m_sequenceNumber;
}

void 
UmtsRlcAmHeader::SetLengthIndicator(uint16_t value)
{
  m_lengthIndicator=value;
}

uint16_t 
UmtsRlcAmHeader::GetLengthIndicator()
{
  return m_lengthIndicator;
}

void 
UmtsRlcAmHeader::SetLastNumber(uint16_t value)
{
  m_lastNumber=value;
}

uint16_t 
UmtsRlcAmHeader::GetLastNumber()
{
  return m_lastNumber;
}

void 
UmtsRlcAmHeader::SetLastFragment(uint8_t value)
{
  m_lastFragment=value;
}

uint8_t 
UmtsRlcAmHeader::GetLastFragment()
{
  return m_lastFragment;
}

void 
UmtsRlcAmHeader::SetFirstFragment(uint8_t value)
{
  m_firstFragment=value;
}

uint16_t 
UmtsRlcAmHeader::GetFirstFragment()
{
  return m_firstFragment;
}

} // namespace ns3

