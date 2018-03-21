/****************************************************************************/
/// @file    MapFacilities.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MapFacilities.h"

#ifdef SUMO_ON
#include "../../configfile_parsers/sumoMapParser/SUMOdigital-map.h"
#endif

#include <sstream>
#include <set>
#include <cstdlib>
#include <cfloat>
using namespace std;

namespace ics_facilities
{

MapFacilities::MapFacilities()
{
  originLatitude = 0;
  originLongitude = 0;
  originAltitude = 0;
}

MapFacilities::~MapFacilities()
{
  if (!lanes.empty())
    lanes.clear();
  if (!edges.empty())
    edges.clear();
  if (!junctions.empty())
    junctions.clear();
  if (!trafficLights.empty())
    trafficLights.clear();
}

void MapFacilities::setOriginCoordinates(latitude_t originLat, longitude_t originLon, altitude_t originAlt)
{
  originLatitude = originLat;
  originLongitude = originLon;
  originAltitude = originAlt;
}

void MapFacilities::setOriginCoordinates(latitude_t originLat, longitude_t originLon)
{
  originLatitude = originLat;
  originLongitude = originLon;
}

const Lane* MapFacilities::getLane(roadElementID_t laneID) const
{
  map<roadElementID_t, Lane>::const_iterator itLane;
  itLane = lanes.find(laneID);
  if (itLane == lanes.end())
  {
    cerr << "[facilities] Lane " << laneID << " not found." << endl;
    return NULL;
  }
  else
    return &(itLane->second);
}

const Edge* MapFacilities::getEdge(roadElementID_t edgeID) const
{
  map<roadElementID_t, Edge>::const_iterator itEdge;
  itEdge = edges.find(edgeID);
  if (itEdge == edges.end())
  {
    cerr << "[facilities] Edge " << edgeID << " not found." << endl;
    return NULL;
  }
  else
    return &(itEdge->second);
}

const Junction* MapFacilities::getJunction(roadElementID_t junctionID) const
{
  map<roadElementID_t, Junction>::const_iterator itJunction;
  itJunction = junctions.find(junctionID);
  if (itJunction == junctions.end())
  {
    cerr << "[facilities] Junction " << junctionID << " not found." << endl;
    return NULL;
  }
  else
    return &(itJunction->second);
}

const TrafficLight* MapFacilities::getTrafficLight(trafficLightID_t trafficLightID) const
{
  map<trafficLightID_t, TrafficLight>::const_iterator itTrafficLight;
  itTrafficLight = trafficLights.find(trafficLightID);
  if (itTrafficLight == trafficLights.end())
  {
    cerr << "[facilities] Traffic Light " << trafficLightID << " not found." << endl;
    return NULL;
  }
  else
    return &(itTrafficLight->second);
}

const Edge* MapFacilities::getEdgeFromLane(roadElementID_t laneID) const
{
  map<roadElementID_t, Edge>::const_iterator it;
  for (it = edges.begin(); it != edges.end(); it++)
  {
    if (((Edge) (it->second)).containsLane(laneID))
    {
      return &(it->second);
    }
  }
  return NULL;
}

const Junction* MapFacilities::getJunctionFromLane(roadElementID_t laneID) const
{
  map<roadElementID_t, Junction>::const_iterator it;
  for (it = junctions.begin(); it != junctions.end(); it++)
  {
    if (((Junction) (it->second)).containsLane(laneID))
    {
      return &(it->second);
    }
  }
  return NULL;
}

Lane* MapFacilities::convertPoint2Map(Point2D &pos)
{
  float minDistance = HUGE_VAL;
  float newDistance = HUGE_VAL;
  Point2D lineStart;
  Point2D lineEnd;
  Point2D intersection;
  Lane *result = NULL;

  if (lanes.empty())
  {
    cerr << "[facilities] ERROR: The loaded map does not contain any lane!" << endl;
    abort();
  }

  map<roadElementID_t, Lane>::iterator itL;
  for (itL = lanes.begin(); itL != lanes.end(); itL++)
  {
    Lane *currLane = (Lane*) &itL->second;
    const vector<Point2D> currLaneShape = currLane->getShape();
    if (currLaneShape.size() > 1)
    {
      for (unsigned int i = 0; i < currLaneShape.size() - 1; i++)
      {
        lineStart = currLaneShape[i];
        lineEnd = currLaneShape[i + 1];
        // if this line is no candidate for lying closer to the Cartesian position
        // than the line determined so far, skip it
        if ((lineStart.y() > (pos.y() + minDistance) && lineEnd.y() > (pos.y() + minDistance))
            || (lineStart.y() < (pos.y() - minDistance) && lineEnd.y() < (pos.y() - minDistance))
            || (lineStart.x() > (pos.x() + minDistance) && lineEnd.x() > (pos.x() + minDistance))
            || (lineStart.x() < (pos.x() - minDistance) && lineEnd.x() < (pos.x() - minDistance)))
        {
          continue;
        }
        else
        {
          newDistance = closestDistancePointLine(pos, lineStart, lineEnd, intersection);
          if (newDistance < minDistance)
          {
            // new distance is shorter: save the found road map position
            minDistance = newDistance;
            // get the names of the edge and the lane
            result = currLane;
          }
        }
      }
    }
  }
  return result;
}

const Junction* MapFacilities::getClosestJunction(Point2D& pos)
{
  roadElementID_t closestJunctID;
  const Junction* closestJunct;

  const Lane* lane = convertPoint2Map(pos);

  if (!lane->getJunctionID().empty())
    closestJunctID = lane->getJunctionID();
  else
  {
    vector<Lane*> nextLanes = lane->getNextLanes();
    float minDistance = HUGE_VAL;
    for (unsigned int i = 0; i < nextLanes.size(); i++)
    {
      if (!nextLanes[i]->getJunctionID().empty())
      {
        const vector<Point2D> nextShape = nextLanes[i]->getShape();
        float distance = pos.distanceTo(nextShape[0]);
        if (distance < minDistance)
        {
          minDistance = distance;
          closestJunctID = nextLanes[i]->getJunctionID();
        }
      }
    }
  }
  closestJunct = getJunction(closestJunctID);
  return closestJunct;
}

const vector<roadElementID_t> MapFacilities::getNeighboringJunctions(roadElementID_t junctionID) const
{
  map<roadElementID_t, Junction>::const_iterator it;
  it = junctions.find(junctionID);
  if (it == junctions.end())
  {
    cerr << "[facilities] ERROR - The junction " << junctionID << " does not exist!" << endl;
    abort();
  }
  return it->second.getNeighboringJunctions();
}

vector<const Edge*>* MapFacilities::getEdgesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID) const
{
  if (junctAID == junctBID)
  {
    cerr << "[facilities] WARNING: The two junctions for the retrieval of the common edges are the same." << endl;
    return NULL;
  }

  set<const Edge*> setEdges;

  const Junction *junctA = getJunction(junctAID);
  const Junction *junctB = getJunction(junctBID);
  if ((junctA == NULL) || (junctB == NULL))
    return NULL;
  const vector<roadElementID_t> lanesA = junctA->getIncomingLaneIDs();
  const vector<roadElementID_t> lanesB = junctB->getIncomingLaneIDs();
  for (unsigned int i = 0; i < lanesA.size(); i++)
  {
    const Lane *currLaneA = getLane(lanesA[i]);
    const Edge *currEdgeA = getEdge(currLaneA->getEdgeID());
    for (unsigned int j = 0; i < lanesB.size(); j++)
    {
      const Lane *currLaneB = getLane(lanesB[j]);
      const Edge *currEdgeB = getEdge(currLaneB->getEdgeID());

      if (currEdgeA->getID() == currEdgeB->getID())
      {
        setEdges.insert(currEdgeB);
      }
    }
  }
  set<const Edge*>::iterator itset;
  vector<const Edge*>* vEdges = new vector<const Edge*>();
  for (itset = setEdges.begin(); itset != setEdges.end(); itset++)
  {
    vEdges->push_back(*itset);
  }
  return vEdges;
}

vector<const Lane*>* MapFacilities::getLanesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID) const
{
  if (junctAID == junctBID)
  {
    cerr << "[facilities] WARNING: The two junctions for the retrieval of the common lanes are the same." << endl;
    return NULL;
  }

  set<const Lane*> setLanes;

  const Junction *junctA = getJunction(junctAID);
  const Junction *junctB = getJunction(junctBID);
  if ((junctA == NULL) || (junctB == NULL))
    return NULL;
  const vector<roadElementID_t> lanesA = junctA->getIncomingLaneIDs();
  const vector<roadElementID_t> lanesB = junctB->getIncomingLaneIDs();
  for (unsigned int i = 0; i < lanesA.size(); i++)
  {
    const Lane *currLaneA = getLane(lanesA[i]);
    for (unsigned int j = 0; i < lanesB.size(); j++)
    {
      const Lane *currLaneB = getLane(lanesB[j]);
      if (currLaneA->getID() == currLaneB->getID())
      {
        setLanes.insert(currLaneB);
      }
    }
  }
  set<const Lane*>::iterator itset;
  vector<const Lane*>* vLanes = new vector<const Lane*>();
  for (itset = setLanes.begin(); itset != setLanes.end(); itset++)
  {
    vLanes->push_back(*itset);
  }
  return vLanes;
}

vector<const Junction*>* MapFacilities::getJunctionsFromEdge(roadElementID_t edgeID)
{
  const Edge *edge = getEdge(edgeID);
  if (edge == NULL)
  {
    cerr << "[facilities] ERROR: Edge " << edgeID << " not found." << endl;
    return NULL;
  }

  set<const Junction*> setJunctions;

  vector<roadElementID_t> vectLanesIDs = edge->getLaneIDs();
  for (unsigned int i = 0; i < vectLanesIDs.size(); i++)
  {
    const Lane *currLane = getLane(vectLanesIDs[i]);
    const vector<Lane*> vectPrevLanes = currLane->getPrevLanes();
    for (unsigned int j = 0; j < vectPrevLanes.size(); j++)
    {
      if (!vectPrevLanes[j]->getJunctionID().empty())
      {
        const Junction *jun = getJunction(vectPrevLanes[j]->getJunctionID());
        setJunctions.insert(jun);
      }
    }
    const vector<Lane*> vectNextLanes = currLane->getNextLanes();
    for (unsigned int j = 0; j < vectNextLanes.size(); j++)
    {
      if (!vectNextLanes[j]->getJunctionID().empty())
      {
        const Junction *jun = getJunction(vectNextLanes[j]->getJunctionID());
        setJunctions.insert(jun);
      }
    }
  }
  set<const Junction*>::iterator itset;
  vector<const Junction*>* vJunctions = new vector<const Junction*>();
  for (itset = setJunctions.begin(); itset != setJunctions.end(); itset++)
  {
    vJunctions->push_back(*itset);
  }
  return vJunctions;
}

void MapFacilities::setLaneStatus(roadElementID_t laneID, laneStatus newStatus)
{
  map<roadElementID_t, Lane>::iterator itLane;
  itLane = lanes.find(laneID);
  if (itLane != lanes.end())
    itLane->second.setStatus(newStatus);
}

void MapFacilities::setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight)
{
  map<roadElementID_t, Lane>::iterator itLane;
  itLane = lanes.find(laneID);
  if (itLane != lanes.end())
    itLane->second.setWeight(newWeight);
}

void MapFacilities::updateTrafficLightDynamicInformation(const trafficLightID_t &tlID, std::string& state)
{
  map<trafficLightID_t, TrafficLight>::iterator tl = trafficLights.find(tlID);
  if (tl == trafficLights.end())
  {
    TrafficLight trafficLight = TrafficLight::CreateTrafficLight(tlID);
    trafficLight.setActive(true);
    trafficLight.setActiveAllLanes(true);
    tl = trafficLights.insert(make_pair(tlID, trafficLight)).first;
  }
  if (!tl->second.setState(state))
    cerr << "[facilities] Can't update traffic light " << tlID << ": wrong number of lanes in state." << endl;
}

bool MapFacilities::mapParser(string mapFilename)
{
  bool configFlag = false;

#ifdef SUMO_ON

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Load SUMOmap" << endl;
#endif // _DEBUG

  // Load SUMO map
  sumo_map::SUMODigitalMap sumoMap;
  sumoMap.loadMap(mapFilename);

  // ==== convert the SUMO map to the ICS format ====

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Convert Junctions" << endl;
#endif // _DEBUG

  // convert Junctions
  map<string, sumo_map::SUMOJunction>::iterator itSUMOJunction;
  for (itSUMOJunction = sumoMap.junctions.begin(); itSUMOJunction != sumoMap.junctions.end(); itSUMOJunction++)
  {
    sumo_map::SUMOJunction *currSUMOJun = &(itSUMOJunction->second);
    stringstream sJid;
    sJid << currSUMOJun->id;
    Junction currJunction(sJid.str());
    currJunction.setCenter(currSUMOJun->center);
    currJunction.setIncomingLaneIDs(currSUMOJun->stringIncSUMOLanes);
    currJunction.setInternalLaneIDs(currSUMOJun->stringIntSUMOLanes);
    junctions.insert(pair<roadElementID_t, Junction>(currJunction.getID(), currJunction));
  }

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Convert Traffic Lights" << endl;
#endif // _DEBUG

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Convert Lanes and Edges" << endl;
#endif // _DEBUG

  // convert Lanes and Edges
  map<string, sumo_map::SUMOEdge>::iterator itSUMOEdge;
  for (itSUMOEdge = sumoMap.edges.begin(); itSUMOEdge != sumoMap.edges.end(); itSUMOEdge++)
  {
    // get the ID of the edge
    stringstream sEid;
    sEid << itSUMOEdge->first;
    Edge currEdge(sEid.str());
    vector<roadElementID_t> currEdgeLaneIDs;

    // get the IDs of the lanes
    map<string, sumo_map::SUMOLane>::iterator itSUMOLane;
    sumo_map::SUMOEdge *e = &(itSUMOEdge->second);
    itSUMOLane = e->lanes.begin();

    for (itSUMOLane = e->lanes.begin(); itSUMOLane != e->lanes.end(); itSUMOLane++)
    {
      stringstream sLid;
      sLid << itSUMOLane->first;
      roadElementID_t currLaneID = sLid.str();
      sumo_map::SUMOLane *l = &(itSUMOLane->second);

      if (!currLaneID.empty())
      {
        // Add the laneID to the vector of the current edge
        currEdgeLaneIDs.push_back(currLaneID);

        // Check that the current lane does not exist in the lanes map
        map<roadElementID_t, Lane>::iterator itLanes;
        itLanes = lanes.find(currLaneID);
        if (itLanes == lanes.end())
        {
          // if there is not, create the new lane
          Lane currLane(currLaneID);
          // set the mxSpeed of the lane
          currLane.setMaxSpeed((maxSpeed_t) l->maxspeed);
          // set the length of the lane
          currLane.setLength((laneLength_t) l->length);
          // retrieve the shape
          vector<Point2D> laneShape = l->shape;
          currLane.setShape(laneShape);
          // set the Edge the lane belongs to
          currLane.setEdgeID(currEdge.getID());
          // Missing: junctionID
          if (!e->inner)
          {
            // Look for the junction that has this lane
            map<string, sumo_map::SUMOJunction>::iterator itSUMOjunction;
            for (itSUMOjunction = sumoMap.junctions.begin(); itSUMOjunction != sumoMap.junctions.end();
                itSUMOjunction++)
            {
              vector<string>::iterator itj;
              for (itj = itSUMOjunction->second.stringIntSUMOLanes.begin();
                  itj < itSUMOjunction->second.stringIntSUMOLanes.end(); itj++)
              {
                if (*itj == currLaneID)
                {
                  currLane.setJunctionID(itSUMOjunction->second.id);
                  break;
                }
              }
            }
          }
          lanes.insert(pair<roadElementID_t, Lane>(currLane.getID(), currLane));
        }
      }
    }
    currEdge.setLaneIDs(&currEdgeLaneIDs);
    edges.insert(pair<roadElementID_t, Edge>(currEdge.getID(), currEdge));
  }

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Connect Lanes with each other" << endl;
#endif // _DEBUG

  // Connect Lanes with the previous ones and the following ones
  for (itSUMOEdge = sumoMap.edges.begin(); itSUMOEdge != sumoMap.edges.end(); itSUMOEdge++)
  {
    // Read the current edge
    map<string, sumo_map::SUMOLane>::iterator itSUMOLane;
    for (itSUMOLane = itSUMOEdge->second.lanes.begin(); itSUMOLane != itSUMOEdge->second.lanes.end(); itSUMOLane++)
    {
      // read the current Lane belonging to the current edge
      vector<sumo_map::SUMOLane *>::iterator itSUMOCurrLane;

      // find the corresponding Lane
      map<roadElementID_t, Lane>::iterator itL;
      itL = lanes.find(itSUMOLane->first);

      // previous lanes
      vector<Lane*> prevLanes;
      prevLanes.reserve(0);
      bool hasPrevLanes = false;
      for (itSUMOCurrLane = itSUMOLane->second.prevSUMOLanes.begin();
          itSUMOCurrLane < itSUMOLane->second.prevSUMOLanes.end(); itSUMOCurrLane++)
      {
        // read the ID of the prevLane
        map<roadElementID_t, Lane>::iterator itLp;
        itLp = lanes.find((*itSUMOCurrLane)->id);
        if (itLp != lanes.end())
        {
          prevLanes.push_back(&(itLp->second));
          hasPrevLanes = true;
        }
      }
      if (hasPrevLanes)
        itL->second.setPrevlanes(prevLanes);

      // next lanes
      vector<Lane*> nextLanes;
      nextLanes.reserve(0);
      bool hasNextLanes = false;
      for (itSUMOCurrLane = itSUMOLane->second.nextSUMOLanes.begin();
          itSUMOCurrLane < itSUMOLane->second.nextSUMOLanes.end(); itSUMOCurrLane++)
      {
        // read the ID of the prevLane
        map<roadElementID_t, Lane>::iterator itLp;
        itLp = lanes.find((*itSUMOCurrLane)->id);
        if (itLp != lanes.end())
        {
          nextLanes.push_back(&(itLp->second));
          hasNextLanes = true;
        }
      }
      if (hasNextLanes)
        itL->second.setNextLanes(nextLanes);
    }
  }

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Find the neighboring junctions for all of those." << endl;
#endif // _DEBUG

  // Every Junction will have a vector with the ID of the closest junctions around.
  map<roadElementID_t, Junction>::iterator itJunct;
  for (itJunct = junctions.begin(); itJunct != junctions.end(); itJunct++)
  {
    Junction* currJunction = &(itJunct->second);
    vector<roadElementID_t> currIncomingLanes = itJunct->second.getIncomingLaneIDs();
    for (unsigned int iLane = 0; iLane < currIncomingLanes.size(); iLane++)
    {
      const Lane* currLane = NULL;
      currLane = getLane(currIncomingLanes[iLane]);
      if ((currLane == NULL) || (currLane->getJunctionID() != ""))
      {
        // this would mean that the incoming lane is already inside an intersection: it should not happen!
        cout << "[facilities] Error in the map configuration. Lane " << currIncomingLanes[iLane]
            << " is not inside a junction." << endl;
        return false;
      }

      // check among the nextLanes if any of those are in a neighbor junction
      const vector<Lane*> currNextLanes = ((Lane*) currLane)->getNextLanes();
      vector<Lane*>::const_iterator itNextLane;
      for (itNextLane = currNextLanes.begin(); itNextLane != currNextLanes.end(); itNextLane++)
      {
        Lane* currNextLane = (*itNextLane);
        if (currNextLane->getJunctionID() != "")
        {
          // this currNextlane is inside a junction and thus I can the junction to the neighbor list (if not already done)
          currJunction->addNeighboringJunction(currNextLane->getJunctionID());
        }
      }

      // check among the prevLanes if any of those are in a neighbor junction
      const vector<Lane*> currPrevLanes = ((Lane*) currLane)->getPrevLanes();
      vector<Lane*>::const_iterator itPrevLane;
      for (itPrevLane = currPrevLanes.begin(); itPrevLane != currPrevLanes.end(); itPrevLane++)
      {
        Lane* currPrevLane = (*itPrevLane);
        if (currPrevLane->getJunctionID() != "")
        {
          // this currNextlane is inside a junction and thus I can the junction to the neighbor list (if not already done)
          currJunction->addNeighboringJunction(currPrevLane->getJunctionID());
        }
      }
    }
  }

  configFlag = true;

#elif VANETMOBISIM_ON
  configFlag = false;
#else
  cerr << "[facilities] Error: Traffic simulator not set." << endl;
  configFlag = false;
#endif

#ifdef _DEBUG_MAP
  cout << "[facilities] DEB: Map parsing completed. Returning with state: " << configFlag << endl;
#endif // _DEBUG

  return configFlag;
}

float MapFacilities::closestDistancePointLine(const Point2D &point, /**< Coordinates of the point (x,y). */
const Point2D &lineStart, /**< Coordinates of the first point of the line. */
const Point2D &lineEnd, /**< Coordinates of the last point of the line. */
Point2D &outIntersection)
{ /**< Coordinates of the intersection point (output). */
  Point2D directVec(lineEnd.x() - lineStart.x(), lineEnd.y() - lineStart.y());
  float u = (((point.x() - lineStart.x()) * directVec.x()) + ((point.y() - lineStart.y()) * directVec.y()))
      / (directVec.x() * directVec.x() + directVec.y() * directVec.y());

  // if closest point does not fall within the line segment,
  //      then return line start or line end

  if (u >= 0.0f && u <= 1.0f)
  {
    outIntersection.set(lineStart.x() + u * directVec.x(), lineStart.y() + u * directVec.y());
  }
  else
  {
    if (u < 0.0f)
    {
      outIntersection = lineStart;
    }
    if (u > 1.0f)
    {
      outIntersection = lineEnd;
    }
  }
  return point.distanceTo(outIntersection);
}

latitude_t MapFacilities::getLat0()
{
  return originLatitude;
}

longitude_t MapFacilities::getLon0()
{
  return originLongitude;
}

altitude_t MapFacilities::getAlt0()
{
  return originAltitude;
}

Area2D* MapFacilities::getWholeArea(vector<Area2D*> areas)
{
  if (areas.size() == 1)
  {
    Lane *currLane = (Lane*) dynamic_cast<const Lane*>(areas[0]);
    if (currLane != NULL)
      return convertLane2Rectangle(*currLane);
    Edge *currEdge = (Edge*) dynamic_cast<const Edge*>(areas[0]);
    if (currEdge != NULL)
      return convertEdge2Rectangle(*currEdge);
    Junction *currJunction = (Junction*) dynamic_cast<const Junction*>(areas[0]);
    if (currJunction != NULL)
      return convertJunction2Rectangle(*currJunction);
    TrafficLight *currTrafficLight = (TrafficLight*) dynamic_cast<const TrafficLight*>(areas[0]);
    if (currTrafficLight != NULL)
    {
      cerr << "[facilities] Warning: a vector of areas contains a traffic light!" << endl;
      return NULL;
    }
    //will need to check before the returned pointer can be safely deleted
    return (Area2D*) areas[0];
  }
  else
  {
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = FLT_MIN;
    float maxY = FLT_MIN;

    for (unsigned int i = 0; i < areas.size(); i++)
    {
      Rectangle *currRect = NULL;
      Lane *currLane = (Lane*) dynamic_cast<const Lane*>(areas[i]);
      if (currLane != NULL)
        currRect = convertLane2Rectangle(*currLane);
      Edge *currEdge = (Edge*) dynamic_cast<const Edge*>(areas[i]);
      if (currEdge != NULL)
        currRect = convertEdge2Rectangle(*currEdge);
      Junction *currJunction = (Junction*) dynamic_cast<const Junction*>(areas[i]);
      if (currJunction != NULL)
        currRect = convertJunction2Rectangle(*currJunction);
      Rectangle *currRectangle = (Rectangle*) dynamic_cast<const Rectangle*>(areas[i]);
      if (currRectangle != NULL)
        currRect = currRectangle;
      Ellipse *currEllipse = (Ellipse*) dynamic_cast<const Ellipse*>(areas[i]);
      if (currEllipse != NULL)
      {
        Rectangle currRect_tmp = currEllipse->getCircumscribedRectangle();
        currRect = &currRect_tmp;
      }
      if (currRect != NULL)
      {
        const vector<Point2D> vertices = currRect->getVertices();
        for (unsigned int j = 0; j < 4; j++)
        {
          if (vertices[j].x() < minX)
            minX = vertices[j].x();
          if (vertices[j].y() < minY)
            minY = vertices[j].y();
          if (vertices[j].x() > maxX)
            maxX = vertices[j].x();
          if (vertices[j].x() > maxY)
            maxY = vertices[j].y();
        }
      }
      if (currRectangle == NULL)
        delete currRect;
      Circle *currCircle = (Circle*) dynamic_cast<const Circle*>(areas[i]);
      if (currCircle != NULL)
      {
        Point2D center = (Point2D) currCircle->getCenter();
        float circleMinX = center.x() - currCircle->getRadius();
        float circleMinY = center.y() - currCircle->getRadius();
        float circleMaxX = center.x() + currCircle->getRadius();
        float circleMaxY = center.y() + currCircle->getRadius();
        if (circleMinX < minX)
          minX = circleMinX;
        if (circleMinY < minY)
          minY = circleMinY;
        if (circleMaxX > maxX)
          maxX = circleMaxX;
        if (circleMaxY > maxY)
          maxY = circleMaxY;
      }
      ConvexPolygon *currConvexPolygon = (ConvexPolygon*) dynamic_cast<const ConvexPolygon*>(areas[i]);
      if ((currConvexPolygon != NULL) && (currConvexPolygon->getNumberOfVertices() != 4))
      {
        const vector<Point2D> vertices = currConvexPolygon->getVertices();
        for (unsigned int j = 0; j < currConvexPolygon->getNumberOfVertices(); j++)
        {
          if (vertices[j].x() < minX)
            minX = vertices[j].x();
          if (vertices[j].y() < minY)
            minY = vertices[j].y();
          if (vertices[j].x() > maxX)
            maxX = vertices[j].x();
          if (vertices[j].x() > maxY)
            maxY = vertices[j].y();
        }
      }
    }
    vector<Point2D> verts;
    verts.push_back(Point2D(minX, minY));
    verts.push_back(Point2D(maxX, minY));
    verts.push_back(Point2D(maxX, maxY));
    verts.push_back(Point2D(minX, maxY));
    Rectangle *rect = new Rectangle(verts);
    return rect;
  }
}

Circle MapFacilities::getCircleFromArea(Area2D* area)
{
  Rectangle *currRect = NULL;

  // Road elements
  Lane *currLane = (Lane*) dynamic_cast<const Lane*>(area);
  if (currLane != NULL)
    currRect = convertLane2Rectangle(*currLane);
  Edge *currEdge = (Edge*) dynamic_cast<const Edge*>(area);
  if (currEdge != NULL)
    currRect = convertEdge2Rectangle(*currEdge);
  Junction *currJunction = (Junction*) dynamic_cast<const Junction*>(area);
  if (currJunction != NULL)
    currRect = convertJunction2Rectangle(*currJunction);
  if (currRect != NULL)
  {
    Circle circle = currRect->getCircumscribedCircle();
    delete currRect;
    return circle;
  }

  // Shapes
  Rectangle *currRectangle = (Rectangle*) dynamic_cast<const Rectangle*>(area);
  if (currRectangle != NULL)
  {
    currRect = currRectangle;
    Circle circle = currRect->getCircumscribedCircle();
    return circle;
  }
  Ellipse *currEllipse = (Ellipse*) dynamic_cast<const Ellipse*>(area);
  if (currEllipse != NULL)
  {
    Circle circle = currEllipse->getCircumscribedCircle();
    return circle;
  }
  Circle *currCircle = (Circle*) dynamic_cast<const Circle*>(area);
  if (currCircle != NULL)
  {
    Circle circle = *currCircle;
    return circle;
  }
  ConvexPolygon *currConvexPolygon = (ConvexPolygon*) dynamic_cast<const ConvexPolygon*>(area);
  if (currConvexPolygon != NULL)
  {
    Circle circle = currConvexPolygon->getCircumscribedCircle();
    return circle;
  }
  cerr << "[facilities] getCircleFromArea() - The area in input is not valid." << endl;
  abort();
}

Rectangle* MapFacilities::convertLane2Rectangle(Lane& lane)
{
  return lane.createRectangleFromShape();
}

Rectangle* MapFacilities::convertEdge2Rectangle(Edge& edge)
{
  float minX = FLT_MAX;
  float minY = FLT_MAX;
  float maxX = FLT_MIN;
  float maxY = FLT_MIN;
  const vector<roadElementID_t>& laneIDs = edge.getLaneIDs();
  for (unsigned int i = 0; i < laneIDs.size(); i++)
  {
    Lane *currLane = (Lane*) getLane(laneIDs[i]);
    Rectangle *currRect = convertLane2Rectangle(*currLane);
    const vector<Point2D> vertices = currRect->getVertices();
    for (unsigned int j = 0; j < 4; j++)
    {
      if (vertices[j].x() < minX)
        minX = vertices[j].x();
      if (vertices[j].y() < minY)
        minY = vertices[j].y();
      if (vertices[j].x() > maxX)
        maxX = vertices[j].x();
      if (vertices[j].y() > maxY)
        maxY = vertices[j].y();
    }
  }
  vector<Point2D> verts;
  verts.push_back(Point2D(minX, minY));
  verts.push_back(Point2D(maxX, minY));
  verts.push_back(Point2D(maxX, maxY));
  verts.push_back(Point2D(minX, maxY));
  Rectangle *rect = new Rectangle(verts);
  return rect;
}

Rectangle* MapFacilities::convertJunction2Rectangle(Junction& junction)
{
  float minX = FLT_MAX;
  float minY = FLT_MAX;
  float maxX = FLT_MIN;
  float maxY = FLT_MIN;
  const vector<roadElementID_t>& laneIDs = junction.getInternalLaneIDs();
  for (unsigned int i = 0; i < laneIDs.size(); i++)
  {
    Lane *currLane = (Lane*) getLane(laneIDs[i]);
    Rectangle *currRect = convertLane2Rectangle(*currLane);
    const vector<Point2D> vertices = currRect->getVertices();
    for (unsigned int j = 0; j < 4; j++)
    {
      if (vertices[j].x() < minX)
        minX = vertices[j].x();
      if (vertices[j].y() < minY)
        minY = vertices[j].y();
      if (vertices[j].x() > maxX)
        maxX = vertices[j].x();
      if (vertices[j].y() > maxY)
        maxY = vertices[j].y();
    }
  }
  vector<Point2D> verts;
  verts.push_back(Point2D(minX, minY));
  verts.push_back(Point2D(maxX, minY));
  verts.push_back(Point2D(maxX, maxY));
  verts.push_back(Point2D(minX, maxY));
  Rectangle *rect = new Rectangle(verts);
  return rect;
}

}
