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
/// @file    Junction.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Junction.h"

namespace ics_facilities {

Junction::Junction(roadElementID_t junctionID) {
    elementType = JUNCTION;
    area2DType = ROADELEMENT;
    ID = junctionID;
}

Junction::~Junction() {
    elementType = JUNCTION;
    area2DType = ROADELEMENT;
}

roadElementID_t                 Junction::getID() const {
    return ID;
}

roadElementType                 Junction::getRoadElementType() const {
    return elementType;
}

Area2DType                      Junction::getArea2DType() const {
    return area2DType;
}

const Point2D&                  Junction::getCenter() const {
    return center;
}

const vector<roadElementID_t>&  Junction::getIncomingLaneIDs() const {
    return incomingLanesID;
}

const vector<roadElementID_t>&  Junction::getInternalLaneIDs() const {
    return internalLanesID;
}

const vector<roadElementID_t>&  Junction::getNeighboringJunctions() const {
    return neighboringJunctions;
}

void                            Junction::setCenter(Point2D center) {
    this->center = center;
}

void                            Junction::setIncomingLaneIDs(vector<roadElementID_t> IncomingLanesID) {
    if (!incomingLanesID.empty()) {
        incomingLanesID.clear();
    }
    incomingLanesID = IncomingLanesID;
    return;
}

void                            Junction::setInternalLaneIDs(vector<roadElementID_t> InternalLanesID) {
    if (!internalLanesID.empty()) {
        internalLanesID.clear();
    }
    internalLanesID = InternalLanesID;
    return;
}

void                            Junction::addNeighboringJunction(roadElementID_t junctionID) {
    if (junctionID == "") {
        return;
    }
    for (unsigned int i = 0; i < neighboringJunctions.size(); i++) {
        if (neighboringJunctions[i] == junctionID) {
            return;
        }
    }
    neighboringJunctions.push_back(junctionID);
    return;
}

bool                            Junction::containsLane(roadElementID_t laneID) {
    for (unsigned int i = 0; i < internalLanesID.size(); i++) {
        if (internalLanesID[i] == laneID) {
            return true;
        }
    }
    return false;
}

bool                            Junction::connectedToLane(roadElementID_t laneID) {
    for (unsigned int i = 0; i < incomingLanesID.size(); i++) {
        if (incomingLanesID[i] == laneID) {
            return true;
        }
    }
    return false;
}

}
