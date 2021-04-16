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
/// @file    FacilitiesManager.h
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

#ifndef FACILITIESMANAGER_H_
#define FACILITIESMANAGER_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "./facilities/ICSFacilities.h"

// Namespace of the classes defined inside the iCS Facilities.
using namespace ics_facilities;

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class FacilitiesManager
 * @brief Interface between the iCS Facilities blocks and the rest of the iCS.
 */
class FacilitiesManager {
public:

    /**
     * @brief Constructor.
     */
    FacilitiesManager();

    /**
     * @brief Destructor.
     */
    virtual ~FacilitiesManager();

    // ===============================================================
    // ====================== Configuration methods ==================
    // ===============================================================

    /**
     * @brief Configures the iCS Facilities by reading the settings from the configuration file.
     * @param[in] facilitiesConfigFilename String of the main configuration file of the facilities.
     * @return true if the configuration succeeded.
     * @return false if the configuration failed in any of the configuration tasks.
     */
    bool configureICSFacilities(string facilitiesConfigFilename);

    // ===============================================================
    // ====================== Map related methods ====================
    // ===============================================================

    // ******** From the ICSFacilities

    /**
     * @brief Updates the traffic like status (green, yellow or red).
     * @param[in] tlID ID of the traffic light
     * @param[in] tlSignal signal of the traffic light
     * @param[in] info Information associated to it.
     */
    void updateTrafficLightDynamicInformation(const trafficLightID_t& tlID, std::string& state);

    // ******** Class methods

    /**
     * @brief Returns the status of the lane (set to OPEN by default).
     * @param[in] laneID ID of the lane.
     * @return laneStatus Status of the lane.
     */
    laneStatus getLaneStatus(roadElementID_t laneID) const;

    /**
     * @brief Returns the weight of the lane (set to 0 by default). This weight can be used for several algorithms.
     * @param[in] laneID ID of the lane.
     * @return laneWeight_t Weight of the lane.
     */
    laneWeight_t getLaneWeight(roadElementID_t laneID) const;

    /**
     * @brief Returns the maximum speed on the lane (set to 10 m/s by default).
     * @param[in] laneID ID of the lane.
     * @return maxSpeed_t Maximum speed.
     */
    maxSpeed_t getLaneMaxSpeed(roadElementID_t laneID) const;

    /**
     * @brief Returns the total length of the lane.
     * @param[in] ID of the lane.
     * @return Length.
     */
    laneLength_t getLaneLength(roadElementID_t laneID) const;

    /**
     * @brief Returns the shape of the lane as vector of points that are connected by straight segments.
     * @param[in] ID of the lane.
     * @return vector of points.
     */
    const vector<Point2D>& getLaneShape(roadElementID_t laneID) const;

    /**
     * @brief Returns the IDs of the lanes that merge into the one in input.
     * @param[in] ID of the lane.
     * @return vector pointer containing the IDs of the previous lanes.
     */
    vector<roadElementID_t>* getLanePrevLanesIDs(roadElementID_t laneID);

    /**
     * @brief Returns the IDs of the lanes that departs from the one in input.
     * @param[in] ID of the lane.
     * @return vector pointer containing the IDs of the next lanes.
     */
    vector<roadElementID_t>* getLaneNextLanesIDs(roadElementID_t laneID);

    /**
     * @brief Returns the edge ID (road name) where the input lane is.
     * @param[in] ID of the lane.
     */
    roadElementID_t getEdgeIDFromLane(roadElementID_t laneID) const;

    /**
     * @brief Returns the junction ID where the input lane is.
     * @param[in] ID of the lane.
     */
    roadElementID_t getJunctionIDFromLane(roadElementID_t laneID) const;

    /**
     * @brief Returns the traffic light ID that controls the input lane.
     * @param[in] ID of the lane.
     */
    trafficLightID_t getIDTrafficLightControllingLane(roadElementID_t laneID) const;

    /**
     * @brief Given a point, it returns the ID of the closest lane.
     * @param[in] position of the point.
     * @return The ID of the closest lane.
     */
    roadElementID_t convertPoint2LaneID(Point2D& pos) const;

    /**
     * @brief Returns the vector of all the lanes that connect two junctions.
     * @param[in] IDs of the two junctions.
     * @return pointer of the vector that contains the ID of the lanes.
     */
    vector<roadElementID_t>* getLanesIDsFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID);

    /**
     * @brief Returns the vector containing the IDs of the lanes associated to the input edge.
     * @param[in] ID of the edge.
     * @return Vector with the ID of the lanes.
     */
    const vector<roadElementID_t>& getLaneIDsFromEdge(roadElementID_t edgeID) const;

    /**
     * @brief Returns the ID of the edges (if more than one exists) that are between two junctions.
     * @param[in] ID of the junctions.
     * @return Pointer to a vector containing the IDs of the edges.
     */
    vector<roadElementID_t>* getEdgeIDsFromJunction(roadElementID_t junctionID_A, roadElementID_t junctionID_B);

    /**
     * @brief Returns the center of the junction.
     * @param[in] ID of the junction.
     * @return Point representing the center of the junction.
     */
    const Point2D& getJunctionCenter(roadElementID_t junctionID) const;

    /**
     * @brief Returns the lanes that lead to the input junction.
     * @param[in] ID of the junction.
     * @return Vector containing the IDs of the junction's incoming lanes.
     */
    const vector<roadElementID_t>& getJunctionIncomingLaneIDs(roadElementID_t junctionID) const;

    /**
     * @brief Returns the lanes that are internal with respect to the input junction.
     * @param[in] ID of the junction.
     * @return vector containing the IDs of the junction's internal lanes.
     */
    const vector<roadElementID_t>& getJunctionInternalLaneIDs(roadElementID_t junctionID) const;

    /**
     * @brief Returns the closest junction (Euclidean distance) to a given point.
     * @param[in] point position.
     * @return ID of the resulting closest junction.
     */
    roadElementID_t getClosestJunctionID(Point2D& pos);

    /**
     * @brief Return the neighboring junctions (i.e. connected at most one road segment) to the given junction.
     * @param[in] junctionID Identity of the input junction
     * @return Vector of IDs of the close junctions.
     */
    const vector<roadElementID_t> getNeighboringJunctions(roadElementID_t junctionID) const;

    /**
     * @brief Returns the ID of the junctions connected to the input edge (road segment).
     * @param[in] ID of the edge.
     * @return Pointer to a vector containing the junction IDs.
     */
    vector<roadElementID_t>* getJunctionsIDsFromEdge(roadElementID_t edgeID);

    /**
     * @brief Sets the status of the input lane.
     * @param[in] ID of the lane and new status.
     * @return void.
     */
    void setLaneStatus(roadElementID_t laneID, laneStatus newStatus);

    /**
     * @brief Gets the edge ID where the lane is.
     * @param[in] LaneID ID of the lane.
     * @return ID of the edge, if found.
     */
    roadElementID_t getEdgeIDFromLane(roadElementID_t laneID);

    /**
     * @brief Gets the junction ID where the lane is.
     * @param[in] ID of the lane.
     * @return ID of the junction, if found.
     */
    roadElementID_t getJunctionIDFromLane(roadElementID_t laneID);

    /**
     * @brief Sets the weight of that lane (set to 0 by default).
     * @param[in] ID of the lane and weight.
     * @return void.
     */
    void setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight);

    /**
     * @brief Returns the Latitude of the origin coordinate of the map.
     * @return latitude_t latitude value.
     */
    latitude_t getLat0();

    /**
     * @brief Returns the Longitude of the origin coordinate of the map.
     * @return longitude_t longitude value.
     */
    longitude_t getLon0();

    /**
     * @brief Returns the Altitude of the origin coordinate of the map.
     * @return altitude_t altitude value.
     */
    altitude_t getAlt0();

    // ===============================================================
    // ====================== Stations related methods ===============
    // ===============================================================

    // ******** From the ICSFacilities

    /**
     * @brief Updates the dynamic information (position, speed, etc...) related to mobile stations.
     * @param[in] ID of the station and information to update.
     * @return void.
     */
    void updateMobileStationDynamicInformation(stationID_t stationId, TMobileStationDynamicInfo info);

    /**
     * @brief Enables the radio access technology of a given station, if the technology is available.
     * @param[in] ID of the station and RAT ID to be enabled.
     * @return true if enabled, false if not available.
     */
    bool enableRAT(stationID_t stationID, RATID RATtoBeEnabled);

    /**
     * @brief Disables the radio access technology of a given station, if the technology is available.
     * @param[in] ID of the station and RAT ID to be disabled.
     * @return true if disabled, false if not available.
     */
    bool disableRAT(stationID_t stationID, RATID RATtoBeDisabled);

    /**
     * @brief Enables the radio access technology in all the stations, if the technology is available.
     * @param[in] RAT ID to be enabled.
     * @return true if all the stations have enabled it, false otherwise.
     */
    bool enableRATAllStations(RATID RATtoBeEnabled);

    /**
     * @brief Disables the radio access technology in all the stations, if the technology is available.
     * @param[in] RAT ID to be disabled.
     * @return true if all the stations have disabled it, false otherwise.
     */
    bool disableRATAllStations(RATID RATtoBeDisabled);

    /**
     * @brief Returns the default penetration rates for all the radio access technologies.
     * @return a map containing the radio access technology (key) and the penetration rate (value).
     */
    const map<RATID, float>& getDefaultPenetrationRates() const;

    /**
     * @brief Returns the default communication profiles (used by the network simulator) for all the radio access technologies.
     * @return a map containing the radio access technology (key) and the associated communication profile (value).
     */
    const map<RATID, string>& getDefaultCommunicationProfiles() const;

    // ******** Class methods

    /**
     * @brief Returns the type of the station (i.e., mobile or fixed station).
     * @param[in] ID of the station.
     * @return Type of station.
     */
    const icsstationtype_t getStationType(stationID_t stationID) const;

    /**
     * @brief Returns the current position of the station.
     * @param[in] ID of the station.
     * @return Position of station.
     */
    const Point2D& getStationPosition(stationID_t stationID) const;

    /**
     * @brief Returns the current X position of the station.
     * @param[in] ID of the station.
     * @return X-position of station.
     */
    float getStationPositionX(stationID_t stationID) const;

    /**
     * @brief Returns the current Y position of the station.
     * @param[in] ID of the station.
     * @return Y-position of station.
     */
    float getStationPositionY(stationID_t stationID) const;

    /**
     * @brief Returns the radio access technologies available on the station.
     * @param[in] ID of the station.
     * @return Pointer to the vector of radio access technology IDs.
     */
    vector<RATID>* getStationRATs(stationID_t stationID);

    /**
     * @brief Returns the radio access technologies active on the station.
     * @param[in] ID of the station.
     * @return Pointer to the vector of radio access technology IDs.
     */
    vector<RATID>* getStationActiveRATs(stationID_t stationID);

    /**
     * @brief Checks if a station has a certain RAT available.
     * @param[in] ID of the station and interested RAT.
     * @return true if the technology is available (but it does not specify if it also active).
     */
    bool isRATdefinedInStation(stationID_t stationID, RATID technology);

    /**
     * @brief Checks if a station has a certain RAT active.
     * @param[in] ID of the station and interested RAT.
     * @return true if the technology is active.
     */
    bool isRATactiveInStation(stationID_t stationID, RATID technology);

    /**
     * @brief Returns the speed of the mobile station.
     * @param[in] ID of the station.
     * @return Speed of the station.
     */
    speed_t getMobileStationSpeed(stationID_t stationID);

    /**
     * @brief Returns the acceleration of the mobile station.
     * @param[in] ID of the station.
     * @return Acceleration of the station.
     */
    acceleration_t getMobileStationAcceleration(stationID_t stationID);

    /**
     * @brief Returns the direction (as angle from the North) of the mobile station.
     * @param[in] ID of the station.
     * @return Direction of the station.
     */
    direction_t getMobileStationDirection(stationID_t stationID);

    /**
     * @brief Returns the length of the mobile station.
     * @param[in] ID of the station.
     * @return Length of the station.
     */
    stationSize_t getMobileStationVehicleLegth(stationID_t stationID);

    /**
     * @brief Returns the width of the mobile station.
     * @param[in] ID of the station.
     * @return Width of the station.
     */
    stationSize_t getMobileStationVehicleWidth(stationID_t stationID);

    /**
     * @brief Returns the height of the mobile station.
     * @param[in] ID of the station.
     * @return Height of the station.
     */
    stationSize_t getMobileStationVehicleHeight(stationID_t stationID);

    /**
     * @brief Returns the state of a mobile station's exterior lights (default false).
     * @param[in] ID of the station.
     * @return true is the exterior lights are on, false otherwise.
     */
    bool getMobileStationExteriorLights(stationID_t stationID);

    /**
     * @brief Returns the ID of the lane where a station is at the current simulation step.
     * @param[in] ID of the station.
     * @return ID of the lane.
     */
    roadElementID_t getMobileStationLaneID(stationID_t stationID);

    /**
     * @brief Returns the communication profile of a given fixed station.
     * @param[in] ID of the station.
     * @return Communication profile name (useful for the network simulator).
     */
    string getFixedStationCommunicationProfile(stationID_t stationID);

    /**
     * @brief Returns the number of stations ever participated to the simulation scenario up to the current time step.
     * @return Number of stations.
     */
    unsigned int getStationsNumber();

    /**
     * @brief Returns the number of mobile stations ever entered in the simulation scenario up to the current time step.
     * @return Number of mobile stations.
     */
    unsigned int getMobileStationsNumber();

    /**
     * @brief Returns the number of fixed stations in the simulation scenario.
     * @return Number of fixed stations.
     */
    unsigned int getFixedStationsNumber();

    /**
     * @brief Returns the list of iCS IDs of all the stations.
     * @return Pointer to the vector that contains all the IDs.
     */
    vector<stationID_t>* getAllStationsIDs();

    /**
     * @brief Returns the list of iCS IDs of all the mobile stations.
     * @return Pointer to the vector that contains all the IDs.
     */
    vector<stationID_t>* getAllMobileStationsIDs();

    /**
     * @brief Returns the list of iCS IDs of all the fixed stations.
     * @return Pointer to the vector that contains all the IDs.
     */
    vector<stationID_t>* getAllFixedStationsIDs();

    /**
     * @brief Given an area, it returns the ID of the stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of station IDs
     */
    vector<stationID_t>* getStationsIDsInArea(GeometricShape& area);


    /**
     * @brief Given a vector of areas, it returns the ID of the stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of station IDs
     */
    vector<stationID_t>* getStationsIDsInArea(vector<RoadElement*>& area);

    /**
     * @brief Given an area, it returns the ID of the mobile stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of mobile station IDs
     */
    vector<stationID_t>* getMobileStationsIDsInArea(GeometricShape& area);

    /**
     * @brief Given a vector of areas, it returns the ID of the mobile stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of mobile station IDs
     */
    vector<stationID_t>* getMobileStationsIDsInArea(vector<RoadElement*>& area);

    /**
     * @brief Given an area, it returns the ID of the fixed stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of fixed station IDs
     */
    vector<stationID_t>* getFixedStationsIDsInArea(GeometricShape& area);

    /**
     * @brief Given a vector of areas, it returns the ID of the fixed stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of fixed station IDs
     */
    vector<stationID_t>* getFixedStationsIDsInArea(vector<RoadElement*>& area);

    /**
     * @brief Returns the position of all the fixed stations.
     * @return Pointer to the map containing the station IDs (key) and the stations' position (value).
     */
    map<stationID_t, Point2D>* getAllFixedStationsPositions();

    /**
     * @brief Returns the RAT of all the fixed stations. Note that each Fixed Station can have only one RAT.
     * @return  Pointer to the map containing the station IDs (key) and the stations' RAT (value).
     */
    map<stationID_t, RATID>* getAllFixedStationsRAT();

    /**
     * @brief Returns the active RAT of all the fixed stations. Note that each Fixed Station can have only one RAT.
     * @return  Pointer to the map containing the station IDs (key) and the stations' active RAT (value).
     */
    map<stationID_t, RATID>* getAllFixedStationsActiveRAT();

    /**
     * @brief Returns the current position of all the mobile stations ever entered in the scenario.
     * @return Pointer to the map containing the station IDs (key) and the stations' position (value).
     */
    map<stationID_t, Point2D>* getAllMobileStationsPositions();

    /**
     * @brief Returns the active RATs of all the mobile stations.
     * @return Pointer to the map containing the station IDs (key) and the stations' RATs (value).
     */
    map<stationID_t, vector<RATID> >* getAllMobileStationsRATs();

    /**
     * @brief Returns the active RATs of all the mobile stations.
     * @return Pointer to the map containing the station IDs (key) and the stations' active RATs (value).
     */
    map<stationID_t, vector<RATID> >* getAllMobileStationsActiveRATs();

    // ===============================================================
    // ====================== LDM related methods ====================
    // ===============================================================

    // ******** From the ICSFacilities

    /**
     * @brief Stores the received message.
     * @param[in] actionID ActionID associated to the message.
     * @param[in] receivers Vector with the IDs of the receivers.
     * @return TRUE The storage was successfully completed.
     * @return FALSE The storage was not completed.
     */
    bool storeMessage(actionID_t actionID, vector<stationID_t> receivers);

    /**
     * @brief Creates the CAM payload for a given station.
     * @param[in] stationID ID of the station.
     * @return actionID Action ID associated to the created CAM payload.
     */
    actionID_t createCAMpayload(stationID_t stationID);

    /**
     * @brief Creates the DENM payload for a given station.
     * @param[in] stationID ID of the station.
     * @param[in] expiryTime Expiration time step for the message.
     * @param[in] frequency Repetition frequency of the message
     * @param[in] reliability Reliability of the event.
     * @param[in] negation Negation of the event.
     * @param[in] causeCode Code indicating the cause of the message.
     * @param[in] severity Severity of the event.
     * @param[in] latitude Latitude of the event.
     * @param[in] longitude Longitude of the event.
     * @param[in] relevanceArea Relevance area of the message, expressed as a vector containing geometric areas and road elements.
     * @return actionID Action ID associated to the created CAM payload.
     */
    actionID_t createDENMpayload(stationID_t stationID, icstime_t expiryTime, icstime_t frequency,
                                 denmReliability_t reliability, bool negation, denmSituationCause_t causeCode, denmSituationSeverity_t severity,
                                 latitude_t latitude, longitude_t longitude, vector<Area2D*> relevanceArea);

    /**
     * @brief Create the Facility payload for the application message to be transmitted.
     * @param[in] stationID ID of the station.
     * @param[in] comType Type of the message: cam, denm, topobroadcast, geobroadcast, geoanycast, unicast, multicast.
     * @param[in] destination Structure describing the destination of the message.
     * @param[in] preferredTechnologies Vector containing the IDs of the preferred technologies.
     * @param[in] messageLength Length of the message to be sent (in bytes).
     * @param[in] communicationProfile Communication profile for the technology selector.
     * @param[in] applicationId Identifier of the application that generated the message.
     * @param[in] applicationMessageType Type of message for the application (defined by the application).
     * @param[in] messageSequenceNumber Sequence number of the message for the application.
     * @return
     */
    actionID_t createApplicationMessagePayload(stationID_t stationID, messageType_t comType,
            TApplicationMessageDestination destination, unsigned char preferredTechnologies, unsigned short messageLength,
            unsigned char communicationProfile, int applicationId, unsigned char applicationMessageType,
            int messageSequenceNumber, string& extra);

    /**
     * @brief Returns the relevant area for the messages to be saved in the received message table iFMT.
     * @return Vector containing the relevant areas expressed as geometric areas or as road elements.
     */
    const vector<Area2D*>& getRelevantArea();

    /**
     * @brief Returns the type of stations whose received messages will be stored in the received message table iFMT.
     * @return Map containing for all the station types if each of these are relevant (true) or not (false).
     */
    map<icsstationtype_t, bool>* getRelevantStationTypes() const;

    /**
     * @brief Returns the type of messages that will be stored in the received message table iFMT.
     * @return Map containing for all the messages if each of these are relevant (true) or not (false).
     */
    map<messageType_t, bool>* getRelevantMessageTypes() const;

    /**
     * @brief Returns the direction (angle from the North) that a mobile station needs to have so that the messages that it has received will be stored in the received message table iFMT.
     */
    direction_t getRelevantDirection() const;

    /**
     * @brief Returns the accuracy of the relevant direction.
     */
    direction_t getRelevantDirectionAccuracy() const;

    /**
     * @brief Returns the default number of time steps that a message will stay in the iFMT for.
     */
    icstime_t getDefaultMessageLifeInterval() const;

    /**
     * @brief Sets the relevant station type.
     * @param[in] relevStaTypes Map containing the station types (keys) and if they are relevant (boolean values).
     */
    void setRelevantStationTypes(map<icsstationtype_t, bool>& relevStaTypes);

    /**
     * @brief Sets the relevant direction.
     * @param[in] relevDirection Relevant direction value.
     * @param[in] relevDirectionAccuracy Direction accuracy.
     */
    void setRelevantDirection(direction_t relevDirection, direction_t relevDirectionAccuracy);

    /**
     * @brief Sets the default time interval before a message in the IFMT is deleted.
     * @param[in] defMesLifeInterval Number of time steps.
     */
    void setDefaultMessageLifeInterval(icstime_t defMesLifeInterval);

    /**
     * @brief Sets the message relevant area.
     * @param[in] areas Vector of pointers to the areas (either geometric or road related).
     */
    void setRelevantArea(vector<Area2D*>& areas);

    /**
     * @brief Add a relevant area to the ones already specified (if any).
     * @param[in] relevArea Pointer to the area (either geometric or road related).
     */
    void addRelevantArea(Area2D* relevArea);

    /**
     * @brief Delete a relevant area from the ones specified.
     * @param[in] relevArea Pointer to the area (either geometric or road related).
     */
    void deleteRelevantArea(Area2D* relevArea);

    /**
     * @brief Delete all the relevant areas specified.
     */
    void clearRelevantArea();

    /**
     * @brief Update the simulation time inside the Facilities block.
     * @param[in] newSimTime New simulation time.
     */
    void updateClock(icstime_t newSimTime);

    /**
     * @brief Given a vector of areas, it returns the rectangle that encloses them all.
     * @param[in] areas Vector of areas (they can be geometric shapes or road elements).
     * @return Returns the Area2D type pointer if the vector contains just one element of type geometric shape. It returns a pointer to a newly allocated rectangle otherwise.
     */
    Area2D* getWholeArea(vector<Area2D*> areas);

    /**
     * @brief Given a geometric shape or a road element, returns a circle object that includes the area.
     * @param[in] area Geometric shape or road element.
     * @return Returns a circle object that includes the area given in input.
     */
    Circle getCircleFromArea(Area2D* area);

    /**
     * @brief Given a set of geometric shapes or a road elements, returns a circle object that includes the area.
     * @param[in] areas Set of geometric shapes and road elements.
     * @return Returns a circle object that includes the area given in input.
     */
    Circle getCircleFromAreas(std::vector<TArea> areas);

    /**
     * @brief Converts a vector of TArea to a vector of Area2D*.
     * @param[in] areas Set of TArea structure.
     * @return Returns a vector of Area2D*.
     */
    std::vector<Area2D*> convertAreas(std::vector<TArea> areas);

    /**
     * @brief Converts a vector of Area2D* to a vector of TArea.
     * @param[in] areas Set of Area2D* structure.
     * @return Returns a vector of TArea structure.
     */
    std::vector<TArea> convertAreas(std::vector<const Area2D*> areas);

    /**
     * @brief Checks if the station has ever sent any message.
     * @param[in] stationID ID of the station.
     * @return TRUE The station has sent at least one message from the beginning of the simulation.
     * @return FALSE The station never sent messages.
     */
    bool hasStationEverSentMessages(stationID_t stationID);

    /**
     * @brief Returns the last sequence number (which is part of an actionID) that was generated for a given station.\
      * @param[in] stationID ID of the station.
     */
    seqNo_t getLastSeqNoForStation(stationID_t stationID);

    /**
     * @brief Returns the scheduled message deletion time.
     * @param[in] actionID Action ID that points to the target message.
     */
    icstime_t getReceivedMessageDeletionTimeFromTable(actionID_t actionID);

    /**
     * @brief Returns the list of mobile stations (ID and position) whose messages are in the LDM of a given station.
     * @param[in] receiverID ID of the receiving station.
     * @return Map containing the station IDs (keys) and positions (values).
     */
    map<stationID_t, Point2D>* getMobileStationsPositionsAroundStations(stationID_t receiverID);

    /**
     * @brief Returns the list of mobile stations (ID and position) whose messages are in the LDM of a given station and were received in a specific temporal window.
     * @param[in] receiverID ID of the receiving station.
     * @param[in] start Temporal window start time.
     * @param[in] stop Temporal window stop time.
     * @return Map containing the station IDs (keys) and positions (values).
     */
    map<stationID_t, Point2D>* getMobileStationsPositionsAroundStationsInTemporalWindow(stationID_t receiverID,
            icstime_t start, icstime_t stop);

    /**
     * @brief Returns the list of fixed stations (ID and position) whose messages are in the LDM of a given station.
     * @param[in] receiverID ID of the receiving station.
     * @return Map containing the station IDs (keys) and positions (values).
     */
    map<stationID_t, Point2D>* getFixedStationsPositionsAroundStations(stationID_t receiverID);

    /**
     * @brief Returns the list of fixed stations (ID and position) whose messages are in the LDM of a given station and were received in a specific temporal window.
     * @param[in] receiverID ID of the receiving station.
     * @param[in] start Temporal window start time.
     * @param[in] stop Temporal window stop time.
     * @return Map containing the station IDs (keys) and positions (values).
     */
    map<stationID_t, Point2D>* getFixedStationsPositionsAroundStationsInTemporalWindow(stationID_t receiverID,
            icstime_t start, icstime_t stop);

    // ******** Class methods

    /**
     * @brief Returns the type of the message with a given Action ID.
     * @param[in] actionID Reference Action ID.
     */
    messageType_t getReceivedMessageType(actionID_t actionID);

    /**
     * @brief Returns the sender of the message with a given Action ID.
     * @param[in] actionID Reference Action ID.
     */
    stationID_t getReceivedMessageSenderID(actionID_t actionID);

    /**
     * @brief Returns the generation time of the message with a given Action ID.
     * @param[in] actionID Reference Action ID.
     */
    icstime_t getReceivedMessageGenerationTime(actionID_t actionID);

    /**
     * @brief Returns the number of messages that a station has received and that are in the iFMT.
     * @param[in] stationID ID of the station.
     */
    unsigned int getNumberOfReceivedMessageInTable(stationID_t stationID);

    /**
     * @brief Returns a pointer to a vector of Action IDs of messages received from a certain time of the simulation.
     * @param[in] startTime Simulation time start.
     */
    vector<actionID_t>* getLastMessagesActionIDs(icstime_t startTime);

    /**
     * @brief Returns a pointer to a map of Action IDs (keys) and types (values) of messages received from a certain time of the simulation.
     * @param[in] startTime Simulation time start.
     */
    map<actionID_t, messageType_t>* getLastMessagesTypes(icstime_t startTime);

    /**
     * @brief Returns a pointer to a vector containing the Action IDs of the messages of a specified type.
     * @param[in] messageType Type of the messages to consider.
     */
    vector<actionID_t>* getAllSpecificMessagesActionIDs(messageType_t messageType);

    /**
     * @brief Returns a pointer to a vector that contains the Action IDs of all the messages sent by a station and that are still in the iFMT.
     * @param[in] stationID ID of the station.
     */
    vector<actionID_t>* getMessagesActionIDsFromSender(stationID_t senderID);

    /**
     * @brief Returns the positions of the receivers when they received the message.
     * @param[in] actionID Action ID of the message.
     * @return Pointer to the map containing the station IDs (keys) and the associated positions (values).
     */
    map<stationID_t, Point2D>* getReceiversPositionAtReceptionTime(actionID_t actionID);

    /**
     * @brief Returns the reception times of a message for all the receivers.
     * @param[in] actionID Action ID of the message.
     * @return Pointer to the map containing the station IDs (keys) and the associated reception times (values).
     */
    map<stationID_t, icstime_t>* getReceiversReceptionTime(actionID_t actionID);

    /**
     * @brief Returns the preferred RATs for a given application message.
     * @param[in] actionID Action ID of the message.
     * @return Unsigned char indicating the preferred RATs (1=preferred. The position of the technologies are determined by enum RATID).
     */
    unsigned char getApplicationMessagePreferredTechnologies(actionID_t actionID);

    /**
     * @brief Returns the size in bytes of the application message.
     * @param[in] actionID Action ID of the message.
     */
    unsigned short getApplicationMessageLength(actionID_t actionID);

    /**
     * @brief Returns the information about the destination of an application message.
     * @param[in] actionID Action ID of the message.
     */
    TApplicationMessageDestination getApplicationMessageDestination(actionID_t actionID);

    /**
     * @brief Returns a pointer to a vector containing the information about all the relevant CAM messages received by a certain station.
     * @param[in] stationID ID of the receiving station.
     */
    vector<TCamInformation>* getInfoFromLastCAMsReceivedByStation(stationID_t stationID);

    /**
     * @brief Returns the pointer to the received message that has a given Action ID.
     * @param[in] actionID Reference Action ID.
     */
    ReceivedMessage* getReceivedMessage(actionID_t actionID) const;

    /**
     * @brief Returns the pointer to the payload of a received message with a given Action ID.
     * @param[in] actionID Reference Action ID.
     */
    FacilityMessagePayload* getReceivedMessagePayload(actionID_t actionID) const;

    /**
     * @brief Returns the pointer to a given station (NULL if the station does not exist).
     * @param[in] stationID ID of the station.
     */
    const Station* getStation(stationID_t stationID) const;

    /**
     * @brief Returns a map structure with the ids of the stations (keys) and the pointer to them (values).
     */
    const map<stationID_t, Station*>& getAllStations() const;

    /**
     * @brief Returns the pointer to the traffic light.
     * @param[in] trafficLightID ID of the traffic light.
     * @return Pointer to the traffic light object.
     */
    const TrafficLight* getTrafficLight(trafficLightID_t trafficLightID) const;

    Point2D  getStationPositionsFromMobilityHistory(icstime_t time, stationID_t stationId);

    map<stationID_t, const Station*>* getStationsInArea(GeometricShape& area);


private:

    /**
     * @brief pointer to the iCS facilities block.
     */
    ICSFacilities* facilities;

    //====== MAP RELATED methods (from the ICSFacilities) ======

    /**
     * @brief Returns the pointer to the lane.
     * @param[in] laneID ID of the lane.
     * @return Pointer to the lane object.
     */
    const Lane* getLane(roadElementID_t laneID) const;

    /**
     * @brief Returns the pointer to the edge.
     * @param[in] edgeID ID of the edge.
     * @return Pointer to the edge object.
     */
    const Edge* getEdge(roadElementID_t edgeID) const;

    /**
     * @brief Returns the pointer to the junction.
     * @param[in] junctionID ID of the junction.
     * @return Pointer to the junction object.
     */
    const Junction* getJunction(roadElementID_t junctionID) const;

    /**
     * @brief Returns the pointer to the edge (road segment) where the input lane is.
     * @param[in] laneID ID of the lane.
     * @return Pointer to the edge object.
     */
    const Edge* getEdgeFromLane(roadElementID_t laneID) const;

    /**
     * @brief Returns the pointer to the junction if the lane is inside a junction (null otherwise).
     * @param[in] laneID ID of the lane.
     * @return Pointer to the junction object.
     */
    const Junction* getJunctionFromLane(roadElementID_t laneID) const;

    /**
     * @brief Returns the pointer to the lane that is the closest to a point.
     * @param[in] pos X-Y point.
     * @return Pointer to the lane object.
     */
    const Lane* convertPoint2Map(Point2D& pos) const;

    /**
     * @brief Returns the pointer to the closest junction (by Euclidean distance) that is the closest to a point.
     * @param[in] pos X-Y point.
     * @return Pointer to the junction object.
     */
    const Junction* getClosestJunction(Point2D pos) const;

    /**
     * @brief Returns a vector containing the pointers to the edges that connect two junctions (the order of the junction matters).
     * @param[in] junctionID_A First junction.
     * @param[in] junctionID_B Second junction.
     * @return Pointer to the vector of pointers of edge objects.
     */
    vector<const Edge*>* getEdgesFromJunction(roadElementID_t junctionID_A, roadElementID_t junctionID_B);

    /**
     * @brief Returns a vector containing the pointers to the lanes that connect two junctions (the order of the junction matters).
     * @param[in] junctionID_A First junction.
     * @param[in] junctionID_B Second junction.
     * @return Pointer to the vector of pointers of lane objects.
     */
    vector<const Lane*>* getLanesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID);

    /**
     * @brief Returns a vector containing the pointers to the junctions that are at the beginning and end of a given edge.
     * @param[in] edgeID ID of the Edge.
     * @return Pointer to the vector of pointers of junction objects.
     */
    vector<const Junction*>* getJunctionsFromEdge(roadElementID_t edgeID);

    //====== STATIONS RELATED methods (from the ICSFacilities) ======

    /**
     * @brief Returns the pointer to a given mobile station (NULL if the station does not exist).
     * @param[in] mobileStationID ID of the mobile station.
     */
    const MobileStation* getMobileStation(stationID_t mobileStationID);

    /**
     * @brief Returns the pointer to a given fixed station (NULL if the station does not exist).
     * @param[in] fixedStationID ID of the fixed station.
     */
    const FixedStation* getFixedStation(stationID_t fixedStationID);

    /**
     * @brief Returns a pointer to a map structure with the ids of the mobile stations (keys) and the pointer to them (values).
     */
    map<stationID_t, const MobileStation*>* getAllMobileStations();

    /**
     * @brief Returns a pointer to a map structure with the ids of the fixed stations (keys) and the pointer to them (values).
     */
    map<stationID_t, const FixedStation*>* getAllFixedStations();

    /**
     * @brief Given a vector of areas, it returns the pointers to the stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of pointers to the stations
     */
    //map<stationID_t, const Station*>* getStationsInArea(GeometricShape &area);

    /**
     * @brief Given a vector of areas, it returns the pointers to the stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of pointers to the stations
     */
    map<stationID_t, const Station*>* getStationsInArea(vector<RoadElement*>& area);

    /**
     * @brief Given a vector of areas, it returns the pointers to the mobile stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of pointers to the stations
     */
    map<stationID_t, const MobileStation*>* getMobileStationsInArea(GeometricShape& area);

    /**
     * @brief Given a vector of areas, it returns the pointers to the mobile stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of pointers to the stations
     */
    map<stationID_t, const MobileStation*>* getMobileStationsInArea(vector<RoadElement*>& area);

    /**
     * @brief Given a vector of areas, it returns the pointers to the fixed stations inside.
     * @param[in] Area expressed as geometric shape (circle, rectangle, ellipse, or convex polygon)
     * @return Pointer to the vector of pointers to the stations
     */
    map<stationID_t, const FixedStation*>* getFixedStationsInArea(GeometricShape& area);

    /**
     * @brief Given a vector of areas, it returns the pointers to the fixed stations inside.
     * @param[in] Vector of area pointers expressed as road elements (i.e., lane, edge or junctions).
     * @return Pointer to the vector of pointers to the stations
     */
    map<stationID_t, const FixedStation*>* getFixedStationsInArea(vector<RoadElement*>& area);

    //====== MESSAGES RELATED methods (from the ICSFacilities) ======

    /**
     * @brief It returns the pointers to all the stored messages that are received by a given station.
     * @param[in] stationID ID of the station.
     * @return map containing the action IDs (keys) and pointers to the received messages (values).
     */
    map<actionID_t, ics_facilities::ReceivedMessage*>* getLDM(stationID_t stationID);

    /**
     * @brief It returns the pointers to all the stored messages that have been received from a certain time.
     * @param[in] startTime Starting simulation step of the message report.
     * @return map containing the action IDs (keys) and pointers to the received messages (values).
     */
    map<actionID_t, ics_facilities::ReceivedMessage*>* getLastMessages(icstime_t startTime);

    /**
     * @brief It returns the pointers to all the stored messages of a certain type.
     * @param[in] messageType Type of the messages to be reported.
     * @return map containing the action IDs (keys) and pointers to the received messages (values).
     */
    map<actionID_t, ics_facilities::ReceivedMessage*>* getAllSpecificMessages(messageType_t messageType);

    /**
     * @brief Returns a pointer to a map that contains the pointers to all the messages sent by a station and that are still in the iFMT.
     * @param[in] stationID ID of the station.
     */
    map<actionID_t, ics_facilities::ReceivedMessage*>* getMessagesFromSender(stationID_t senderID);

    /**
     * @brief Returns all the CAM messages sent by a station that were received by at least one other station.
     * @param[in] stationID ID of the station.
     * @return Vector of pointers to the CAM messages.
     */
    vector<CAMPayloadGeneral*>* getGeneratedCAMsFromStationInTable(stationID_t stationID);

    /**
     * @brief Returns the pointer to the last CAM that was generated by a given station and received by at least one other.
     * @param[in] stationID ID of the station.
     */
    CAMPayloadGeneral* getLastGeneratedCAM(stationID_t stationID);

    /**
     * @brief Returns a vector with the pointers to the last (during the current sim step) CAM messages that have been received by a given station.
     * @param[in] stationID ID of the station.
     */
    vector<CAMPayloadGeneral*>* getLastCAMsReceivedByStation(stationID_t stationID);

};

} // ics namespace

#endif /* FACILITIESMANAGER_H_ */
