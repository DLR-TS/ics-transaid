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
#include "trace-manager.h"
#include "log/log.h"

namespace protocol {
namespace application {

TraceManager::TraceManager() {
}

bool TraceManager::TraceConnect(const std::string& key, const CallbackBase& callback) {
    TraceMap::const_iterator it = m_traceMap.find(key);
    if (it == m_traceMap.end()) {
        NS_LOG_WARN(key << " was not found");
        return false;
    }
    it->second->Connect(callback);
    return true;
}
bool TraceManager::TraceDisconnect(const std::string& key, const CallbackBase& callback) {
    TraceMap::const_iterator it = m_traceMap.find(key);
    if (it == m_traceMap.end()) {
        return false;
    }
    it->second->Disconnect(callback);
    return true;
}

void TraceManager::RegisterTrace(const std::string& key, TracedCallbackBase& trace) {
    m_traceMap.insert(make_pair(key, &trace));
    trace.SetName(key);
}
void TraceManager::RemoveTrace(const std::string& key) {
    m_traceMap.erase(key);
}

} /* namespace application */
} /* namespace protocol */
