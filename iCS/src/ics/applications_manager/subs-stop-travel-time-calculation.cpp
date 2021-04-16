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
/// @file    subs-stop-travel-time-calculation.cpp
/// @author
/// @date
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

#include <cstdlib>
#include <typeinfo>
#include <math.h>

#include "subs-stop-travel-time-calculation.h"
#include "app-commands-subscriptions-constants.h"
#include "../wirelesscom_sim_message_tracker/V2X-geobroadcast-area.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"

using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// member method definitions
// ===========================================================================
SubsStopTravelTimeCalculation::SubsStopTravelTimeCalculation(int appId, stationID_t stationId, vector<Point2D> vertex, float frequency, float msgRegenerationTime, int msgLifeTime) : Subscription(stationId) {
    m_id = ++m_subscriptionCounter;
    m_name = "STOP TRAVEL TIME CALCULATION";
    m_appId = appId;
    m_nodeId = stationId;
    m_communicationArea = ConvexPolygon(vertex);
    m_geobroadcastArea = NULL;

    if (frequency <= 0) {
        frequency = 1;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStopTravelTimeCalculation() Frequency is negative or zero, set to 1 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_frequency = frequency;
    }

    if (msgRegenerationTime < 0) {
        m_messageRegenerationTime = 0;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStopTravelTimeCalculation() Message regeneration time is negative, set to 0 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_messageRegenerationTime = msgRegenerationTime;
    }

    if (msgLifeTime < 0) {
        m_messageLifeTime = 0;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStopTravelTimeCalculation() Message life time is negative, set to 0 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_messageLifeTime = msgLifeTime;
    }

    CreateGeobroadcastArea();
}

SubsStopTravelTimeCalculation::~SubsStopTravelTimeCalculation() {
    m_geobroadcastArea = NULL;
}

V2xGeobroadcastArea*
SubsStopTravelTimeCalculation::GetGeobroadcastArea() {
    return m_geobroadcastArea;
}

int
SubsStopTravelTimeCalculation::CreateGeobroadcastArea() {
    if (m_geobroadcastArea != NULL) {
#ifdef LOG_ON
        IcsLog::LogLevel("CreateGeobroadcastArea() Geobroadcast area already created for the subscription.", kLogLevelInfo);
#endif
        return EXIT_FAILURE;
    }

    int calculatedPayloadLength;
    m_geobroadcastArea = new V2xGeobroadcastArea(m_id, m_frequency, calculatedPayloadLength);
    SyncManager::m_v2xMessageTracker->m_v2xGeobroadcastAreaCollection->push_back(m_geobroadcastArea);

    return EXIT_SUCCESS;
}

int
SubsStopTravelTimeCalculation::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager) {
    int receiverId; //message.receiver;
    receiverId = message.receiverIcsID;
    if (AddStoppedTravelTimeStation(receiverId) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int
SubsStopTravelTimeCalculation::AddStoppedTravelTimeStation(stationID_t nodeId) {
    for (vector<stationID_t>::iterator it = m_stationsReceivedStopCommand.begin() ; it != m_stationsReceivedStopCommand.end() ; ++it) {
        if ((*it) == nodeId) {   // Do not add to the collection if the ID already exists
            return EXIT_SUCCESS;
        }
    }

    // New station has received notification to stop TravelTime calculation
    m_stationsReceivedStopCommand.push_back(nodeId);

    return EXIT_SUCCESS;
}

int
SubsStopTravelTimeCalculation::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message) {
//    message.receiverIcsID;
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessReceivedUnicastMessage() SubsCalculateTravelTime not based on Unicasting", kLogLevelWarning);
#endif
    return EXIT_SUCCESS;
}
}
