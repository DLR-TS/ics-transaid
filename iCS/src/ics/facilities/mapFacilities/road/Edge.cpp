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
/// @file    Edge.cpp
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

#include "Edge.h"

#include <cfloat>

namespace ics_facilities {

Edge::Edge() {
    elementType = EDGE;
    area2DType = ROADELEMENT;
    ID = "";
}

Edge::Edge(roadElementID_t edgeID) {
    elementType = EDGE;
    area2DType = ROADELEMENT;
    ID = edgeID;
}

Edge::~Edge()  { }

roadElementID_t Edge::getID() const {
    return ID;
}

roadElementType Edge::getRoadElementType() const {
    return elementType;
}

Area2DType Edge::getArea2DType() const {
    return area2DType;
}

const vector<roadElementID_t>& Edge::getLaneIDs() const {
    return laneIDs;
}

void Edge::setLaneIDs(vector<roadElementID_t>* pLaneIDs) {
    if (!laneIDs.empty()) {
        laneIDs.clear();
    }
    laneIDs = *pLaneIDs;
    return;
}

bool Edge::containsLane(roadElementID_t laneID) {
    for (unsigned int i = 0; i < laneIDs.size(); i++) {
        if (laneIDs[i] == laneID) {
            return true;
        }
    }
    return false;
}

}
