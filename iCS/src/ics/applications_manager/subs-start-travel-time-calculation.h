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
/// @file    subs-start-travel-time-calculation.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef SUBSCRIPTION_START_TRAVEL_TIME_CALCULATION_H
#define SUBSCRIPTION_START_TRAVEL_TIME_CALCULATION_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

#include "subscription.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/geometric/ConvexPolygon.h"


namespace ics {

// ===========================================================================
// class declarations
// ===========================================================================
class V2xGeobroadcastArea;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsStartTravelTimeCalculation
 * @brief
 * @todo To be commented.
 */
class SubsStartTravelTimeCalculation : public Subscription {

public:
    /**
     * @brief Constructor.
     * @param[in] appId The identifier of the subscription.
     * @param[in] stationId Station that owns the subscription.
     * @param[in] vertex The collection of vertex that define the start of TTE broadcasting area.
     */
    SubsStartTravelTimeCalculation(int appId, ics_types::stationID_t stationId, std::vector<ics_types::Point2D> vertex, float frequency, float msgRegenerationTime, int msgLifeTime);

    /// @brief Destructor.
    ~SubsStartTravelTimeCalculation();

    /// @todo To be commented.
    V2xGeobroadcastArea* GetGeobroadcastArea();

    /// @todo To be commented.
    int CreateGeobroadcastArea();

    /// @todo To be commented.
    int ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager);

    /// @todo To be commented.
    int ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message);

    /// @todo To be commented
    int AddNewTravelTimeStation(ics_types::stationID_t);

    /// @brief Message regeneration time.
    float m_messageRegenerationTime;

    /// @brief Message lifetime.
    int m_messageLifeTime;

    /// @brief Times the message will be repeated per second in the geo area.
    float m_frequency;

    /// @brief The zone in which the message to start TTE are sent.
    ics_types::ConvexPolygon m_communicationArea;

private:

    /// @brief The broadcast area object
    V2xGeobroadcastArea* m_geobroadcastArea;

    /// @brief Stations received the message to compute travel time
    std::vector<ics_types::stationID_t> m_stationsReceivedStartCommand;
};

}

#endif