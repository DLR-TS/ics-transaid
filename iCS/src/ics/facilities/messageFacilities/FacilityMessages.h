/****************************************************************************/
/// @file    FacilityMessages.h
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

#ifndef FACILITYMESSAGES_H_
#define FACILITYMESSAGES_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../stationFacilities/Station.h"
#include "../../../utils/ics/geometric/Ellipse.h"

#include <vector>


namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================

/* ******************************
 *
 *
 *     RECEIVER CLASS
 *
 *
 * ******************************
 */

/**
* @class Receiver
* @brief Class that contains the ID of a station and its position and time step corresponding to the moment when the message was received.
*/
class Receiver {
public:
    stationID_t	receiverID;
    icstime_t	receptionTime;
    Point2D     position;
};

/* ******************************
 *
 *
 *     GENERIC MESSAGE PAYLOAD
 *
 *
 * ******************************
 */

/**
* @class FacilityMessagePayload
* @brief Generic payload for Facilities messages (e.g., CAM, DENM, ...).
*/
class FacilityMessagePayload {
public:
    FacilityMessagePayload() {};
    virtual ~FacilityMessagePayload() {};

    virtual void setSenderID(stationID_t senderID) = 0;
    virtual void setTimeStamp(icstime_t generationTime) = 0;
    virtual void setMessageType(messageType_t messageType) = 0;

    virtual stationID_t getSenderID() const = 0;
    virtual icstime_t getTimeStamp() const = 0;
    virtual messageType_t getMessageType() const = 0;
    std::string m_extra;

protected:
    stationID_t senderID;
    icstime_t generationTime;
    messageType_t messageType;
};


/* ******************************
 *
 *
 *     CAM MESSAGE CLASSES
 *
 *
 * ******************************
 */

/**
* @class CAMPayloadGeneral
* @brief Generic payload for CAM messages.
*/
class CAMPayloadGeneral: public ics_facilities::FacilityMessagePayload {
public:
    CAMPayloadGeneral() {};
    virtual ~CAMPayloadGeneral() {};

    void setSenderID(stationID_t senderID);
    void setTimeStamp(icstime_t generationTime);
    void setMessageType(messageType_t messageType);

    void setStationType(stationType_t stationType);
    void setPosition(Point2D pos, double localLat, double localLon, double localAlt);
    void setPosition(Point2D pos);

    stationType_t getStationType() const;
    stationID_t getSenderID() const;
    icstime_t getTimeStamp() const;
    messageType_t getMessageType() const;

    latitude_t getNodeLatitude() const;
    longitude_t getNodeLongitude() const;
    altitude_t getNodeAltitude() const;
    Point2D getPosition() const;

protected:
    stationType_t stationType;

    Point2D nodePosition;       /// @brief  Cartesian (x,y) position of the node.
    latitude_t nodeLatitude;
    longitude_t nodeLongitude;
    altitude_t nodeAltitude;
    latitude_t localLat;        /// @brief  Latitude of the origin (0,0,0).
    longitude_t localLon;       /// @brief  Longitude of the origin (0,0,0).
    altitude_t localAlt;        /// @brief  Altitude of the origin (0,0,0).

    void setNodeLatitude(float latitude);
    void setNodeLongitude(float longitude);
    void setNodeAltitude(float altitude);
};

/**
* @class CAMPayloadBasicVehicleProfile
* @brief Basic profile of CAM messages for vehicles.
*/
class CAMPayloadBasicVehicleProfile: public ics_facilities::CAMPayloadGeneral {
public:
    CAMPayloadBasicVehicleProfile() {};
    virtual ~CAMPayloadBasicVehicleProfile() {};

    void setStationProfile(unsigned short stationProfile);
    void setVehicleSpeed(speed_t speed);
    void setHeading(direction_t heading);
    void setStationLength(stationSize_t length);
    void setStationWidth(stationSize_t width);
    void setVehicleAcceleration(acceleration_t acceleration);
    void setExteriorLightsStatus(exteriorLights_t exteriorLightsStatus);

    unsigned short  getStationProfile() const;
    speed_t getVehicleSpeed() const;
    direction_t getHeading() const;
    stationSize_t getStationLength() const;
    stationSize_t getStationWidth() const;
    acceleration_t getVehicleAcceleration() const;
    exteriorLights_t getExteriorLightsStatus() const;

protected:
    unsigned short  stationProfile;		// NEVER USED
    speed_t speed;
    direction_t     heading;
    stationSize_t   length;
    stationSize_t   width;
    acceleration_t  acceleration;
    exteriorLights_t exteriorLights;
};

/**
* @class CAMPayloadBasicRSU
* @brief Basic profile of CAM messages for RSUs.
*/
class CAMPayloadBasicRSU: public ics_facilities::CAMPayloadGeneral {
public:
    CAMPayloadBasicRSU() {};
    virtual ~CAMPayloadBasicRSU() {};
};


/* ******************************
 *
 *
 *     DENM MESSAGE CLASS
 *
 *
 * ******************************
 */

/**
* @class DENMPayload
* @brief Payload of DENM messages.
*/
class DENMPayload: public ics_facilities::FacilityMessagePayload {
public:
    DENMPayload() {};
    virtual ~DENMPayload() {};

    // From abstract class
    void setSenderID(stationID_t senderID);
    void setTimeStamp(icstime_t generationTime);
    void setMessageType(messageType_t messageType);

    stationID_t getSenderID() const;
    icstime_t getTimeStamp() const;
    messageType_t getMessageType() const;

    // Management related methods
    void setActionID(actionID_t actionID);
    void setTimeVariables(icstime_t generationTime, icstime_t expiryTime, icstime_t frequency);
    void setReliability(denmReliability_t reliability);
    void setIsNegation(bool negation);

    actionID_t getActionID();
    icstime_t getGenerationTime() const;
    icstime_t getExpiryTime() const;
    icstime_t getFrequency() const;
    denmReliability_t getReliability() const;
    bool getIsNegation() const;

    // Situation related methods
    void setSituation(denmSituationCause_t causeCode, denmSituationSeverity_t severity);

    denmSituationCause_t getSituationCause() const;
    denmSituationSeverity_t getSituationSeverity() const;

    // Location related methods
    void setSituationLatitude(latitude_t latitude);
    void setSituationLongitude(longitude_t longitude);
    void setRelevanceArea(vector<Area2D*> relevanceArea);

    latitude_t getSituationLatitude() const;
    longitude_t getSituationLongitude() const;
    vector<Area2D*> getRelevanceArea() const;

private:
    // Management variables
    actionID_t actionID;
    icstime_t generationTime;
    icstime_t expiryTime;
    icstime_t frequency;
    denmReliability_t reliability;
    bool negation;

    // Situation variables
    denmSituationCause_t causeCode;
    denmSituationSeverity_t severity;

    // Location variables
    latitude_t latitude;
    longitude_t longitude;
    vector<Area2D*> relevanceArea;
};


/* ******************************
 *
 *
 *     APPLICATION MESSAGE CLASS
 *
 *
 * ******************************
 */

/**
* @class ApplicationMessagePayload
* @brief Wraps the application message to store it in the iFMT.
*/
class ApplicationMessagePayload: public ics_facilities::FacilityMessagePayload {
public:
    ApplicationMessagePayload();
    virtual ~ApplicationMessagePayload() {};

    // From abstract class
    stationID_t getSenderID() const;
    icstime_t getTimeStamp() const;
    messageType_t getMessageType() const;

    void setSenderID(stationID_t senderID);
    void setTimeStamp(icstime_t generationTime);
    void setMessageType(messageType_t messageType);

    // From this class
    unsigned short  getMessageLength() const;
    TApplicationMessageDestination getDestination() const;
    unsigned char getPreferredTechnologies() const;
    unsigned char getCommunicationProfile() const;
    int getApplicationId();
    unsigned char getApplicationMessageType();
    int getMessageSequenceNumber();

    void setApplicationMessageLength(unsigned short  applicationMessageLength);
    void setDestination(TApplicationMessageDestination destination);
    void setPreferredTechnologies(unsigned char preferredTechnologies);
    void setCommunicationProfile(unsigned char communicationProfile);
    void setApplicationId(int  appId);
    void setApplicationMessageType(unsigned char msgType);
    void setMessageSequenceNumber(int seqNo);


protected:
    stationID_t senderID;
    icstime_t generationTime;
    messageType_t communicationType;
    TApplicationMessageDestination destination;
    unsigned short applicationMessageLength;
    unsigned char preferredTechnologies;
    unsigned char communicationProfile;
    int applicationId;
    unsigned char applicationMessageType;
    int messageSequenceNumber;

};


/* ******************************
 *
 *
 *     ReceivedMessage CLASS
 *
 *
 * ******************************
 */

/**
* @class
* @brief .
*/
class ReceivedMessage {
public:
		ReceivedMessage()
		{
			//I can delete the payload even if never assigned
			payload = NULL;
		}
		;
    virtual ~ReceivedMessage() {};

    messageType_t getMessageType() const;
    actionID_t getActionID() const;
    const vector<Receiver>& getReceiversList() const;
    const FacilityMessagePayload* getPayload() const;

    void setMessageType(messageType_t type);
    void setActionID(actionID_t actionID);
    void setPayload(FacilityMessagePayload* payload);
    void setReceivers(vector<Receiver>& receivers);
    void addReceivers(vector<Receiver>& receivers);

    void deleteReceiver(stationID_t recStation);
    void deleteReceiversList();

    bool isStationAmongReceivers(stationID_t stationID);

private:
    messageType_t messageType;
    actionID_t actionID;
    FacilityMessagePayload* payload;
    vector<Receiver>receivers;
};

}

#endif /* FACILITYMESSAGES_H_ */

