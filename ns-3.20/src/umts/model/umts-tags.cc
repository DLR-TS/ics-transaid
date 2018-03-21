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

#include "umts-tags.h"
#include "ns3/tag.h"
#include "umts-manager.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
UmtsTxRateTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsTxRateTag")
    .SetParent<Tag> ()
    .AddConstructor<UmtsTxRateTag> ()

  ;
  return tid;
}

TypeId
UmtsTxRateTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsTxRateTag::UmtsTxRateTag()
{
}

uint32_t
UmtsTxRateTag::GetSerializedSize (void) const
{
  return 8;
}

void
UmtsTxRateTag::Serialize (TagBuffer i) const
{
  i.WriteDouble (m_txRate);
}

void
UmtsTxRateTag::Deserialize (TagBuffer i)
{
  m_txRate = i.ReadDouble ();
}

void
UmtsTxRateTag::Set (double rate)
{
  m_txRate = rate;
}

double
UmtsTxRateTag::Get () const
{
  return m_txRate;
}

void
UmtsTxRateTag::Print (std::ostream &os) const
{
  os << "Tx Rate=" << m_txRate;
}



TypeId
UmtsReceiverIdTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsReceiverIdTag")
    .SetParent<Tag> ()
    .AddConstructor<UmtsReceiverIdTag> ()

  ;
  return tid;
}

TypeId
UmtsReceiverIdTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsReceiverIdTag::UmtsReceiverIdTag()
{
}

uint32_t
UmtsReceiverIdTag::GetSerializedSize (void) const
{
  return 8;
}

void
UmtsReceiverIdTag::Serialize (TagBuffer i) const
{  
  i.WriteU64 (m_receiverId);
}

void
UmtsReceiverIdTag::Deserialize (TagBuffer i)
{
  
  m_receiverId = i.ReadU64();
    
}

void
UmtsReceiverIdTag::Set (uint32_t id)
{
  m_receiverId= id;
  
}

uint32_t
UmtsReceiverIdTag::Get () const
{
  
  return m_receiverId;
}

void
UmtsReceiverIdTag::Print (std::ostream &os) const
{
  os << "Rx ID=" << m_receiverId;
}




TypeId
UmtsPacketTypeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsPacketTypeTag")
    .SetParent<Tag> ()
    .AddConstructor<UmtsPacketTypeTag> ()

  ;
  return tid;
}

TypeId
UmtsPacketTypeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsPacketTypeTag::UmtsPacketTypeTag()
{
}

uint32_t
UmtsPacketTypeTag::GetSerializedSize (void) const
{
  return 1;
}

void
UmtsPacketTypeTag::Serialize (TagBuffer i) const
{ 
  i.WriteU8 (m_packetType);
}

void
UmtsPacketTypeTag::Deserialize (TagBuffer i)
{  
  m_packetType= i.ReadU8();   
}

void
UmtsPacketTypeTag::Set (uint8_t value)
{
  m_packetType= value;
  
}

uint8_t
UmtsPacketTypeTag::Get () const
{
  
  return m_packetType;
}

void
UmtsPacketTypeTag::Print (std::ostream &os) const
{
  os << "Packet Type=" << m_packetType;
}

TypeId
UmtsCodeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsCodeTag")
    .SetParent<Tag> ()
    .AddConstructor<UmtsCodeTag> ()

  ;
  return tid;
}

TypeId
UmtsCodeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

UmtsCodeTag::UmtsCodeTag()
{
}

uint32_t
UmtsCodeTag::GetSerializedSize (void) const
{
  return 1;
}

void
UmtsCodeTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_code);
}

void
UmtsCodeTag::Deserialize (TagBuffer i)
{
  m_code = i.ReadU8 ();
}

void
UmtsCodeTag::Set (uint8_t code)
{
  m_code = code;
}

uint8_t
UmtsCodeTag::Get () const
{
  return m_code;
}

void
UmtsCodeTag::Print (std::ostream &os) const
{
  os << "Sender ID=" << m_code;
}



} // namespace ns3
