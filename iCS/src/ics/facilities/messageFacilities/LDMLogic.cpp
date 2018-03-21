/****************************************************************************/
/// @file    LDMLogic.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 17, 2010
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

#include <limits>
#include <sstream>
#include <set>
#include <sstream>
#include <iomanip>

#include "LDMLogic.h"
#include "../../configfile_parsers/LDMrules-configfile-parser.h"
#include "../../../utils/ics/iCSGeoUtils.h"
#include "../../../utils/ics/log/ics-log.h"
#include "../../../utils/common/TplConvert.h"
#include "../../sync-manager.h"

#ifdef WIN32
#include <limits>
#define NAN std::numeric_limits<float>::quiet_NaN()
#endif

namespace ics_facilities
{

// Internal function
bool convertPoint_str2Point2D(ics_parsing::point_str p, Point2D *point2D, latitude_t lat0, longitude_t lon0,
    altitude_t alt0);

/* ***************************************************************************
 * ***************************************************************************
 * ***************************************************************************
 * **********                                                       **********
 * **********             PUBLIC METHODS                            **********
 * **********                                                       **********
 * ***************************************************************************
 * ***************************************************************************
 ***************************************************************************/

LDMLogic::LDMLogic(MapFacilities* mapFac, StationFacilities* staFac)
{
  this->mapFac = mapFac;
  this->staFac = staFac;
  if ((mapFac == NULL) || (staFac == NULL))
  {
    cerr << "[facilities] ERROR: LDMLogic cannot be allocated because";
    cerr << " either the MapFacilities or the StationFacilities (or both) was not allocated!" << endl;
    abort();
  }
  isRelevantDirectionDefined = false;
}

LDMLogic::~LDMLogic()
{
  if (!iFPT.empty())
  {
    for (map<actionID_t, ReceivedMessage*>::iterator it = iFPT.begin(); it != iFPT.end(); it++)
      delete (it->second);
    iFPT.clear();
  }
  if (!iFMT.empty())
  {
    for (map<actionID_t, ReceivedMessage*>::iterator it = iFMT.begin(); it != iFMT.end(); it++)
      delete (it->second);
    iFMT.clear();
  }
  if (!stationsSeqNoRecord.empty())
    stationsSeqNoRecord.clear();
  if (!eventList.empty())
    eventList.clear();
  if (!deletionList.empty())
    deletionList.clear();
  if (!relevantArea.empty())
  {
    for (vector<Area2D*>::iterator it = relevantArea.begin(); it != relevantArea.end(); it++)
      delete *it;
    relevantArea.clear();
  }
  if (!relevantMessages.empty())
    relevantMessages.clear();
}

ReceivedMessage* LDMLogic::getReceivedMessage(actionID_t actionID) const
{
  map<actionID_t, ReceivedMessage*>::const_iterator it = iFMT.find(actionID);
  if (it == iFMT.end())
  {
    return NULL;
  }
  return it->second;
}

FacilityMessagePayload* LDMLogic::getReceivedMessagePayload(actionID_t actionID) const
{
  //Need to use the other table
  map<actionID_t, ReceivedMessage*>::const_iterator it = iFPT.find(actionID);
  if (it == iFPT.end())
  {
    it = iFMT.find(actionID);
    if (it == iFMT.end())
    {
    	ostringstream oss;
    	oss << "LDMLogic::getReceivedMessagePayload returning NULL for actionID " << actionID;

      ics::IcsLog::LogLevel(oss.str().c_str(), ics::kLogLevelWarning);
      return NULL;
    }
  }
  return (FacilityMessagePayload*) (it->second->getPayload());
}

map<actionID_t, ReceivedMessage*>* LDMLogic::getLDM(stationID_t stationID)
{
  map<actionID_t, ReceivedMessage*> *LDM = new map<actionID_t, ReceivedMessage*>();
  map<actionID_t, ReceivedMessage*>::iterator it;
  for (it = iFMT.begin(); it != iFMT.end(); it++)
  {
    ReceivedMessage *currMessage = it->second;
    vector<Receiver> receivers = currMessage->getReceiversList();

    for (vector<Receiver>::iterator itV = receivers.begin(); itV != receivers.end(); itV++)
    {
      if (itV->receiverID == stationID)
      {
        LDM->insert(pair<actionID_t, ReceivedMessage*>(it->first, currMessage));
      }
    }
  }
  return LDM;
}

map<actionID_t, ReceivedMessage*>* LDMLogic::getLastMessages(icstime_t startTime)
{
  map<actionID_t, ReceivedMessage*> *lastMessages = new map<actionID_t, ReceivedMessage*>();
  map<actionID_t, ReceivedMessage*>::iterator it;
  for (it = iFMT.begin(); it != iFMT.end(); it++)
  {
    vector<Receiver> receiversList = it->second->getReceiversList();
    if ((receiversList.size() > 0) && (receiversList[0].receptionTime >= startTime))
      lastMessages->insert(pair<actionID_t, ReceivedMessage*>(it->first, it->second));
  }
  return lastMessages;
}

map<actionID_t, ReceivedMessage*>* LDMLogic::getAllSpecificMessages(messageType_t type)
{
  map<actionID_t, ReceivedMessage*> *specificMessages = new map<actionID_t, ReceivedMessage*>();
  map<actionID_t, ReceivedMessage*>::iterator it;
  for (it = iFMT.begin(); it != iFMT.end(); it++)
  {
    ReceivedMessage *currMessage = it->second;
    if (currMessage->getMessageType() == type)
    {
      specificMessages->insert(pair<actionID_t, ReceivedMessage*>(it->first, currMessage));
    }
  }
  return specificMessages;
}

map<actionID_t, ReceivedMessage*>* LDMLogic::getMessagesFromSender(stationID_t stationID)
{
  map<actionID_t, ReceivedMessage*> *senderMessages = new map<actionID_t, ReceivedMessage*>();
  map<actionID_t, ReceivedMessage*>::iterator it;
  for (it = iFMT.begin(); it != iFMT.end(); it++)
  {
    ReceivedMessage *currMessage = it->second;
    const FacilityMessagePayload* messagePayload = currMessage->getPayload();
    if (messagePayload->getSenderID() == stationID)
    {
      senderMessages->insert(pair<actionID_t, ReceivedMessage*>(it->first, currMessage));
    }
  }
  return senderMessages;
}

CAMPayloadGeneral* LDMLogic::getLastGeneratedCAM(stationID_t stationID)
{
  vector<CAMPayloadGeneral*>* allCAMs = getGeneratedCAMsFromStationInTable(stationID);
  CAMPayloadGeneral* lastCAM = NULL;
  icstime_t lastReceivedTime = 0;
  for (vector<CAMPayloadGeneral*>::iterator it = allCAMs->begin(); it != allCAMs->end(); it++)
  {
    if ((*it)->getTimeStamp() >= lastReceivedTime)
    {
      lastReceivedTime = (*it)->getTimeStamp();
      lastCAM = *it;
    }
  }
  return lastCAM;
}

vector<CAMPayloadGeneral*>* LDMLogic::getLastCAMsReceivedByStation(stationID_t stationID)
{
  vector<CAMPayloadGeneral*>* cams = new vector<CAMPayloadGeneral*>();
  vector<ReceivedMessage*> allCams = getReceivedTypeMessagesFromStation(stationID, CAM);
  for (vector<ReceivedMessage*>::iterator it = allCams.begin(); it != allCams.end(); it++)
  {
    if (getMessageReceptionTimeByReceiver(stationID, *it) == simTime)
    {
      FacilityMessagePayload* payload = (FacilityMessagePayload*) (*it)->getPayload();
      CAMPayloadGeneral* cam = dynamic_cast<CAMPayloadGeneral*>(payload);
      if (NULL != cam)
        cams->push_back(cam);
    }
  }
  return cams;
}

map<stationID_t, Point2D>* LDMLogic::getClassifiedStationsPositionsAroundStations(stationID_t receiverID,
    icsstationtype_t type)
{
  map<stationID_t, Point2D>* result = new map<stationID_t, Point2D>();
  vector<ReceivedMessage*> interestingReceivedMessages = getReceivedTypeMessagesFromStation(receiverID, CAM);
  for (unsigned int i = 0; i < interestingReceivedMessages.size(); i++)
  {
    const FacilityMessagePayload* payload = (interestingReceivedMessages[i])->getPayload();
    stationID_t senderID = payload->getSenderID();
    if (staFac->isStationOfType(senderID, type))
    {
      CAMPayloadGeneral* cam = dynamic_cast<CAMPayloadGeneral*>((FacilityMessagePayload*) payload);
      if (NULL == cam)
      {
        cerr
            << "[facilities] - LDMLogic::getMobileStationsPositionsAroundStations(stationID_t receiverID) - ERROR: the message "
                "with action_ID: " << (interestingReceivedMessages[i])->getActionID() << " is not a CAM message!"
            << endl;
        abort();
      }
      result->insert(pair<stationID_t, Point2D>(cam->getSenderID(), cam->getPosition()));
    }
  }
  return result;
}

map<stationID_t, Point2D>* LDMLogic::getClassifiedStationsPositionsAroundStationsInTemporalWindow(
    stationID_t receiverID, icsstationtype_t type, icstime_t start, icstime_t stop)
{
  map<stationID_t, Point2D>* result = new map<stationID_t, Point2D>();
  vector<ReceivedMessage*> interestingReceivedMessages = getReceivedTypeMessagesFromStation(receiverID, CAM);
  for (unsigned int i = 0; i < interestingReceivedMessages.size(); i++)
  {
    vector<Receiver> receiversList = (interestingReceivedMessages[i])->getReceiversList();
    if ((receiversList.size() > 0) && (receiversList[0].receptionTime >= start)
        && (receiversList[0].receptionTime <= start))
    {
      const FacilityMessagePayload* payload = (interestingReceivedMessages[i])->getPayload();
      stationID_t senderID = payload->getSenderID();
      if (staFac->isStationOfType(senderID, type))
      {
        CAMPayloadGeneral* cam = dynamic_cast<CAMPayloadGeneral*>((FacilityMessagePayload*) payload);
        if (NULL == cam)
        {
          cerr
              << "[facilities] - LDMLogic::getMobileStationsPositionsAroundStations(stationID_t receiverID) - ERROR: the message "
                  "with action_ID: " << (interestingReceivedMessages[i])->getActionID() << " is not a CAM message!"
              << endl;
          abort();
        }
        result->insert(pair<stationID_t, Point2D>(cam->getSenderID(), cam->getPosition()));
      }
    }
  }
  return result;
}

vector<CAMPayloadGeneral*>* LDMLogic::getGeneratedCAMsFromStationInTable(stationID_t stationID)
{
  vector<CAMPayloadGeneral*>* cams = new vector<CAMPayloadGeneral*>();

  map<actionID_t, ReceivedMessage*>* messages = getMessagesFromSender(stationID);
  for (map<actionID_t, ReceivedMessage*>::iterator it = messages->begin(); it != messages->end(); it++)
  {
    if (it->second->getMessageType() == CAM)
    {
      FacilityMessagePayload* payload = (FacilityMessagePayload*) it->second->getPayload();
      CAMPayloadGeneral* cam = dynamic_cast<CAMPayloadGeneral*>(payload);
      cams->push_back(cam);
    }
  }
  return cams;
}

const vector<Area2D*>& LDMLogic::getRelevantArea()
{
  return relevantArea;
}

map<icsstationtype_t, bool>* LDMLogic::getRelevantStationTypes() const
{
  return (map<icsstationtype_t, bool>*) &relevantStationTypes;
}

map<messageType_t, bool>* LDMLogic::getRelevantMessageTypes() const
{
  return (map<messageType_t, bool>*) &relevantMessages;
}

direction_t LDMLogic::getRelevantDirection() const
{
  return relevantDirection;
}

direction_t LDMLogic::getRelevantDirectionAccuracy() const
{
  return relevantDirectionAccuracy;
}

void LDMLogic::setRelevantArea(vector<Area2D*>& area)
{
  clearRelevantArea();
  relevantArea = area;
}

void LDMLogic::addRelevantArea(Area2D *relevArea)
{
  relevantArea.push_back(relevArea);
}

void LDMLogic::deleteRelevantArea(Area2D *relevArea)
{
  vector<Area2D*>::iterator it;
  for (it = relevantArea.begin(); it != relevantArea.end(); it++)
  {
    if (*it == relevArea)
    {
      delete *it;
      relevantArea.erase(it);
    }
  }
}

void LDMLogic::clearRelevantArea()
{
  if (!relevantArea.empty())
    relevantArea.clear();
}

void LDMLogic::setRelevantStationTypes(map<icsstationtype_t, bool> &relevStaTypes)
{
  this->relevantStationTypes = relevStaTypes;
}

void LDMLogic::setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy)
{
  this->relevantDirection = relevDirection;
  if (relevDirectionAccuracy > 0)
    this->relevantDirectionAccuracy = relevDirectionAccuracy;
  else
    this->relevantDirectionAccuracy = -relevDirectionAccuracy;
}

bool LDMLogic::configureRelevanceRules(string relevFilename)
{
  bool configFlag = false;

#ifdef _DEBUG_LDM
  cout << "[facilities] LDMLogic: LDM rules settings going to be loaded from " << relevFilename << endl;
#endif

  ics_parsing::LDMrulesGetConfig ldmConfig;
  ldmConfig.readConfigFile(relevFilename);

  configFlag = true;

  // Get defaultMessageLifeInterval
 // Fixed for different timestep
  defaultMessageLifeInterval = ldmConfig.getDefaultMessageLifeInterval() * ics::SyncManager::m_timeResolution;
#ifdef _DEBUG_LDM
  cout << "[facilities] LDMLogic: default Message Life Interval set to " << defaultMessageLifeInterval << " simulation steps." << endl;
#endif

  // Get relevant Areas
  vector<ics_parsing::lane_str> relevantLanes = ldmConfig.getRelevantLanes();
#ifdef _DEBUG_LDM
  if (relevantLanes.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant lanes loaded (" << relevantLanes.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantLanes.size(); i++)
  {
    Lane *tmp = NULL;
    tmp = (Lane*) mapFac->getLane(relevantLanes[i].ID);
    if (tmp != NULL)
      relevantArea.push_back(tmp);
  }
  vector<ics_parsing::edge_str> relevantEdges = ldmConfig.getRelevantEdges();
#ifdef _DEBUG_LDM
  if (relevantEdges.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant edges loaded (" << relevantEdges.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantEdges.size(); i++)
  {
    Edge *tmp = NULL;
    tmp = (Edge*) mapFac->getEdge(relevantEdges[i].ID);
    if (tmp != NULL)
      relevantArea.push_back(tmp);
  }
  vector<ics_parsing::junction_str> relevantJunctions = ldmConfig.getRelevantJunctions();
#ifdef _DEBUG_LDM
  if (relevantJunctions.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant junctions loaded (" << relevantJunctions.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantJunctions.size(); i++)
  {
    Junction *tmp = NULL;
    tmp = (Junction*) mapFac->getJunction(relevantJunctions[i].ID);
    if (tmp != NULL)
      relevantArea.push_back(tmp);
  }
  vector<ics_parsing::circle_str> relevantCircles = ldmConfig.getRelevantCircles();
#ifdef _DEBUG_LDM
  if (relevantCircles.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant circles loaded (" << relevantCircles.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantCircles.size(); i++)
  {
    Point2D circle;
    if (convertPoint_str2Point2D(relevantCircles[i].m_center, &circle, mapFac->getLat0(), mapFac->getLon0(),
        mapFac->getAlt0()))
    {
      float radius = relevantCircles[i].m_radius;
      Circle *tmp = new Circle(circle, radius);
      relevantArea.push_back(tmp);
    }
    else
      return false;
  }
  vector<ics_parsing::ellipse_str> relevantEllipses = ldmConfig.getRelevantEllipses();
#ifdef _DEBUG_LDM
  if (relevantEllipses.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant ellipses loaded (" << relevantEllipses.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantEllipses.size(); i++)
  {
    // First constructor: Ellipse(Point2D focus1, Point2D focus2, float eccentricity);
    if (relevantEllipses[i].m_eccentricity != NAN)
    {
      Point2D focus1, focus2;
      if (convertPoint_str2Point2D(relevantEllipses[i].m_focus1, &focus1, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
          convertPoint_str2Point2D(relevantEllipses[i].m_focus2, &focus2, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
      {
        float eccentricity = relevantEllipses[i].m_eccentricity;
        Ellipse *tmp = new Ellipse(focus1, focus2, eccentricity);
        relevantArea.push_back(tmp);
      }
      else return false;
    }
    // Second constructor: Ellipse(Point2D center, float majorAxis, float minorAxis, float rotationAngleRadians)
    else
    {
      if ((relevantEllipses[i].m_minorAxis != NAN) &&
          (relevantEllipses[i].m_majorAxis != NAN))
      {
        Point2D center;
        if (convertPoint_str2Point2D(relevantEllipses[i].m_center, &center, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
        {
          float minorAxis = relevantEllipses[i].m_minorAxis;
          float majorAxis = relevantEllipses[i].m_majorAxis;
          float rotationAngleRadians = relevantEllipses[i].m_rotationAngleRadians;
          Ellipse *tmp = new Ellipse(center, majorAxis, minorAxis, rotationAngleRadians);
          relevantArea.push_back(tmp);
        }
        else return false;
      }
    }
  }
  vector<ics_parsing::rectangle_str> relevantRectangles = ldmConfig.getRelevantRectangles();
#ifdef _DEBUG_LDM
  if (relevantRectangles.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant rectangles loaded (" << relevantRectangles.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantRectangles.size(); i++)
  {
    // First constructor: Rectangle(std::vector<Point2D> verts)
    if (((relevantRectangles[i].m_vertexA.m_X != NAN) || (relevantRectangles[i].m_vertexA.m_Lat != NAN)) &&
    ((relevantRectangles[i].m_vertexB.m_X != NAN) || (relevantRectangles[i].m_vertexB.m_Lat != NAN)) &&
    ((relevantRectangles[i].m_vertexC.m_X != NAN) || (relevantRectangles[i].m_vertexC.m_Lat != NAN)) &&
    ((relevantRectangles[i].m_vertexD.m_X != NAN) || (relevantRectangles[i].m_vertexD.m_Lat != NAN)))
    {
      Point2D vertexA, vertexB, vertexC, vertexD;
      if (convertPoint_str2Point2D(relevantRectangles[i].m_vertexA, &vertexA, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
      convertPoint_str2Point2D(relevantRectangles[i].m_vertexB, &vertexB, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
      convertPoint_str2Point2D(relevantRectangles[i].m_vertexC, &vertexC, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
      convertPoint_str2Point2D(relevantRectangles[i].m_vertexD, &vertexD, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
      {
        vector<Point2D> verteces;
        verteces.push_back(vertexA);
        verteces.push_back(vertexB);
        verteces.push_back(vertexC);
        verteces.push_back(vertexD);
        Rectangle *tmp = new Rectangle(verteces);
        relevantArea.push_back(tmp);
      }
      else return false;
    }
    else
    {
      // Second constructor: Rectangle(Point2D vertA, Point2D vertB, Point2D center)
      if (((relevantRectangles[i].m_vertexA.m_X != NAN) || (relevantRectangles[i].m_vertexA.m_Lat != NAN)) &&
      ((relevantRectangles[i].m_vertexB.m_X != NAN) || (relevantRectangles[i].m_vertexB.m_Lat != NAN)) &&
      ((relevantRectangles[i].m_center.m_X != NAN) || (relevantRectangles[i].m_center.m_Lat != NAN)))
      {
        Point2D vertexA, vertexB, center;
        if (convertPoint_str2Point2D(relevantRectangles[i].m_vertexA, &vertexA, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
        convertPoint_str2Point2D(relevantRectangles[i].m_vertexB, &vertexB, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
        convertPoint_str2Point2D(relevantRectangles[i].m_center, &center, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
        {
          Rectangle *tmp = new Rectangle(vertexA, vertexB, center);
          relevantArea.push_back(tmp);
        }
        else return false;
      }
      // Third constructor: Rectangle(Point2D pointA, Point2D pointB, float height)
      if (((relevantRectangles[i].m_pointA.m_X != NAN) || (relevantRectangles[i].m_pointA.m_Lat != NAN)) &&
      ((relevantRectangles[i].m_pointB.m_X != NAN) || (relevantRectangles[i].m_pointB.m_Lat != NAN)) &&
      (relevantRectangles[i].m_height != NAN))
      {
        Point2D pointA, pointB;
        if (convertPoint_str2Point2D(relevantRectangles[i].m_pointA, &pointA, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()) &&
        convertPoint_str2Point2D(relevantRectangles[i].m_pointB, &pointB, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
        {
          float height = relevantRectangles[i].m_height;
          Rectangle *tmp = new Rectangle(pointA, pointB, height);
          relevantArea.push_back(tmp);
        }
        else return false;
      }
    }
  }
  vector<ics_parsing::convexPolygon_str> relevantConvexPolygons = ldmConfig.getRelevantConvexPolygons();
#ifdef _DEBUG_LDM
  if (relevantConvexPolygons.size() > 0)
  {
    cout << "[facilities] LDMLogic: relevant polygons loaded (" << relevantConvexPolygons.size() << ")." << endl;
  }
#endif
  for (unsigned int i = 0; i < relevantConvexPolygons.size(); i++)
  {

    vector<ics_parsing::point_str> verteces_str = relevantConvexPolygons[i].m_verteces;
    vector<Point2D> verteces;
    for (unsigned int j = 0; j < verteces_str.size(); j++)
    {
      Point2D v;
      if (convertPoint_str2Point2D(verteces_str[j], &v, mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0()))
      {
        verteces.push_back(v);
      }
      else
        return false;
    }
    if (verteces.size() > 2)
    {
      ConvexPolygon *tmp = new ConvexPolygon(verteces);
      relevantArea.push_back(tmp);
    }
  }

  // Get relevant Direction
  ics_parsing::relevantDirection_str relDir_str = ldmConfig.getRelevantDirection();
  if (relDir_str.m_defined)
  {
    relevantDirection = relDir_str.m_radiansFromNorth;
    relevantDirectionAccuracy = relDir_str.m_accuracy;
    isRelevantDirectionDefined = false;
#ifdef _DEBUG_LDM
    cout << "[facilities] LDMLogic: relevant direction loaded." << endl;
#endif
  }

  // Get relevant Station Types
  ics_parsing::relevantStationTypes_str relStas_str = ldmConfig.getRelevantStationTypes();
  if ((relStas_str.m_fixed != NAN) && ((relStas_str.m_mobile != NAN)))
  {
    relevantStationTypes.insert(pair<icsstationtype_t, bool>(STATION_FIXED, relStas_str.m_fixed));
    relevantStationTypes.insert(pair<icsstationtype_t, bool>(STATION_MOBILE, relStas_str.m_mobile));
#ifdef _DEBUG_LDM
    cout << "[facilities] LDMLogic: relevant station types loaded." << endl;
#endif
  }

  // Get relevant Message Types
  ics_parsing::relevantMessages_str relMes_str = ldmConfig.getRelevantMessages();
  if ((relMes_str.m_cam != NAN) && ((relMes_str.m_denm != NAN)))
  {
    bool cam = relMes_str.m_cam;
    bool denm = relMes_str.m_denm;
    bool topobroadcast = relMes_str.m_topobroadcast;
    bool geobroadcast = relMes_str.m_geobroadcast;
    bool geoanycast = relMes_str.m_geoanycast;
    bool unicast = relMes_str.m_unicast;
    bool multicast = relMes_str.m_multicast;
    relevantMessages.insert(pair<messageType_t, bool>(CAM, cam));
    relevantMessages.insert(pair<messageType_t, bool>(DENM, denm));
    relevantMessages.insert(pair<messageType_t, bool>(TOPOBROADCAST, topobroadcast));
    relevantMessages.insert(pair<messageType_t, bool>(GEOBROADCAST, geobroadcast));
    relevantMessages.insert(pair<messageType_t, bool>(GEOANYCAST, geoanycast));
    relevantMessages.insert(pair<messageType_t, bool>(UNICAST, unicast));
    relevantMessages.insert(pair<messageType_t, bool>(MULTICAST, multicast));
#ifdef _DEBUG_LDM
    cout << "[facilities] LDMLogic: relevant message types loaded." << endl;
#endif
  }

  return configFlag;
}

void LDMLogic::setDefaultMessageLifeInterval(icstime_t defMesLifeInterval)
{
  defaultMessageLifeInterval = defMesLifeInterval;
}

icstime_t LDMLogic::getDefaultMessageLifeInterval() const
{
  return defaultMessageLifeInterval;
}

void LDMLogic::updateClock(icstime_t newSimTime)
{
  simTime = newSimTime;
  int rM = launchIFMTCleanupManager(simTime);
  int rP = iFPTCleanup(simTime);
#ifdef LOG_ON
	ostringstream log;
	log << "[LDMLogic::cleanup] IFMT size " << iFMT.size() << ", removed " << rM << ". IFPT size " << iFPT.size()
			<< ", removed " << rP;
	ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);
#endif
}

bool LDMLogic::hasStationEverSentMessages(stationID_t stationID)
{
  map<stationID_t, seqNo_t>::iterator it = stationsSeqNoRecord.find(stationID);
  if (it != stationsSeqNoRecord.end())
    return true;
  return false;
}

seqNo_t LDMLogic::getLastSeqNoForStation(stationID_t stationID)
{
  map<stationID_t, seqNo_t>::iterator it = stationsSeqNoRecord.find(stationID);
  if (it != stationsSeqNoRecord.end())
    return it->second;
  cerr << "[facilities] - Station " << stationID << "has not sent any message until now (time: " << simTime
      << "). Next time check by using "
          "this method: bool hasStationEverSentMessages(stationID_t stationID)" << endl;
  abort();
}

icstime_t LDMLogic::getReceivedMessageDeletionTimeFromTable(actionID_t actionID)
{
  map<actionID_t, icstime_t>::iterator it = eventList.find(actionID);
  if (it != eventList.end())
    return it->second;
  cerr << "[facilities] - Message with ActionID " << actionID << "has never been received." << endl;
  abort();
}

// ..........................................................................................

actionID_t LDMLogic::createCAMpayload(stationID_t stationID)
{
  // Create a new sequence number for this message
  seqNo_t seqNo = getNewSeqNo(stationID);
  // Create the actionID for this message
  actionID_t actionID = createActionID(stationID, seqNo);
  // Create CAM payload
  Station *sta = (Station*) staFac->getStation(stationID);
  if (sta == NULL)
  {
    cerr << "[facilities] ERROR: LDMLogic::createCAMpayload(): The station with ID " << stationID << " was "
        "not found. No CAM can be created by a non-existent station!" << endl;
    return NULL;
  }
  if (sta->getType() == STATION_MOBILE)
  {
    MobileStation* msta = dynamic_cast<MobileStation*>(sta);
    if (msta == NULL)
      return NULL;

    CAMPayloadBasicVehicleProfile* cam = new CAMPayloadBasicVehicleProfile();
    if (cam == NULL)
      return NULL;

    cam->setStationProfile(111);         // TODO: LDMLogic::createCAMmessage - setStationProfile()
    cam->setVehicleSpeed(msta->getSpeed());
    cam->setHeading(msta->getDirection());
    cam->setStationLength(msta->getVehicleLegth());
    cam->setStationWidth(msta->getVehicleWidth());
    cam->setVehicleAcceleration(msta->getAcceleration());
    cam->setExteriorLightsStatus(msta->getExteriorLights());

//        cam->setVersion(1);
    cam->setSenderID(stationID);
    cam->setTimeStamp(simTime);
    cam->setPosition(sta->getPosition(), mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0());
    cam->setMessageType(CAM);
//        cam->setStationType(sta->);                     // TODO: LDMLogic::createCAMmessage - setStationType()

    if (!storePayload(actionID, cam, CAM))
      cerr << "[facilities] Warning: the payload of the message with actionID " << actionID
          << " was created but it was either already stored in the table, or not stored at all." << endl;
  }
  else
  {
    CAMPayloadBasicRSU* cam = new CAMPayloadBasicRSU();
    if (cam == NULL)
      return NULL;

//        cam->setVersion(1);
    cam->setSenderID(stationID);
    cam->setTimeStamp(simTime);
    cam->setPosition(sta->getPosition(), mapFac->getLat0(), mapFac->getLon0(), mapFac->getAlt0());
    cam->setMessageType(CAM);
//        cam->setStationType(sta->);                     // TODO: LDMLogic::createCAMmessage - setStationType()

    if (!storePayload(actionID, cam, CAM))
    {
      stringstream log;
      log << "[facilities] Warning: the payload of the message with actionID " << actionID
          << " was created but it was either already stored in the table, or not stored at all.";
      ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelWarning);
    }
  }

  stringstream log;
  log << "[facilities] - createCAMpayload() - station: " << stationID << " | actionID: " << actionID;
  ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);

  return actionID;
}

actionID_t LDMLogic::createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
    denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
    latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea)
{
  // Create a new sequence number for this message
  seqNo_t seqNo = getNewSeqNo(stationID);
  // Create the actionID for this message
  actionID_t actionID = createActionID(stationID, seqNo);
  // Create DENM payload
  DENMPayload* denm = new DENMPayload();
  if (denm == NULL)
    return NULL;

  denm->setActionID(actionID);
  denm->setSenderID(stationID);
  denm->setTimeVariables(simTime, expiryTime, frequency);
  denm->setSituation(causeCode, severity);
  denm->setSituationLatitude(latitude);
  denm->setSituationLongitude(longitude);
  denm->setIsNegation(negation);
  denm->setReliability(reliability);
  denm->setRelevanceArea(relevanceArea);
  denm->setMessageType(DENM);

  if (!storePayload(actionID, denm, DENM))
    cerr << "[facilities] Warning: the payload of the message with actionID " << actionID
        << " was created but it was either already stored in the table, or not stored at all." << endl;

  stringstream log;
  log << "[facilities] createDENMpayload - station: " << stationID << " | actionID: " << actionID;
  ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);

  return actionID;
}

actionID_t LDMLogic::createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
    TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
    unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
    int messageSequenceNumber, string &extra)
{

  // Create a new sequence number for this message
  seqNo_t seqNo = getNewSeqNo(stationID);
  // Create the actionID for this message
  actionID_t actionID = createActionID(stationID, seqNo);
  // Create payload for Application Message
  ApplicationMessagePayload* payload = new ApplicationMessagePayload();

  payload->m_extra = extra;
  payload->setSenderID(stationID);
  payload->setTimeStamp(simTime);

  if ((comType != CAM) && (comType != DENM))
    payload->setMessageType(comType);
  else
  {
  	delete payload;
    return NULL;
  }

  payload->setApplicationMessageLength(messageLength);
  payload->setPreferredTechnologies(preferredTechnologies);
  payload->setDestination(destination);
  payload->setCommunicationProfile(communicationProfile);
  payload->setApplicationId(applicationId);
  payload->setApplicationMessageType(applicationMessageType);
  payload->setMessageSequenceNumber(messageSequenceNumber);

  if (!storePayload(actionID, payload, comType))
  {
  	ostringstream err;
    err << "[facilities] Warning: the payload of the message with actionID " << actionID
        << " was created but it was either already stored in the table, or not stored at all.";
    ics::IcsLog::LogLevel(err.str().c_str(), ics::kLogLevelError);
  }
  ostringstream log;
  log << "iCS -> facilities: createApplicationMessagePayload - station: " << stationID << " | actionID: " << actionID;
  ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);

  return actionID;

}

bool LDMLogic::storeMessage(actionID_t actionID, vector<stationID_t> receivers)
{
  map<actionID_t, ReceivedMessage*>::iterator it_iFPT;
  map<actionID_t, ReceivedMessage*>::iterator it_iFMT;
  bool entry_found_in_iFPT;
  bool entry_found_in_iFMT;

  // Search for the payload and message associated to the actionID in the iFPT and iFMT tables
  it_iFPT = iFPT.find(actionID);
  it_iFMT = iFMT.find(actionID);
  if (it_iFPT == iFPT.end())
    entry_found_in_iFPT = false;
  else
    entry_found_in_iFPT = true;
  if (it_iFMT == iFMT.end())
    entry_found_in_iFMT = false;
  else
    entry_found_in_iFMT = true;

  // If the actionID is nor in the iFPT nor in the iFMT, it means that it does not represent any payload. (return false)
  if (!entry_found_in_iFPT && !entry_found_in_iFMT)
  {
    return false;
  }

  // If the actionID is in both the iFPT and the iFMT, then we have a critical error and the program will be aborted
  if (entry_found_in_iFPT && entry_found_in_iFMT)
  {
  	ostringstream log;
  	log << "[LDMLogic::storeMessage] - The message with ActionID " << actionID
        << " is in both the iFPT and iFMT. That is not allowed!";
  	ics::IcsLog::LogLevel(log.str().c_str(), ics::kLogLevelError);
    abort();
  }
  // If the list of receivers is empty, then the storage of the message in the iFPT is useless
  if (receivers.empty())
  {
		// If the payload is in the iFPT, delete it (return false)
		ostringstream log;
		log << "[LDMLogic::storeMessage] Receivers empty for actionid " << actionID;
		if (entry_found_in_iFPT)
		{
			log << " removing payload form iFPT table";
			delete it_iFPT->second;
			iFPT.erase(it_iFPT);
		}
		ics::IcsLog::LogLevel(log.str().c_str(), ics::kLogLevelWarning);
		return false;
  }
  // Check the relevance of the payload type if it is in the iFPT
  if (entry_found_in_iFPT && !isMessageRelevant(*it_iFPT->second))
  {
		ostringstream log;
		log << "[LDMLogic::storeMessage] message is not relevant for actionid " << actionID;
		ics::IcsLog::LogLevel(log.str().c_str(), ics::kLogLevelWarning);
		delete it_iFPT->second;
		iFPT.erase(it_iFPT);
		return false;
  }
  // After we decided that the message is interesting, we can create a unique pointer to the message
  ReceivedMessage *recMsg = NULL;
  if (entry_found_in_iFPT)
    recMsg = it_iFPT->second;
  else
    recMsg = it_iFMT->second;

#ifdef _DEBUG_MESSAGE_STORAGE
  cout << "[DEBUG - storeMessage()] - ActionID: " << actionID << endl;
  cout << "                           Type: ";
  switch (recMsg->getMessageType())
  {
    case 0:
    {
      cout << "CAM" << endl;
      break;
    }
    case 1:
    {
      cout << "DENM" << endl;
      break;
    }
    case 2:
    {
      cout << "BROADCAST" << endl;
      break;
    }
    case 3:
    {
      cout << "GEOBROADCAST" << endl;
      break;
    }
    case 4:
    {
      cout << "GEOANYCAST" << endl;
      break;
    }
    case 5:
    {
      cout << "UNICAST" << endl;
      break;
    }
    case 6:
    {
      cout << "MULTICAST" << endl;
      break;
    }
    default:
    {
      cout << (int) recMsg->getMessageType();
      break;
    }
  }
  cout << "                           Receivers: ";
  for (unsigned int i_deb = 0; i_deb < receivers.size(); i_deb++)
  cout << receivers[i_deb] << " ";
  cout << endl;
#endif

  // --- Check the relevance of the receivers
  // Check that the receivers are relevant according to their types
  vector<stationID_t>* relStations_type = getReceiversRelevantStations(receivers);
#ifdef _DEBUG_MESSAGE_STORAGE
  cout << "                           Relevant Receivers by type: ";
  for (unsigned int i_deb = 0; i_deb < relStations_type->size(); i_deb++)
  cout << (*relStations_type)[i_deb] << " ";
  cout << endl;
#endif

  // Check that the receivers are relevant according to their position
  vector<stationID_t>* relStations_area = getReceiversInRelevantArea(*relStations_type);
#ifdef _DEBUG_MESSAGE_STORAGE
  cout << "                           Relevant Receivers by area: ";
  for (unsigned int i_deb = 0; i_deb < relStations_area->size(); i_deb++)
  cout << (*relStations_area)[i_deb] << " ";
  cout << endl;
#endif

  // Check that the receivers are relevant according to their direction
  vector<stationID_t>* relStations_dir = getReceiversOnRelevantDirection(*relStations_area);
#ifdef _DEBUG_MESSAGE_STORAGE
  cout << "                           Relevant Receivers by dir : ";
  for (unsigned int i_deb = 0; i_deb < relStations_dir->size(); i_deb++)
  cout << (*relStations_dir)[i_deb] << " ";
  cout << endl;
#endif

  vector<stationID_t>* relStations = NULL;
  messageType_t msgType = recMsg->getPayload()->getMessageType();
  if (msgType == GEOBROADCAST || msgType == GEOUNICAST || msgType == GEOANYCAST || msgType == DENM)
    relStations = getReceiversInGeocastArea(*relStations_dir, recMsg->getPayload());
#ifdef _DEBUG_MESSAGE_STORAGE
  cout << "                           Relevant Receivers by geocast area : ";
  for (unsigned int i_deb = 0; i_deb < relStations->size(); i_deb++)
  cout << (*relStations)[i_deb] << " ";
  cout << endl;
#endif
  else
  {
    relStations = new vector<stationID_t>();
    relStations->assign(relStations_dir->begin(), relStations_dir->end());
  }
  delete (relStations_type);
  delete (relStations_area);
  delete (relStations_dir);

  // If there are new relevant receivers
  if (relStations->size() > 0)
  {

    // Schedule the message removal time
    icstime_t expiryMessageTime = 0;
    switch (recMsg->getMessageType())
    {
    case (DENM):
    {
      FacilityMessagePayload* facMes = (FacilityMessagePayload*) recMsg->getPayload();
      DENMPayload *denm = dynamic_cast<DENMPayload*>(facMes);
      expiryMessageTime = denm->getExpiryTime();
      break;
    }
    default:
    {
      expiryMessageTime = simTime + defaultMessageLifeInterval;
      break;
    }
    }
    scheduleMessageRemoval(actionID, expiryMessageTime);

    // Create the receivers list (receiverID and receptionTime) to append to the message
    vector<Receiver> relevantReceivers;
    for (vector<stationID_t>::iterator i = relStations->begin(); i != relStations->end(); i++)
    {
      Station* recStation = (Station*) staFac->getStation(*i);
      if (recStation != NULL)
      {
        // Link the relevant receiver with the reception time and position
        Receiver receiver;
        receiver.receiverID = *i;
        receiver.receptionTime = simTime;
        receiver.position.set(recStation->getPosition());
        // Add the receiver to the receivers' vector
        relevantReceivers.push_back(receiver);
      }
    }
    // If the message is already in the iFMT
    if (entry_found_in_iFMT)
    {
      // Add the receiver to the receivers' vector
      recMsg->addReceivers(relevantReceivers);
    }

    // If the payload is still in the iFPT
    if (entry_found_in_iFPT)
    {
      // Create a new entry in the iFMT for the message
      recMsg->setReceivers(relevantReceivers);
      iFMT.insert(pair<actionID_t, ReceivedMessage*>(actionID, recMsg));
      // Delete the entry from the iFPT without deleting the payload itself that has been now stored in the iFMT
      iFPT.erase(it_iFPT);
    }

#ifdef LOG_ON
    stringstream log;
    log << "[facilities] - storeMessage: [actionID|MesgType|Receivers list] ";
    log << "[" << actionID;
    log << "|" << toString(recMsg->getMessageType());
    log << "| ";
    for (vector<Receiver>::iterator log_i = relevantReceivers.begin(); log_i != relevantReceivers.end(); log_i++)
    {
      log << log_i->receiverID << " ";
    }
    log << "]";
    ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);
#endif

    delete (relStations);
    return true;

  }
  // Else, the new receivers are not relevant (return false)
  delete (relStations);
  return false;
}

/* ***************************************************************************
 * ***************************************************************************
 * ***************************************************************************
 * **********                                                       **********
 * **********             PRIVATE METHODS                           **********
 * **********                                                       **********
 * ***************************************************************************
 * ***************************************************************************
 ***************************************************************************/

// ===============================================================
// ====================== Table cleanup     ======================
// ===============================================================
void LDMLogic::scheduleMessageRemoval(actionID_t actionID, icstime_t deletionTime)
{
  map<actionID_t, icstime_t>::iterator itEventList;
  map<icstime_t, vector<actionID_t> >::iterator itDeletionList;

  // Search if the message has been already scheduled for deletion from the iFMT
  itEventList = eventList.find(actionID);

  // If the message was already scheduled, erase it
  if (itEventList != eventList.end())
  {
    itDeletionList = deletionList.find(itEventList->second);
    if (itDeletionList == deletionList.end())
    {
      cerr << "[LDMLogic::scheduleMessageRemoval] ERROR: The deletionList and the eventList are not synchronized!"
          << endl;
      abort();
    }
    for (vector<actionID_t>::iterator it = itDeletionList->second.begin(); it < itDeletionList->second.end(); it++)
    {
      if (*it == actionID)
        itDeletionList->second.erase(it);
      if (itDeletionList->second.size() == 0)
      {
        deletionList.erase(itDeletionList);
      }
    }
    eventList.erase(itEventList);
  }

  eventList.insert(pair<actionID_t, icstime_t>(actionID, deletionTime));
  itDeletionList = deletionList.find(deletionTime);
  if (itDeletionList != deletionList.end())
  {
    itDeletionList->second.push_back(actionID);
  }
  else
  {
    vector<actionID_t> actions;
    actions.push_back(actionID);
    deletionList.insert(pair<icstime_t, vector<actionID_t> >(deletionTime, actions));
  }
}

int LDMLogic::iFPTCleanup(icstime_t deletionTime)
{
	int number = 0;
	const int refTime = deletionTime - defaultMessageLifeInterval;
	for (map<actionID_t, ReceivedMessage*>::iterator it = iFPT.begin(); it != iFPT.end();)
	{
		int time = it->second->getPayload()->getTimeStamp();
		//Use greater so they will stay one step longer
		if (refTime > time)
		{

#ifdef _DEBUG_MESSAGE_STORAGE
{
            ostringstream log;
            log << "removed " << it->first << " time " << time << " refTime " << refTime;
            ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);
}
#endif
			++number;
			const ApplicationMessagePayload* payload =
					dynamic_cast<const ApplicationMessagePayload*>(it->second->getPayload());
			if (payload != NULL)
				payload->getDestination().DeleteAreas();
			delete it->second->getPayload();
			delete it->second;
			iFPT.erase(it++);
		} else
			++it;
	}
	return number;
}

int LDMLogic::launchIFMTCleanupManager(icstime_t deletionTime)
{
  map<icstime_t, vector<actionID_t> >::iterator itDeletionList;
  itDeletionList = deletionList.find(deletionTime);
  vector<actionID_t>::iterator it;

#ifdef LOG_ON
  stringstream log, log2;
  log << "[facilities] - launchIFMTCleanupManager: total number of stored messages iFMT: " << iFMT.size();
  ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);
  log2 << "[facilities] messages to be deleted this time: ";
#endif
  int number = 0;
  if (itDeletionList != deletionList.end())
  {
    for (it = itDeletionList->second.begin(); it < itDeletionList->second.end(); it++)
    {
#ifdef LOG_ON
      log2 << *it << ",";
#endif
      ++number;
      // Remove message from the iFMT table
      deleteMessage(*it);
      eventList.erase(*it);
    }
    deletionList.erase(deletionTime);
#ifdef LOG_ON
    ics::IcsLog::LogLevel((log2.str()).c_str(), ics::kLogLevelInfo);
#endif
  }
  return number;
}

bool LDMLogic::deleteMessage(actionID_t actionID)
{
  map<actionID_t, ReceivedMessage*>::iterator it = iFMT.find(actionID);
  if (it == iFMT.end())
    return false;
	const ApplicationMessagePayload* payload = dynamic_cast<const ApplicationMessagePayload*>(it->second->getPayload());
	if (payload != NULL)
		payload->getDestination().DeleteAreas();
	delete it->second->getPayload();
	delete it->second;
	iFMT.erase(it);
  return true;
}

// ===============================================================
// ====================== Table Maintenance ======================
// ===============================================================

actionID_t LDMLogic::createActionID(stationID_t stationID, seqNo_t seqNo)
{
  std::stringstream actionIDstream;
  actionIDstream << setfill('0');
  actionIDstream << setw(ceil(2.41 * sizeof(stationID_t))) << right << stationID << setw(ceil(2.41 * sizeof(seqNo_t)))
      << right << seqNo;
  return actionIDstream.str();
}

stationID_t LDMLogic::getStationIDfromActionID(actionID_t actionID)
{
  std::string stationIDString = actionID.substr(0, ceil(2.41 * sizeof(stationID_t)));
  return (stationID_t) TplConvert::_2int(stationIDString.c_str());
}

seqNo_t LDMLogic::getSequenceNumberfromActionID(actionID_t actionID)
{
  std::string seqNoString = actionID.substr(ceil(2.41 * sizeof(stationID_t)), ceil(2.41 * sizeof(seqNo_t)));
  return (seqNo_t) TplConvert::_2int(seqNoString.c_str());
}

seqNo_t LDMLogic::getNewSeqNo(stationID_t stationID)
{
  map<stationID_t, seqNo_t>::iterator it;
  it = stationsSeqNoRecord.find(stationID);
  if (it == stationsSeqNoRecord.end())
  {  // If the station never transmitted before, start with the sequence number zero
    stationsSeqNoRecord.insert(pair<stationID_t, seqNo_t>(stationID, 0));
    return 0;
  }
  else
  {                                 // Increase the message sequence number for this station
    it->second = (it->second + 1) % USHRT_MAX;
    return it->second;
  }
}

bool LDMLogic::storePayload(actionID_t actionID, FacilityMessagePayload* payload, messageType_t type)
{
  // Create the ReceivedMessage
  ReceivedMessage* message = new ReceivedMessage();
  message->setActionID(actionID);
  message->setPayload(payload);
  message->setMessageType(type);
#ifdef LOG_ON
  stringstream log;
  log << "LDMLogic::storePayload: " << actionID;
  ics::IcsLog::LogLevel((log.str()).c_str(), ics::kLogLevelInfo);
#endif
  pair<map<actionID_t, ReceivedMessage*>::iterator, bool> check;
  check = iFPT.insert(pair<actionID_t, ReceivedMessage*>(actionID, message));
  if (!check.second)
    delete (message);
  return check.second;
}

// ===============================================================
// ====================== Relevance Check Rules ==================
// ===============================================================

vector<stationID_t>* LDMLogic::getReceiversInRelevantArea(vector<stationID_t>& receivers)
{
  vector<stationID_t>* relevantReceivers = new vector<stationID_t>();
  if (relevantArea.size() == 0)
  {
    for (unsigned int i = 0; i < receivers.size(); i++)
    {
      relevantReceivers->push_back(receivers[i]);
    }
    return relevantReceivers;
  }

  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    Station *currStation = (Station*) staFac->getStation(receivers[i]);
    for (unsigned int i = 0; i < relevantArea.size(); i++)
    {
      Area2DType currArea2DType = relevantArea[i]->getArea2DType();

      switch (currArea2DType)
      {
      case ROADELEMENT:
      {
        RoadElement *relAreaItem = dynamic_cast<RoadElement*>(relevantArea[i]);
        if (staFac->isStationInArea(currStation->getID(), *relAreaItem))
          relevantReceivers->push_back(receivers[i]);
        break;
      }
      case GEOMETRICSHAPE:
      {
        GeometricShape *relAreaItem = dynamic_cast<GeometricShape*>(relevantArea[i]);
        if (staFac->isStationInArea(currStation->getID(), *relAreaItem))
          relevantReceivers->push_back(receivers[i]);
        break;
      }
      default:
      {
        break;
      }
      }
    }
  }
  return relevantReceivers;
}

vector<stationID_t>* LDMLogic::getReceiversRelevantStations(vector<stationID_t>& receivers)
{
  vector<stationID_t>* relevantReceivers = new vector<stationID_t>();
  if (relevantStationTypes.size() == 0)
  {
    for (unsigned int i = 0; i < receivers.size(); i++)
    {
      relevantReceivers->push_back(receivers[i]);
    }
    return relevantReceivers;
  }

  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    Station *currStation = (Station*) staFac->getStation(receivers[i]);
    map<icsstationtype_t, bool>::iterator itType = relevantStationTypes.find(currStation->getType());
    if ((itType != relevantStationTypes.end()) && (itType->second))
      relevantReceivers->push_back(receivers[i]);
  }
  return relevantReceivers;
}

vector<stationID_t>* LDMLogic::getReceiversOnRelevantDirection(vector<stationID_t>& receivers)
{
  vector<stationID_t>* relevantReceivers = new vector<stationID_t>();
  if (!isRelevantDirectionDefined)
  {
    for (unsigned int i = 0; i < receivers.size(); i++)
    {
      relevantReceivers->push_back(receivers[i]);
    }
    return relevantReceivers;
  }
  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    Station *currStation = (Station*) staFac->getStation(receivers[i]);
    // Stations are not affected by this search. Also stations that do not have the direction defined are not analyzed.
    if ((currStation->getType() == STATION_FIXED) || (((MobileStation*) currStation)->getDirection() != NAN))
    relevantReceivers->push_back(receivers[i]);
    // Only MOBILE stations that are moving in the relevant direction (with defined accuracy) are considered
    if ((currStation->getType() == STATION_MOBILE)
        && (((MobileStation*) currStation)->getDirection() <= relevantDirection + relevantDirectionAccuracy)
        && (((MobileStation*) currStation)->getDirection() >= relevantDirection - relevantDirectionAccuracy))
      relevantReceivers->push_back(receivers[i]);
  }
  return relevantReceivers;
}

vector<stationID_t>* LDMLogic::getReceiversInGeocastArea(vector<stationID_t> receivers,
    const FacilityMessagePayload* msg)
{
  vector<stationID_t>* relevantReceivers = new vector<stationID_t>();

  std::vector<Area2D*> geocastArea;
  messageType_t type = msg->getMessageType();
  if (type == DENM)
  {
    DENMPayload* denm = dynamic_cast<DENMPayload*>((FacilityMessagePayload*) msg);
//    Will not work because tmpArea is deleted once the if is closed.
//    std::vector<Area2D*> tmpArea = denm->getRelevanceArea();
//    geocastArea = &tmpArea;
    geocastArea = denm->getRelevanceArea();
  }
  else if (type == GEOBROADCAST || type == GEOUNICAST || type == GEOANYCAST)
  {
    ApplicationMessagePayload* appMsg = dynamic_cast<ApplicationMessagePayload*>((FacilityMessagePayload*) msg);
    TApplicationMessageDestination dest = appMsg->getDestination();
//		Will not work because dest is deleted once the if is closed.
//		geocastArea = &(dest.dest_areas);
    geocastArea = dest.dest_areas;
  }
  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    stationID_t currStationID = receivers.at(i);
    Point2D currPos = staFac->getStation(currStationID)->getPosition();
    for (unsigned int j = 0; j < geocastArea.size(); j++)
    {
      bool found = false;
      Area2D* currArea = geocastArea.at(j);
      Area2DType areaType = currArea->getArea2DType();
      if (areaType == GEOMETRICSHAPE)
      {
        GeometricShape* shape = dynamic_cast<GeometricShape*>(currArea);
        found = shape->isInternal(currPos);
      }
      else if (areaType == ROADELEMENT)
      {
        RoadElement* element = dynamic_cast<RoadElement*>(currArea);
        Lane* closest = mapFac->convertPoint2Map(currPos);
        roadElementID_t closestID = closest->getID();
        if (element->getRoadElementType() == LANE)
        {
          if (element->getID() == closestID)
            found = true;
        }
        else if (element->getRoadElementType() == EDGE)
        {
          Edge* edge = dynamic_cast<Edge*>(element);
          found = edge->containsLane(closestID);
        }
        else if (element->getRoadElementType() == JUNCTION)
        {
          Junction* junction = dynamic_cast<Junction*>(element);
          found = junction->containsLane(closestID);
        }
      }
      if (found)
      {
        relevantReceivers->push_back(currStationID);
        break;
      }
    }
  }
  return relevantReceivers;
}

bool LDMLogic::isMessageRelevant(const ReceivedMessage& message)
{
  if (relevantMessages.size() == 0)
    return true;

#ifdef _DEBUG_LDM
  map<messageType_t, bool>::iterator it;
  for (it = relevantMessages.begin(); it != relevantMessages.end(); it++)
  cout << "[facilities] LDMLogic::isMessageRelevant() - relevantMessages: " << it->first << " relevant? " << it->second << endl;
#endif

  map<messageType_t, bool>::iterator itType = relevantMessages.find(message.getMessageType());
  if ((itType != relevantMessages.end()) && (itType->second))
    return true;
  return false;
}

vector<ReceivedMessage*> LDMLogic::getReceivedMessagesFromStation(stationID_t receiverID)
{
  vector<ReceivedMessage*> interestingReceivedMessages;
  for (map<actionID_t, ReceivedMessage*>::iterator it = iFMT.begin(); it != iFMT.end(); it++)
  {
    if (it->second->isStationAmongReceivers(receiverID))
      interestingReceivedMessages.push_back(it->second);
  }
  return interestingReceivedMessages;
}

vector<ReceivedMessage*> LDMLogic::getReceivedTypeMessagesFromStation(stationID_t receiverID, messageType_t type)
{
  vector<ReceivedMessage*> interestingReceivedMessages;
  for (map<actionID_t, ReceivedMessage*>::iterator it = iFMT.begin(); it != iFMT.end(); it++)
  {
    if ((it->second->getMessageType() == type) && (it->second->isStationAmongReceivers(receiverID)))
      interestingReceivedMessages.push_back(it->second);
  }
  return interestingReceivedMessages;
}

icstime_t LDMLogic::getMessageReceptionTimeByReceiver(stationID_t receiverID, ReceivedMessage* message)
{
  vector<Receiver> receivers = message->getReceiversList();
  vector<Receiver>::iterator it = receivers.begin();
  for (vector<Receiver>::iterator it = receivers.begin(); it != receivers.end(); it++)
  {
    if (it->receiverID == receiverID)
      return it->receptionTime;
  }
  return -2;
}

// ===============================================================
// ====================== Internal          ======================
// ===============================================================

bool convertPoint_str2Point2D(ics_parsing::point_str p, Point2D *point2D, latitude_t lat0, longitude_t lon0,
altitude_t alt0)
{
	if ((p.m_X != NAN) && (p.m_Y != NAN))
	{
		point2D->set(p.m_X, p.m_Y);
	}
	else
	{
		if ((p.m_Lat != NAN) && (p.m_Lon != NAN))
		{
			vector <double> localCoordinates = GeoToGeoCentrConvert(p.m_Lat, p.m_Lon, 0);
			point2D->set(localCoordinates[0], localCoordinates[1]);
		}
		else
		{
			cerr <<"[facilities] Error: the point is not correctly defined. It should be defined either through its (x,y) or (lat,long) coordinates." << endl;
			return false;
		}
	}
	return true;
}

}

