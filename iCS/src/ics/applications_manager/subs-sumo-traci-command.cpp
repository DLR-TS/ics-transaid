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

#include "subs-sumo-traci-command.h"
#include "app-message-manager.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics {
SubsSumoTraciCommand::SubsSumoTraciCommand(int appId, ics_types::stationID_t stationId, unsigned char* msg,
        int msgSize) :
    Subscription(stationId) {
    m_id = ++m_subscriptionCounter;
    m_name = "EXECUTES A TRACI COMMAND IN SUMO";
    m_appId = appId;
    tcpip::Storage message(msg, msgSize);
    m_executionId = message.readInt();
    ostringstream oss;
    oss << "SubsSumoTraciCommand id " << m_id << " executionId " << m_executionId;
    IcsLog::LogLevel(oss.str().c_str(), kLogLevelInfo);
    while (message.valid_pos()) {
        m_request.writeChar(message.readChar());
    }
//		One shot subscription
    ExecuteCommand();
}

SubsSumoTraciCommand::~SubsSumoTraciCommand() {
}

int SubsSumoTraciCommand::InformApp(AppMessageManager* messageManager) {
//		Move it here for recurring subscription.
//		ExecuteCommand();
    return messageManager->CommandSendSubscriptionSumoTraciCommand(m_nodeId, m_id, m_executionId, m_result);
}

bool SubsSumoTraciCommand::ExecuteCommand() {
//		Uncomment if used as recurring subscription.
//		m_result.reset();
    IcsLog::LogLevel("SubsSumoTraciCommand::ExecuteCommand", kLogLevelInfo);
    return SyncManager::m_trafficSimCommunicator->TraciCommand(m_request, m_result) == EXIT_SUCCESS;
}

} /* namespace ics */
