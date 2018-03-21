/****************************************************************************/
/// @file    Edge.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef EDGE_H_
#define EDGE_H_

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
 * @class Edge
 * @brief Represents a road segment composed by lanes.
*/
class Edge: public ics_facilities::RoadElement {
public:

    /**
    * @brief Constructor.
    */
    Edge();

    /**
    * @brief Constructor.
    * @param[in] edgeID ID of the edge to be created.
    */
    Edge(roadElementID_t edgeID);

    /**
    * @brief Destructor.
    */
    virtual ~Edge();

    /**
    * @brief Returns the ID of the edge.
    */
    roadElementID_t getID() const;

    /**
    * @brief Returns EDGE.
    */
    roadElementType getRoadElementType() const;

    /**
    * @brief Returns ROADELEMENT.
    */
    Area2DType getArea2DType() const;

    /**
    * @brief Returns the vector containing the IDs of the lanes contained in the edge.
    */
    const vector<roadElementID_t>& getLaneIDs() const;

    /**
    * @brief Returns the vector containing the IDs of the lanes contained in the edge.
    * @param[in] pLaneIDs Pointer to the vector containing the IDs of the lanes.
    */
    void setLaneIDs(vector<roadElementID_t> *pLaneIDs);

    /**
    * @brief Checks if a lane is contained in the edge.
    * @param[in] laneID ID of the lane.
    * @return TRUE The lane is contained in the egde.
    * @return FALSE The lane is not contained in the egde.
    */
    bool containsLane(roadElementID_t laneID);

private:

    /// @brief Vector containing the ID of the lanes that form the edge.
    vector<roadElementID_t> laneIDs;
};

} //namespace

#endif /* EDGE_H_ */
