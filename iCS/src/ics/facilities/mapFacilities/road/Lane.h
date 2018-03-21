/****************************************************************************/
/// @file    Lane.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef LANE_H_
#define LANE_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "RoadElement.h"
#include "../../../../utils/ics/geometric/Rectangle.h"

#include <vector>
using namespace std;

namespace ics_facilities {
// ===========================================================================
// struct definitions
// ===========================================================================
typedef float maxSpeed_t;
typedef float laneLength_t;
typedef float laneWeight_t;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Junction
 * @brief Represents a road junction.
*/
class Lane: public ics_facilities::RoadElement {
public:

    /**
    * @brief Constructor.
    * @param[in] laneID ID of the lane to be created.
    */
    Lane(roadElementID_t laneID);

    /**
    * @brief Destructor.
    */
    virtual ~Lane();

    /**
    * @brief Returns the ID of the lane.
    */
    roadElementID_t getID() const;

    /**
    * @brief Returns LANE.
    */
    roadElementType getRoadElementType() const;

    /**
    * @brief Returns ROADELEMENT.
    */
    Area2DType getArea2DType() const;

    /**
    * @brief Returns the status of the lane (open, closed or jammed).
    */
    laneStatus getStatus() const;

    /**
    * @brief Returns the weight of the lane. The weight can represents anything, depending on the usage.
    */
    laneWeight_t getWeight() const;

    /**
    * @brief Returns the maximum speed allowed on the lane.
    */
    maxSpeed_t getMaxSpeed() const;

    /**
    * @brief Returns the total length of the lane.
    */
    laneLength_t getLength() const;

    /**
    * @brief Returns the shape of the lane, i.e. the vector of points that define the shape.
    */
    const vector<Point2D>& getShape() const;

    /**
    * @brief Returns the pointers of the lanes that merge into the current one.
    */
    const vector<Lane*>& getPrevLanes() const;

    /**
    * @brief Returns the pointers of the lanes that will depart from the current one.
    */
    const vector<Lane*>& getNextLanes() const;

    /**
    * @brief Returns the ID of the edge that includes this lane.
    */
    roadElementID_t getEdgeID() const;

    /**
    * @brief Returns the ID of the junction that includes this lane, if the lane is internal.
    */
    roadElementID_t getJunctionID() const;

    /**
    * @brief Returns the ID of the traffic light associated to the lane, if any.
    */
    trafficLightID_t getTrafficLightID() const;

    /**
    * @brief Sets the status of the lane.
    * @param[in] status New status of the lane.
    */
    void setStatus(laneStatus status);

    /**
    * @brief Sets the weight of the lane.
    * @param[in] weight New weight of the lane.
    */
    void setWeight(laneWeight_t weight);

    /**
    * @brief Sets the maximum speed on the lane.
    * @param[in] maxSpeed New maximum speed on the lane.
    */
    void setMaxSpeed(maxSpeed_t maxSpeed);

    /**
    * @brief Sets the length of the lane.
    * @param[in] length Length of the lane.
    */
    void setLength(laneLength_t length);

    /**
    * @brief Sets the shape of the lane.
    * @param[in] shape Shape of the lane.
    */
    void setShape(vector<Point2D> shape);

    /**
    * @brief Sets the previous lanes' IDs of the lane.
    * @param[in] prevLanes Vector of pointers to the previous lanes.
    */
    void setPrevlanes(vector<Lane*> prevLanes);

    /**
    * @brief Sets the next lanes' IDs of the lane.
    * @param[in] prevLanes Vector of pointers to the next lanes.
    */
    void setNextLanes(vector<Lane*> nextLanes);

    /**
    * @brief Sets the edgeID of the lane.
    * @param[in] edgeID ID of the edge containing the lane.
    */
    void setEdgeID(roadElementID_t edgeID);

    /**
    * @brief Sets the junctionID of the lane.
    * @param[in] junctionID ID of the junction containing the lane.
    */
    void setJunctionID(roadElementID_t junctionID);

    /**
    * @brief Sets the trafficLightID of the lane.
    * @param[in] trafficLightID ID of the traffic light controlling the lane.
    */
    void setTrafficLightID(trafficLightID_t trafficLightID);

    /**
    * @brief Creates and returns a rectangle pointer that includes the lane.
    */
    ics_types::Rectangle* createRectangleFromShape();

private:

    /// @brief Status of the lane.
    laneStatus status;

    /// @brief Weight of the lane for algorithms like Dijkstra.
    laneWeight_t weight;

    /// @brief Maximum speed allowed on the lane.
    maxSpeed_t maxSpeed;

    /// @brief Total length of the lane.
    laneLength_t length;

    /// @brief Shape of the junction, defined by a set of points.
    vector<Point2D> shape;

    /// @brief Vector of pointers to the previous lanes.
    vector<Lane*> prevLanes;

    /// @brief Vector of pointers to the next lanes.
    vector<Lane*> nextLanes;

    /// @brief ID of the edge containing the lane.
    roadElementID_t edgeID;

    /// @brief ID of the junction containing the lane.
    roadElementID_t junctionID;

    /// @brief ID of the traffic light controlling the lane.
    trafficLightID_t trafficLightID;

};

}

#endif /* LANE_H_ */
