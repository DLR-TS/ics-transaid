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
/*
 * iCStypes.h
 *
 *  Created on: May 7, 2010
 *      Author: cataldi
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#ifndef ICSTYPES_H_
#define ICSTYPES_H_

#include "./geometric/Point2D.h"
#include "geometric/Area2D.h"

#include <string>
#include <vector>

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

namespace ics_types {

/* ******************************
 *
 *
 * iCS general related
 *
 *
 * ******************************
 */
typedef int icstime_t;
enum icsstationtype_t {
    STATION_MOBILE, STATION_FIXED
};

/* ******************************
 *
 *
 * Map related
 *
 *
 * ******************************
 */
enum ShapeType {
    CIRCLE, ELLIPSE, RECTANGLE, CONVEXPOLIGON
};
enum roadElementType {
    LANE, EDGE, JUNCTION
};
enum laneStatus {
    LANE_OPEN, LANE_CLOSED, LANE_JAMMED
};
enum tlStatus {
    GREEN, GREEN_MAIUSC, YELLOW, YELLOW_MAIUSC, RED, RED_MAIUSC, OFF, OFF_MAIUSC, UNKNOWN
};
typedef std::string roadElementID_t;
typedef std::string trafficLightID_t;
typedef float latitude_t;
typedef float longitude_t;
typedef float altitude_t;

/* ******************************
 *
 *
 * Stations related
 *
 *
 * ******************************
 */
enum RATID {
    WAVE, UMTS, WiMAX, DVBH, LTE
};
enum RSUType {
};
typedef unsigned int stationID_t;
typedef float stationSize_t;
typedef float speed_t;
typedef float acceleration_t;
typedef float direction_t;

//enum wiperSystemFront_t         {IDLE,
//                                 INTERVAL,
//                                 NORMAL,
//                                 FAST,
//                                 WASHERACTIVE};

// NOTE: the exteriorLights could be more complicated, such as the enum below,
//       but it depends whether the traffic simulator has this information
typedef bool exteriorLights_t;
//enum exteriorLights_t           {LOWBEAMHEADLIGHTSON,
//                                 HIGHBEAMHEADLIGHTSON,
//                                 LEFTTURNSIGNALON,
//                                 RIGHTTURNSIGNALON,
//                                 AUTOMATICLIGHTCONTROLON,
//                                 DAYTIMERUNNINGLIGHTON,
//                                 FOGLIGHTON,
//                                 PARKINGLIGHTSON};

enum stationType_t {
    staType_NONE,
    staType_UNKNOWN,
    staType_SPECIAL,
    staType_MOTO,
    staType_CAR,
    staType_CAROTHER,
    staType_BUS,
    staType_TRAFFICSIGNAL,
    staType_BASICRSU
};
/**
 * @struct MobileStationDynamicInfo
 * @brief Structure to share info about dynamic stations between iCS core and facilities.
 * @author Julen Maneros (CBT)
 */
struct MobileStationDynamicInfo {
    float positionX;
    float positionY;
    speed_t speed;
    acceleration_t acceleration;
    direction_t direction;
    exteriorLights_t exteriorLights;
    stationSize_t length;
    stationSize_t width;
    stationSize_t height;
    roadElementID_t lane;
    icstime_t timeStep;

} typedef TMobileStationDynamicInfo;

/**
 * @struct TrafficLightDynamicInfo
 * @brief Structure to share info about traffic light dynamic information between iCS core and facilities.
 * @author Pasquale Cataldi (EURE)
 */
struct TrafficLightDynamicInfo {
    tlStatus status;             // status of the light (green, yellow, red or unknown)
    bool active;             // true if the traffic light is working, false otherwise
} typedef TTrafficLightDynamicInfo;

/* ******************************
 *
 *
 * messages related
 *
 *
 * ******************************
 */
typedef unsigned short seqNo_t;
typedef std::string actionID_t;
typedef unsigned short protocolversion_t;
enum messageType_t {
    CAM,               // Facility message
    DENM,              // Facility message
    TOPOBROADCAST,     // Application message
    UNICAST,           // Application message
    MULTICAST,         // Application message
    GEOBROADCAST,      // Application message
    GEOUNICAST,        // Application message
    GEOANYCAST
};
// Application message
static const std::string toString(messageType_t value) {
    std::string ret = (std::string[]) {
        "CAM", "DENM", "TOPOBROADCAST", "UNICAST", "MULTICAST",
        "GEOBROADCAST", "GEOUNICAST", "GEOANYCAST"
    } [value];
    return ret;
}
typedef unsigned short rssi_t;
typedef double snr_t;
/**
 * @struct ApplicationMessageDestination
 * @brief Structure to store the destination of an application message inside the iCS Facilities
 * @author Pasquale Cataldi (EURE)
 */
struct ApplicationMessageDestination {
    std::vector<stationID_t> dest_stations;      // It should be used for MULTICAST messages
    stationID_t dest_station;       // It should be used for UNICAST messages
    std::vector<Area2D*> dest_areas;         // It should be used for GEOBROADCAST and GEOANYCAST messages
    unsigned short dest_numHops;       // It should be used for TOPOBROADCAST messages
    //Can't put it in the decostructor because it is called every time a reference dies
    void DeleteAreas() {
        for (std::vector<Area2D*>::iterator it = dest_areas.begin(); it != dest_areas.end(); ++it) {
            delete *it;
        }
        dest_areas.clear();
    }
} typedef TApplicationMessageDestination;

struct Area {
    Area2DType type;
    ShapeType shapeType;
    Area2D* shape;
    roadElementType elementType;
    roadElementID_t roadElementID;
} typedef TArea;

/**
 * @struct CamInformation
 * @brief Structure to pass information contained in CAM to the application.
 * @author Pasquale Cataldi (EURE)
 */
struct CamInformation {
    // General basic CAM profile
    stationID_t senderID;
    Point2D senderPosition;
    icstime_t generationTime;
    icsstationtype_t staType;
    // Vehicle CAM profile
    speed_t speed;
    direction_t angle;
    acceleration_t acceleration;
    stationSize_t length;
    stationSize_t width;
    int lights;
    // Location Referencing information
    roadElementID_t laneID;
    roadElementID_t edgeID;
    roadElementID_t junctionID;
    // Buffer variables to create the packet
    int camInfoBuffSize;    // Size in bytes of the above information fields (not considering camInfoSize)
} typedef TCamInformation;

/* ******************************
 *
 *
 * DENM messages related
 *
 *
 * ******************************
 */
typedef char denmReliability_t;
enum denmSituationCause_t {
    demnCause_UNKNOWN, demnCause_ROADWORKS, demnCause_BROKENDOWNVEHICLE
};
enum denmSituationSeverity_t {
    demnSeverity_LOW, demnSeverity_MIDDLE
};

/**
 * Traffic Management Application related.
 */
enum TrafficApplicationResultMessageState {
    kToBeScheduled = 0, kScheduled, kArrived, kToBeApplied, kToBeDiscarded, kMissed,
};

}

#endif /* ICSTYPES_H_ */
