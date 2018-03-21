/****************************************************************************/
/// @file    MobileStation.h
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

#ifndef MOBILESTATION_H_
#define MOBILESTATION_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

/*
 *
 */
#include "Station.h"

namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class MobileStation
* @brief Represents a mobile station, such as vehicles, buses, emergency vehicles, etc...
*
* These stations can move and can have multiple radio access technologies.
*/
class MobileStation: public ics_facilities::Station {
public:

    /**
    * @brief Constructor.
    * param[in] ID Station ID.
    */
    MobileStation(stationID_t ID);

    /**
    * @brief Destructor.
    */
    virtual ~MobileStation();

    /**
    * @brief Returns the station speed.
    * @return Speed of the vehicle.
    */
    speed_t getSpeed() const;

    /**
    * @brief Returns the station acceleration.
    * @return Acceleration of teh vehicle.
    */
    acceleration_t getAcceleration() const;

    /**
    * @brief Returns the station direction.
    * @return Angle value from the North.
    */
    direction_t getDirection() const;

    /**
    * @brief Returns the length of the station.
    * @return Length of the station.
    */
    stationSize_t getVehicleLegth() const;

    /**
    * @brief Returns the width of the station.
    * @return Width of the station.
    */
    stationSize_t getVehicleWidth() const;

    /**
    * @brief Returns the height of the station.
    * @return Height of the station.
    */
    stationSize_t getVehicleHeight() const;

    /**
    * @brief Returns the exterior lights status.
    * @return On or Off.
    */
    exteriorLights_t getExteriorLights() const;

    /**
    * @brief Returns the lane that the station is traveling over.
    * @return Id of the lane.
    */
    roadElementID_t getLaneID() const;

    /**
    * @brief Sets the speed of the mobile station.
    * @param[in] speed Speed value.
    */
    void setSpeed(speed_t speed);

    /**
    * @brief Sets the acceleration of the mobile station.
    * @param[in] acceleration Acceleration value.
    */
    void setAcceleration(acceleration_t acceleration);

    /**
    * @brief Sets the direction of the mobile station.
    * @param[in] direction Direction value.
    */
    void setDirection(direction_t direction);

    /**
    * @brief Sets the length of the mobile station.
    * @param[in] length Length value.
    */
    void setVehicleLegth(stationSize_t length);

    /**
    * @brief Sets the width of the mobile station.
    * @param[in] width Width value.
    */
    void setVehicleWidth(stationSize_t width);

    /**
    * @brief Sets the height of the mobile station.
    * @param[in] height Height value.
    */
    void setVehicleHeight(stationSize_t height);

    /**
    * @brief Sets the status of the exterior lights.
    * @param[in] newLightsStatus Exterior lights status.
    */
    void setExteriorLights(exteriorLights_t newLightsStatus);

    /**
    * @brief Sets the id of the current lane the station is traveling over.
    * @param[in] laneID Id of the lane.
    */
    void setLaneID(roadElementID_t laneID);

private:

    /// @brief Speed of the mobile station.
    speed_t speed;

    /// @brief Acceleration of the mobile station.
    acceleration_t acceleration;

    /// @brief Direction of the mobile station.
    direction_t direction;

    /// @brief Length of the mobile station.
    stationSize_t length;

    /// @brief Width of the mobile station.
    stationSize_t width;

    /// @brief Height of the mobile station.
    stationSize_t height;

    /// @brief Exterior light status of the mobile station.
    exteriorLights_t exteriorLights;

    /// @brief Id of the lane the mobile station is traveling over.
    roadElementID_t laneID;
};

}

#endif /* MOBILESTATION_H_ */
