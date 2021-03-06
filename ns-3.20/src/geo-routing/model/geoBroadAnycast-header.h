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

#ifndef GEOAB_HEADER_H
#define GEOAB_HEADER_H

#include "ns3/header.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/c2c-common-header.h"

namespace ns3 {

/**
 * \brief The packet extended header for a C2C broadcast packet
 */

class GeoABcastHeader : public Header
{
public:
  // must be implemented to become a valid new header.
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;



  // allow protocol-specific access to the header data.

  void SetSeqNb (uint16_t sn);
  uint16_t GetSeqNb (void) const;

  void SetLifeTime (uint8_t lt);
  uint8_t GetLifeTime (void) const;

  void SetSourPosVector (c2cCommonHeader::LongPositionVector vector);
  c2cCommonHeader::LongPositionVector GetSourPosVector (void) const;

  void SetGeoAreaPos1 (struct c2cCommonHeader::geoAreaPos  areapos1);
  struct c2cCommonHeader::geoAreaPos  GetGeoAreaPos1 (void) const;

  void SetGeoAreaPos2 (struct c2cCommonHeader::geoAreaPos  areapos2);
  struct c2cCommonHeader::geoAreaPos  GetGeoAreaPos2 (void) const;

  void SetAreaSize (uint32_t as);
  uint32_t GetAreaSize (void) const;


private:

  uint16_t m_sequencenb;
  uint8_t m_lifetime;
  uint8_t m_reserved;
  c2cCommonHeader::LongPositionVector m_sourposvector;
  struct c2cCommonHeader::geoAreaPos m_geoareapos1;
  struct c2cCommonHeader::geoAreaPos m_geoareapos2;
  uint32_t m_areasize;  // really a uint4_t
};
}; //namespace ns3

#endif   /*GEOAB_HEADER_H*/

