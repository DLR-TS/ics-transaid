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

#include "endPoints.h"
#include "losPoint.h"
#include <iostream>

namespace ns3 {

EndPoints::EndPoints()
{}

EndPoints::~EndPoints()
{}

void 
EndPoints::InsertPoint (losPoint* location, bool visibility)
{
  visibilityPoint* point = new visibilityPoint;
  point->_location = location;
  point->_visibility = visibility;
  m_visibilityPoints.push_back (point);
}

losPoint*
EndPoints::GetClosestPoint (double x, double y)
{
  visibilityPoints::const_iterator closestVisibilityPoint;
  closestVisibilityPoint = GetVisibilityPoint (x,y);
  if (closestVisibilityPoint != m_visibilityPoints.end())
    {
      return (*closestVisibilityPoint)->_location;
    }
  else
    {
      //cout << "Error in EndPoints::GetClosestPoint" << endl;
      return NULL;
    }
}

bool 
EndPoints::GetVisibility (double x, double y)
{
  visibilityPoints::const_iterator closestVisibilityPoint;
  closestVisibilityPoint = GetVisibilityPoint (x,y);
  if (closestVisibilityPoint != m_visibilityPoints.end())
    {
      return (*closestVisibilityPoint)->_visibility;
    }
  else
    {
      //cout << "Error in EndPoints::GetVisibility" << endl;
      return false;
    }
}

std::vector<visibilityPoint*>::const_iterator 
EndPoints::GetVisibilityPoint (double x, double y)
{
  visibilityPoints::const_iterator closestVisibilityPoint = m_visibilityPoints.end();
  losPoint* location = new losPoint (x,y);
  double dist = 100000;
  for(visibilityPoints::const_iterator it = m_visibilityPoints.begin(); it != m_visibilityPoints.end(); ++it)
    {
      double distTemp = CalculateDistance (*location,*((*it)->_location));
      if (distTemp < dist)
        {
          dist = distTemp;
          closestVisibilityPoint = it;
        }
    }
  delete location;
  return closestVisibilityPoint;
}

} // namespace ns3
