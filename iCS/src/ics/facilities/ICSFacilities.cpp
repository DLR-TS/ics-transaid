/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************/
/// @file    ICSFacilities.cpp
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

#include "ICSFacilities.h"

#include <cstdlib>
using namespace std;

namespace ics_facilities {

ICSFacilities::ICSFacilities() {
    mapFac = NULL;
    staFac = NULL;
    LDMtable = NULL;
}

ICSFacilities::~ICSFacilities() {
    if (mapFac != NULL) {
        delete (mapFac);
    }
    if (staFac != NULL) {
        delete (staFac);
    }
    if (LDMtable != NULL) {
        delete (LDMtable);
    }
}

bool ICSFacilities::configureMap(string mapFilename) {
    mapFac = new MapFacilities();
    return mapFac->mapParser(mapFilename);
}

void ICSFacilities::configureLocalCoordinates(latitude_t lat0, longitude_t lon0, altitude_t alt0) {
    mapFac->setOriginCoordinates(lat0, lon0, alt0);
}

bool ICSFacilities::configureStations(string stationFilename) {
    staFac = new StationFacilities(mapFac);
    return staFac->configure(stationFilename);
}

bool ICSFacilities::configureRelevanceRules(string relevFilename) {
    LDMtable = new LDMLogic(mapFac, staFac);
    return LDMtable->configureRelevanceRules(relevFilename);
}

// ===============================================================
// ====================== Map related       ======================
// ===============================================================

const Lane* ICSFacilities::getLane(roadElementID_t laneID) const {
    if (mapFac != NULL) {
        return mapFac->getLane(laneID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Edge* ICSFacilities::getEdge(roadElementID_t edgeID) const {
    if (mapFac != NULL) {
        return mapFac->getEdge(edgeID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Junction* ICSFacilities::getJunction(roadElementID_t junctionID) const {
    if (mapFac != NULL) {
        return mapFac->getJunction(junctionID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const TrafficLight* ICSFacilities::getTrafficLight(trafficLightID_t trafficLightID) const {
    if (mapFac != NULL) {
        return mapFac->getTrafficLight(trafficLightID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Edge* ICSFacilities::getEdgeFromLane(roadElementID_t laneID) const {
    if (mapFac != NULL) {
        return mapFac->getEdgeFromLane(laneID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Junction* ICSFacilities::getJunctionFromLane(roadElementID_t laneID) const {
    if (mapFac != NULL) {
        return mapFac->getJunctionFromLane(laneID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Lane* ICSFacilities::convertPoint2Map(Point2D& pos) const {
    if (mapFac != NULL) {
        return mapFac->convertPoint2Map(pos);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const Junction* ICSFacilities::getClosestJunction(Point2D pos) const {
    if (mapFac != NULL) {
        return mapFac->getClosestJunction(pos);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

const vector<roadElementID_t> ICSFacilities::getNeighboringJunctions(roadElementID_t junctionID) const {
    if (mapFac != NULL) {
        return mapFac->getNeighboringJunctions(junctionID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

vector<const Edge*>* ICSFacilities::getEdgesFromJunction(roadElementID_t junctionID_A, roadElementID_t junctionID_B) {
    if (mapFac != NULL) {
        return mapFac->getEdgesFromJunctions(junctionID_A, junctionID_B);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

vector<const Lane*>* ICSFacilities::getLanesFromJunctions(roadElementID_t junctionID_A, roadElementID_t junctionID_B) {
    if (mapFac != NULL) {
        return mapFac->getLanesFromJunctions(junctionID_A, junctionID_B);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

vector<const Junction*>* ICSFacilities::getJunctionsFromEdge(roadElementID_t edgeID) {
    if (mapFac != NULL) {
        return mapFac->getJunctionsFromEdge(edgeID);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setLaneStatus(roadElementID_t laneID, laneStatus newStatus) {
    if (mapFac != NULL) {
        mapFac->setLaneStatus(laneID, newStatus);
        return;
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight) {
    if (mapFac != NULL) {
        mapFac->setLaneWeight(laneID, newWeight);
        return;
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::updateTrafficLightDynamicInformation(const trafficLightID_t& tlID, std::string& state) {
    if (mapFac != NULL) {
        mapFac->updateTrafficLightDynamicInformation(tlID, state);
        return;
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

Area2D* ICSFacilities::getWholeArea(vector<Area2D*> areas) {
    if (mapFac != NULL) {
        return mapFac->getWholeArea(areas);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

Circle ICSFacilities::getCircleFromArea(Area2D* area) {
    if (mapFac != NULL) {
        return mapFac->getCircleFromArea(area);
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

latitude_t ICSFacilities::getLat0() {
    if (mapFac != NULL) {
        return mapFac->getLat0();
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

longitude_t ICSFacilities::getLon0() {
    if (mapFac != NULL) {
        return mapFac->getLon0();
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

altitude_t ICSFacilities::getAlt0() {
    if (mapFac != NULL) {
        return mapFac->getAlt0();
    }
    cerr << "[facilities] ERROR: Map not allocated by the ICSFacilities" << endl;
    abort();
}

// ===============================================================
// ====================== Stations related  ======================
// ===============================================================

const Station* ICSFacilities::getStation(stationID_t stationID) const {
    if (staFac != NULL) {
        return staFac->getStation(stationID);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

const MobileStation* ICSFacilities::getMobileStation(stationID_t mobileStationID) {
    if (staFac != NULL) {
        return staFac->getMobileStation(mobileStationID);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

const FixedStation* ICSFacilities::getFixedStation(stationID_t fixedStationID) {
    if (staFac != NULL) {
        return staFac->getFixedStation(fixedStationID);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

const map<stationID_t, Station*>& ICSFacilities::getAllStations() const {
    if (staFac != NULL) {
        return staFac->getAllStations();
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const MobileStation*>* ICSFacilities::getAllMobileStations() {
    if (staFac != NULL) {
        return staFac->getAllMobileStations();
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const FixedStation*>* ICSFacilities::getAllFixedStations() {
    if (staFac != NULL) {
        return staFac->getAllFixedStations();
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info) {
    if (staFac != NULL) {
        staFac->updateMobileStationDynamicInformation(stationId, info);
        return;
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const Station*>* ICSFacilities::getStationsInArea(GeometricShape& area) {
    if (staFac != NULL) {
        return staFac->getStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const Station*>* ICSFacilities::getStationsInArea(vector<RoadElement*>& area) {
    if (staFac != NULL) {
        return staFac->getStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const MobileStation*>* ICSFacilities::getMobileStationsInArea(GeometricShape& area) {
    if (staFac != NULL) {
        return staFac->getMobileStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const MobileStation*>* ICSFacilities::getMobileStationsInArea(vector<RoadElement*>& area) {
    if (staFac != NULL) {
        return staFac->getMobileStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const FixedStation*>* ICSFacilities::getFixedStationsInArea(GeometricShape& area) {
    if (staFac != NULL) {
        return staFac->getFixedStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, const FixedStation*>* ICSFacilities::getFixedStationsInArea(vector<RoadElement*>& area) {
    if (staFac != NULL) {
        return staFac->getFixedStationsInArea(area);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::enableRAT(stationID_t stationID, RATID RATtoBeEnabled) {
    if (staFac != NULL) {
        return staFac->enableRAT(stationID, RATtoBeEnabled);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::disableRAT(stationID_t stationID, RATID RATtoBeDisabled) {
    if (staFac != NULL) {
        return staFac->disableRAT(stationID, RATtoBeDisabled);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::enableRATAllStations(RATID RATtoBeEnabled) {
    if (staFac != NULL) {
        return staFac->enableRATAllStations(RATtoBeEnabled);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::disableRATAllStations(RATID RATtoBeDisabled) {
    if (staFac != NULL) {
        return staFac->disableRATAllStations(RATtoBeDisabled);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

const map<RATID, float>& ICSFacilities::getDefaultPenetrationRates() const {
    if (staFac != NULL) {
        return staFac->getDefaultPenetrationRates();
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

const map<RATID, string>& ICSFacilities::getDefaultCommunicationProfiles() const {
    if (staFac != NULL) {
        return staFac->getDefaultCommunicationProfiles();
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();
}

// ===============================================================
// ====================== LDM related       ======================
// ===============================================================

map<actionID_t, ReceivedMessage*>* ICSFacilities::getLDM(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->getLDM(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

ReceivedMessage* ICSFacilities::getReceivedMessage(actionID_t actionID) const {
    if (LDMtable != NULL) {
        return LDMtable->getReceivedMessage(actionID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

FacilityMessagePayload* ICSFacilities::getReceivedMessagePayload(actionID_t actionID) const {
    if (LDMtable != NULL) {
        return LDMtable->getReceivedMessagePayload(actionID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<actionID_t, ReceivedMessage*>* ICSFacilities::getLastMessages(icstime_t startTime) {
    if (LDMtable != NULL) {
        return LDMtable->getLastMessages(startTime);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<actionID_t, ReceivedMessage*>* ICSFacilities::getAllSpecificMessages(messageType_t messageType) {
    if (LDMtable != NULL) {
        return LDMtable->getAllSpecificMessages(messageType);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<actionID_t, ReceivedMessage*>* ICSFacilities::getMessagesFromSender(stationID_t senderID) {
    if (LDMtable != NULL) {
        return LDMtable->getMessagesFromSender(senderID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

vector<CAMPayloadGeneral*>* ICSFacilities::getGeneratedCAMsFromStationInTable(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->getGeneratedCAMsFromStationInTable(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

CAMPayloadGeneral* ICSFacilities::getLastGeneratedCAM(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->getLastGeneratedCAM(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

vector<CAMPayloadGeneral*>* ICSFacilities::getLastCAMsReceivedByStation(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->getLastCAMsReceivedByStation(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

const vector<Area2D*>& ICSFacilities::getRelevantArea() {
    if (LDMtable != NULL) {
        return LDMtable->getRelevantArea();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<icsstationtype_t, bool>* ICSFacilities::getRelevantStationTypes() const {
    if (LDMtable != NULL) {
        return LDMtable->getRelevantStationTypes();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<messageType_t, bool>* ICSFacilities::getRelevantMessageTypes() const {
    if (LDMtable != NULL) {
        return LDMtable->getRelevantMessageTypes();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

direction_t ICSFacilities::getRelevantDirection() const {
    if (LDMtable != NULL) {
        return LDMtable->getRelevantDirection();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

direction_t ICSFacilities::getRelevantDirectionAccuracy() const {
    if (LDMtable != NULL) {
        return LDMtable->getRelevantDirectionAccuracy();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

icstime_t ICSFacilities::getDefaultMessageLifeInterval() const {
    if (LDMtable != NULL) {
        return LDMtable->getDefaultMessageLifeInterval();
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setRelevantStationTypes(map<icsstationtype_t, bool>& relevStaTypes) {
    if (LDMtable != NULL) {
        LDMtable->setRelevantStationTypes(relevStaTypes);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy) {
    if (LDMtable != NULL) {
        LDMtable->setRelevantDirection(relevDirection, relevDirectionAccuracy);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setDefaultMessageLifeInterval(icstime_t defMesLifeInterval) {
    if (LDMtable != NULL) {
        LDMtable->setDefaultMessageLifeInterval(defMesLifeInterval);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::setRelevantArea(vector<Area2D*>& areas) {
    if (LDMtable != NULL) {
        LDMtable->setRelevantArea(areas);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::addRelevantArea(Area2D* relevArea) {
    if (LDMtable != NULL) {
        LDMtable->addRelevantArea(relevArea);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::deleteRelevantArea(Area2D* relevArea) {
    if (LDMtable != NULL) {
        LDMtable->deleteRelevantArea(relevArea);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::clearRelevantArea() {
    if (LDMtable != NULL) {
        LDMtable->clearRelevantArea();
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

void ICSFacilities::updateClock(icstime_t newSimTime) {
    if (LDMtable != NULL) {
        LDMtable->updateClock(newSimTime);
        return;
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::hasStationEverSentMessages(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->hasStationEverSentMessages(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

seqNo_t ICSFacilities::getLastSeqNoForStation(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->getLastSeqNoForStation(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

icstime_t ICSFacilities::getReceivedMessageDeletionTimeFromTable(actionID_t actionID) {
    if (LDMtable != NULL) {
        return LDMtable->getReceivedMessageDeletionTimeFromTable(actionID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, Point2D>* ICSFacilities::getMobileStationsPositionsAroundStations(stationID_t receiverID) {
    if (LDMtable != NULL) {
        return LDMtable->getClassifiedStationsPositionsAroundStations(receiverID, STATION_MOBILE);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, Point2D>* ICSFacilities::getMobileStationsPositionsAroundStationsInTemporalWindow(
    stationID_t receiverID, icstime_t start, icstime_t stop) {
    if (LDMtable != NULL)
        return LDMtable->getClassifiedStationsPositionsAroundStationsInTemporalWindow(receiverID, STATION_MOBILE, start,
                stop);
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, Point2D>* ICSFacilities::getFixedStationsPositionsAroundStations(stationID_t receiverID) {
    if (LDMtable != NULL) {
        return LDMtable->getClassifiedStationsPositionsAroundStations(receiverID, STATION_FIXED);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

map<stationID_t, Point2D>* ICSFacilities::getFixedStationsPositionsAroundStationsInTemporalWindow(
    stationID_t receiverID, icstime_t start, icstime_t stop) {
    if (LDMtable != NULL)
        return LDMtable->getClassifiedStationsPositionsAroundStationsInTemporalWindow(receiverID, STATION_FIXED, start,
                stop);
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

actionID_t ICSFacilities::createCAMpayload(stationID_t stationID) {
    if (LDMtable != NULL) {
        return LDMtable->createCAMpayload(stationID);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

actionID_t ICSFacilities::createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
        denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
        latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea) {
    if (LDMtable != NULL)
        return LDMtable->createDENMpayload(stationID, expiryTime, frequency, reliability, negation, causeCode, severity,
                                           latitude, longitude, relevanceArea);
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

actionID_t ICSFacilities::createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
        TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
        unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
        int messageSequenceNumber, string& extra) {
    if (LDMtable != NULL)
        return LDMtable->createApplicationMessagePayload(stationID, comType, destination, preferredTechnologies,
                messageLength, communicationProfile, applicationId, applicationMessageType, messageSequenceNumber, extra);
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}

bool ICSFacilities::storeMessage(actionID_t actionID, vector<stationID_t> receivers) {
    if (LDMtable != NULL) {
        return LDMtable->storeMessage(actionID, receivers);
    }
    cerr << "[facilities] ERROR: LDM not allocated by the ICSFacilities" << endl;
    abort();
}


Point2D ICSFacilities::getStationPositionsFromMobilityHistory(icstime_t time, stationID_t stationId) {
    if (staFac != NULL) {
        return staFac->getNodePositionFromMobilityHistory(time, stationId);
    }
    cerr << "[facilities] ERROR: Station Facilities not allocated by the ICSFacilities" << endl;
    abort();

}

}
