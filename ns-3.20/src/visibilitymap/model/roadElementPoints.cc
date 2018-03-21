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

#include "roadElementPoints.h"
#include "losPoint.h"
#include <iostream>

namespace ns3 {

RoadElementPoints::RoadElementPoints()
{}

RoadElementPoints::~RoadElementPoints()
{}

void 
RoadElementPoints::InsertLosPoint (string pointId, losPoint* location)
{
  m_losPoints.insert (std::make_pair(pointId, location));
}

losPoint* 
RoadElementPoints::GetLosPoint (string pointId)
{
  losPoints::iterator it = m_losPoints.begin();
  it = m_losPoints.find(pointId);
  if ( it != m_losPoints.end() ) 
    {
      return it->second;
    }
  else
    {
      return NULL;
    }
}

losPoint* 
RoadElementPoints::GetStartIntersection (void)
{
  losPoints::iterator it = m_losPoints.end();
  if (m_losPoints.size () > 1)
    {
      --it;
      --it;
    }
  else
    {
      --it;
    }
  return it->second;
}

losPoint* 
RoadElementPoints::GetEndIntersection (void)
{
  losPoints::iterator it = m_losPoints.end();
  --it;
  return it->second;
}

} // namespace ns3
