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

#ifndef VISIBILITY_MAP_MODEL_H
#define VISIBILITY_MAP_MODEL_H

#include "ns3/visibility-model.h"
#include "ns3/mobility-model.h"
#include "ns3/object.h"

namespace ns3 {

class VisibilityMap;
class RoadElements;
class MobilityModel;

/**
 * @class VisibilityMapModel
 * @brief This class provides the LOS/NLOS visibility between tx and rx based on the information obtained from a visibility map file that provides the visibility conditions for a set of grids that compose each street in the road network.
 */

class VisibilityMapModel : public VisibilityModel
{
public:
  static TypeId GetTypeId (void);
  VisibilityMapModel (void);
  ~VisibilityMapModel ();
  VisibilityMapModel (std::string path);
  bool GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  void GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const;
  void SetVisibilityFilePath (std::string path);
  void InitializeVisibilityModel (void);
  void SetRoadElements (RoadElements* roadElements);

private:
  std::string CheckIfInternal (std::string edgeId) const; 
  std::string GetEdgeId (std::string laneId) const;
  VisibilityMap* m_map;
  RoadElements* m_roadElements;
};

} // namespace ns3

#endif /* VISIBILITY_MAP_MODEL_H */
