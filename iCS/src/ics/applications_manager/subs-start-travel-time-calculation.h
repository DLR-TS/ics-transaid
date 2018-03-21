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


namespace ics
{

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
class SubsStartTravelTimeCalculation : public Subscription
{

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