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

#include "umts-rlc-ack-header.h"
#include "ns3/header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("UmtsRlcAckHeader");


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsRlcAckHeader);

TypeId
UmtsRlcAckHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsRlcAckHeader")
    .SetParent<Header> ()

  ;
  return tid;
}

TypeId
UmtsRlcAckHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsRlcAckHeader::UmtsRlcAckHeader ()
{
  m_dataControl=0;
  m_cpt=0;
  m_ackNumber=0;
  m_extension=0;
  m_nackNumber=0;

}

uint32_t
UmtsRlcAckHeader::GetSerializedSize (void) const
{
  /*
  * This is the size of a MPE Header
  */

  return 5;
}

void
UmtsRlcAckHeader::Serialize (Buffer::Iterator start) const
{

  uint16_t value = m_dataControl;
  value = value << 15;
  value = value | (m_cpt << 13);
  value = value | (m_ackNumber<<2);
  value = value | m_extension;

  Buffer::Iterator i = start;
  
  i.WriteHtonU16 (value);
  i.WriteU8 (m_nackNumber);
  
    i.WriteU8(m_lastFragment);
  i.WriteU8(m_firstFragment);

}

uint32_t
UmtsRlcAckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint16_t value = i.ReadNtohU16 ();
  m_dataControl = (value >> 15) & 1;
  m_cpt = (value >> 13) & 7;
  m_ackNumber = (value >> 2) & 1023;
  m_extension = value & 3;
  
  
  m_nackNumber = i.ReadU8 ();
  
   m_lastFragment=i.ReadU8 ();
  m_firstFragment=i.ReadU8 ();
    
  return GetSerializedSize (); // The number of bytes consumed.
}

void
UmtsRlcAckHeader::Print (std::ostream &os) const
{
  os << "****RLC AM Header**** "
     << "ACK Number: " << m_ackNumber<< " "
     << "Nack Number: " << m_nackNumber;
}

void 
UmtsRlcAckHeader::SetDataControl (uint16_t value)
{
 m_dataControl=value; 
}

uint16_t 
UmtsRlcAckHeader::GetDataControl ()
{
  return m_dataControl;
}
  
void 
UmtsRlcAckHeader::SetCpt (uint16_t value)
{
  m_cpt=value;
}

uint16_t 
UmtsRlcAckHeader::GetCpt ()
{
  return m_cpt;
}

void 
UmtsRlcAckHeader::SetAckNumber(uint16_t value)
{
  m_ackNumber=value;
}

uint16_t 
UmtsRlcAckHeader::GetAckNumber()
{
  return m_ackNumber;
}

void 
UmtsRlcAckHeader::SetNackNumber(uint8_t value)
{
  m_nackNumber=value;
}
  
uint8_t 
UmtsRlcAckHeader::GetNackNumber()
{
  return m_nackNumber;
}

void 
UmtsRlcAckHeader::SetLastFragment(uint8_t value)
{
  m_lastFragment=value;
}

uint8_t 
UmtsRlcAckHeader::GetLastFragment()
{
  return m_lastFragment;
}

void 
UmtsRlcAckHeader::SetFirstFragment(uint8_t value)
{
  m_firstFragment=value;
}

uint16_t 
UmtsRlcAckHeader::GetFirstFragment()
{
  return m_firstFragment;
}

} // namespace ns3

