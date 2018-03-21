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

#ifndef BUILDING_MAP_MODEL_H
#define BUILDING_MAP_MODEL_H

#include "ns3/visibility-model.h"
#include "ns3/mobility-model.h"
#include "ns3/object.h"

namespace ns3 {

/**
 * @class BuildingMapModel
 * @brief This class provides the LOS/NLOS visibility between tx and rx based on the information obtained from a building map file that provides the positions of the walls of the buildings (x1,y1,x2,y2) present in the scenario
 */

class BuildingMapModel : public VisibilityModel
{
public:
  struct Segment {
    double x1;
    double y1;
    double x2;
    double y2;
  };
  static TypeId GetTypeId (void);
  BuildingMapModel (void);
  ~BuildingMapModel ();
  BuildingMapModel (std::string path);
  bool GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  void GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const;
  void InitializeVisibilityModel (void);


private:
  bool DoSegmentsIntersect (BuildingMapModel::Segment seg1, BuildingMapModel::Segment seg2) const;
  std::vector<struct BuildingMapModel::Segment> m_buildingList;

};

} // namespace ns3

#endif /* BUILDING_MAP_MODEL_H */
