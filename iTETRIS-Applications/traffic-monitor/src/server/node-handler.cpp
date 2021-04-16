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

#include "node-handler.h"
#include "current-time.h"
#include "program-configuration.h"
#include "fixed-station.h"
#include "log/log.h"
#include "log/ToString.h"

#include "fixed-station.h"
#include <sstream>
#include <climits>

namespace protocol {
namespace server {

using namespace std;
using namespace application;

NodeHandler::NodeHandler() {
    m_storage = new PayloadStorage();
    m_timeStepBuffer = new CircularBuffer<int>(ProgramConfiguration::GetMessageLifetime());
}

NodeHandler::~NodeHandler() {
    if (m_nodes.size() > 0) {
        for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
            delete it->second;
        }
        m_nodes.clear();
    }
    delete m_storage;
    delete m_timeStepBuffer;
}

void NodeHandler::updateTimeStep(const int timeStep) {
    int oldTimeStep;
    if (m_timeStepBuffer->addValue(timeStep, oldTimeStep)) {
        m_storage->expiredPayloadCleanUp(oldTimeStep);
    }
}

bool NodeHandler::getNode(const int id, Node*& node) const {
    NodeMap::const_iterator it = m_nodes.find(id);
    if (it == m_nodes.end()) {
        return false;
    }
    node = it->second;
    return true;
}

bool NodeHandler::asStation(const int nodeId, FixedStation*& station) const {
    Node* node;
    if (getNode(nodeId, node)) {
        station = dynamic_cast<application::FixedStation*>(node);
        return station != NULL;
    }
    return false;
}

bool NodeHandler::createMobileNode(const int nodeId, const int ns3NodeId, const std::string& sumoNodeId,
                                   const std::string& sumoType, const std::string& sumoClass) {
    if (m_nodes.find(nodeId) != m_nodes.end()) {
        return false;
    }
    Node* node = new MobileNode(nodeId, ns3NodeId, sumoNodeId, sumoType, sumoClass);
    ostringstream oss;
    oss << "Added new mobile node with id " << nodeId << " ns3id " << ns3NodeId << " sumoId " << sumoNodeId
        << " sumoType " << sumoType << " sumoClass " << sumoClass;
    Log::WriteLog(oss);
    addNode(node);
    return true;
}

bool NodeHandler::askForSubscription(const int nodeId, const int subscriptionId, tcpip::Storage*& request) {
    Node* node;
    if (!getNode(nodeId, node)) {
        if (ProgramConfiguration::IsRsu(nodeId)) {
            node = new FixedStation(nodeId);
            Log::WriteLog(ostringstream("Added new fixed station with id " + toString(nodeId)));
        } else {
            node = new MobileNode(nodeId);
            Log::WriteLog(ostringstream("Added new mobile node with id " + toString(nodeId)));
        }
        addNode(node);
    }
    return node->askForSubscription(subscriptionId, request);
}

bool NodeHandler::endSubscription(const int nodeId, const int subscriptionId, const int subscriptionType) {
    Node* node;
    if (getNode(nodeId, node)) {
        bool result = node->isToUnsubscribe(subscriptionId);
        if (result) {
            node->removeSubscription(subscriptionId);
        }
        return result;
    }
    return true;
}

int NodeHandler::mobilityInformation(const int nodeId, const vector<MobilityInfo*>& info) {
    int count = 0;
    for (vector<MobilityInfo*>::const_iterator it = info.begin(); it != info.end(); ++it) {
        Node* node;
        if (getNode((*it)->id, node)) {
            node->updateMobilityInformation(*it);
        } else {
            if ((*it)->isMobile) {
                node = new MobileNode(*it);
            } else {
                node = new FixedStation((*it)->id);
                node->updateMobilityInformation(*it);
            }
            Log::WriteLog(ostringstream("Added new node with id " + toString((*it)->id)));
            ++count;
            addNode(node);
        }
    }
    FixedStation* node;
    if (asStation(nodeId, node)) {
        node->mobilityInformationHasRun();
    }
    return count;
}

string NodeHandler::insertPayload(const Payload* payload, bool deleteOnRead) {
    StoragePolicy policy = deleteOnRead ? kDeleteOnRead : kMultipleRead;
    return m_storage->insert(payload, policy);
}

void NodeHandler::applicationMessageReceive(const vector<Message>& messages) {
    for (vector<Message>::const_iterator it = messages.begin(); it != messages.end(); ++it) {
        Node* node;
        if (getNode(it->m_destinationId, node)) {
            Payload* payload = NULL;
            if (m_storage->find(it->m_extra, payload)) {
                payload->snr = it->m_snr;
            }
            node->applicationMessageReceive(it->m_messageId, payload);
            //The payload is deleted if necessary
            if (m_storage->asPolicy(it->m_extra) == kDeleteOnRead) {
                delete payload;
            }
        }
    }
}

bool NodeHandler::applicationExecute(const int nodeId, DirectionValueMap& data) {
    if (ProgramConfiguration::GetStartTime() >= CurrentTime::Now()) {
        return false;
    }
    Node* node;
    if (getNode(nodeId, node)) {
        return node->applicationExecute(data);
    }
    return false;
}

void NodeHandler::ConfirmSubscription(const int nodeId, const int subscriptionId, const bool status) {
    if (status) {
        Node* node;
        if (getNode(nodeId, node)) {
            node->setToUnsubscribe(subscriptionId);
            ostringstream log;
            log << "[Node " << nodeId << "] Confirmed subscription " << subscriptionId
                << ". Will be unsuscribed on the next timestep";
            Log::WriteLog(log);
        }
    } else {
        ostringstream log;
        log << "[Node " << nodeId << "] Error scheduling subscription " << subscriptionId;
        Log::Write(log, kLogLevelWarning);
    }
}

void NodeHandler::deleteNode(int nodeId) {
    NodeMap::iterator it = m_nodes.find(nodeId);
    if (it != m_nodes.end()) {
        delete it->second;
        m_nodes.erase(it);
    }
}

void NodeHandler::trafficLightInformation(const int nodeId, const bool error, const std::vector<std::string>& data) {
    FixedStation* station;
    if (asStation(nodeId, station)) {
        station->trafficLightInformation(error, data);
    }
}

void NodeHandler::setToUnsubscribe(const int nodeId, const int subscriptionId) {
    Node* station;
    if (getNode(nodeId, station)) {
        station->setToUnsubscribe(subscriptionId);
    }
}

void NodeHandler::sumoTraciCommandResult(const int nodeId, const int executionId, tcpip::Storage& storage) {
    Node* station;
    if (getNode(nodeId, station)) {
        station->sumoTraciCommandResult(executionId, storage);
    }
}

} /* namespace server */
} /* namespace protocol */
