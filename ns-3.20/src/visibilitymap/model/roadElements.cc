/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "roadElements.h"
#include "roadElementPoints.h"
#include <iostream>

namespace ns3 {

RoadElements::RoadElements()
{}

RoadElements::~RoadElements()
{}

void 
RoadElements::InsertEdge (string elementId, RoadElementPoints* pointsLocations)
{
  m_edges.insert (std::make_pair(elementId, pointsLocations));
}

void 
RoadElements::InsertJunction (string elementId, RoadElementPoints* pointsLocations)
{
  m_junctions.insert (std::make_pair(elementId, pointsLocations));
}

RoadElementPoints* 
RoadElements::GetEdge (string elementId)
{
  roadElement::iterator it = m_edges.begin();
  it = m_edges.find(elementId);
  if ( it != m_edges.end() ) 
    {
      return it->second;
    }
  else
    {
      return NULL;
    }
}

RoadElementPoints* 
RoadElements::GetJunction (string elementId)
{
  roadElement::iterator it = m_junctions.begin();
  it = m_junctions.find(elementId);
  if ( it != m_junctions.end() ) 
    {
      return it->second;
    }
  else
    {
      return NULL;
    }
}

} // namespace ns3
