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
/// @file    Junction.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef JUNCTION_H_
#define JUNCTION_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "RoadElement.h"
#include "Lane.h"

#include <vector>
using namespace std;

namespace ics_facilities {
// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Junction
 * @brief Represents a road junction.
*/
class Junction: public ics_facilities::RoadElement {
public:

    /**
    * @brief Constructor.
    * @param[in] junctionID ID of the junction to be created.
    */
    Junction(roadElementID_t junctionID);

    /**
    * @brief Destructor.
    */
    virtual ~Junction();

    /**
    * @brief Returns the ID of the junction.
    */
    roadElementID_t getID() const;

    /**
    * @brief Returns JUNCTION.
    */
    roadElementType getRoadElementType() const;

    /**
    * @brief Returns ROADELEMENT.
    */
    Area2DType getArea2DType() const;

    /**
    * @brief Returns the center point of the junction.
    */
    const Point2D& getCenter() const;

    /**
    * @brief Returns the list of incoming lanes, i.e. the ones who will end at the junction.
    */
    const vector<roadElementID_t>& getIncomingLaneIDs() const;

    /**
    * @brief Returns the list of internal lanes, i.e. the ones used by vehicles to cross the junction.
    */
    const vector<roadElementID_t>& getInternalLaneIDs() const;

    /**
    * @brief Returns the list of the junctions that are connected to this one by only one road segment.
    */
    const vector<roadElementID_t>& getNeighboringJunctions() const;

    /**
    * @brief Sets the center point of the junction.
    * @param[in] center Center of the junction.
    */
    void setCenter(Point2D center);

    /**
    * @brief Sets the incoming lanes.
    * @param[in] pIncomingLanesID Vector of IDs of the incoming lanes.
    */
    void setIncomingLaneIDs(vector<roadElementID_t> pIncomingLanesID);

    /**
    * @brief Sets the incoming lanes.
    * @param[in] pInternalLanesID Vector of IDs of the internal lanes.
    */
    void setInternalLaneIDs(vector<roadElementID_t> pInternalLanesID);

    /**
    * @brief Add the ID of a junction to the list of neighbors.
    * @param[in] junctionID ID of the neighboring junction to be added.
    */
    void addNeighboringJunction(roadElementID_t junctionID);

    /**
    * @brief Check if the given lane is internal to the junction.
    * @return TRUE The lane is internal.
    * @return FALSE The lane is not internal.
    */
    bool containsLane(roadElementID_t laneID);

    /**
    * @brief Check if the given lane leads to the junction.
    * @return TRUE The lane leads to the junction.
    * @return FALSE The lane does not lead to the junction.
    */
    bool connectedToLane(roadElementID_t laneID);

private:

    /// @brief Center of the junction.
    Point2D center;

    /// @brief Vector containing the IDs of the lanes inside the junction.
    vector<roadElementID_t> incomingLanesID;

    /// @brief Vector containing the IDs of the lanes that lead the junction.
    vector<roadElementID_t> internalLanesID;

    /// @brief Vector containing the IDs of the junctions that are connected to this one by one road segment (edge).
    vector<roadElementID_t> neighboringJunctions;
};

} // namespace

#endif /* JUNCTION_H_ */
