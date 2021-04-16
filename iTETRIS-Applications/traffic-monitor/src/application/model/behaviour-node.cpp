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

namespace protocol {
namespace application {

///BehaviourNode implementation
bool BehaviourNode::Enabled = true;
uint16_t BehaviourNode::ResponseTimeSpacing = 10;
double BehaviourNode::SinkThreshold = 20;

BehaviourNode::BehaviourNode(iCSInterface* controller) :
    Behaviour(controller) {
    m_enabled = Enabled;
    m_responseTimeSpacing = ResponseTimeSpacing;

    m_rnd = ns3::UniformVariable();
    m_eventResponse = 0;

    RegisterTrace("NodeSendData", m_traceSendData);
}

BehaviourNode::~BehaviourNode() {
    Scheduler::Cancel(m_eventResponse);
}

void BehaviourNode::Start() {
    if (!m_enabled) {
        return;
    }
    Behaviour::Start();
}
void BehaviourNode::Stop() {
    Scheduler::Cancel(m_eventResponse);
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

///BehaviourNodeWithSink implementation

BehaviourNodeWithSink::BehaviourNodeWithSink(iCSInterface* controller) :
    BehaviourNode(controller) {
    m_sinkThreshold = SinkThreshold;
}

BehaviourNodeWithSink::~BehaviourNodeWithSink() {
}

void BehaviourNodeWithSink::Receive(server::Payload* payload, double snr) {
    NS_LOG_FUNCTION(Log());
    if (!m_enabled) {
        return;
    }
    CommHeader* commHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
    if (commHeader->getMessageType() != MT_RSU_BEACON) {
        NS_LOG_WARN(Log() << "Received an unknown message " << commHeader->getMessageType());
        return;
    }
    BeaconHeader* beaconHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_END, beaconHeader);
    int rsuId = commHeader->getSourceId();
    VehicleDirection dir(beaconHeader->getDirection(), beaconHeader->getVehicleMovement());
    if (m_muteRsu.id == rsuId && m_muteRsu.muted && m_muteRsu.dir == dir) {
        NS_LOG_INFO(
            Log() << "The sink threshold of " << m_sinkThreshold << " has been reached for the RSU " << rsuId << " direction " << dir);
        return;
    }
    //  if (GetController()->IsConformantDirection(beaconHeader->getDirection()))
    if (GetController()->IsConformantDirectionAndMovement(dir, commHeader->getSourcePosition())) {
        NodeInfo rsu;
        rsu.nodeId = rsuId;
        rsu.position = commHeader->getSourcePosition();
        rsu.conformantDirection = dir;
        if (m_muteRsu.id != rsuId && m_muteRsu.dir != dir) {
            m_muteRsu = RSU(rsuId, dir);
        }

        double nextTime = m_rnd.GetValue(m_responseTimeSpacing,
                                         beaconHeader->getMaxResponseTime() - m_responseTimeSpacing);
        Scheduler::Cancel(m_eventResponse);
        m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourNodeWithSink::EventSendResponse, this, rsu);
        NS_LOG_INFO(Log() << "scheduled a beacon response in " << nextTime);
    } else {
        NS_LOG_INFO(
            Log() << "beacon direction " << dir << " is not conformant with the current direction " << GetController()->GetDirection());
    }
}

void BehaviourNodeWithSink::EventSendResponse(NodeInfo rsu) {
    NS_LOG_FUNCTION(Log());
    //  if (!GetController()->IsConformantDirection(rsu.direction))
    if (!GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position)) {
        NS_LOG_WARN(
            Log() << "The node has a direction no longer conformant. CurrDir=" << GetController()->GetDirection() << ", RsuDir=" << rsu.conformantDirection);
        return;
    }
    double distance;
    bool last = false;
    if ((distance = GetDistance(rsu.position, GetController()->GetPosition())) < m_sinkThreshold) {
        if (rsu.conformantDirection.vMov == APPROACHING) {
            NS_LOG_INFO(
                Log() << "The node has crossed the sink threshold. Current distance=" << distance << " sink=" << m_sinkThreshold << ". Last message sent.");
            last = true;
            m_muteRsu.muted = true;
        } else {
            NS_LOG_INFO(
                Log() << "The node is still too close to the rsu. No response will be sent. Dir=" << rsu.conformantDirection << " distance=" << distance << " sink=" << m_sinkThreshold);
            return;
        }
    }

    NodeInfo node;
    node.nodeId = rsu.nodeId; //NodeId of the source
    node.position = GetController()->GetPosition();
    node.direction = GetController()->GetDirection();
    node.conformantDirection = rsu.conformantDirection;
    node.currentSpeed = GetController()->GetNodeSampler()->GetSpeed(1);
    node.avgSpeedSmall = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleSmall);
    node.avgSpeedHigh = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleHigh);
    node.lastMessage = last;

    BeaconResponseHeader* responseHeader = new BeaconResponseHeader();
    responseHeader->setSourceDirection(node.direction);
    responseHeader->setCurrentSpeed(node.currentSpeed);
    responseHeader->setAvgSpeedLow(node.avgSpeedSmall);
    responseHeader->setAvgSpeedHigh(node.avgSpeedHigh);
    responseHeader->setConformantDirection(node.conformantDirection.dir);
    responseHeader->setVehicleMovement(node.conformantDirection.vMov);
    responseHeader->setLastMessage(last);

    GetController()->SendTo(rsu.nodeId, responseHeader, PID_SPEED);
    m_traceSendData(node);
    NS_LOG_DEBUG(
        Log() << "Sent beacon response to RSU " << rsu.nodeId << " for direction=" << rsu.conformantDirection << " distance=" << distance);
}

///BehaviourNodeWithSink implementation

BehaviourNodeWithoutSink::BehaviourNodeWithoutSink(iCSInterface* controller) :
    BehaviourNode(controller) {
}

BehaviourNodeWithoutSink::~BehaviourNodeWithoutSink() {
}

void BehaviourNodeWithoutSink::Receive(server::Payload* payload, double snr) {
    NS_LOG_FUNCTION(Log());
    if (!m_enabled) {
        return;
    }
    CommHeader* commHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
    if (commHeader->getMessageType() != MT_RSU_BEACON) {
        NS_LOG_WARN(Log() << "Received an unknown message " << commHeader->getMessageType());
        return;
    }
    BeaconHeader* beaconHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_END, beaconHeader);
    int rsuId = commHeader->getSourceId();
    VehicleDirection dir(beaconHeader->getDirection(), beaconHeader->getVehicleMovement());
    NodeInfo rsu;
    rsu.nodeId = rsuId;
    rsu.position = commHeader->getSourcePosition();
    rsu.conformantDirection = dir;
    std::map<std::string, bool>::iterator it = m_activeDirections.find(dir.getId());
    if (it == m_activeDirections.end()) {
        m_activeDirections.insert(std::make_pair(dir.getId(), false));
    }
    if (GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position)
            || m_activeDirections[rsu.conformantDirection.getId()]) {

        double nextTime = m_rnd.GetValue(m_responseTimeSpacing,
                                         beaconHeader->getMaxResponseTime() - m_responseTimeSpacing);
        Scheduler::Cancel(m_eventResponse);
        m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourNodeWithoutSink::EventSendResponse, this, rsu);
        NS_LOG_INFO(Log() << "scheduled a beacon response in " << nextTime << " for direction " << dir);
    } else {
        NS_LOG_INFO(Log() << "direction " << dir << " not active");
    }
}

void BehaviourNodeWithoutSink::EventSendResponse(NodeInfo rsu) {
    NS_LOG_FUNCTION(Log());
    if (GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position)) {
        SendRespose(rsu);
    } else if (m_activeDirections[rsu.conformantDirection.getId()]) {
        SendNoLongerConformant(rsu);
    }
}

void BehaviourNodeWithoutSink::SendRespose(NodeInfo rsu) {
    NS_LOG_FUNCTION(Log());
    double distance = GetDistance(rsu.position, GetController()->GetPosition());
    m_activeDirections[rsu.conformantDirection.getId()] = true;

    NodeInfo node;
    node.nodeId = rsu.nodeId; //NodeId of the source
    node.position = GetController()->GetPosition();
    node.direction = GetController()->GetDirection();
    node.conformantDirection = rsu.conformantDirection;
    node.currentSpeed = GetController()->GetNodeSampler()->GetSpeed(1);
    node.avgSpeedSmall = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleSmall);
    node.avgSpeedHigh = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleHigh);
    node.lastMessage = false;

    BeaconResponseHeader* responseHeader = new BeaconResponseHeader();
    responseHeader->setSourceDirection(node.direction);
    responseHeader->setCurrentSpeed(node.currentSpeed);
    responseHeader->setAvgSpeedLow(node.avgSpeedSmall);
    responseHeader->setAvgSpeedHigh(node.avgSpeedHigh);
    responseHeader->setConformantDirection(node.conformantDirection.dir);
    responseHeader->setVehicleMovement(node.conformantDirection.vMov);
    responseHeader->setLastMessage(false);

    GetController()->SendTo(rsu.nodeId, responseHeader, PID_SPEED);
    m_traceSendData(node);
    NS_LOG_DEBUG(
        Log() << "Sent beacon response to RSU " << rsu.nodeId << " for direction=" << rsu.conformantDirection << " distance=" << distance);
}

void BehaviourNodeWithoutSink::SendNoLongerConformant(NodeInfo rsu) {
    NS_LOG_FUNCTION(Log());
    m_activeDirections[rsu.conformantDirection.getId()] = false;
    NodeInfo node;
    node.nodeId = rsu.nodeId; //NodeId of the source
    node.position = GetController()->GetPosition();
    node.direction = GetController()->GetDirection();
    node.distance = GetDistance(rsu.position, node.position);
    node.conformantDirection = rsu.conformantDirection;
    node.currentSpeed = -10;
    node.avgSpeedSmall = -10;
    node.avgSpeedHigh = -10;
    node.lastMessage = false;

    NoLongerConformantHeader* header = new NoLongerConformantHeader();
    header->setConformantDirection(rsu.conformantDirection.dir);
    header->setVehicleMovement(rsu.conformantDirection.vMov);
    header->setSourceDirection(node.direction);

    GetController()->SendTo(rsu.nodeId, header, PID_SPEED);
    m_traceSendData(node);
    NS_LOG_DEBUG(
        Log() << "Sent NoLongerConformant to RSU " << rsu.nodeId << ". My dir=" << node.direction << ", expected dir=" << rsu.conformantDirection << ". Dist=" << node.distance);
}
} /* namespace application */
} /* namespace protocol */
