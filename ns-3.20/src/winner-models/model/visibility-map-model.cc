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

#include "visibility-map-model.h"
#include "ns3/itetris-mobility-model.h"

#include "ns3/readVisibilityMap.h"
#include "ns3/roadElements.h"
#include "ns3/visibilityMap.h"
#include "ns3/roadElementPoints.h"
#include "ns3/log.h"
#include "ns3/string.h"

NS_LOG_COMPONENT_DEFINE ("VisibilityMapModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (VisibilityMapModel);

TypeId 
VisibilityMapModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VisibilityMapModel")
    .SetParent<VisibilityModel> ()
    .AddConstructor<VisibilityMapModel> ()
    ;
  return tid;
}

VisibilityMapModel::VisibilityMapModel ()
  : m_map (0),
    m_roadElements (0)
{}

VisibilityMapModel::~VisibilityMapModel ()
{}

VisibilityMapModel::VisibilityMapModel (std::string path)
{
  m_path = path;
  ReadVisibilityMap readMap = ReadVisibilityMap (path);
  readMap.ReadXmlFile ();
  m_map = readMap.GetVisibilityMap ();
  m_roadElements = readMap.GetRoadElements ();
}

void 
VisibilityMapModel::SetVisibilityFilePath (std::string path) 
{
  ReadVisibilityMap readMap = ReadVisibilityMap (path);
  readMap.ReadXmlFile ();
  m_map = readMap.GetVisibilityMap ();
  m_roadElements = readMap.GetRoadElements ();
}

void 
VisibilityMapModel::InitializeVisibilityModel (void) 
{
  ReadVisibilityMap readMap = ReadVisibilityMap (m_path);
  readMap.ReadXmlFile ();
  m_map = readMap.GetVisibilityMap ();
  m_roadElements = readMap.GetRoadElements ();
}

void 
VisibilityMapModel::SetRoadElements (RoadElements* roadElements) 
{
  m_roadElements = roadElements;
}

void
VisibilityMapModel::GetNlosDistances(Ptr<MobilityModel> a, Ptr<MobilityModel> b, double &dist1, double &dist2) const
{
  dist1 = dist2 = 0.0;
  referencePoint pointA, pointB;

  pointA.elementId = DynamicCast<ItetrisMobilityModel>(a)->GetEdgeId ();
  pointA.location.x = a->GetPosition ().x;
  pointA.location.y = a->GetPosition ().y;
  pointB.elementId = DynamicCast<ItetrisMobilityModel>(b)->GetEdgeId ();
  pointB.location.x = b->GetPosition ().x;
  pointB.location.y = b->GetPosition ().y;

  string pAElementId = CheckIfInternal (pointA.elementId);
  string pBElementId = CheckIfInternal (pointB.elementId);
  
  RoadElementPoints* edgeA = m_roadElements->GetEdge(pAElementId);
  losPoint startInterA = *(edgeA->GetStartIntersection());
  losPoint endInterA = *(edgeA->GetEndIntersection());
  RoadElementPoints* edgeB = m_roadElements->GetEdge(pBElementId);
  losPoint startInterB = *(edgeB->GetStartIntersection());
  losPoint endInterB = *(edgeB->GetEndIntersection());

  NS_LOG_DEBUG ("Point A -> Start intersection x=" << startInterA.x << " y=" << startInterA.y);
  NS_LOG_DEBUG ("Point A -> End intersection x=" << endInterA.x << " y=" << endInterA.y);
  NS_LOG_DEBUG ("Point B -> Start intersection x=" << startInterB.x << " y=" << startInterB.y);
  NS_LOG_DEBUG ("Point B -> End intersection x=" << endInterB.x << " y=" << endInterB.y);

  if ( (startInterA == startInterB) || (startInterA == endInterB) )
    {
      NS_LOG_DEBUG ("Nodes share one intersection");
      dist1 = CalculateDistance (pointA.location, startInterA); 
      dist2 = CalculateDistance (pointB.location, startInterA);
    }
  else if ( (endInterA == startInterB) || (endInterA == endInterB) )
    {
      NS_LOG_DEBUG ("Nodes share one intersection");
      dist1 = CalculateDistance (pointA.location, endInterA); 
      dist2 = CalculateDistance (pointB.location, endInterA);  
    }
  else
    {
      NS_LOG_DEBUG ("Nodes do not share any intersection");
      double dist = CalculateDistance (pointA.location, pointB.location);
      dist1 = dist2 = dist/sqrt(2);
    }

  NS_LOG_DEBUG ("Distances d1 = " << dist1 << " d2 = " << dist2);
}

std::string 
VisibilityMapModel::CheckIfInternal (std::string edgeId) const
{
  string resEdgeId = edgeId;
  size_t found = edgeId.find (":"); 
  if (found == 0)
    {
      // Internal Edge
      resEdgeId = GetEdgeId (edgeId);     
    }
  return resEdgeId;
}

std::string 
VisibilityMapModel::GetEdgeId (std::string laneId) const
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
VisibilityMapModel::GetVisibility (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const
{
  bool vis = true;
  referencePoint pointA, pointB;

  pointA.elementId = DynamicCast<ItetrisMobilityModel>(a)->GetEdgeId ();
  pointA.location.x = a->GetPosition ().x;
  pointA.location.y = a->GetPosition ().y;
  pointB.elementId = DynamicCast<ItetrisMobilityModel>(b)->GetEdgeId ();
  pointB.location.x = b->GetPosition ().x;
  pointB.location.y = b->GetPosition ().y;
    
  if (pointA.elementId.compare("") != 0 && pointB.elementId.compare("") != 0)
    {
      vis = m_map->GetVisibility(pointA,pointB);
    }
  else
    {
      NS_LOG_DEBUG ("Mobility model not initialized");
    }
  NS_LOG_DEBUG ("Visibility = " << vis);
  return vis;
}

} // namespace ns3
