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
/// @file    vehicle-node.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef VEHICLE_NODE_H
#define VEHICLE_NODE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "itetris-node.h"

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class VehicleNode
 * @brief Represents a vehicle in the simulation.
 */
class VehicleNode : public ITetrisNode {
public:

    /**
    * @brief Constructor.
    * @param[in] nodeId Vehicle identifier.
    */
    VehicleNode(const std::string& nodeId);

    /// @brief True if the vehicle moved since the last time-step.
    bool m_moved;

    /**
    * @brief Changes the current speed of the vehicle and returns the acceleration.
    * @param[in] speed The new speed.
    * @return The acceleration of the vehicle since the last speed update.
    * @todo The division by 1 has to be changed if the value is in a subsecond basis.
    */
    float ChangeSpeed(float speed);

    /**
    * @brief Checks if the vehicle changed its position since the last timestep.
    * @param[in] position Position to compare
    * @return True if the vehicle moved.
    */
    bool CheckPosition(std::pair<float, float> position);

    /**
    * @brief Returns the current speed of the vehicle.
    * @return The current speed.
    */
    float GetSpeed() const;

    /**
    * @brief Returns the current direction of the vehicle.
    * @return The current heading.
    */
    float GetHeading() const;

    float GetDirection() const;

    float GetAcceleration() const;

    /// @brief Type of vehicle in SUMO
    std::string m_SumoType;
    /// @brief Class of vehicle in SUMO
    std::string m_SumoClass;


private:

    /// @brief Speed value in the last time step.
    float m_lastSpeed;

    /// @brief Speed of the vehicle int he current time step.
    float m_currentSpeed;

};

}

#endif
