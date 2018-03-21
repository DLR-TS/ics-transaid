/****************************************************************************/
/// @file    ICSFacilities.h
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

/****************************************************************************************
 * Added functionalities for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

#ifndef ICSFACILITIES_H_
#define ICSFACILITIES_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "./mapFacilities/MapFacilities.h"
#include "./stationFacilities/StationFacilities.h"
#include "./messageFacilities/LDMLogic.h"

#include <map>
#include <string>
using namespace std;

namespace ics_facilities
{

class ICSFacilities
{
public:

  /**
   * @brief Constructor.
   */
  ICSFacilities();

  /**
   * @brief Destructor.
   */
  virtual ~ICSFacilities();

  // ===============================================================
  // ====================== Configuration      =====================
  // ===============================================================

  bool configureMap(string mapFilename);
  void configureLocalCoordinates(latitude_t lat0, longitude_t lon0, altitude_t alt0);
  bool configureStations(string stationFilename);
  bool configureRelevanceRules(string relevFilename);

  // ===============================================================
  // ====================== Map related       ======================
  // ===============================================================

  const Lane* getLane(roadElementID_t laneID) const;
  const Edge* getEdge(roadElementID_t edgeID) const;
  const Junction* getJunction(roadElementID_t junctionID) const;
  const TrafficLight* getTrafficLight(trafficLightID_t trafficLightID) const;
  const Edge* getEdgeFromLane(roadElementID_t laneID) const;
  const Junction* getJunctionFromLane(roadElementID_t laneID) const;

  const Lane* convertPoint2Map(Point2D& pos) const;
  const Junction* getClosestJunction(Point2D pos) const;
  const vector<roadElementID_t> getNeighboringJunctions(roadElementID_t junctionID) const;
  vector<const Edge*>* getEdgesFromJunction(roadElementID_t junctionID_A, roadElementID_t junctionID_B);
  vector<const Lane*>* getLanesFromJunctions(roadElementID_t junctionID_A, roadElementID_t junctionID_B);
  vector<const Junction*>* getJunctionsFromEdge(roadElementID_t edgeID);

  void setLaneStatus(roadElementID_t laneID, laneStatus newStatus);
  void setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight);

  void updateTrafficLightDynamicInformation(const trafficLightID_t &tlID, std::string& state);

  Area2D* getWholeArea(vector<Area2D*> areas);
  Circle getCircleFromArea(Area2D* area);

  latitude_t getLat0();
  longitude_t getLon0();
  altitude_t getAlt0();

  // ===============================================================
  // ====================== Stations related  ======================
  // ===============================================================

  const Station* getStation(stationID_t stationID) const;
  const MobileStation* getMobileStation(stationID_t mobileStationID);
  const FixedStation* getFixedStation(stationID_t fixedStationID);
  const map<stationID_t, Station*>& getAllStations() const;
  map<stationID_t, const MobileStation*>* getAllMobileStations();
  map<stationID_t, const FixedStation*>* getAllFixedStations();
  void updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info);

  map<stationID_t, const Station*>* getStationsInArea(GeometricShape &area);
  map<stationID_t, const Station*>* getStationsInArea(vector<RoadElement*> &area);
  map<stationID_t, const MobileStation*>* getMobileStationsInArea(GeometricShape &area);
  map<stationID_t, const MobileStation*>* getMobileStationsInArea(vector<RoadElement*> &area);
  map<stationID_t, const FixedStation*>* getFixedStationsInArea(GeometricShape &area);
  map<stationID_t, const FixedStation*>* getFixedStationsInArea(vector<RoadElement*> &area);

  bool enableRAT(stationID_t stationID, RATID RATtoBeEnabled);
  bool disableRAT(stationID_t stationID, RATID RATtoBeDisabled);
  bool enableRATAllStations(RATID RATtoBeEnabled);
  bool disableRATAllStations(RATID RATtoBeDisabled);

  const map<RATID, float>& getDefaultPenetrationRates() const;
  const map<RATID, string>& getDefaultCommunicationProfiles() const;

  // ===============================================================
  // ====================== LDM related       ======================
  // ===============================================================

  bool storeMessage(actionID_t actionID, vector<stationID_t> receivers);

  actionID_t createCAMpayload(stationID_t stationID);

  actionID_t createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
      denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
      latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea);

  actionID_t createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
      TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
      unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
      int messageSequenceNumber, string &extra);

  ReceivedMessage* getReceivedMessage(actionID_t actionID) const;
  FacilityMessagePayload* getReceivedMessagePayload(actionID_t actionID) const;
  map<actionID_t, ReceivedMessage*>* getLDM(stationID_t stationID);
  map<actionID_t, ReceivedMessage*>* getLastMessages(icstime_t startTime);
  map<actionID_t, ReceivedMessage*>* getAllSpecificMessages(messageType_t messageType);
  map<actionID_t, ReceivedMessage*>* getMessagesFromSender(stationID_t senderID);

  vector<CAMPayloadGeneral*>* getGeneratedCAMsFromStationInTable(stationID_t stationID);
  CAMPayloadGeneral* getLastGeneratedCAM(stationID_t stationID);
  vector<CAMPayloadGeneral*>* getLastCAMsReceivedByStation(stationID_t stationID);

  // Relevant Check settings (get and set methods)
  const vector<Area2D*>& getRelevantArea();
  map<icsstationtype_t, bool>* getRelevantStationTypes() const;
  map<messageType_t, bool>* getRelevantMessageTypes() const;
  direction_t getRelevantDirection() const;
  direction_t getRelevantDirectionAccuracy() const;
  icstime_t getDefaultMessageLifeInterval() const;
  void setRelevantStationTypes(map<icsstationtype_t, bool> &relevStaTypes);
  void setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy);
  void setDefaultMessageLifeInterval(icstime_t defMesLifeInterval);
  void setRelevantArea(vector<Area2D*>& areas);
  void addRelevantArea(Area2D *relevArea);
  void deleteRelevantArea(Area2D *relevArea);
  void clearRelevantArea();

  void updateClock(icstime_t newSimTime);
  bool hasStationEverSentMessages(stationID_t stationID);
  seqNo_t getLastSeqNoForStation(stationID_t stationID);
  icstime_t getReceivedMessageDeletionTimeFromTable(actionID_t actionID);

  map<stationID_t, Point2D>* getMobileStationsPositionsAroundStations(stationID_t receiverID);
  map<stationID_t, Point2D>* getMobileStationsPositionsAroundStationsInTemporalWindow(stationID_t receiverID,
      icstime_t start, icstime_t stop);
  map<stationID_t, Point2D>* getFixedStationsPositionsAroundStations(stationID_t receiverID);
  map<stationID_t, Point2D>* getFixedStationsPositionsAroundStationsInTemporalWindow(stationID_t receiverID,
      icstime_t start, icstime_t stop);

  Point2D  getStationPositionsFromMobilityHistory(icstime_t time, stationID_t stationId);

private:
  MapFacilities* mapFac;
  StationFacilities* staFac;
  LDMLogic* LDMtable;
};

}

#endif /* ICSFACILITIES_H_ */
