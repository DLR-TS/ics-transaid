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
/// @file    V2X-geobroadcast-area.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef V2X_GEOBROADCAST_AREA_H
#define V2X_GEOBROADCAST_AREA_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class V2xGeobroadcastArea
 * @brief Defines a geobroadcast area in which messages are going to be sent.
 */
class V2xGeobroadcastArea {
public:

    V2xGeobroadcastArea();
    /**
    * @brief Constructor.
    * @param[in] subscriptionId Subscription identifier that requested the area.
    * @param[in] frequency Frequency of message transmission within the area.
    * @param[in] payloadLength Length of the message payload.
    */
    V2xGeobroadcastArea(int subscriptionId, float frequency, unsigned int payloadLength);

    /// @brief Destructor.
    ~V2xGeobroadcastArea();

    /// @brief Subscription identifier that requested the area.
    int m_subscriptionId;

    /**
     * @brief Frequency of CAM beaconing in the area.
     * @note Set to 2Hz by default.
     */
    float m_frequency;

    /**
     * @brief Length of the message payload.
     * @note Set to 60 (bytes) by default.
     */
    unsigned int m_payloadLength;
};

}

#endif