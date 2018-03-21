/****************************************************************************/
/// @file    MapFacilities.h
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

#ifndef MAPFACILITIES_H_
#define MAPFACILITIES_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "./road/Lane.h"
#include "./road/Edge.h"
#include "./road/Junction.h"
#include "./road/TrafficLight.h"
#include "../../../utils/ics/geometric/Shapes.h"

#include <map>
#include <vector>
using namespace std;

namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class MapFacilities
* @brief Represents the digital map data structure and provides topological methods.
*/
class MapFacilities {
public:

    /**
    * @brief Constructor.
    */
    MapFacilities();

    /**
    * @brief Destructor.
    */
    virtual ~MapFacilities();

    /**
    * @brief Parses the file that contains the topology map description.
    * @param[in] mapFilename Table that contains the identifiers related to a certain message.
    * @return True: if the map is correctly loaded. False, otherwise.
    */
    bool mapParser(string mapFilename);

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
    * @brief Returns the pointer to the traffic light.
    * @param[in] trafficLightID ID of the traffic light.
    * @return Pointer to the traffic light object.
    */
    const TrafficLight* getTrafficLight(trafficLightID_t trafficLightID) const;

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
    Lane* convertPoint2Map(Point2D& pos);

    /**
    * @brief Returns the pointer to the closest junction (by Euclidean distance) that is the closest to a point.
    * @param[in] pos X-Y point.
    * @return Pointer to the junction object.
    */
    const Junction* getClosestJunction(Point2D& pos);

    /**
    * @brief Given a junction, it returns the neighboring junctions connected to it by one edge.
    * @param[in] junctionID ID of the junction.
    * @return Vector containing the IDs of the neighboring junctions.
    */
    const vector<roadElementID_t> getNeighboringJunctions(roadElementID_t junctionID) const;

    /**
    * @brief Returns a vector containing the pointers to the edges that connect two junctions (the order of the junction matters).
    * @param[in] junctionID_A First junction.
    * @param[in] junctionID_B Second junction.
    * @return Pointer to the vector of pointers of edge objects.
    */
    vector<const Edge*>* getEdgesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID) const;

    /**
    * @brief Returns a vector containing the pointers to the lanes that connect two junctions (the order of the junction matters).
    * @param[in] junctionID_A First junction.
    * @param[in] junctionID_B Second junction.
    * @return Pointer to the vector of pointers of lane objects.
    */
    vector<const Lane*>* getLanesFromJunctions(roadElementID_t junctAID, roadElementID_t junctBID) const;

    /**
    * @brief Returns a vector containing the pointers to the junctions that are at the beginning and end of a given edge.
    * @param[in] edgeID ID of the Edge.
    * @return Pointer to the vector of pointers of junction objects.
    */
    vector<const Junction*>* getJunctionsFromEdge(roadElementID_t edgeID);

    /**
    * @brief Sets the status of the input lane.
    * @param[in] ID of the lane and new status.
    * @return void.
    */
    void setLaneStatus(roadElementID_t laneID, laneStatus newStatus);

    /**
    * @brief Sets the weight of that lane (set to 0 by default).
    * @param[in] ID of the lane and weight.
    * @return void.
    */
    void setLaneWeight(roadElementID_t laneID, laneWeight_t newWeight);

    /**
    * @brief Updates the traffic like status (green, yellow or red).
    * @param[in] tlID ID of the traffic light
    * @param[in] tlSignal signal of the traffic light
    * @param[in] info Information associated to it.
    */
    void updateTrafficLightDynamicInformation(const trafficLightID_t &tlID, std::string& state);

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

    /**
    * @brief Given a vector of areas, it returns the rectangle that encloses them all.
    * @param[in] areas Vector of areas (they can be geometric shapes or road elements).
    * @return Returns the Area2D type pointer if the vector contains just one element. It returns a pointer to a newly allocated rectangle otherwise.
    */
    Area2D* getWholeArea(vector<Area2D*> areas);

    /**
    * @brief Given a geometric shape or a road element, returns a circle object that includes the area.
    * @param[in] area Geometric shape or road element.
    * @return Returns a circle object that includes the area given in input.
    */
    Circle getCircleFromArea(Area2D* area);

    /**
    * @brief Stores the coordinates of the origin of the local Cartesian reference system.
    * @param[in] originLat Latitude of the origin point.
    * @param[in] originLon Longitude of the origin point.
    * @param[in] originAlt Altitude of the origin point.
    */
    void setOriginCoordinates(latitude_t originLat, longitude_t originLon, altitude_t originAlt);

    /**
    * @brief Stores the coordinates of the origin of the local Cartesian reference system. The altitude is not set.
    * @param[in] originLat Latitude of the origin point.
    * @param[in] originLon Longitude of the origin point.
    */
    void setOriginCoordinates(latitude_t originLat, longitude_t originLon);

private:

    /// @brief Vector containing the Lane objects.
    map<roadElementID_t, Lane> lanes;

    /// @brief Vector containing the Edge objects.
    map<roadElementID_t, Edge> edges;

    /// @brief Vector containing the Junction objects.
    map<roadElementID_t, Junction> junctions;

    /// @brief Vector containing the TrafficLight objects.
    map<trafficLightID_t, TrafficLight> trafficLights;


    /// @brief Latitude of the origin coordinate.
    latitude_t originLatitude;

    /// @brief Longitude of the origin coordinate.
    longitude_t originLongitude;

    /// @brief Altitude of the origin coordinate.
    altitude_t originAltitude;

    /**
    * @brief Finds the distance of a point from a line.
    * @param[in] &point Reference to the point.
    * @param[in] &lineStart Reference to one of the two points that define a line.
    * @param[in] &lineEnd Reference to one of the two points that define a line.
    * @param[out] &outIntersection Reference to the intersection point (output).
    * @return Distance
    */
    float closestDistancePointLine(const Point2D &point, const Point2D &lineStart, const Point2D &lineEnd, Point2D &outIntersection);

    /**
    * @brief Considers the lane shape and returns a rectangle (positioned parallel to the axes) that contains the lane.
    * @param[in] &lane Reference to the lane.
    * @return Pointer to the created Rectangle.
    */
    ics_types::Rectangle* convertLane2Rectangle(Lane& lane);

    /**
    * @brief Considers the edge shape and returns a rectangle (positioned parallel to the axes) that contains the edge.
    * @param[in] &edge Reference to the edge.
    * @return Pointer to the created Rectangle.
    */
    ics_types::Rectangle* convertEdge2Rectangle(Edge& edge);

    /**
    * @brief Considers the junction shape and returns a rectangle (positioned parallel to the axes) that contains the junction.
    * @param[in] &lane Reference to the junction.
    * @return Pointer to the created Rectangle.
    */
    ics_types::Rectangle* convertJunction2Rectangle(Junction& junction);
};

}

#endif /* MAPFACILITIES_H_ */
