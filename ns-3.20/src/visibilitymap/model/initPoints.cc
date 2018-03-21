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

#include "initPoints.h"
#include "losPoint.h"
#include "endEdges.h"
#include <iostream>

namespace ns3 {

InitPoints::InitPoints()
{}

InitPoints::~InitPoints()
{}

void 
InitPoints::InsertPoint (losPoint* location, EndEdges* edges)
{
  edgesSet* point = new edgesSet;
  point->_location = location;
  point->_edges = edges;
  m_initPoints.push_back (point);
}

losPoint*
InitPoints::GetClosestPoint (double x, double y)
{
  points::const_iterator closestPoint;
  closestPoint = GetPointIterator (x,y);
  if (closestPoint != m_initPoints.end())
    {
      return (*closestPoint)->_location;
    }
  else
    {
      //cout << "Error in InitPoints::GetClosestPoint" << endl;
      return NULL;
    }
}

EndEdges* 
InitPoints::GetEndEdges (double x, double y)
{
  points::const_iterator closestPoint;
  closestPoint = GetPointIterator (x,y);
  if (closestPoint != m_initPoints.end())
    {
      return (*closestPoint)->_edges;
    }
  else
    {
      //cout << "Error in InitPoints::GetEndEdges " << endl;
      return NULL;
    }
}

std::vector<edgesSet*>::const_iterator 
InitPoints::GetPointIterator (double x, double y)
{
  points::const_iterator closestPoint = m_initPoints.end();
  losPoint* location = new losPoint (x,y);
  double dist = 100000;
  for(points::const_iterator it = m_initPoints.begin(); it != m_initPoints.end(); ++it)
    {
      double distTemp = CalculateDistance (*location,*((*it)->_location));
      if (distTemp < dist)
        {
          dist = distTemp;
          closestPoint = it;
        }
    }
  delete location;
  return closestPoint;
}

} // namespace ns3
