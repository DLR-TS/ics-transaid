/****************************************************************************/
/// @file    FacilitiesManager.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
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

#include "FacilitiesManager.h"
#include "./configfile_parsers/facilities-configfile-parser.h"
#include "../utils/common/FileHelpers.h"

#ifdef LOG_ON
#include "../utils/ics/log/ics-log.h"
#include <sstream>
#endif

namespace ics
{

/* ***************************************************************************
 * ***************************************************************************
 * ***************************************************************************
 * **********                                                       **********
 * **********             PUBLIC METHODS                            **********
 * **********                                                       **********
 * ***************************************************************************
 * ***************************************************************************
 ***************************************************************************/

FacilitiesManager::FacilitiesManager()
{
  facilities = NULL;
}

FacilitiesManager::~FacilitiesManager()
{
  if (facilities != NULL)
    delete (facilities);
}

bool FacilitiesManager::configureICSFacilities(string facilitiesConfigFilename)
{
  bool configFlag = true;

  // Check the existence of the facilitiesConfigFilename
  if (!FileHelpers::exists(facilitiesConfigFilename))
  {
    cerr << "[iCS - Facilities] - ERROR: The iCS Facilities configuration file '" << facilitiesConfigFilename
        << "' was not found." << endl;
    return false;
  }
  // parse the facilities config file
  ics_parsing::FacilitiesGetConfig facConfig;
  facConfig.readConfigFile(facilitiesConfigFilename);

  float originLatitude = facConfig.getLocalLatitude();
  float originLongitude = facConfig.getLocalLongitude();
  float originAltitude = facConfig.getLocalAltitude();

  // Check the existence of the mapConFileName
  string mapConFileName = facConfig.getMapConfigFilename();
  if (!FileHelpers::exists(mapConFileName))
  {
    cerr << "[iCS - Facilities] - ERROR: The map configuration file '" << mapConFileName
        << "' of the iCS Facilities was not found." << endl;
    return false;
  }
  // Check the existence of the stationConfFileName
  string stationConfFileName = facConfig.getStationsConfigFilename();
  if (!FileHelpers::exists(stationConfFileName))
  {
    cerr << "[iCS - Facilities] - ERROR: The stations configuration file '" << stationConfFileName
        << "' of the iCS Facilities was not found." << endl;
    return false;
  }
  // Check the existence of the relevConfFilename
  string relevConfFilename = facConfig.getLDMrulesConfigFilename();
  if (!FileHelpers::exists(relevConfFilename))
  {
    cerr << "[iCS - Facilities] - ERROR: The received message storage rules configuration file '" << relevConfFilename
        << "' of the iCS Facilities was not found." << endl;
    return false;
  }

  // allocate the ics facilities
  if (facilities == NULL)
    facilities = new ICSFacilities();
  else
  {
    cerr << "[iCS - Facilities] This is embarrassing: facilities already allocated!" << endl;
    return false;
  }

  configFlag &= facilities->configureMap(mapConFileName);
  if (originAltitude != -10000)
    facilities->configureLocalCoordinates(originLatitude, originLongitude, originAltitude);
  else
    facilities->configureLocalCoordinates(originLatitude, originLongitude, 0);
#ifdef LOG_ON
  {
    std::stringstream log;
    if (configFlag)
      log << "[iCS - Facilities] - map loaded from " << mapConFileName;
    else
      log << "[iCS - Facilities] - map NOT loaded from " << mapConFileName;
    ics::IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
  }
#endif
#ifdef _DEBUG_MAP
  cout << "[iCS - Facilities] DEB: FacilityManager: map loaded from " << mapConFileName << endl;
#endif

  configFlag &= facilities->configureStations(stationConfFileName);
#ifdef LOG_ON
  {
    std::stringstream log;
    if (configFlag)
      log << "[iCS - Facilities] - stations settings loaded from " << stationConfFileName;
    else
      log << "[iCS - Facilities] - stations settings NOT loaded from " << stationConfFileName;
    ics::IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
  }
#endif
#ifdef _DEBUG_STATIONS
  cout << "[iCS - Facilities] DEB: FacilityManager: stations settings loaded from " << stationConfFileName << endl;
#endif

  configFlag &= facilities->configureRelevanceRules(relevConfFilename);
#ifdef LOG_ON
  {
    std::stringstream log;
    if (configFlag)
      log << "[iCS - Facilities] - LDM settings loaded from " << relevConfFilename;
    else
      log << "[iCS - Facilities] - LDM settings NOT loaded from " << relevConfFilename;
    ics::IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
  }
#endif
#ifdef _DEBUG_LDM
  cout << "[iCS - Facilities] DEB: FacilityManager: LDM settings loaded from " << relevConfFilename << endl;
#endif

#ifdef LOG_ON
  {
    std::stringstream log;
    if (configFlag)
      log << "[iCS - Facilities] - iCS Facilities configuration completed.";
    else
      log << "[iCS - Facilities] - iCS Facilities configuration NOT completed.";
    ics::IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
  }
#endif

  return configFlag;
}

// ===============================================================
// ====================== Map related       ======================
// ===============================================================

roadElementID_t FacilitiesManager::getClosestJunctionID(Point2D& pos)
{
  const Junction* junction = getClosestJunction(pos);
  return junction->getID();
}

const vector<roadElementID_t> FacilitiesManager::getNeighboringJunctions(roadElementID_t junctionID) const
{
  if (facilities != NULL)
    return facilities->getNeighboringJunctions(junctionID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<roadElementID_t>* FacilitiesManager::getEdgeIDsFromJunction(roadElementID_t junctionID_A,
    roadElementID_t junctionID_B)
{
  vector<const Edge*>* vEdges = getEdgesFromJunction(junctionID_A, junctionID_B);
  vector<roadElementID_t>* vEdgeIDs = new vector<roadElementID_t>();
  vector<const Edge*>::iterator it;
  for (it = vEdges->begin(); it != vEdges->end(); it++)
    vEdgeIDs->push_back((*it)->getID());
  delete vEdges;
  return vEdgeIDs;
}

vector<roadElementID_t>* FacilitiesManager::getLanesIDsFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID)
{
  vector<const Lane*>* vLanes = getLanesFromJunctions(junctAID, junctBID);
  vector<roadElementID_t>* vLaneIDs = new vector<roadElementID_t>();
  vector<const Lane*>::iterator it;
  for (it = vLanes->begin(); it != vLanes->end(); it++)
    vLaneIDs->push_back((*it)->getID());
  delete vLanes;
  return vLaneIDs;
}

vector<roadElementID_t>* FacilitiesManager::getJunctionsIDsFromEdge(roadElementID_t edgeID)
{
  vector<const Junction*>* vJunctions = facilities->getJunctionsFromEdge(edgeID);
  vector<roadElementID_t>* vJunctionsIDs = new vector<roadElementID_t>();
  vector<const Junction*>::iterator it;
  for (it = vJunctions->begin(); it != vJunctions->end(); it++)
    vJunctionsIDs->push_back((*it)->getID());
  delete vJunctions;
  return vJunctionsIDs;
}

roadElementID_t FacilitiesManager::getEdgeIDFromLane(roadElementID_t laneID)
{
  const Edge* edge = getEdgeFromLane(laneID);
  if (edge != NULL)
    return edge->getID();
  return "";
}

roadElementID_t FacilitiesManager::getJunctionIDFromLane(roadElementID_t laneID)
{
  const Junction* junction = getJunctionFromLane(laneID);
  if (junction != NULL)
    return junction->getID();
  return "";
}

void FacilitiesManager::setLaneStatus(roadElementID_t laneID, laneStatus newStatus)
{
  if (facilities != NULL)
  {
    facilities->setLaneStatus(laneID, newStatus);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight)
{
  if (facilities != NULL)
  {
    facilities->setLaneWeight(laneID, newWeight);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

latitude_t FacilitiesManager::getLat0()
{
  if (facilities != NULL)
    return facilities->getLat0();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

longitude_t FacilitiesManager::getLon0()
{
  if (facilities != NULL)
    return facilities->getLon0();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

altitude_t FacilitiesManager::getAlt0()
{
  if (facilities != NULL)
    return facilities->getAlt0();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::updateTrafficLightDynamicInformation(const trafficLightID_t &tlID, std::string& state)
{
  if (facilities != NULL)
  {
    facilities->updateTrafficLightDynamicInformation(tlID, state);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

laneStatus FacilitiesManager::getLaneStatus(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getStatus();
}

laneWeight_t FacilitiesManager::getLaneWeight(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getWeight();
}

maxSpeed_t FacilitiesManager::getLaneMaxSpeed(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getMaxSpeed();
}

laneLength_t FacilitiesManager::getLaneLength(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getLength();
}

const vector<Point2D>& FacilitiesManager::getLaneShape(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getShape();
}

vector<roadElementID_t>* FacilitiesManager::getLanePrevLanesIDs(roadElementID_t laneID)
{
  const Lane* lane = getLane(laneID);
  const vector<Lane*> prevLanes = lane->getPrevLanes();
  vector<roadElementID_t> *prevLanesIDs = new vector<roadElementID_t>();
  for (unsigned int i = 0; i < prevLanes.size(); i++)
    prevLanesIDs->push_back(prevLanes[i]->getID());
  return prevLanesIDs;
}

vector<roadElementID_t>* FacilitiesManager::getLaneNextLanesIDs(roadElementID_t laneID)
{
  const Lane* lane = getLane(laneID);
  const vector<Lane*> nextLanes = lane->getNextLanes();
  vector<roadElementID_t> *nextLanesIDs = new vector<roadElementID_t>();
  for (unsigned int i = 0; i < nextLanes.size(); i++)
    nextLanesIDs->push_back(nextLanes[i]->getID());
  return nextLanesIDs;
}

roadElementID_t FacilitiesManager::getEdgeIDFromLane(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getEdgeID();
}

roadElementID_t FacilitiesManager::getJunctionIDFromLane(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getJunctionID();
}

trafficLightID_t FacilitiesManager::getIDTrafficLightControllingLane(roadElementID_t laneID) const
{
  const Lane* lane = getLane(laneID);
  return lane->getTrafficLightID();
}

const vector<roadElementID_t>& FacilitiesManager::getLaneIDsFromEdge(roadElementID_t edgeID) const
{
  const Edge* edge = getEdge(edgeID);
  return edge->getLaneIDs();
}

const Point2D& FacilitiesManager::getJunctionCenter(roadElementID_t junctionID) const
{
  const Junction* junction = getJunction(junctionID);
  return junction->getCenter();
}

const vector<roadElementID_t>& FacilitiesManager::getJunctionIncomingLaneIDs(roadElementID_t junctionID) const
{
  const Junction* junction = getJunction(junctionID);
  return junction->getIncomingLaneIDs();
}

const vector<roadElementID_t>& FacilitiesManager::getJunctionInternalLaneIDs(roadElementID_t junctionID) const
{
  const Junction* junction = getJunction(junctionID);
  return junction->getInternalLaneIDs();
}

roadElementID_t FacilitiesManager::convertPoint2LaneID(Point2D& pos) const
{
  const Lane* lane = convertPoint2Map(pos);
  return lane->getID();
}

// ===============================================================
// ====================== Stations related  ======================
// ===============================================================

void FacilitiesManager::updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info)
{
  if (facilities != NULL)
  {
    facilities->updateMobileStationDynamicInformation(stationId, info);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

bool FacilitiesManager::enableRAT(stationID_t stationID, RATID RATtoBeEnabled)
{
  if (facilities != NULL)
    return facilities->enableRAT(stationID, RATtoBeEnabled);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

bool FacilitiesManager::disableRAT(stationID_t stationID, RATID RATtoBeDisabled)
{
  if (facilities != NULL)
    return facilities->disableRAT(stationID, RATtoBeDisabled);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

bool FacilitiesManager::enableRATAllStations(RATID RATtoBeEnabled)
{
  if (facilities != NULL)
    return facilities->enableRATAllStations(RATtoBeEnabled);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

bool FacilitiesManager::disableRATAllStations(RATID RATtoBeDisabled)
{
  if (facilities != NULL)
    return facilities->disableRATAllStations(RATtoBeDisabled);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<stationID_t>* FacilitiesManager::getStationsIDsInArea(GeometricShape &area)
{
  map<stationID_t, const Station*>* stations = getStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const Station*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getStationsIDsInArea(vector<RoadElement*> &area)
{
  map<stationID_t, const Station*>* stations = getStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const Station*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getMobileStationsIDsInArea(GeometricShape &area)
{
  map<stationID_t, const MobileStation*>* stations = getMobileStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const MobileStation*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}


vector<stationID_t>* FacilitiesManager::getMobileStationsIDsInArea(vector<RoadElement*> &area)
{
  map<stationID_t, const MobileStation*>* stations = getMobileStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const MobileStation*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getFixedStationsIDsInArea(GeometricShape &area)
{
  map<stationID_t, const FixedStation*>* stations = getFixedStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getFixedStationsIDsInArea(vector<RoadElement*> &area)
{
  map<stationID_t, const FixedStation*>* stations = getFixedStationsInArea(area);
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = stations->begin(); it != stations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(it->first);
  }
  delete stations;
  return stationsIDs;
}

const icsstationtype_t FacilitiesManager::getStationType(stationID_t stationID) const
{
  Station* sta = (Station*) getStation(stationID);
  return sta->getType();
}

const Point2D& FacilitiesManager::getStationPosition(stationID_t stationID) const
{
  const Station* sta = getStation(stationID);
  return sta->getPosition();
}

float FacilitiesManager::getStationPositionX(stationID_t stationID) const
{
  Point2D p = getStationPosition(stationID);
  return p.x();
}

float FacilitiesManager::getStationPositionY(stationID_t stationID) const
{
  Point2D p = getStationPosition(stationID);
  return p.y();
}

vector<RATID>* FacilitiesManager::getStationRATs(stationID_t stationID)
{
  Station* sta = (Station*) getStation(stationID);
  return sta->getRATs();
}

vector<RATID>* FacilitiesManager::getStationActiveRATs(stationID_t stationID)
{
  Station* sta = (Station*) getStation(stationID);
  return sta->getActiveRATs();
}

bool FacilitiesManager::isRATdefinedInStation(stationID_t stationID, RATID technology)
{
  vector<RATID>* rats = getStationRATs(stationID);
  vector<RATID>::const_iterator it;
  bool flag_defined = false;
  for (it = rats->begin(); it != rats->end(); it++)
  {
    if (*it == technology)
    {
      flag_defined = true;
      break;
    }
  }
  delete rats;
  return flag_defined;
}

bool FacilitiesManager::isRATactiveInStation(stationID_t stationID, RATID technology)
{
  vector<RATID>* rats = getStationActiveRATs(stationID);
  vector<RATID>::const_iterator it;
  bool flag_active = false;
  for (it = rats->begin(); it != rats->end(); it++)
  {
    if (*it == technology)
    {
      flag_active = true;
      break;
    }
  }
  delete rats;
  return flag_active;
}

speed_t FacilitiesManager::getMobileStationSpeed(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getSpeed();
}

acceleration_t FacilitiesManager::getMobileStationAcceleration(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getAcceleration();
}

direction_t FacilitiesManager::getMobileStationDirection(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getDirection();
}

stationSize_t FacilitiesManager::getMobileStationVehicleLegth(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getVehicleLegth();
}

stationSize_t FacilitiesManager::getMobileStationVehicleWidth(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getVehicleWidth();
}

stationSize_t FacilitiesManager::getMobileStationVehicleHeight(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getVehicleHeight();
}

bool FacilitiesManager::getMobileStationExteriorLights(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getExteriorLights();
}

roadElementID_t FacilitiesManager::getMobileStationLaneID(stationID_t stationID)
{
  MobileStation* sta = (MobileStation*) getMobileStation(stationID);
  return sta->getLaneID();
}

string FacilitiesManager::getFixedStationCommunicationProfile(stationID_t stationID)
{
  FixedStation* sta = (FixedStation*) getFixedStation(stationID);
  return sta->getCommunicationProfile();
}

const map<RATID, float>& FacilitiesManager::getDefaultPenetrationRates() const
{
  if (facilities != NULL)
    return facilities->getDefaultPenetrationRates();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const map<RATID, string>& FacilitiesManager::getDefaultCommunicationProfiles() const
{
  if (facilities != NULL)
    return facilities->getDefaultCommunicationProfiles();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, Point2D>* FacilitiesManager::getAllFixedStationsPositions()
{
  map<stationID_t, const FixedStation*>* allFixedStations = getAllFixedStations();
  map<stationID_t, Point2D>* positions = new map<stationID_t, Point2D>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = allFixedStations->begin();
      it != allFixedStations->end(); it++)
  {
    if (it->second != NULL)
      positions->insert(pair<stationID_t, Point2D>(it->first, it->second->getPosition()));
  }
  return positions;
}

map<stationID_t, RATID>* FacilitiesManager::getAllFixedStationsRAT()
{
  map<stationID_t, const FixedStation*>* allFixedStations = getAllFixedStations();
  map<stationID_t, RATID>* rats = new map<stationID_t, RATID>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = allFixedStations->begin();
      it != allFixedStations->end(); it++)
  {
    vector<RATID> *currRATs = ((FixedStation*) it->second)->getRATs();
    if (currRATs->size() > 0)
    {
      vector<RATID>::const_iterator itRat = currRATs->begin();
      rats->insert(pair<stationID_t, RATID>(it->first, *itRat));
    }
    else
    {
      cerr << "ERROR: [iCS - Facilities] - getAllFixedStationsRAT() - station " << it->first
          << " does not have any RAT set." << endl;
    }
    delete currRATs;
  }
  return rats;
}

map<stationID_t, RATID>* FacilitiesManager::getAllFixedStationsActiveRAT()
{
  map<stationID_t, const FixedStation*>* allFixedStations = getAllFixedStations();
  map<stationID_t, RATID>* rats = new map<stationID_t, RATID>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = allFixedStations->begin();
      it != allFixedStations->end(); it++)
  {
    vector<RATID> *currRATs = ((FixedStation*) it->second)->getActiveRATs();
    if (currRATs->size() > 0)
    {
      vector<RATID>::const_iterator itRat = currRATs->begin();
      rats->insert(pair<stationID_t, RATID>(it->first, *itRat));
    }
    delete currRATs;
  }
  return rats;
}

map<stationID_t, Point2D>* FacilitiesManager::getAllMobileStationsPositions()
{
  map<stationID_t, const MobileStation*>* allMobileStations = getAllMobileStations();
  map<stationID_t, Point2D>* positions = new map<stationID_t, Point2D>();
  for (map<stationID_t, const MobileStation*>::const_iterator it = allMobileStations->begin();
      it != allMobileStations->end(); it++)
    if (it->second != NULL)
      positions->insert(pair<stationID_t, Point2D>(it->first, it->second->getPosition()));
  return positions;
}

map<stationID_t, vector<RATID> >* FacilitiesManager::getAllMobileStationsRATs()
{
  map<stationID_t, const MobileStation*>* allMobileStations = getAllMobileStations();
  map<stationID_t, vector<RATID> >* rats = new map<stationID_t, vector<RATID> >();
  for (map<stationID_t, const MobileStation*>::const_iterator it = allMobileStations->begin();
      it != allMobileStations->end(); it++)
  {
    vector<RATID> *currRATs = ((MobileStation*) it->second)->getRATs();
    rats->insert(pair<stationID_t, vector<RATID> >(it->first, *currRATs));
    delete currRATs;
  }
  return rats;
}

map<stationID_t, vector<RATID> >* FacilitiesManager::getAllMobileStationsActiveRATs()
{
  map<stationID_t, const MobileStation*>* allMobileStations = getAllMobileStations();
  map<stationID_t, vector<RATID> >* rats = new map<stationID_t, vector<RATID> >();
  for (map<stationID_t, const MobileStation*>::const_iterator it = allMobileStations->begin();
      it != allMobileStations->end(); it++)
  {
    vector<RATID> *currActiveRATs = ((MobileStation*) it->second)->getActiveRATs();
    rats->insert(pair<stationID_t, vector<RATID> >(it->first, *currActiveRATs));
    delete currActiveRATs;
  }
  return rats;
}

vector<stationID_t>* FacilitiesManager::getAllStationsIDs()
{
  const map<stationID_t, Station*> allStations = getAllStations();
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, Station*>::const_iterator it = allStations.begin(); it != allStations.end(); it++)
    if (it->second != NULL)
      stationsIDs->push_back(it->second->getID());
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getAllMobileStationsIDs()
{
  map<stationID_t, const MobileStation*>* allMobileStations = getAllMobileStations();
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const MobileStation*>::const_iterator it = allMobileStations->begin();
      it != allMobileStations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(((MobileStation*) it->second)->getID());
  }
  return stationsIDs;
}

vector<stationID_t>* FacilitiesManager::getAllFixedStationsIDs()
{
  map<stationID_t, const FixedStation*>* allFixedStations = getAllFixedStations();
  vector<stationID_t>* stationsIDs = new vector<stationID_t>();
  for (map<stationID_t, const FixedStation*>::const_iterator it = allFixedStations->begin();
      it != allFixedStations->end(); it++)
  {
    if (it->second != NULL)
      stationsIDs->push_back(((FixedStation*) it->second)->getID());
  }
  return stationsIDs;
}

unsigned int FacilitiesManager::getStationsNumber()
{
  const map<stationID_t, Station*> allStations = getAllStations();
  return allStations.size();
}

unsigned int FacilitiesManager::getMobileStationsNumber()
{
  map<stationID_t, const MobileStation*>* allMobileStations = getAllMobileStations();
  return allMobileStations->size();
}

unsigned int FacilitiesManager::getFixedStationsNumber()
{
  map<stationID_t, const FixedStation*>* allFixedStations = getAllFixedStations();
  return allFixedStations->size();
}

// ===============================================================
// ====================== LDM related       ======================
// ===============================================================

bool FacilitiesManager::storeMessage(actionID_t actionID, vector<stationID_t> receivers)
{
  if (facilities != NULL)
    return facilities->storeMessage(actionID, receivers);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

actionID_t FacilitiesManager::createCAMpayload(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->createCAMpayload(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

actionID_t FacilitiesManager::createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
    denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
    latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea)
{
  if (facilities != NULL)
    return facilities->createDENMpayload(stationID, expiryTime, frequency, reliability, negation, causeCode, severity,
        latitude, longitude, relevanceArea);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

actionID_t FacilitiesManager::createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
    TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
    unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
    int messageSequenceNumber, string &extra)
{
  if (facilities != NULL)
    return facilities->createApplicationMessagePayload(stationID, comType, destination, preferredTechnologies,
        messageLength, communicationProfile, applicationId, applicationMessageType, messageSequenceNumber,extra);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const vector<Area2D*>& FacilitiesManager::getRelevantArea()
{
  if (facilities != NULL)
    return facilities->getRelevantArea();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<icsstationtype_t, bool>* FacilitiesManager::getRelevantStationTypes() const
{
  if (facilities != NULL)
    return facilities->getRelevantStationTypes();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<messageType_t, bool>* FacilitiesManager::getRelevantMessageTypes() const
{
  if (facilities != NULL)
    return facilities->getRelevantMessageTypes();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

direction_t FacilitiesManager::getRelevantDirection() const
{
  if (facilities != NULL)
    return facilities->getRelevantDirection();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

direction_t FacilitiesManager::getRelevantDirectionAccuracy() const
{
  if (facilities != NULL)
    return facilities->getRelevantDirectionAccuracy();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

icstime_t FacilitiesManager::getDefaultMessageLifeInterval() const
{
  if (facilities != NULL)
    return facilities->getDefaultMessageLifeInterval();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::setRelevantStationTypes(map<icsstationtype_t, bool> &relevStaTypes)
{
  if (facilities != NULL)
  {
    facilities->setRelevantStationTypes(relevStaTypes);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy)
{
  if (facilities != NULL)
  {
    facilities->setRelevantDirection(relevDirection, relevDirectionAccuracy);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::setDefaultMessageLifeInterval(icstime_t defMesLifeInterval)
{
  if (facilities != NULL)
  {
    facilities->setDefaultMessageLifeInterval(defMesLifeInterval);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::setRelevantArea(vector<Area2D*>& areas)
{
  if (facilities != NULL)
  {
    facilities->setRelevantArea(areas);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::addRelevantArea(Area2D *relevArea)
{
  if (facilities != NULL)
  {
    facilities->addRelevantArea(relevArea);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::deleteRelevantArea(Area2D *relevArea)
{
  if (facilities != NULL)
  {
    facilities->deleteRelevantArea(relevArea);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::clearRelevantArea()
{
  if (facilities != NULL)
  {
    facilities->clearRelevantArea();
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

void FacilitiesManager::updateClock(icstime_t newSimTime)
{
  if (facilities != NULL)
  {
    facilities->updateClock(newSimTime);
    return;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

Area2D* FacilitiesManager::getWholeArea(vector<Area2D*> areas)
{
  if (facilities != NULL)
    return facilities->getWholeArea(areas);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

Circle FacilitiesManager::getCircleFromArea(Area2D* area)
{
  if (facilities != NULL)
    return facilities->getCircleFromArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

Circle FacilitiesManager::getCircleFromAreas(std::vector<TArea> areas)
{
  std::vector<Area2D*> areasPtr = convertAreas(areas);
  Area2D* wholeArea = getWholeArea(areasPtr);
  Circle ret = getCircleFromArea(wholeArea);
  //Need to check if the returned pointer is not the one inside the vector
  if (areas.size() == 1 && areas[0].shape != wholeArea)
    delete wholeArea;
  return ret;
}

std::vector<Area2D*> FacilitiesManager::convertAreas(std::vector<TArea> areas)
{
  if (facilities != NULL)
  {
    vector<Area2D*> areasPtr;
    for (unsigned int i = 0; i < areas.size(); i++)
    {
      if (areas.at(i).type == GEOMETRICSHAPE)
        areasPtr.push_back(areas.at(i).shape);
      else
      { // ROADELEMENT
        if (areas.at(i).elementType == LANE)
        {
          Lane* lane = (Lane*) getLane(areas.at(i).roadElementID);
          areasPtr.push_back(lane);
        }
        else if (areas.at(i).elementType == EDGE)
        {
          Edge* edge = (Edge*) getEdge(areas.at(i).roadElementID);
          areasPtr.push_back(edge);
        }
        else if (areas.at(i).elementType == JUNCTION)
        {
          Junction* junction = (Junction*) getEdge(areas.at(i).roadElementID);
          areasPtr.push_back(junction);
        }
      }
    }
    return areasPtr;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

std::vector<TArea> FacilitiesManager::convertAreas(std::vector<const Area2D*> areas)
{
  if (facilities != NULL)
  {
    std::vector<TArea> retAreas;
    for (unsigned int i = 0; i < areas.size(); i++)
    {
      TArea tmp;
      Area2D* areaPtr = (Area2D*) areas.at(i);
      if (areaPtr->getArea2DType() == GEOMETRICSHAPE)
      {
        GeometricShape* shape = static_cast<GeometricShape*>(areaPtr);
        tmp.type = GEOMETRICSHAPE;
        tmp.shape = shape;
        tmp.shapeType = shape->getShapeType();
      }
      else
      { // ROADELEMENT
        RoadElement* element = static_cast<RoadElement*>(areaPtr);
        tmp.elementType = element->getRoadElementType();
        tmp.roadElementID = element->getID();
      }
      retAreas.push_back(tmp);
    }
    return retAreas;
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

bool FacilitiesManager::hasStationEverSentMessages(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->hasStationEverSentMessages(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

seqNo_t FacilitiesManager::getLastSeqNoForStation(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getLastSeqNoForStation(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

icstime_t FacilitiesManager::getReceivedMessageDeletionTimeFromTable(actionID_t actionID)
{
  if (facilities != NULL)
    return facilities->getReceivedMessageDeletionTimeFromTable(actionID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, Point2D>* FacilitiesManager::getMobileStationsPositionsAroundStations(stationID_t receiverID)
{
  if (facilities != NULL)
    return facilities->getMobileStationsPositionsAroundStations(receiverID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, Point2D>* FacilitiesManager::getMobileStationsPositionsAroundStationsInTemporalWindow(
    stationID_t receiverID, icstime_t start, icstime_t stop)
{
  if (facilities != NULL)
    return facilities->getMobileStationsPositionsAroundStationsInTemporalWindow(receiverID, start, stop);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, Point2D>* FacilitiesManager::getFixedStationsPositionsAroundStations(stationID_t receiverID)
{
  if (facilities != NULL)
    return facilities->getMobileStationsPositionsAroundStations(receiverID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, Point2D>* FacilitiesManager::getFixedStationsPositionsAroundStationsInTemporalWindow(
    stationID_t receiverID, icstime_t start, icstime_t stop)
{
  if (facilities != NULL)
    return facilities->getMobileStationsPositionsAroundStationsInTemporalWindow(receiverID, start, stop);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

messageType_t FacilitiesManager::getReceivedMessageType(actionID_t actionID)
{
  ics_facilities::ReceivedMessage* message = getReceivedMessage(actionID);
  return message->getMessageType();
}

stationID_t FacilitiesManager::getReceivedMessageSenderID(actionID_t actionID)
{
  FacilityMessagePayload* payload = getReceivedMessagePayload(actionID);
  return payload->getSenderID();
}

icstime_t FacilitiesManager::getReceivedMessageGenerationTime(actionID_t actionID)
{
  FacilityMessagePayload* payload = getReceivedMessagePayload(actionID);
  return payload->getTimeStamp();
}

unsigned int FacilitiesManager::getNumberOfReceivedMessageInTable(stationID_t stationID)
{
  map<actionID_t, ics_facilities::ReceivedMessage*>* table = getLDM(stationID);
  unsigned int number = table->size();
  delete table;
  return number;
}

vector<actionID_t>* FacilitiesManager::getLastMessagesActionIDs(icstime_t startTime)
{
  map<actionID_t, ics_facilities::ReceivedMessage*>* messages = getLastMessages(startTime);
  if (messages->size() == 0)
    return NULL;

  vector<actionID_t>* messagesIDs = new vector<actionID_t>();
  map<actionID_t, ics_facilities::ReceivedMessage*>::const_iterator it;
  for (it = messages->begin(); it != messages->end(); it++)
    messagesIDs->push_back(it->second->getActionID());
  return messagesIDs;
}

map<actionID_t, messageType_t>* FacilitiesManager::getLastMessagesTypes(icstime_t startTime)
{
  map<actionID_t, ics_facilities::ReceivedMessage*>* messages = getLastMessages(startTime);
  if (messages->size() == 0)
    return NULL;

  map<actionID_t, messageType_t>* messagesTypes = new map<actionID_t, messageType_t>();
  map<actionID_t, ics_facilities::ReceivedMessage*>::const_iterator it;
  for (it = messages->begin(); it != messages->end(); it++)
    messagesTypes->insert(pair<actionID_t, messageType_t>(it->second->getActionID(), it->second->getMessageType()));
  return messagesTypes;
}

vector<actionID_t>* FacilitiesManager::getAllSpecificMessagesActionIDs(messageType_t messageType)
{
  map<actionID_t, ics_facilities::ReceivedMessage*>* messages = getAllSpecificMessages(messageType);
  if (messages->size() == 0)
    return NULL;

  vector<actionID_t>* messagesIDs = new vector<actionID_t>();
  map<actionID_t, ics_facilities::ReceivedMessage*>::const_iterator it;
  for (it = messages->begin(); it != messages->end(); it++)
    messagesIDs->push_back(it->second->getActionID());
  return messagesIDs;
}

vector<actionID_t>* FacilitiesManager::getMessagesActionIDsFromSender(stationID_t stationID)
{
  map<actionID_t, ics_facilities::ReceivedMessage*>* messages = getMessagesFromSender(stationID);
  if (messages->size() == 0)
    return NULL;

  vector<actionID_t>* messagesIDs = new vector<actionID_t>();
  map<actionID_t, ics_facilities::ReceivedMessage*>::const_iterator it;
  for (it = messages->begin(); it != messages->end(); it++)
    messagesIDs->push_back(it->second->getActionID());
  return messagesIDs;
}

map<stationID_t, Point2D>* FacilitiesManager::getReceiversPositionAtReceptionTime(actionID_t actionID)
{
  ics_facilities::ReceivedMessage* message = getReceivedMessage(actionID);
  if (message == NULL)
    return NULL;

  vector<Receiver> receivers = message->getReceiversList();
  map<stationID_t, Point2D>* receiversPosition = new map<stationID_t, Point2D>();
  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    receiversPosition->insert(pair<stationID_t, Point2D>(receivers[i].receiverID, receivers[i].position));
  }
  return receiversPosition;
}

map<stationID_t, icstime_t>* FacilitiesManager::getReceiversReceptionTime(actionID_t actionID)
{
  ics_facilities::ReceivedMessage* message = getReceivedMessage(actionID);
  if (message == NULL)
    return NULL;

  vector<Receiver> receivers = message->getReceiversList();
  map<stationID_t, icstime_t>* receiversReceptionTime = new map<stationID_t, icstime_t>();
  for (unsigned int i = 0; i < receivers.size(); i++)
  {
    receiversReceptionTime->insert(pair<stationID_t, icstime_t>(receivers[i].receiverID, receivers[i].receptionTime));
  }
  return receiversReceptionTime;
}

TApplicationMessageDestination FacilitiesManager::getApplicationMessageDestination(actionID_t actionID)
{
  FacilityMessagePayload* facPayload = getReceivedMessagePayload(actionID);
  messageType_t messageType = facPayload->getMessageType();
  if (messageType == CAM || messageType == DENM)
  {
    cerr << "[iCS - Facilities] - ERROR: FacilitiesManager::getApplicationMessageDestination() - "
        "The message with actionID " << actionID << " is not an application message!";
    abort();
  }
  ApplicationMessagePayload* appPayload = NULL;
  appPayload = dynamic_cast<ApplicationMessagePayload*>(facPayload);
  if (appPayload == NULL)
  {
    cerr << "[iCS - Facilities] - ERROR: FacilitiesManager::getApplicationMessageDestination() - "
        "The message with actionID " << actionID << " is not an application message!";
    abort();
  }
  return appPayload->getDestination();
}

vector<TCamInformation>* FacilitiesManager::getInfoFromLastCAMsReceivedByStation(stationID_t stationID)
{
  vector<CAMPayloadGeneral*>* cams = getLastCAMsReceivedByStation(stationID);
  if (cams == NULL)
  {
    delete cams;
    return NULL;
  }

  vector<TCamInformation>* info = new vector<TCamInformation>();
  for (vector<CAMPayloadGeneral*>::const_iterator it = cams->begin(); it != cams->end(); it++)
  {

    const Station* sta = getStation((*it)->getSenderID());

    if (sta != NULL)
    {
      TCamInformation camInfo;
      // General basic CAM profile
      camInfo.senderID = ((Station*) sta)->getID();
      camInfo.senderPosition = (*it)->getPosition();
      camInfo.generationTime = (*it)->getTimeStamp();
      camInfo.staType = ((Station*) sta)->getType();
      // Vehicle CAM profile
      if (camInfo.staType == STATION_MOBILE)
      {
        camInfo.speed = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getVehicleSpeed();
        camInfo.angle = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getHeading();
        camInfo.acceleration = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getVehicleAcceleration();
        camInfo.length = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getStationLength();
        camInfo.width = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getStationWidth();
        camInfo.lights = static_cast<CAMPayloadBasicVehicleProfile*>(*it)->getExteriorLightsStatus();
      }
      else
      {
        camInfo.speed = 0;
        camInfo.angle = 0;
        camInfo.acceleration = 0;
        camInfo.length = 0;
        camInfo.width = 0;
        camInfo.lights = 0;
      }
      // Location Referencing information
      camInfo.laneID = convertPoint2LaneID(camInfo.senderPosition);
      camInfo.edgeID = getEdgeIDFromLane(camInfo.laneID);
      camInfo.junctionID = getJunctionIDFromLane(camInfo.laneID);

      // Compute the bufferSize
      camInfo.camInfoBuffSize = 0;
      camInfo.camInfoBuffSize += 20;   // Basic profile fields
      camInfo.camInfoBuffSize += 24;   // Vehicle profile fields
      camInfo.camInfoBuffSize += (4 + camInfo.laneID.length());
      camInfo.camInfoBuffSize += (4 + camInfo.edgeID.length());
      camInfo.camInfoBuffSize += (4 + camInfo.junctionID.length());

      info->push_back(camInfo);
    }
  }
  delete cams;
  return info;
}

unsigned char FacilitiesManager::getApplicationMessagePreferredTechnologies(actionID_t actionID)
{
  FacilityMessagePayload* facPayload = getReceivedMessagePayload(actionID);
  messageType_t messageType = facPayload->getMessageType();
  if (messageType == CAM || messageType == DENM)
  {
    return (unsigned char) WAVE;
  }
  ApplicationMessagePayload* appPayload = NULL;
  appPayload = dynamic_cast<ApplicationMessagePayload*>(facPayload);
  if (appPayload == NULL)
  {
    return 0x00;
  }
  return appPayload->getPreferredTechnologies();
}

unsigned short FacilitiesManager::getApplicationMessageLength(actionID_t actionID)
{
  FacilityMessagePayload* facPayload = getReceivedMessagePayload(actionID);
  messageType_t messageType = facPayload->getMessageType();
  if (messageType == CAM || messageType == DENM)
    return NULL;

  ApplicationMessagePayload* appPayload = NULL;
  appPayload = dynamic_cast<ApplicationMessagePayload*>(facPayload);
  if (appPayload == NULL)
    return NULL;

  return appPayload->getMessageLength();
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
// ====================== Map related       ======================
// ===============================================================
const Lane* FacilitiesManager::getLane(roadElementID_t laneID) const
{
  if (facilities != NULL)
    return facilities->getLane(laneID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const Edge* FacilitiesManager::getEdge(roadElementID_t edgeID) const
{
  if (facilities != NULL)
    return facilities->getEdge(edgeID);

  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();

}

const Junction* FacilitiesManager::getJunction(roadElementID_t junctionID) const
{
  if (facilities != NULL)
    return facilities->getJunction(junctionID);

  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const TrafficLight* FacilitiesManager::getTrafficLight(trafficLightID_t trafficLightID) const
{
  if (facilities != NULL)
    return facilities->getTrafficLight(trafficLightID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const Edge* FacilitiesManager::getEdgeFromLane(roadElementID_t laneID) const
{
  if (facilities != NULL)
    return facilities->getEdgeFromLane(laneID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const Junction* FacilitiesManager::getJunctionFromLane(roadElementID_t laneID) const
{
  if (facilities != NULL)
    return facilities->getJunctionFromLane(laneID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const Lane* FacilitiesManager::convertPoint2Map(Point2D& pos) const
{
  if (facilities != NULL)
    return facilities->convertPoint2Map(pos);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const Junction* FacilitiesManager::getClosestJunction(Point2D pos) const
{
  if (facilities != NULL)
    return facilities->getClosestJunction(pos);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<const Edge*>* FacilitiesManager::getEdgesFromJunction(roadElementID_t junctionID_A, roadElementID_t junctionID_B)
{
  if (facilities != NULL)
  {
    return facilities->getEdgesFromJunction(junctionID_A, junctionID_B);
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<const Lane*>* FacilitiesManager::getLanesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID)
{
  if (facilities != NULL)
  {
    return facilities->getLanesFromJunctions(junctAID, junctBID);
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<const Junction*>* FacilitiesManager::getJunctionsFromEdge(roadElementID_t edgeID)
{
  if (facilities != NULL)
  {
    return facilities->getJunctionsFromEdge(edgeID);
  }
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

// ===============================================================
// ====================== Stations related  ======================
// ===============================================================

const Station* FacilitiesManager::getStation(stationID_t stationID) const
{
  if (facilities != NULL)
    return facilities->getStation(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const MobileStation* FacilitiesManager::getMobileStation(stationID_t mobileStationID)
{
  if (facilities != NULL)
    return facilities->getMobileStation(mobileStationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const FixedStation* FacilitiesManager::getFixedStation(stationID_t fixedStationID)
{
  if (facilities != NULL)
    return facilities->getFixedStation(fixedStationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

const map<stationID_t, Station*>& FacilitiesManager::getAllStations() const
{
  if (facilities != NULL)
    return facilities->getAllStations();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}



map<stationID_t, const MobileStation*>* FacilitiesManager::getAllMobileStations()
{
  if (facilities != NULL)
    return facilities->getAllMobileStations();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const FixedStation*>* FacilitiesManager::getAllFixedStations()
{
  if (facilities != NULL)
    return facilities->getAllFixedStations();
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const Station*>* FacilitiesManager::getStationsInArea(GeometricShape &area)
{
  if (facilities != NULL)
    return facilities->getStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const Station*>* FacilitiesManager::getStationsInArea(vector<RoadElement*> &area)
{
  if (facilities != NULL)
    return facilities->getStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const MobileStation*>* FacilitiesManager::getMobileStationsInArea(GeometricShape &area)
{
  if (facilities != NULL)
    return facilities->getMobileStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const MobileStation*>* FacilitiesManager::getMobileStationsInArea(vector<RoadElement*> &area)
{
  if (facilities != NULL)
    return facilities->getMobileStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const FixedStation*>* FacilitiesManager::getFixedStationsInArea(GeometricShape &area)
{
  if (facilities != NULL)
    return facilities->getFixedStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<stationID_t, const FixedStation*>* FacilitiesManager::getFixedStationsInArea(vector<RoadElement*> &area)
{
  if (facilities != NULL)
    return facilities->getFixedStationsInArea(area);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

// ===============================================================
// ====================== Messages related  ======================
// ===============================================================

ReceivedMessage* FacilitiesManager::getReceivedMessage(actionID_t actionID) const
{
  if (facilities != NULL)
    return facilities->getReceivedMessage(actionID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

FacilityMessagePayload* FacilitiesManager::getReceivedMessagePayload(actionID_t actionID) const
{
  if (facilities != NULL)
    return facilities->getReceivedMessagePayload(actionID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<actionID_t, ics_facilities::ReceivedMessage*>* FacilitiesManager::getLDM(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getLDM(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<actionID_t, ics_facilities::ReceivedMessage*>* FacilitiesManager::getLastMessages(icstime_t startTime)
{
  if (facilities != NULL)
    return facilities->getLastMessages(startTime);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<actionID_t, ics_facilities::ReceivedMessage*>* FacilitiesManager::getAllSpecificMessages(messageType_t messageType)
{
  if (facilities != NULL)
    return facilities->getAllSpecificMessages(messageType);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

map<actionID_t, ics_facilities::ReceivedMessage*>* FacilitiesManager::getMessagesFromSender(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getMessagesFromSender(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<CAMPayloadGeneral*>* FacilitiesManager::getGeneratedCAMsFromStationInTable(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getGeneratedCAMsFromStationInTable(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

CAMPayloadGeneral* FacilitiesManager::getLastGeneratedCAM(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getLastGeneratedCAM(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

vector<CAMPayloadGeneral*>* FacilitiesManager::getLastCAMsReceivedByStation(stationID_t stationID)
{
  if (facilities != NULL)
    return facilities->getLastCAMsReceivedByStation(stationID);
  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
  abort();
}

Point2D  FacilitiesManager::getStationPositionsFromMobilityHistory(icstime_t time, stationID_t stationId){
	  if (facilities != NULL)
	    return facilities->getStationPositionsFromMobilityHistory(time, stationId);
	  cerr << "[iCS - Facilities] ERROR: Facilities not allocated by the Facilities Manager" << endl;
	  abort();
}



} // namespace

