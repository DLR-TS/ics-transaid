/****************************************************************************/
/// @file    subscription.cpp
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

#include <cstdlib>

#include "subscription.h"

using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int Subscription::m_subscriptionCounter = 0;

// ===========================================================================
// member method definitions
// ===========================================================================
Subscription::Subscription(stationID_t stationId)
{
    m_nodeId = stationId;
}

Subscription::~Subscription() {}

int
Subscription::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager)
{
    return EXIT_FAILURE;
}

int
Subscription::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message)
{
    return EXIT_FAILURE;
}

}
