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

#ifndef VISIBILITY_MAP_H
#define VISIBILITY_MAP_H

#include <map>
#include <string>
#include "losPoint.h"

using namespace std;

namespace ns3 {

class InitPoints;

struct referencePoint {
  string elementId;
  losPoint location;
};

/**
 * @class VisibilityMap
 * @brief This class in used by the winner-models/VisibilityMapModel to obtain the visibility conditions between a given pair of locations in a road network
 */

class VisibilityMap
{
public:
  VisibilityMap ();
  ~VisibilityMap ();
  void InsertEdge (string edgeId, InitPoints* initPoints);
  InitPoints* GetInitPoints (string edgeId);
  bool GetVisibility (const referencePoint &origen, const referencePoint &destination);
  
private:
  void CheckIfInternal (string& edgeId);
  std::string GetEdgeId (std::string laneId);
  typedef map<string,InitPoints*> edges;
  edges m_edges; 
};

} // namespace ns3

#endif /* VISIBILITY_MAP_H */
