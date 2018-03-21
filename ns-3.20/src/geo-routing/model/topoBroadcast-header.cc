/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include "topoBroadcast-header.h"

namespace ns3 {

TypeId
topoBroadcastHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::topoBroadcastHeader")
    .SetParent<Header> ()
    .AddConstructor<topoBroadcastHeader> ()
  ;
  return tid;
}

TypeId
topoBroadcastHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
topoBroadcastHeader::GetSerializedSize (void) const
{
//this is the size of an ITS network header

  return 32;
}

void
topoBroadcastHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU16 (m_sequencenb);
  i.WriteU8 (m_lifetime);
  i.WriteU8 (m_reserved);
  i.WriteU64 (m_sourposvector.gnAddr);
  i.WriteU32 (m_sourposvector.Ts);
  i.WriteU32 (m_sourposvector.Lat);
  i.WriteU32 (m_sourposvector.Long);
  i.WriteU16 (m_sourposvector.Speed);
  i.WriteU16 (m_sourposvector.Heading);
  i.WriteU16 (m_sourposvector.Alt);
  i.WriteU8 (m_sourposvector.TimeAcc << 4 | m_sourposvector.PosAcc);
  i.WriteU8 (m_sourposvector.SpeedAcc << 5 | (m_sourposvector.HeadingAcc << 2) | m_sourposvector.AltAcc);
}

uint32_t
topoBroadcastHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_sequencenb = i.ReadU16();
  m_lifetime = i.ReadU8();
  m_reserved = i.ReadU8();
  m_sourposvector.gnAddr = i.ReadU64();
  m_sourposvector.Ts = i.ReadU32();
  m_sourposvector.Lat = i.ReadU32();
  m_sourposvector.Long = i.ReadU32();
  m_sourposvector.Speed = i.ReadU16();
  m_sourposvector.Heading = i.ReadU16();
  m_sourposvector.Alt = i.ReadU16();
  uint8_t m_timac_posac = i.ReadU8();   // this byte includes TimAc (4bit) and PosAc (4bit)
  m_sourposvector.TimeAcc = m_timac_posac >> 4 & 15;
  m_sourposvector.PosAcc = m_timac_posac & 15;
  uint8_t m_sac_hac_aac = i.ReadU8();   // this byte includes SAc (3bit), HAc (3bit) and AAc (2bit)
  m_sourposvector.SpeedAcc = m_sac_hac_aac >> 5 & 7;
  m_sourposvector.HeadingAcc = m_sac_hac_aac >> 2 & 7;
  m_sourposvector.AltAcc = m_sac_hac_aac & 3;

  return GetSerializedSize(); // the number of bytes consumed.
}

void
topoBroadcastHeader::Print (std::ostream &os) const
{
 os << "****TopoBroadcastHeader Header**** "
	   << "source Id: " << m_sourposvector.gnAddr << " "
         << "source Speed: " << m_sourposvector.Speed
        ;
}


void
topoBroadcastHeader::SetSeqNb (uint16_t sn)
{
	m_sequencenb = sn;
}

uint16_t
topoBroadcastHeader::GetSeqNb (void) const
{
  return m_sequencenb;
}

void
topoBroadcastHeader::SetLifeTime (uint8_t lt)
{
	m_lifetime = lt;
}

uint8_t
topoBroadcastHeader::GetLifeTime (void) const
{
  return m_lifetime;
}

void
topoBroadcastHeader::SetSourPosVector (c2cCommonHeader::LongPositionVector vector)
{
  m_sourposvector.gnAddr = vector.gnAddr;
  m_sourposvector.Ts = vector.Ts;
  m_sourposvector.Lat = vector.Lat;
  m_sourposvector.Long = vector.Long;
  m_sourposvector.Speed = vector.Speed;
  m_sourposvector.Heading = vector.Heading;
  m_sourposvector.Alt = vector.Alt;
  m_sourposvector.TimeAcc = vector.TimeAcc;
  m_sourposvector.PosAcc = vector.PosAcc;
  m_sourposvector.SpeedAcc = vector.SpeedAcc;
  m_sourposvector.HeadingAcc = vector.HeadingAcc;
  m_sourposvector.AltAcc = vector.AltAcc;
}

c2cCommonHeader::LongPositionVector
topoBroadcastHeader::GetSourPosVector (void) const
{
  return m_sourposvector;
}

};
