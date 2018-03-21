/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <Fatma.Hrizi@eurecom.fr>
 */

#include "ns3/log.h"
#include "road-side-unit.h"

NS_LOG_COMPONENT_DEFINE ("RoadSideUnit");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (RoadSideUnit);

TypeId RoadSideUnit::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RoadSideUnit")
    .AddConstructor<RoadSideUnit> ()
    .SetParent<Object> ()                  
    ;
  return tid;
}

RoadSideUnit::RoadSideUnit ()
  : m_nodeId(0), 
    m_lat(0),
    m_lon (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

RoadSideUnit::~RoadSideUnit ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void RoadSideUnit::SetNodeId (uint32_t nodeId)
{
  m_nodeId = nodeId;
}

void RoadSideUnit::SetLat (uint32_t lat)
{
  m_lat = lat;
}

void RoadSideUnit::SetLon (uint32_t lon)
{
  m_lon = lon;
}

uint32_t RoadSideUnit::GetNodeId (void) const
{
  return (m_nodeId);
}

uint32_t RoadSideUnit::GetLat (void) const
{ 
  return (m_lat);
}

uint32_t RoadSideUnit::GetLon (void) const
{
  return (m_lon);
}

} // namespace ns3