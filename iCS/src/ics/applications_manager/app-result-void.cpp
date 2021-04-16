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
/// @file    app-result-void.cpp
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

#include <iostream>
#include <cstdlib>

#include "foreign/tcpip/storage.h"
#include "app-result-void.h"
#include "../traffic_sim_communicator/traffic-simulator-communicator.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/log/ics-log.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace tcpip;
using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// member method definitions
// ===========================================================================
ResultVoid::ResultVoid(stationID_t owner, int appHandlerId) {
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool
ResultVoid::ProcessResult(Storage& storage) {
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessResult() The application does not return any result and no data is proccessed.", kLogLevelInfo);
#endif
    return true;
}

int
ResultVoid::ApplyResult(SyncManager* syncManager) {
#ifdef LOG_ON
    IcsLog::LogLevel("ApplyResult() The application does not return any result and no data is applied.", kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

vector<pair<int, stationID_t> >
ResultVoid::GetReceivedMessages() {
    vector<pair<int, stationID_t> > auxiliar;
#ifdef LOG_ON
    IcsLog::LogLevel("GetReceivedMessages() The application does not return any result and no messages are not received.", kLogLevelInfo);
#endif
    return auxiliar;
}

}

