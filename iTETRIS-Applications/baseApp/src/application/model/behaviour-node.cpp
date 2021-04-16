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

#include "behaviour-node.h"
#include "ics-interface.h"
#include "node-sampler.h"

namespace baseapp {
namespace application {

///BehaviourNode implementation
bool BehaviourNode::Enabled = true;
double BehaviourNode::SinkThreshold = 20;

BehaviourNode::BehaviourNode(iCSInterface* controller) :
    Behaviour(controller) {
    m_enabled = Enabled;
    m_responseTimeSpacing = Behaviour::DefaultResponseTimeSpacing;
    m_rnd = ns3::UniformVariable();
    m_eventResponse = 0;

    RegisterTrace("NodeSendData", m_traceSendData);
}

BehaviourNode::~BehaviourNode()
{}

void BehaviourNode::Start() {
    if (!m_enabled) {
        return;
    }
    Behaviour::Start();
}
void BehaviourNode::Stop() {
    Behaviour::Stop();
}

bool BehaviourNode::IsSubscribedTo(ProtocolId pid) const {
    return pid == PID_SPEED;
}

void BehaviourNode::Receive(server::Payload* payload, double snr) {
}

bool BehaviourNode::Execute(DirectionValueMap& data) {
    return false;
}


} /* namespace application */
} /* namespace protocol */
