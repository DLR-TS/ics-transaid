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

void Edge::setLaneIDs(vector<roadElementID_t> *pLaneIDs) {
    if (!laneIDs.empty())
        laneIDs.clear();
    laneIDs = *pLaneIDs;
    return;
}

bool Edge::containsLane(roadElementID_t laneID) {
    for (unsigned int i=0; i<laneIDs.size(); i++) {
        if (laneIDs[i] == laneID)
            return true;
    }
    return false;
}

}
