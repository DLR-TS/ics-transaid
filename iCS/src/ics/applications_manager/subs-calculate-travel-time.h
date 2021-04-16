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
/// @file    subs-calculate-travel-time.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef SUBSCRIPTION_CALCULATE_TRAVEL_TIME_H
#define SUBSCRIPTION_CALCULATE_TRAVEL_TIME_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

#include "subscription.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/geometric/ConvexPolygon.h"
#include "app-message-manager.h"

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsCalculateTravelTime
 * @brief
 * @todo To be commented.
 */
class SubsCalculateTravelTime : public Subscription {

public:
    /**
     * @brief Constructor.
     * @param[in] appId The identifier of the subscription.
     * @param[in] stationId Station that owns the subscription.
     * @param[in] vertex The collection of vertex that define the start of TTE broadcasting area.
     */
    SubsCalculateTravelTime(int subId, ics_types::stationID_t stationId);

    /// @brief Destructor
    ~SubsCalculateTravelTime();

    /// @todo To be commented.
    void Start(ics_types::stationID_t stationId);

    /// @todo To be commented.
    void Stop(ics_types::stationID_t stationId);

    /// @todo To be commented.
    int InformApp(AppMessageManager* messageManager);

    /// @todo To be commented
    int ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager);

    /// @todo To be commented
    int ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message);

private:

    /// @todo To be commented
    bool m_startCommandReceived;

    /// @todo To be commented
    bool m_stopCommandReceived;

    /// @todo To be commented
    int m_startingStationId;

    /// @todo To be commented
    int m_endingStationId;
};

}

#endif
