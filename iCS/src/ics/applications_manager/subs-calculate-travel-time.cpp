/****************************************************************************/
/// @file    subs-calculate-travel-time.cpp
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

#include <typeinfo>
#include <math.h>
#include <cstdlib>

#include "subs-calculate-travel-time.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"
#include "../itetris-node.h"
#include "subs-start-travel-time-calculation.h"
#include "subs-stop-travel-time-calculation.h"

using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
SubsCalculateTravelTime::SubsCalculateTravelTime(int appId, stationID_t stationId) : Subscription(stationId)
{
    m_id = ++m_subscriptionCounter;
    m_name = "CALCULATE TRAVEL TIME FLAGS";
    m_appId = appId;
    m_nodeId = stationId;
    m_startCommandReceived = false;
    m_stopCommandReceived = false;
    m_startingStationId = -1;
    m_endingStationId = -1;
}

SubsCalculateTravelTime::~SubsCalculateTravelTime() {}

void
SubsCalculateTravelTime::Start(stationID_t stationId)
{
    m_startCommandReceived = true;
    m_startingStationId = stationId;
}

void
SubsCalculateTravelTime::Stop(stationID_t stationId)
{
    m_stopCommandReceived = true;
    m_endingStationId = stationId;
}

int
SubsCalculateTravelTime::InformApp(AppMessageManager* messageManager)
{
    if (m_startCommandReceived == false && m_stopCommandReceived == false) {
        IcsLog::LogLevel("InformApp() No information about SubsCalculateTravelTime will be sent to app.",kLogLevelInfo);
    }

    // The vehicle go through the starting TTE RSU
    if (m_startCommandReceived == true && m_stopCommandReceived == false) {
        if (messageManager->CommandSendSubcriptionCalculateTravelTimeFlags(m_nodeId, m_startingStationId, -1) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    // The vehicle go through the starting TTE RSU and the stop TTE RSU
    // The App should calculate the TTE
    if (m_startCommandReceived == true && m_stopCommandReceived == true) {
        if (messageManager->CommandSendSubcriptionCalculateTravelTimeFlags(m_nodeId, m_startingStationId, m_endingStationId) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    // The vehicle go through the stop TTE RSU but NOT the start TTE RSU
    if (m_startCommandReceived == false && m_stopCommandReceived == true) {
        if (messageManager->CommandSendSubcriptionCalculateTravelTimeFlags(m_nodeId, -1, m_endingStationId) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


int
SubsCalculateTravelTime::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager)
{
    if (syncManager == NULL) {
#ifdef LOG_ON
        IcsLog::LogLevel("ProcessReceivedGeobroadcastMessage() SyncManager is NULL", kLogLevelError);
#endif
        return EXIT_FAILURE;
    }
    int senderId = message.senderIcsID;

    // Identify the subscription type (start of stop TTE) to update the appropriate flag
    ITetrisNode* senderNode = NULL;
    senderNode = syncManager->GetNodeByIcsId(senderId);
    if (senderNode == NULL) {
#ifdef LOG_ON
        stringstream log;
        log << "ProcessReceivedGeobroadcastMessage() Sender Node does not exist, senderId = " << senderId;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
        return EXIT_FAILURE;
    }
    for (vector<Subscription*>::iterator it = senderNode->m_subscriptionCollection->begin() ; it != senderNode->m_subscriptionCollection->end() ; ++it) {
        Subscription *sub = (*it);
        Subscription& reference = *sub;
        const type_info& typeinfo = typeid(reference);

        if (typeinfo == typeid(SubsStartTravelTimeCalculation)) {
            Start(senderId);
            return EXIT_SUCCESS;
        }

        if (typeinfo == typeid(SubsStopTravelTimeCalculation)) {
            Stop(senderId);
            return EXIT_SUCCESS;
        }
    }

#ifdef LOG_ON
    IcsLog::LogLevel("ProcessReceivedGeobroadcastMessage() No subscription process the geobroadcast message received", kLogLevelError);
#endif
    return EXIT_FAILURE;
}

int
SubsCalculateTravelTime::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessReceivedUnicastMessage() SubsCalculateTravelTime not based on Unicasting", kLogLevelWarning);
#endif
    return EXIT_SUCCESS;
}

} //namespace