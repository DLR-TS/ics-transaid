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

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsCalculateTravelTime
 * @brief
 * @todo To be commented.
 */
class SubsCalculateTravelTime : public Subscription
{

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
