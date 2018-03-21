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

#include <stdlib.h>

#include "ns3/log.h"
#include "c2c-address.h"
#include "ns3/assert.h"

NS_LOG_COMPONENT_DEFINE ("c2cAddress");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (c2cAddress);

const uint32_t c2cAddress::ANYCAST = 0x9998;
const uint32_t c2cAddress::BROAD = 0x9999;

TypeId 
c2cAddress::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cAddress")
    .SetParent<Object> ()
    .AddConstructor<c2cAddress> ()
    ;
  return tid;
}

c2cAddress::c2cAddress ()
{}

c2cAddress::~c2cAddress ()
{
  free (m_geoareapos1);
  if (m_geoareapos2 != NULL)
  free (m_geoareapos2);
}

//Geo destination address
void
c2cAddress::Set (uint64_t id, struct c2cCommonHeader::geoAreaPos* areapos1, struct c2cCommonHeader::geoAreaPos* areapos2, uint32_t size)
{
  m_geoareapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos2 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_id = id;
  m_geoareapos1->lat = areapos1->lat;
  m_geoareapos1->lon = areapos1->lon;
  m_geoareapos2->lat = areapos2->lat;
  m_geoareapos2->lon = areapos2->lon;
  m_areasize = size;
}

void
c2cAddress::Set (uint64_t id, uint32_t lat1, uint32_t lon1, uint32_t lat2, uint32_t lon2, uint32_t size)
{
  m_id = id;
  m_geoareapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos2 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos1->lat = lat1;
  m_geoareapos1->lon = lon1;
  m_geoareapos2->lat = lat2;
  m_geoareapos2->lon = lon2;
  m_areasize = size;
}

//Node address
void
c2cAddress::Set (uint64_t id, struct c2cCommonHeader::geoAreaPos* areapos1)
{
  m_id = id;
  m_geoareapos1 = areapos1;
  m_geoareapos2 = NULL;
}

//TOPO address
void
c2cAddress::Set (uint64_t id, uint32_t ttl)
{
  m_id = id;
  m_geoareapos1 = NULL;
  m_geoareapos2 = NULL;
  m_areasize = ttl;
}

void
c2cAddress::Set (uint64_t id, uint32_t lat1, uint32_t lon1)
{
  m_id = id;
  m_geoareapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos1->lat = lat1;
  m_geoareapos1->lon = lon1;
  m_geoareapos2 = NULL;
}

void
c2cAddress::Set (c2cAddress addr)
{
  m_id = addr.GetId ();
  m_geoareapos1 = addr.GetGeoAreaPos1 ();
  m_geoareapos2 = addr.GetGeoAreaPos2 ();
  m_areasize = addr.GetAreaSize ();
}

void
c2cAddress::Print (std::ostream &os) const
{
}

uint64_t
c2cAddress::GetId (void) const
{
  return m_id;
}

void
c2cAddress::SetId (uint64_t id)
{
  m_id = id;
}

uint32_t
c2cAddress::GetAreaSize (void) const
{
  return m_areasize;
}

void
c2cAddress::SetAreaSize (uint32_t size)
{
  m_areasize = size;
}

void
c2cAddress::SetGeoAreaPos1 (struct c2cCommonHeader::geoAreaPos*  areapos1)
{
  m_geoareapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos1->lat = areapos1->lat;
  m_geoareapos1->lon = areapos1->lon;
}

struct c2cCommonHeader::geoAreaPos*
c2cAddress::GetGeoAreaPos1 (void) const
{
  return m_geoareapos1;
}

void
c2cAddress::SetGeoAreaPos2 (struct c2cCommonHeader::geoAreaPos*  areapos2)
{
  m_geoareapos2 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
  m_geoareapos2->lat = areapos2->lat;
  m_geoareapos2->lon = areapos2->lon;
}

struct c2cCommonHeader::geoAreaPos*
c2cAddress::GetGeoAreaPos2 (void) const
{
  return m_geoareapos2;
}

bool
c2cAddress::IsGeoBroadcast (void) const
{
  return (m_id == BROAD && m_geoareapos1 != NULL);
}

bool
c2cAddress::IsBroadcast (void) const
{
  return (m_id == BROAD);
}

bool
c2cAddress::IsGeoUnicast (void) const
{
  return (m_id != BROAD && m_id != ANYCAST && m_geoareapos2 == NULL);
}

bool
c2cAddress::IsGeoAnycast (void) const
{
  return (m_id == ANYCAST);
}

bool
c2cAddress::IsTopoBroadcast (void) const
{
  return (m_id == BROAD && m_geoareapos1 == NULL);
}

} // namespace ns3
