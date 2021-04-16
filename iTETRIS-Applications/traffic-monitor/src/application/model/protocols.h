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

#ifndef SRC_APPLICATION_MODEL_PROTOCOLS_H_
#define SRC_APPLICATION_MODEL_PROTOCOLS_H_

#include "data-manager.h"

namespace protocol {
namespace application {

/**
 * Abstract class which represents a protocol
 */
class ExecuteBase {
public:
    virtual ~ExecuteBase() {
    }
    virtual TraceMap& GetTracedCallbacks();
    /**
     * @brief Process the information inside the DataMap to specify which data to return to iCS
     * @param[out] data Data to return to iCS
     * @param[in] dataMap Information known to the rsu
     * @return true if data has to be sent to iCS. False otherwise
     */
    virtual bool Execute(DirectionValueMap& data, const DataMap& dataMap) = 0;
protected:
    /**
     * @brief List of traces of the protocol
     */
    TraceMap m_traceMap;
};

class CentralizedProtocol: public ExecuteBase {
public:
    static double SpaceThreshold;
    static bool ReturnData;
    CentralizedProtocol();
    virtual ~CentralizedProtocol();
    virtual bool Execute(DirectionValueMap&, const DataMap&);
private:
    /**
     * @brief Calculates the data to send back for a particular direction
     */
    void AggregateDataForDirection(const NodeDataMap&, ValueMap&);
    bool CompluteSpeed(const NodeInfo&, const NodeInfo&, double&, double&);
    static std::string TraceName;
    TracedCallback<std::vector<std::string>&> m_traceFlows;
};

} /* namespace application */
} /* namespace protocol */

#endif /* SRC_APPLICATION_MODEL_PROTOCOLS_H_ */
