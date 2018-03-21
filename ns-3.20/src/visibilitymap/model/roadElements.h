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

#ifndef ROAD_ELEMENTS_H
#define ROAD_ELEMENTS_H

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace ns3 {

class RoadElementPoints;

class RoadElements
{
public:
  RoadElements ();
  ~RoadElements ();
  void InsertEdge (string elementId, RoadElementPoints* pointsLocations);
  void InsertJunction (string elementId, RoadElementPoints* pointsLocations);
  RoadElementPoints* GetEdge (string elementId);
  RoadElementPoints* GetJunction (string elementId);
  
private:
  typedef map<string,RoadElementPoints*> roadElement;
  roadElement m_edges;
  roadElement m_junctions;
};

} // namespace ns3

#endif /* ROAD_ELEMENTS_H */
