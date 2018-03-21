/****************************************************************************/
/// @file    FacilityMessages.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 17, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "FacilityMessages.h"
#include "../../../utils/ics/iCSGeoUtils.h"

namespace ics_facilities {


/* ******************************
 *
 *
 *     ReceivedMessage METHODS
 *
 *
 * ******************************
 */

const vector<Receiver>& ReceivedMessage::getReceiversList() const {
    return receivers;
}

messageType_t ReceivedMessage::getMessageType() const {
    return messageType;
}

actionID_t ReceivedMessage::getActionID() const {
    return actionID;
}

const FacilityMessagePayload* ReceivedMessage::getPayload() const {
    return payload;
}

void ReceivedMessage::setReceivers(vector<Receiver>& receivers) {
    this->receivers = receivers;
}

void ReceivedMessage::setMessageType(messageType_t type) {
    this->messageType = type;
}

void ReceivedMessage::setActionID(actionID_t actionID) {
    this->actionID = actionID;
}

void ReceivedMessage::setPayload(FacilityMessagePayload* payload) {
    this->payload = payload;
}

void ReceivedMessage::addReceivers(vector<Receiver>& receivers) {
    if (this->receivers.empty()) {
        this->receivers = receivers;
        return;
    }
    for (vector<Receiver>::iterator it = receivers.begin(); it != receivers.end(); it ++) {
        this->receivers.push_back(*it);
    }

}

void ReceivedMessage::deleteReceiver(stationID_t recStation) {
    for (vector<Receiver>::iterator it = receivers.begin(); it != receivers.end(); it++) {
        if (it->receiverID == recStation) {
            receivers.erase(it);
            return;
        }
    }
}

void ReceivedMessage::deleteReceiversList() {
    receivers.clear();
}

bool ReceivedMessage::isStationAmongReceivers(stationID_t stationID) {
    for (unsigned int i = 0; i < receivers.size(); i++) {
        if (receivers[i].receiverID == stationID)
            return true;
    }
    return false;
}


/* ******************************
 *
 *
 *     CAM METHODS
 *
 *
 * ******************************
 */


// ========== CAMPayloadGeneral

void CAMPayloadGeneral::setSenderID(stationID_t senderID) {
    this->senderID = senderID;
}

void CAMPayloadGeneral::setTimeStamp(icstime_t generationTime) {
    this->generationTime = generationTime;
}

void CAMPayloadGeneral::setMessageType(messageType_t messageType) {
    this->messageType = messageType;
}

void CAMPayloadGeneral::setStationType(stationType_t stationType) {
    this->stationType = stationType;
}

void CAMPayloadGeneral::setNodeLatitude(float latitude) {
    this->nodeLatitude = latitude;
}

void CAMPayloadGeneral::setNodeLongitude(float longitude) {
    this->nodeLongitude = longitude;
}

void CAMPayloadGeneral::setNodeAltitude(float altitude) {
    this->nodeAltitude = altitude;
}

void CAMPayloadGeneral::setPosition(Point2D pos, double localLat, double localLon, double localAlt) {
    this->nodePosition = pos;
    vector <double> geoPos = LocToGeoConvert(pos.x(), pos.y(), 0, localLat, localLon, localAlt);
    setNodeLatitude(geoPos[0]);
    setNodeLongitude(geoPos[1]);
    this->localLat = localLat;
    this->localLon = localLon;
    this->localAlt = localAlt;
}

void CAMPayloadGeneral::setPosition(Point2D pos) {
    this->nodePosition = pos;
    this->localLat = -1.0;
    this->localLon = -1.0;
    this->localAlt = -1.0;
}

stationType_t CAMPayloadGeneral::getStationType() const {
    return stationType;
}

stationID_t CAMPayloadGeneral::getSenderID() const {
    return senderID;
}

icstime_t CAMPayloadGeneral::getTimeStamp() const {
    return generationTime;
}

messageType_t CAMPayloadGeneral::getMessageType() const {
    return messageType;
}

latitude_t CAMPayloadGeneral::getNodeLatitude() const {
    return nodeLatitude;
}

longitude_t CAMPayloadGeneral::getNodeLongitude() const {
    return nodeLongitude;
}

altitude_t CAMPayloadGeneral::getNodeAltitude() const {
    return nodeAltitude;
}

Point2D CAMPayloadGeneral::getPosition() const {
    return nodePosition;
}



// ========== CAMPayloadBasicVehicleProfile

void CAMPayloadBasicVehicleProfile::setStationProfile(unsigned short stationProfile) {
    this->stationProfile = stationProfile;
}

void CAMPayloadBasicVehicleProfile::setVehicleSpeed(speed_t speed) {
    this->speed = speed;
}

void CAMPayloadBasicVehicleProfile::setHeading(direction_t heading) {
    this->heading = heading;
}

void CAMPayloadBasicVehicleProfile::setStationLength(stationSize_t length) {
    this->length = length;
}

void CAMPayloadBasicVehicleProfile::setStationWidth(stationSize_t width) {
    this->width = width;
}

void CAMPayloadBasicVehicleProfile::setVehicleAcceleration(acceleration_t acceleration) {
    this->acceleration = acceleration;
}

void CAMPayloadBasicVehicleProfile::setExteriorLightsStatus(exteriorLights_t exteriorLightsStatus) {
    this->exteriorLights = exteriorLightsStatus;
}

unsigned short CAMPayloadBasicVehicleProfile::getStationProfile() const {
    return stationProfile;
}

speed_t CAMPayloadBasicVehicleProfile::getVehicleSpeed() const {
    return speed;
}

direction_t CAMPayloadBasicVehicleProfile::getHeading() const {
    return heading;
}

stationSize_t CAMPayloadBasicVehicleProfile::getStationLength() const {
    return length;
}

stationSize_t CAMPayloadBasicVehicleProfile::getStationWidth() const {
    return width;
}

acceleration_t CAMPayloadBasicVehicleProfile::getVehicleAcceleration() const {
    return acceleration;
}

exteriorLights_t CAMPayloadBasicVehicleProfile::getExteriorLightsStatus() const {
    return exteriorLights;
}



/* ******************************
 *
 *
 *     DENM METHODS
 *
 *
 * ******************************
 */

// ========== From abstract class

void DENMPayload::setSenderID(stationID_t senderID) {
    this->senderID  = senderID;
}

void DENMPayload::setTimeStamp(icstime_t generationTime) {
    this->generationTime  = generationTime;
}

void DENMPayload::setMessageType(messageType_t messageType) {
    this->messageType  = messageType;
}

stationID_t DENMPayload::getSenderID() const {
    return senderID;
}

icstime_t DENMPayload::getTimeStamp() const {
    return generationTime;
}

messageType_t DENMPayload::getMessageType() const {
    return messageType;
}



// ========== Management related methods


void DENMPayload::setActionID(actionID_t actionID) {
    this->actionID = actionID;
}

void DENMPayload::setTimeVariables(icstime_t generationTime, icstime_t expiryTime, icstime_t frequency) {
    this->generationTime = generationTime;
    this->expiryTime = expiryTime;
    this->frequency = frequency;
}

void DENMPayload::setReliability(denmReliability_t reliability) {
    this->reliability = reliability;
}

void DENMPayload::setIsNegation(bool negation) {
    this->negation = negation;
}

actionID_t DENMPayload::getActionID() {
    return actionID;
}

icstime_t DENMPayload::getGenerationTime() const {
    return generationTime;
}

icstime_t DENMPayload::getExpiryTime() const {
    return expiryTime;
}

icstime_t DENMPayload::getFrequency() const {
    return frequency;
}

denmReliability_t DENMPayload::getReliability() const {
    return reliability;
}

bool DENMPayload::getIsNegation() const {
    return negation;
}


// ========== Situation related methods


void DENMPayload::setSituation(denmSituationCause_t causeCode, denmSituationSeverity_t severity) {
    this->causeCode = causeCode;
    this->severity = severity;
}

denmSituationCause_t    DENMPayload::getSituationCause() const {
    return causeCode;
}

denmSituationSeverity_t DENMPayload::getSituationSeverity() const {
    return severity;
}


// ========== Location related methods


void DENMPayload::setSituationLatitude(latitude_t latitude) {
    this->latitude = latitude;
}

void DENMPayload::setSituationLongitude(longitude_t longitude) {
    this->longitude = longitude;
}

void DENMPayload::setRelevanceArea(vector<Area2D*> relevanceArea) {
    this->relevanceArea = relevanceArea;
}

latitude_t DENMPayload::getSituationLatitude() const {
    return latitude;
}

longitude_t DENMPayload::getSituationLongitude() const {
    return longitude;
}

vector<Area2D*> DENMPayload::getRelevanceArea() const {
    return relevanceArea;
}


/* ******************************
 *
 *
 *     APPLICATION MESSAGE METHODS
 *
 *
 * ******************************
 */

ApplicationMessagePayload::ApplicationMessagePayload() {
    communicationProfile = 0;
}

// ========== From abstract class

void ApplicationMessagePayload::setSenderID(stationID_t senderID) {
    this->senderID  = senderID;
}

void ApplicationMessagePayload::setTimeStamp(icstime_t generationTime) {
    this->generationTime  = generationTime;
}

void ApplicationMessagePayload::setMessageType(messageType_t messageType) {
    this->messageType  = messageType;
}

stationID_t ApplicationMessagePayload::getSenderID() const {
    return senderID;
}

icstime_t ApplicationMessagePayload::getTimeStamp() const {
    return generationTime;
}

messageType_t ApplicationMessagePayload::getMessageType() const {
    return messageType;
}

// From this class

unsigned short ApplicationMessagePayload::getMessageLength() const {
    return applicationMessageLength;
}

TApplicationMessageDestination ApplicationMessagePayload::getDestination() const {
    return destination;
}

unsigned char ApplicationMessagePayload::getPreferredTechnologies() const {
    return preferredTechnologies;
}

unsigned char ApplicationMessagePayload::getCommunicationProfile() const {
    return communicationProfile;
}

void ApplicationMessagePayload::setApplicationMessageLength(unsigned short  applicationMessageLength) {
    this->applicationMessageLength = applicationMessageLength;
}

void ApplicationMessagePayload::setDestination(TApplicationMessageDestination destination) {
    this->destination = destination;
}

void ApplicationMessagePayload::setPreferredTechnologies(unsigned char preferredTechnologies) {
    this->preferredTechnologies = preferredTechnologies;
}

void ApplicationMessagePayload::setCommunicationProfile(unsigned char communicationProfile) {
    this->communicationProfile = communicationProfile;
}

int ApplicationMessagePayload::getApplicationId() {
    return this->applicationId;
}

unsigned char ApplicationMessagePayload::getApplicationMessageType() {
    return applicationMessageType;
}

int ApplicationMessagePayload::getMessageSequenceNumber() {
    return messageSequenceNumber;
}

void ApplicationMessagePayload::setApplicationId(int  appId) {
    this->applicationId = appId;
}

void ApplicationMessagePayload::setApplicationMessageType(unsigned char msgType) {
    this->applicationMessageType = msgType;
}

void ApplicationMessagePayload::setMessageSequenceNumber(int seqNo) {
    this->messageSequenceNumber = seqNo;
}


}
