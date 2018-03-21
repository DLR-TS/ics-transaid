/****************************************************************************/
/// @file    subs-start-travel-time-calculation.cpp
/// @author  Julen Maneros
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

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cstdlib>
#include <typeinfo>
#include <math.h>

#include "subs-start-travel-time-calculation.h"
#include "app-commands-subscriptions-constants.h"
#include "../wirelesscom_sim_message_tracker/V2X-geobroadcast-area.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"

using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
SubsStartTravelTimeCalculation::SubsStartTravelTimeCalculation(int appId, stationID_t stationId, vector<Point2D> vertex, float frequency, float msgRegenerationTime, int msgLifeTime) : Subscription(stationId)
{
    m_id = ++m_subscriptionCounter;
    m_name = "START TRAVEL TIME CALCULATION";
    m_appId = appId;
    m_nodeId = stationId;
    m_communicationArea = ConvexPolygon(vertex);
    m_geobroadcastArea = NULL;

    if (frequency <= 0) {
        frequency = 1;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStartTravelTimeCalculation() Frequency is negative or zero, set to 1 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_frequency = frequency;
    }

    if (msgRegenerationTime < 0) {
        m_messageRegenerationTime = 0;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStartTravelTimeCalculation() Message regeneration time is negative, set to 0 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_messageRegenerationTime = msgRegenerationTime;
    }

    if (msgLifeTime < 0) {
        m_messageLifeTime = 0;
#ifdef LOG_ON
        IcsLog::LogLevel("SubsStartTravelTimeCalculation() Message life time is negative, set to 0 by the iCS", kLogLevelInfo);
#endif
    } else {
        m_messageLifeTime = msgLifeTime;
    }

    CreateGeobroadcastArea();
}

SubsStartTravelTimeCalculation::~SubsStartTravelTimeCalculation()
{
    m_geobroadcastArea = NULL;
}

V2xGeobroadcastArea*
SubsStartTravelTimeCalculation::GetGeobroadcastArea()
{
    return m_geobroadcastArea;
}

int
SubsStartTravelTimeCalculation::CreateGeobroadcastArea()
{
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
SubsStartTravelTimeCalculation::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessReceivedGeobroadcastMessage() SubsStartTravelTimeCalculation not based on Unicasting", kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

int
SubsStartTravelTimeCalculation::AddNewTravelTimeStation(stationID_t nodeId)
{
    for (vector<stationID_t>::iterator it = m_stationsReceivedStartCommand.begin() ; it != m_stationsReceivedStartCommand.end() ; ++it) {
        if ((*it) == nodeId) {   // Do not add to the collection if the ID already exists
            return EXIT_SUCCESS;
        }
    }

    // New station has received notification to collect TravelTime
    m_stationsReceivedStartCommand.push_back(nodeId);

    return EXIT_SUCCESS;
}
int
SubsStartTravelTimeCalculation::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message)
{
#ifdef LOG_ON
    IcsLog::LogLevel("[WARNING] ProcessReceivedUnicastMessage() SubsStartTravelTimeCalculation not based on Unicasting", kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

}
