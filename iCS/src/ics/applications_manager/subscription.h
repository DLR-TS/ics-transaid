/****************************************************************************/
/// @file    subscription.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>

#include "../../utils/ics/iCStypes.h"
#include "../wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "../sync-manager.h"

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class Subscription
* @brief Manages the subscriptions.
*/
class Subscription
{
public:

    /**
    * @param[in] stationId Station that owns the subscription.
    * @brief Constructor.
    */
    Subscription(ics_types::stationID_t stationId);

    /// @brief Destructor.
    virtual ~Subscription();

    /// @todo To be commented
    virtual int ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager);

    /// @todo To be commented
    virtual int ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message);

    /// @brief Stores the amount of subscription in the simulator.
    static int m_subscriptionCounter;

    /// @brief Identifies the subscription univocally.
    int m_id;

    /// @brief The application subscribed.
    int m_appId;

    /// @brief Subscription name.
    std::string m_name;

    /// @brief Id of the station the subscription belongs to.
    ics_types::stationID_t m_nodeId;
};

}

#endif
