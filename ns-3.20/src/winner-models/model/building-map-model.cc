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

#include "building-map-model.h"
#include "ns3/itetris-mobility-model.h"
#include "ns3/log.h"
#include <string>
#include <fstream>

NS_LOG_COMPONENT_DEFINE ("BuildingMapModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BuildingMapModel);

TypeId 
BuildingMapModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BuildingMapModel")
    .SetParent<VisibilityModel> ()
    .AddConstructor<BuildingMapModel> ()
    ;
  return tid;
}


BuildingMapModel::BuildingMapModel ()
{}

BuildingMapModel::~BuildingMapModel ()
{}

BuildingMapModel::BuildingMapModel (std::string path)
{
  m_path = path;
}

void 
BuildingMapModel::InitializeVisibilityModel (void)
{
  std::ifstream* m_is = new std::ifstream ();
  m_is->open (m_path.c_str (), std::ios::in);
  m_is->seekg (0);
  struct BuildingMapModel::Segment building;
  int numBuildings = 0;
  while (m_is->good())
    {
      *m_is >> building.x1 >> building.y1 >> building.x2 >> building.y2;
      m_buildingList.push_back (building);
      numBuildings ++;
    }
  NS_LOG_DEBUG ("Num buildings = " << numBuildings);
  m_is->close ();
  delete m_is;
  m_is = 0;
}

void
BuildingMapModel::GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const
{
  dist1 = dist2 = a->GetDistanceFrom (b) / sqrt(2);
  NS_LOG_DEBUG ("Distances d1 = " << dist1 << " d2 = " << dist2);
}

bool
BuildingMapModel::GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{

  bool vis = true;
  Vector posA = a->GetPosition ();
  Vector posB = b->GetPosition ();
  BuildingMapModel::Segment seg;
  seg.x1 = posA.x;
  seg.y1 = posA.y;
  seg.x2 = posB.x;
  seg.y2 = posB.y;

  for (std::vector<struct BuildingMapModel::Segment>::const_iterator it = m_buildingList.begin(); it != m_buildingList.end(); ++it)
  {
    if ( DoSegmentsIntersect (seg, *it) )
      {
        vis = false;
        break;
      }
  }

  NS_LOG_DEBUG ("Visibility = " << vis);
  return vis;

}

bool 
BuildingMapModel::DoSegmentsIntersect (BuildingMapModel::Segment seg1, BuildingMapModel::Segment seg2) const
{
  // http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/

  bool inter = false;
  double denom = ((seg2.y2-seg2.y1)*(seg1.x2-seg1.x1))-((seg2.x2-seg2.x1)*(seg1.y2-seg1.y1));
  double numa = (seg2.x2-seg2.x1)*(seg1.y1-seg2.y1)-(seg2.y2-seg2.y1)*(seg1.x1-seg2.x1);
  double numb = (seg1.x2-seg1.x1)*(seg1.y1-seg2.y1)-(seg1.y2-seg1.y1)*(seg1.x1-seg2.x1);

  if (denom != 0.0)
    {
      double ua = numa/denom;
      double ub = numb/denom;
      if ( ua <= 1 && ua >= 0 && ub <= 1 && ub >= 0 ) // Segments intesect each other
        {
          inter = true;
          NS_LOG_DEBUG ("Segments intersecting");
          NS_LOG_DEBUG ("Seg1=(" <<seg1.x1 << "," <<seg1.y1 << ") ("<<seg1.x2 << "," << seg1.y2 << ")");
          NS_LOG_DEBUG ("Seg1=(" <<seg2.x1 << "," <<seg2.y1 << ") ("<<seg2.x2 << "," << seg2.y2 << ")");
        }
    }
  else if ( (numa == 0.0) && (numb == 0.0) )
    {
      NS_FATAL_ERROR ("BuildingMapModel::DoSegmentsIntersect - Tx and Rx are in the same position as one of the buildings in the scenario");
    }

  return inter;

}

} // namespace ns3
