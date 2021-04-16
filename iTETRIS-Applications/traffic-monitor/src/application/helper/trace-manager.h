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
#ifndef TRACE_MANAGER_H_
#define TRACE_MANAGER_H_

#include <map>
#include <string>
#include "traced-callback.h"

namespace protocol {
namespace application {
typedef std::map<std::string, TracedCallbackBase*> TraceMap;

/**
 * @brief Utility class to be extended to allow push model interaction.
 * @brief A subclass can bind a name to a trace source at with a client can connect
 * @brief Mainly ported and adapted from ns3
 */
class TraceManager {
public:
    TraceManager();
    virtual ~TraceManager() {
    }

    /**
     * @brief Register to a trace a callback method
     * @brief It does not check the signature of the method.
     * @brief Could cause a runtime error if the incorrect method is registered
     * @param[in] key The trace name
     * @param[in] callback The method to register
     * @return True if success. False otherwise.
     */
    virtual bool TraceConnect(const std::string& key, const CallbackBase& callback);
    /**
     * @brief Disconnect a callback method from a trace if found
     * @param[in] key The trace name
     * @param[in] callback The method to disconnect
     * @return True if success. False otherwise.
     */
    virtual bool TraceDisconnect(const std::string& key, const CallbackBase& callback);

protected:
    virtual void RegisterTrace(const std::string& key, TracedCallbackBase& trace);
    virtual void RemoveTrace(const std::string& key);

private:
    TraceMap m_traceMap;
};

} /* namespace application */
} /* namespace protocol */

#endif /* TRACE_MANAGER_H_ */
