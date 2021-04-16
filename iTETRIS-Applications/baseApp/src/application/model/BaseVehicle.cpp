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
 * BaseVehicle.cpp
 *
 *  Created on: Feb 14, 2020
 *      Author: leo, matthias
 */

#include <cmath>
#include <iostream>
#include <algorithm>
#include "BaseVehicle.h"
#include "application/model/headers.h"

namespace baseapp {
namespace application {

BaseVehicle::BaseVehicle()
    : xPosition(-1),
      speed(-1),
      accel(0),
      xPosition_received(xPosition),
      speed_received(speed),
      accel_received(accel),
      laneIndex(-1),
      length(-1),
      minGap(-1),
      sumoID(""),
      icsID(-1),
      automationType(AT_UNKNOWN),
      lastUpdateTime(-1),
      lastExtrapolationTime(-1),
      initialInformationReceived(false) {}

BaseVehicle::BaseVehicle(double pos, double speed, double accel, int laneIndex,
                         double length, double minGap, const std::string& sumoID,
                         int icsID, AutomationType automationType)
    : xPosition(pos),
      speed(speed),
      accel(accel),
      xPosition_received(xPosition),
      speed_received(speed),
      accel_received(accel),
      laneIndex(laneIndex),
      length(length),
      minGap(minGap),
      sumoID(sumoID),
      icsID(icsID),
      automationType(automationType),
      lastUpdateTime(-1),
      lastExtrapolationTime(-1),
      initialInformationReceived(true) {}

BaseVehicle::BaseVehicle(const std::string& sumoID, int icsID)
    : xPosition(-1),
      speed(-1),
      accel(0),
      xPosition_received(xPosition),
      speed_received(speed),
      accel_received(accel),
      laneIndex(-1),
      length(-1),
      minGap(-1),
      sumoID(sumoID),
      icsID(icsID),
      automationType(AT_UNKNOWN),
      lastUpdateTime(-1),
      lastExtrapolationTime(-1),
      initialInformationReceived(false) {}

double
BaseVehicle::getSpacing(const std::shared_ptr<BaseVehicle> leader) const {
    return leader->xPosition - leader->length - minGap - xPosition;
}

void
BaseVehicle::updateState(int currentTime, const baseapp::application::TransaidHeader::CamInfo* info) {
    if (lastUpdateTime > info->generationTime) {  // this CAM is outdated
        if (DEBUG_BASEVEHICLE) {
            std::cout << currentTime << " veh '" << sumoID
                      << "' received outdated CAM with generationTime "
                      << info->generationTime
                      << " (lastUpdateTime = " << lastUpdateTime << ")\n"
                      << std::endl;
        }
        return;
    }

    lastUpdateTime = info->generationTime;
    accel_received = info->acceleration;
    speed_received = info->speed;
    xPosition_received = info->position.x;
    laneIndex = info->laneIndex;

    // invalidate current estimation
    lastExtrapolationTime = -1;

    // Extrapolate received info to current state
    extrapolateState(currentTime);
}

void
BaseVehicle::extrapolateState(int currentTime) {
    if (currentTime == lastExtrapolationTime) {
        return;
    }

    if (DEBUG_BASEVEHICLE) {
        std::cout << currentTime << " extrapolateState() for veh '" << sumoID << "'\n"
                  << "   xPos = " << xPosition << ", speed = " << speed << ", accel = " << accel << ","
                  << "   lastUpdate = " << lastUpdateTime << ", lastExtrapolation = " << lastExtrapolationTime
                  << std::endl;
    }

    double dt, x0, v0, a0;
    if (lastExtrapolationTime < 0) {
        // don't rely on the current state for extrapolation
        dt = (currentTime - lastUpdateTime) * 0.001;
        x0 = xPosition = xPosition_received;
        v0 = speed = speed_received;
        a0 = accel = accel_received;
    } else {
        // continue extrapolation using the current state
        dt = (currentTime - lastExtrapolationTime) * 0.001;
        x0 = xPosition;
        v0 = speed;
        a0 = accel;
    }

    // Do the extrapolation assuming damped acceleration
    accel = a0 * pow(ACCEL_ESTIMATION_DAMPING, dt);
    speed += (accel + a0) * 0.5 * dt;
    xPosition += (speed + v0) * 0.5 * dt;

    if (DEBUG_BASEVEHICLE) {
        std::cout << "Extrapolated:\n" << " xPos = " << xPosition << ", speed = " << speed << ", accel = " << accel << std::endl;
    }

    lastExtrapolationTime = currentTime;
}

} /* namespace application */
} /* namespace baseapp */
