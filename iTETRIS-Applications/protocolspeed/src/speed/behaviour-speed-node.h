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

#ifndef BEHAVIOUR_SPEED_NODE_H_
#define BEHAVIOUR_SPEED_NODE_H_

#include "behaviour-node.h"
#include "scheduler.h"
#include "random-variable.h"
#include <map>
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {

/**
 * Installed on the nodes if IcsInterface::UseSink is set to true
 * It uses a threshold to communicate to the rsu that it has reached the center of the
 * intersection and that the current message will be its last one the current direction
 */
class BehaviourNodeWithSink: public BehaviourNode {
public:
    BehaviourNodeWithSink(iCSInterface* controller);
    virtual ~BehaviourNodeWithSink();

    virtual void Receive(server::Payload* payload, double snr);

private:
    /**
     * @brief Called by the receive after a random timeout when a beacon message is received
     * @brief The node send a BeaconResponse message
     */
    void EventSendResponse(NodeInfo);
    double m_sinkThreshold;
    RSU m_muteRsu;
};

/**
 * Installed on the nodes if IcsInterface::UseSink is set to false
 * It uses a no longer conformant message to communicate to the rsu that it
 * has passed the center of the intersection
 */
class BehaviourNodeWithoutSink: public BehaviourNode {
public:
    BehaviourNodeWithoutSink(iCSInterface* controller);
    virtual ~BehaviourNodeWithoutSink();

    virtual void Start();
    virtual void Receive(server::Payload* payload, double snr);

private:
    /**
     * @brief Called by the receive after a random timeout when a beacon message is received
     */
    void EventSendResponse(NodeInfo);
    /**
     * @brief Called by the EventSendResponse if the node has not yet passed the intersection
     * @brief The node send a BeaconResponse message
     */
    void SendRespose(NodeInfo);
    /**
     * @brief Called by the EventSendResponse if the node has passed the intersection
     * @brief The node send a NoLongerConformant message
     */
    void SendNoLongerConformant(NodeInfo);
    std::map<std::string, bool> m_activeDirections;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_SPEED_NODE_H_ */
