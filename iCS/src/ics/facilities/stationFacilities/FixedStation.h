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
/// @file    FixedStation.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef FIXEDSTATION_H_
#define FIXEDSTATION_H_

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
* @class FixedStation
* @brief Represents a fixed station, such as a RSU or a BS.
*
* These stations do not move and have only one radio access technology to communicate with the mobile stations.
*/
class FixedStation: public ics_facilities::Station {
public:

    /**
    * @brief Constructor.
    * param[in] ID Station ID.
    */
    FixedStation(stationID_t ID);

    /**
    * @brief Destructor.
    */
    virtual ~FixedStation();

    /**
    * @brief Returns the communication profile associated to the radio access technology used by the station for communication.
    * @return RATs Station's radio access technologies.
    */
    string getCommunicationProfile();

    /**
    * @brief Sets the communication profile.
    * @param[in] commProfile Station's communication profile.
    */
    void setCommunicationProfile(string commProfile);

private:

    /// @brief Station communication profile settings.
    string communicationProfile;

};

}

#endif /* FIXEDSTATION_H_ */
