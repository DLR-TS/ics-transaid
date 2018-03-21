/****************************************************************************/
/// @file    Lane.cpp
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

#include "Lane.h"

#include <cfloat>

namespace ics_facilities {

Lane::Lane(roadElementID_t laneID) {
    elementType = LANE;
    area2DType = ROADELEMENT;
    ID = laneID;
    weight = 0;
    status = LANE_OPEN;
    junctionID = "";
    edgeID = "";
    trafficLightID = "";
    maxSpeed = 10;

    prevLanes.reserve(0);
    nextLanes.reserve(0);
}

Lane::~Lane() { }

roadElementID_t         Lane::getID() const {
    return ID;
}

roadElementType         Lane::getRoadElementType() const {
    return elementType;
}

Area2DType              Lane::getArea2DType() const {
    return area2DType;
}

laneStatus              Lane::getStatus() const {
    return status;
}

laneWeight_t            Lane::getWeight() const {
    return weight;
}

maxSpeed_t              Lane::getMaxSpeed() const {
    return maxSpeed;
}

laneLength_t            Lane::getLength() const {
    return length;
}

const vector<Point2D>&  Lane::getShape() const {
    return shape;
}

const vector<Lane*>&    Lane::getPrevLanes() const {
    return prevLanes;
}

const vector<Lane*>&    Lane::getNextLanes() const {
    return nextLanes;
}

roadElementID_t         Lane::getEdgeID() const {
    return edgeID;
}

roadElementID_t	        Lane::getJunctionID() const {
    return junctionID;
}

trafficLightID_t        Lane::getTrafficLightID() const {
    return trafficLightID;
}

void Lane::setShape(vector<Point2D> Shape) {
    if (!shape.empty())
        shape.clear();
    shape = Shape;
    return;
}

void Lane::setStatus(laneStatus status) {
    this->status = status;
}

void Lane::setWeight(laneWeight_t weight) {
    this->weight = weight;
}

void Lane::setMaxSpeed(maxSpeed_t maxSpeed) {
    this->maxSpeed = maxSpeed;
}

void Lane::setLength(laneLength_t length) {
    this->length = length;
}

void Lane::setPrevlanes(vector<Lane*> prevLanes) {
    if (!this->prevLanes.empty())
        this->prevLanes.clear();
    this->prevLanes = prevLanes;
}

void Lane::setNextLanes(vector<Lane*> nextLanes) {
    if (!this->nextLanes.empty())
        this->nextLanes.clear();
    this->nextLanes = nextLanes;
}

void Lane::setEdgeID(roadElementID_t edgeID) {
    this->edgeID = edgeID;
}

void Lane::setJunctionID(roadElementID_t junctionID) {
    this->junctionID = junctionID;
}

void Lane::setTrafficLightID(trafficLightID_t trafficLightID) {
    this->trafficLightID = trafficLightID;
}

Rectangle* Lane::createRectangleFromShape() {
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = FLT_MIN;
    float maxY = FLT_MIN;
    for (unsigned int i = 0; i < shape.size(); i++) {
        if (shape[i].x() < minX) minX = shape[i].x();
        if (shape[i].y() < minY) minY = shape[i].y();
        if (shape[i].x() > maxX) maxX = shape[i].x();
        if (shape[i].y() > maxY) maxY = shape[i].y();
    }
    vector<Point2D> verts;
    verts.push_back(Point2D(minX, minY));
    verts.push_back(Point2D(maxX, minY));
    verts.push_back(Point2D(maxX, maxY));
    verts.push_back(Point2D(minX, maxY));
    Rectangle *rect = new Rectangle(verts);
    return rect;
}

}
