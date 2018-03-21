/****************************************************************************/
/// @file    MobileStation.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef WIN32
#include <limits>
#define NAN std::numeric_limits<float>::quiet_NaN()
#endif

#include "MobileStation.h"

namespace ics_facilities {

MobileStation::MobileStation(stationID_t ID) {
    this->type = STATION_MOBILE;
    this->ID = ID;
    this->direction = NAN;
    this->speed = 0;
    this->acceleration = 0;
}

MobileStation::~MobileStation() {

}

speed_t MobileStation::getSpeed() const{
    return speed;
}

acceleration_t MobileStation::getAcceleration() const{
    return acceleration;
}

direction_t MobileStation::getDirection() const{
    return direction;
}

stationSize_t MobileStation::getVehicleLegth() const{
    return length;
}

stationSize_t MobileStation::getVehicleWidth() const{
    return width;
}

stationSize_t MobileStation::getVehicleHeight() const{
    return height;
}

exteriorLights_t MobileStation::getExteriorLights() const{
    return exteriorLights;
}

roadElementID_t MobileStation::getLaneID() const{
    return laneID;
}

void MobileStation::setSpeed(speed_t speed) {
    this->speed = speed;
}

void MobileStation::setAcceleration(acceleration_t acceleration) {
    this->acceleration = acceleration;
}

void MobileStation::setDirection(direction_t direction) {
    this->direction = direction;
}

void MobileStation::setVehicleLegth(stationSize_t length) {
    this->length = length;
}

void MobileStation::setVehicleWidth(stationSize_t width) {
    this->width = width;
}

void MobileStation::setVehicleHeight(stationSize_t height) {
    this->height = height;
}

void MobileStation::setExteriorLights(exteriorLights_t newLightsStatus) {
    this->exteriorLights = newLightsStatus;
}

void MobileStation::setLaneID(roadElementID_t laneID) {
    this->laneID = laneID;
}

}
