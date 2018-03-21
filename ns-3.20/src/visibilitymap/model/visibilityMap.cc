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

#include "visibilityMap.h"
#include "initPoints.h"
#include "endEdges.h"
#include "endPoints.h"
#include "ns3/assert.h"
#include <iostream>

namespace ns3 {

VisibilityMap::VisibilityMap ()
{}

VisibilityMap::~VisibilityMap ()
{}

void 
VisibilityMap::InsertEdge (string edgeId, InitPoints* initPoints)
{
  m_edges.insert (std::make_pair(edgeId, initPoints));
}

InitPoints* 
VisibilityMap::GetInitPoints (string edgeId)
{
  edges::iterator it = m_edges.begin();
  it = m_edges.find(edgeId);
  if ( it != m_edges.end() ) 
    {
      return it->second;
    }
  else
    {
      cout << "Error in VisibilityMap::GetInitPoints" << endl;
      return NULL;
    }
}

void
VisibilityMap::CheckIfInternal (string& edgeId)
{
  size_t found = edgeId.find (":"); 
  if (found == 0)
    {
      // Internal Edge
      edgeId = GetEdgeId (edgeId);     
    }
}

std::string 
VisibilityMap::GetEdgeId (std::string laneId)
{
  string edgeId = "";
  size_t found = laneId.find ("_"); 
  size_t pos = string::npos;
  
  while (found != string::npos)
    {
      pos = found;
      found = laneId.find ("_",found+1,1); 
    }
  
  if (pos != string::npos)
    {
      edgeId = laneId.substr (0,pos);
    }
  return edgeId;
}

bool 
VisibilityMap::GetVisibility (const referencePoint &origen, const referencePoint &destination)
{
  string origElementId = origen.elementId;
  string destElementId = destination.elementId;
  CheckIfInternal (origElementId);
  CheckIfInternal (destElementId);

  InitPoints* initPoints = GetInitPoints (origElementId);
  NS_ASSERT (initPoints != NULL);
  losPoint* initPoint = initPoints->GetClosestPoint (origen.location.x, origen.location.y);
  InitPoints* endPointsTemp = GetInitPoints (destElementId);
  NS_ASSERT (endPointsTemp != NULL);
  losPoint* endPoint = endPointsTemp->GetClosestPoint (destination.location.x, destination.location.y);
  // Check first whether origen and destination locations correspond to the same point
  if (initPoint == endPoint)
    {
      return true;
    }

  NS_ASSERT (initPoints != NULL);
  EndEdges* endEdges = initPoints->GetEndEdges (origen.location.x, origen.location.y);
  NS_ASSERT (endEdges != NULL);
  EndPoints* endPoints = endEdges->GetEndPoints (destElementId);
  NS_ASSERT (endPoints != NULL);
  return endPoints->GetVisibility (destination.location.x, destination.location.y);
}

} // namespace ns3

