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
/// @file    subs-get-received-cam-info.cpp
/// @author  Pasquale Cataldi
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

#include "subs-get-received-cam-info.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsGetReceivedCamInfo::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it = subscriptions->begin() ; it < subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsGetReceivedCamInfo*)) {
            SubsGetReceivedCamInfo* subsGetReceivedCamInfo = static_cast<SubsGetReceivedCamInfo*>(sub);
            if (subsGetReceivedCamInfo->m_nodeId == stationID) {
                delete subsGetReceivedCamInfo;
                delete sub;
                return EXIT_SUCCESS;
            }
        }
    }
    return EXIT_SUCCESS;
}

// ===========================================================================
// member method definitions
// ===========================================================================
SubsGetReceivedCamInfo::SubsGetReceivedCamInfo(int appId, ics_types::stationID_t stationId) : Subscription(stationId) {
    m_id = ++m_subscriptionCounter;

    m_name = "RETURN INFORMATION ABOUT RECEIVED CAM MESSAGES";

    m_appId = appId;
}

SubsGetReceivedCamInfo::~SubsGetReceivedCamInfo() { }

std::vector<ics_types::TCamInformation>*
SubsGetReceivedCamInfo::getInformationFromLastReceivedCAMs() {
    std::vector<ics_types::TCamInformation>* info = NULL;
    info = SyncManager::m_facilitiesManager->getInfoFromLastCAMsReceivedByStation(m_nodeId);

#ifdef LOG_ON
    stringstream log;
    log << "[INFO] getInformationFromLastReceivedCAMs() - NodeID: " << m_nodeId << " -> #Rec CAMs: " << info->size() << " |";
    std::vector<ics_types::TCamInformation>::iterator it;
    for (it = info->begin(); it != info->end(); it++) {
        log << " senderID: " << (*it).senderID << " Ts: " << (*it).generationTime << " pos: (" << (*it).senderPosition.x() << "," << (*it).senderPosition.y() << ") |";
    }
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    return info;
}


}
