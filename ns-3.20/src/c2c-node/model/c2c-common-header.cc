/*
 * Copyright (c) 2009  EURECOM, DLR, EU FP7 iTETRIS project
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
 * Author: Fatma HRIZI <hrizi@eurecom.fr>, Matthias ROECKL <matthias.roeckl@dlr.de>
 */

#include "c2c-common-header.h"

#include "ns3/header.h"
#include "ns3/address-utils.h"
#include "ns3/node.h"

namespace ns3 {

TypeId
c2cCommonHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cCommonHeader")
    .SetParent<Header> ()
    .AddConstructor<c2cCommonHeader> ()
  ;
  return tid;
}

TypeId
c2cCommonHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
c2cCommonHeader::GetSerializedSize (void) const
{
  /**
  * This is the size of a C2C Common Header
  */
  if (m_hType == Node::C2C_BEACON) {
	  uint32_t i = GetIpv6ListSize();
	  return 9*4 + i*16;
  }
  else
	  return 9*4;
}

void
c2cCommonHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  bool beacon = false;
   if (m_hType == Node::C2C_BEACON)
   {
 	  beacon = true;
   }
   i.WriteU8 ((m_version << 4)|m_nHeader);
   i.WriteU8 ((m_hType << 4)|m_hSubType);
   i.WriteU8 (m_reserved);
   i.WriteU8 (m_flags);
   i.WriteHtonU16 (m_pLength);
   i.WriteU8 (m_tClass);
   i.WriteU8 (m_hLimit);
   i.WriteHtonU64 (m_sourposvector.gnAddr);
   i.WriteHtonU32 (m_sourposvector.Ts);
   i.WriteHtonU32 (m_sourposvector.Lat);
   i.WriteHtonU32 (m_sourposvector.Long);
   i.WriteHtonU16 (m_sourposvector.Speed);
   i.WriteHtonU16 (m_sourposvector.Heading);
   i.WriteHtonU16 (m_sourposvector.Alt);
   i.WriteU8 (m_sourposvector.TimeAcc << 4 | m_sourposvector.PosAcc);
   i.WriteU8 (m_sourposvector.SpeedAcc << 5 | (m_sourposvector.HeadingAcc << 2) | m_sourposvector.AltAcc);
   if (beacon == true) {
   	   for (unsigned int j=0; j<m_ipv6address.size(); j++) {
   		  //std::cout<<"c2cCommonHeader::Serialize"<<m_ipv6address[j]<<std::endl;
   		   WriteTo (i, m_ipv6address[j]);
   		   //i = i.Next();
   	   }
   }
}

uint32_t
c2cCommonHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint8_t m_version_nheader = i.ReadU8();   // this byte includes Version (4bit) and nhtype (4bit)
  m_version = (m_version_nheader >> 4)&15;
  m_nHeader = m_version_nheader&15;
  uint8_t m_ht_hst = i.ReadU8();   // this byte includes HT (4bit) and HST (4bit)
  m_hType = (m_ht_hst >> 4)&15;
  m_hSubType = m_ht_hst&15;
  m_reserved = i.ReadU8 ();
  m_flags = i.ReadU8 ();
  m_pLength = i.ReadNtohU16 ();
  m_tClass = i.ReadU8 ();
  m_hLimit = i.ReadU8 ();
  m_sourposvector.gnAddr = i.ReadNtohU64 ();
  m_sourposvector.Ts = i.ReadNtohU32 ();
  m_sourposvector.Lat = i.ReadNtohU32 ();
  m_sourposvector.Long = i.ReadNtohU32 ();
  m_sourposvector.Speed = i.ReadNtohU16 ();
  m_sourposvector.Heading = i.ReadNtohU16 ();
  m_sourposvector.Alt = i.ReadNtohU16 ();
  uint8_t m_timac_posac = i.ReadU8 ();   // this byte includes TimAc (4bit) and PosAc (4bit)
  m_sourposvector.TimeAcc = (m_timac_posac >> 4)&15;
  m_sourposvector.PosAcc = m_timac_posac&15;
  uint8_t m_sac_hac_aac = i.ReadU8 ();   // this byte includes SAc (3bit), HAc (3bit) and AAc (2bit)
  m_sourposvector.SpeedAcc = (m_sac_hac_aac >> 5)&7;
  m_sourposvector.HeadingAcc = (m_sac_hac_aac >> 2)&7;
  m_sourposvector.AltAcc = m_sac_hac_aac&3;

  int k=0;
    if ((uint16_t)GetHtype() == Node::C2C_BEACON) {
  	  while (!i.IsEnd())
  	  {
  		 //std::cout<<"c2cCommonHeader::Deserialize 2, type: "<<(uint16_t)GetHtype()<<std::endl;
  		  m_ipv6address.push_back("::");
  		 //std::cout<<"c2cCommonHeader::Deserialize 3"<<std::endl;
  		  ReadFrom (i, m_ipv6address[k]);
  		  k++;
  	  }
    }

    return 9*4 + (k)*16; // The number of bytes consumed.
}

void
c2cCommonHeader::Print (std::ostream &os) const
{
 os << "****C2C Common Header**** "
         << "source ID: " << m_sourposvector.gnAddr << " "
         << "source TS: " << m_sourposvector.Ts << " "
         << "source Latitude: " << m_sourposvector.Lat << " "
         << "source Longitude: " << m_sourposvector.Long << " "
         << "source Altitude: " << m_sourposvector.Alt << " "
         << "source Speed: " << m_sourposvector.Speed << " "
         << "source Heading: " << m_sourposvector.Heading << " "
         << "header type: " << (uint16_t) m_hType << " "
         << "TTL: " << (uint16_t) m_hLimit << " "
         << "payload length: " << m_pLength << " "
         << "source node's IPv6 address: " << (m_ipv6address.size()>0?m_ipv6address[0]:"no address")
        ;
}

void
c2cCommonHeader::SetVersion (uint8_t version)
{
  m_version = version;
}

uint8_t
c2cCommonHeader::GetVersion (void) const
{
  return m_version;
}

void
c2cCommonHeader::SetNextHeader (uint8_t nexth)
{
  m_nHeader = nexth;
}

uint8_t
c2cCommonHeader::GetNextHeader (void) const
{
  return m_nHeader;
}

void
c2cCommonHeader::SetHtype (uint8_t type)
{
  m_hType = type;
}

uint8_t
c2cCommonHeader::GetHtype (void) const
{
  return m_hType;
}

void
c2cCommonHeader::SetHSubtype (uint8_t stype)
{
  m_hSubType = stype;
}

uint8_t
c2cCommonHeader::GetHSubtype (void) const
{
  return m_hSubType;
}

void
c2cCommonHeader::SetFlags (uint8_t flags)
{
 m_flags = flags;
}

Ipv6Address
c2cCommonHeader::GetIpv6AddressSender(uint32_t i)
{
  return m_ipv6address[i];
}

uint32_t
c2cCommonHeader::GetIpv6ListSize() const
{
	return m_ipv6address.size();
}

void
c2cCommonHeader::AddIpv6AddressSender(Ipv6Address addr)
{
	std::cout<<"c2cCommonHeader::AddIpv6AddressSender 1"<<addr<<std::endl;
	m_ipv6address.push_back(addr);
	std::cout<<"c2cCommonHeader::AddIpv6AddressSender 2 vector size:"<<GetIpv6ListSize()<<std::endl;
}

uint8_t
c2cCommonHeader::GetFlags (void) const
{
 return m_flags;
}

void
c2cCommonHeader::SetLength (uint16_t plength)
{
 m_pLength = plength;
}

uint16_t
c2cCommonHeader::GetLength (void) const
{
 return m_pLength;
}

void
c2cCommonHeader::SetTrafClass (uint8_t tclass)
{
  m_tClass = tclass;
}

uint8_t
c2cCommonHeader::GetTrafClass (void) const
{
  return m_tClass;
}

void
c2cCommonHeader::SetHopLimit (uint8_t ttl)
{
 m_hLimit = ttl;
}

uint8_t
c2cCommonHeader::GetHopLimit (void) const
{
 return m_hLimit;
}

void
c2cCommonHeader::SetSourPosVector (struct LongPositionVector vector)
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

struct c2cCommonHeader::LongPositionVector
c2cCommonHeader::GetSourPosVector (void) const
{
  return m_sourposvector;
}

};
