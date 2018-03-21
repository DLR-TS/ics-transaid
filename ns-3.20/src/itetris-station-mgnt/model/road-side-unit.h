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

#ifndef ROAD_SIDE_UNIT_H
#define ROAD_SIDE_UNIT_H

#include "ns3/simulator.h"
#include "ns3/object.h"

namespace ns3
{
/**
 * @class RoadSideUnit 
 * @brief The base class RoadSideUnit can be derived to implement a specific installer which allows attach a communication module to a node.
 */

class RoadSideUnit : public Object
{
  public:
    static TypeId GetTypeId (void);
    RoadSideUnit();
    virtual ~RoadSideUnit();
    uint32_t GetNodeId (void) const;
    uint32_t GetLat (void) const;
    uint32_t GetLon (void) const;
    void SetNodeId (uint32_t nodeId);
    void SetLat (uint32_t lat);
    void SetLon (uint32_t lon);


  private:
    uint32_t m_nodeId;
    uint32_t m_lat;
    uint32_t m_lon;
};

}

#endif