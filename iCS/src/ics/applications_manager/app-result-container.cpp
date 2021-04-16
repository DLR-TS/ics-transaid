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
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#include <cstdlib>

#include "app-result-container.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/log/ics-log.h"
#include "app-result-maximum-speed.h"
#include "app-result-travel-time.h"
#include "app-result-open-buslanes.h"
#include "app-result-traffic-jam-detection.h"
#include "app-result-void.h"
#include "app-result-generic.h"

namespace ics {

ResultContainer* ResultContainer::CreateResultContainer(int type, ics_types::stationID_t nodeId, int handlerId) {
    switch (type) {
        case OUTPUT_SET_SPEED_ADVICE_DEMO:
        case OUTPUT_SET_VEHICLE_MAX_SPEED:
            return new ResultSetMaximumSpeed(nodeId, handlerId);
        case OUTPUT_TRAVEL_TIME_ESTIMATION:
            return new ResultTravelTime(nodeId, handlerId);
        case OUTPUT_TRAFFIC_JAM_DETECTION:
            return new ResultTrafficJamDetection(nodeId, handlerId);
        case OUTPUT_OPEN_BUSLANES:
            return new ResultOpenBuslanes(nodeId, handlerId);
        case OUTPUT_VOID:
            return new ResultVoid(nodeId, handlerId);
        case OUTPUT_GENERIC:
            return new ResultGeneric(nodeId, handlerId);
        default:
            IcsLog::LogLevel("iCS --> Result type is not registered. Please contact Application scientist. ", kLogLevelError);
            return NULL;
    }
}

int ResultContainer::CheckMessage(int appMessageId) {
    return EXIT_FAILURE;
}
int ResultContainer::CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager) {
    return EXIT_FAILURE;
}

void ResultContainer::GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >& v) {
}
bool ResultContainer::AskSendMessageStatus() {
    return false;
}

}
;
