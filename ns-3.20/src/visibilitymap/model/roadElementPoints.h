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

#ifndef ROAD_ELEMENTS_POINTS_H
#define ROAD_ELEMENTS_POINTS_H

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace ns3 {

class losPoint;

class RoadElementPoints
{
public:
  RoadElementPoints ();
  ~RoadElementPoints ();
  void InsertLosPoint (string pointId, losPoint* location);
  losPoint* GetLosPoint (string pointId);
  losPoint* GetStartIntersection (void);
  losPoint* GetEndIntersection (void);
  
private:
  typedef map<string,losPoint*> losPoints;
  losPoints m_losPoints;
};

} // namespace ns3

#endif /* ROAD_ELEMENTS_POINTS_H */

