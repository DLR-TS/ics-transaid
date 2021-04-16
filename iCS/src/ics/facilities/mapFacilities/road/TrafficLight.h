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
/// @file    TrafficLight.h
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

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../../../utils/ics/iCStypes.h"
#include <vector>
using namespace ics_types;

namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TrafficLightLane
 * @brief Represents a lane in a traffic light.
 */
class TrafficLightLane {
public:
    /**
     * @brief Constructor.
     */
    TrafficLightLane();

    /**
     * @brief Destructor.
     */
    virtual ~TrafficLightLane();

    /**
     * @brief Returns the ID of the lane controlled by the traffic light.
     */
    roadElementID_t getControlledLaneID() const;

    /**
     * @brief Returns the ID of the lane that follows the controlled lane.
     */
    roadElementID_t getFollowingLaneID() const;

    /**
     * @brief Returns the status of the traffic light.
     */
    tlStatus getStatus() const;

    /**
     * @brief Check if the traffic light is active.
     * @return TRUE The traffic light is active.
     * @return FALSE The traffic light is not active.
     */
    bool isActive() const;

    /**
     * @brief Sets the status of the traffic light.
     * @param[in] newStatus
     */
    void setStatus(tlStatus newStatus);

    /**
     * @brief Sets the ID of the lane controlled by the traffic light.
     * @param[in] contrLaneID ID of the lane controlled by the traffic light.
     */
    void setControlledLaneID(roadElementID_t& contrLaneID);

    /**
     * @brief Sets the ID of the lane that follows the controlled lane of the traffic light.
     * @param[in] followLaneID ID of the lane that follows the lane after the controlled one.
     */
    void setFollowingLaneID(roadElementID_t& followLaneID);

    /**
     * @brief Switches on or off the traffic light.
     * @param[in] active Indicates whether the traffic light is active or not (FALSE by default).
     */
    void setActive(bool active);

private:

    /// @brief ID of the lane controlled by the traffic light.
    roadElementID_t m_controlledLaneID;

    /// @brief ID of the lane that follows the lane after the controlled one.
    roadElementID_t m_followingLaneID;

    /// @brief Status of the traffic light (UNKNOWN by default).
    tlStatus m_currentStatus;

    /// @brief Indicates whether the traffic light is active or not (FALSE by default).
    bool m_active;
};

/**
 * @class TrafficLight
 * @brief Represents a traffic light.
 */
class TrafficLight {
public:

    /**
     * @brief Constructor.
     * @param[in] trafficLightID ID of the traffic light to be created.
     * */
    TrafficLight(trafficLightID_t trafficLightID);

    /**
     * @brief Destructor.
     */
    virtual ~TrafficLight();

    /**
     * @brief Returns the ID of the traffic light.
     */
    trafficLightID_t getId() const;

    /**
     * @brief Returns the position of the traffic light.
     */
    const Point2D& getPosition() const;

    /**
     * @brief Check if the traffic light is active.
     * @return TRUE The traffic light is active.
     * @return FALSE The traffic light is not active.
     */
    bool isActive() const;

    /**
     * @brief Sets the position of the traffic light.
     * @param[in] pos Position of the traffic light.
     */
    void setPosition(Point2D pos);

    /**
     * @brief Switches on or off the traffic light.
     * @param[in] active Indicates whether the traffic light is active or not (FALSE by default).
     */
    void setActive(bool active);

    /**
     * @brief Returns all the lanes controlled by this traffic light.
     */
    const std::vector<TrafficLightLane>& getLanes() const;

    /**
     * @brief Returns the state of all the lanes of this traffic light
     */
    const std::string& getState() const;

    /**
     * @brief Set the state of all the lanes of this traffic light
     * @param[in] state the new state of all the lanes of the traffic light.
     * @return FALSE if the number of lanes in the state given as input is different form the number of lanes in the collection.
     */
    bool setState(const std::string& state);

    /**
     * @brief Activate of deactivate all the lanes.
     */
    void setActiveAllLanes(bool active);

    /**
     * @brief Add a new lane to this traffic light.
     * @param[in] newLane the new lane to add.
     */
    void addTrafficLightLane(TrafficLightLane& newLane);

    /**
     * @brief Convert a char rRgGyYoO to the appropriate state.
     */
    static tlStatus GetStatusFromChar(char statusChar);

    static TrafficLight CreateTrafficLight(std::string id);

private:

    /// @brief ID of the traffic light.
    trafficLightID_t m_id;

    /// @brief Position of the traffic light.
    Point2D position;

    /// @brief Indicates whether the traffic light is active or not (FALSE by default).
    bool m_active;

    /// @brief Complete state of all lanes of this traffic light. rRgGyYoO format
    std::string m_state;

    /// @brief Collection of lanes controlled by this traffic light
    std::vector<TrafficLightLane> m_lanes;
};

} //namespace

#endif /* TRAFFICLIGHT_H_ */

