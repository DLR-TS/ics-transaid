/****************************************************************************/
/// @file    Station.h
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

#ifndef STATION_H_
#define STATION_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../../utils/ics/iCStypes.h"
using namespace ics_types;

#include <iostream>
#include <vector>
using namespace std;

namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class Station
* @brief Generic station.
*/
class Station {
public:

    /**
    * @brief Constructor.
    */
    Station();

    /**
    * @brief Destructor.
    */
    virtual ~Station();

    /**
    * @brief Returns the ID of the station.
    * @return ID of the station.
    */
    stationID_t getID() const;

    /**
    * @brief Returns the type of the station (fixed or mobile).
    * @return Station type.
    */
    icsstationtype_t getType() const;

    /**
    * @brief Returns the position of the station.
    * @return Station's position.
    */
    const Point2D& getPosition() const;

    /**
    * @brief Returns the radio access technologies available on the station.
    * @return Station's radio access technologies.
    */
    vector<RATID>* getRATs();

    /**
    * @brief Returns the radio access technologies enabled on the station.
    * @return Station's radio access technologies.
    */
    vector<RATID>* getActiveRATs();

    /**
    * @brief Sets the position of the station.
    * @param[in] position Station's new position.
    */
    void setPosition(Point2D position);

    /**
    * @brief Sets the available radio access technologies and if each of them is on or off.
    * @param[in] RATs Radio access technologies
    */
    void setRATs(vector< pair<RATID, bool> > RATs);

    /**
    * @brief Enable the target radio access technology (if available).
    * @param[in] toEnable Radio access technology to enable.
    */
    bool enableRAT(RATID toEnable);

    /**
     * @brief Enable the target radio access technology (if available).
     * @param[in] toEnable Radio access technology to enable.
     */
    bool disableRAT(RATID toDisable);

    /**
     * @brief Used by subscription get mobility info to check if the node is not being removed
     */
    bool isActive;

protected:

    /// @brief Station identity.
    stationID_t ID;

    /// @brief Station type (fixed or mobile).
    icsstationtype_t type;

    /// @brief Station position.
    Point2D position;

    /// @brief Available radio access technologies and status flag.
    vector< pair<RATID, bool> > RATs;
};

}

#endif /* STATION_H_ */
