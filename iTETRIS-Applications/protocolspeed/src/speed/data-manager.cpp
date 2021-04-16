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
#include "data-manager.h"
#include "log/log.h"
#include "behaviour-rsu.h"
#include "ics-interface.h"
#include "protocols.h"
#include "log/console.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {

bool NodeInfoOrdering::operator()(const NodeInfo* left, const NodeInfo* right) {
//			NS_LOG_FUNCTION(left->lastSeen<<"<"<<right->lastSeen);
    return left->lastSeen < right->lastSeen;
}

uint16_t DataManager::ExecuteTime = 1000;
bool DataManager::EnableCentralizedProtocol = true;
bool DataManager::Enabled = true;

DataManager::DataManager(iCSInterface* controller) :
    Behaviour(controller) {
    m_enabled = Enabled;
    m_executeAtThisStep = false;
    m_eventExecute = 0;

    //Register protocols
    if (EnableCentralizedProtocol) {
        m_protocols.push_back(new CentralizedProtocol());
    }
    for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol) {
        ExecuteBase* eb = *(protocol);
        for (TraceMap::iterator it = eb->GetTracedCallbacks().begin(); it != eb->GetTracedCallbacks().end(); ++it) {
            RegisterTrace(it->first, *(it->second));
        }
    }
}

DataManager::~DataManager() {
    for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol) {
        delete *protocol;
    }
    m_protocols.clear();
    RemoveAll();
    Scheduler::Cancel(m_eventExecute);
}
void DataManager::RemoveAll() {
    for (DataMap::iterator dir = m_dataMap.begin(); dir != m_dataMap.end(); ++dir) {
        for (NodeDataMap::iterator node = dir->second.begin(); node != dir->second.end(); ++node) {
            RemoveData(node);
        }
        dir->second.clear();
    }
    m_dataMap.clear();
}
bool DataManager::AddData(NodeInfo* info) {
    DataMap::iterator dir = m_dataMap.find(info->conformantDirection.getId());
    if (dir == m_dataMap.end()) {
        dir = m_dataMap.insert(std::make_pair(info->conformantDirection.getId(), NodeDataMap())).first;
    }
    NodeDataMap::iterator node = dir->second.find(info->nodeId);
    if (node == dir->second.end()) {
        node = dir->second.insert(std::make_pair(info->nodeId, NodeDataCollection())).first;
    }
    bool val = node->second.insert(info).second;
    return val;
}
bool DataManager::GetNodeCollection(NodeInfo* info, NodeDataCollection& collection) {
    DataMap::iterator dir = m_dataMap.find(info->conformantDirection.getId());
    if (dir != m_dataMap.end()) {
        NodeDataMap::iterator node = dir->second.find(info->nodeId);
        if (node != dir->second.end()) {
            collection = node->second;
            return true;
        }
    }
    return false;
}
void DataManager::RemoveData(const std::string& dir, const int& node) {
    DataMap::iterator it = m_dataMap.find(dir);
    if (it != m_dataMap.end()) {
        NodeDataMap::iterator itNDM = it->second.find(node);
        if (itNDM != it->second.end()) {
            RemoveData(itNDM);
            it->second.erase(node);
        }
    }
}
void DataManager::RemoveData(const NodeDataMap::iterator& nodeIt) {
    for (NodeDataCollection::iterator info = nodeIt->second.begin(); info != nodeIt->second.end(); ++info) {
        delete *info;
    }
    nodeIt->second.clear();
}
void DataManager::Start() {
    if (!m_enabled) {
        return;
    }
    NS_LOG_FUNCTION(Log());
    BehaviourRsu* rsu = (BehaviourRsu*) GetController()->GetBehaviour(BehaviourRsu::Type());
    rsu->TraceConnect("NodeReceiveData", MakeCallback(&DataManager::OnBeaconResponse, this));
    rsu->TraceConnect("NodeTimeOut", MakeCallback(&DataManager::OnTimeOutNode, this));
    rsu->TraceConnect("NodeLastMessage", MakeCallback(&DataManager::OnLastMessageNode, this));
    rsu->TraceConnect("NodeNoLongerConforman", MakeCallback(&DataManager::OnNoLongerConforman, this));
    m_eventExecute = Scheduler::Schedule(ExecuteTime, &DataManager::EventExecute, this);
    const std::vector<VehicleDirection> dirs = rsu->GetDirections();
    for (std::vector<VehicleDirection>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
        m_dataMap[dir->getId()] = NodeDataMap();
    }
    Behaviour::Start();
    m_executeAtThisStep = false;
}
void DataManager::Stop() {
    NS_LOG_FUNCTION(Log());
    TraceManager* rsu = GetController()->GetBehaviour(BehaviourRsu::Type());
    rsu->TraceDisconnect("NodeReceiveData", MakeCallback(&DataManager::OnBeaconResponse, this));
    rsu->TraceDisconnect("NodeTimeOut", MakeCallback(&DataManager::OnTimeOutNode, this));
    rsu->TraceDisconnect("NodeLastMessage", MakeCallback(&DataManager::OnLastMessageNode, this));
    rsu->TraceDisconnect("NodeNoLongerConforman", MakeCallback(&DataManager::OnNoLongerConforman, this));
    Scheduler::Cancel(m_eventExecute);
    RemoveAll();
    Behaviour::Stop();
}

bool DataManager::IsSubscribedTo(ProtocolId pid) const {
    //Don't want to receive any message
    return false;
}
void DataManager::Receive(server::Payload* payload, double snr) {
    //Should never be called. Do nothing
}

void DataManager::EventExecute() {
    m_executeAtThisStep = true;
    m_eventExecute = Scheduler::Schedule(ExecuteTime, &DataManager::EventExecute, this);
}

bool DataManager::Execute(const int currentTimeStep, DirectionValueMap& data) {
    if (m_executeAtThisStep) {
        m_executeAtThisStep = false;
        if (m_dataMap.size() == 0) {
            return false;
        }
        NS_LOG_FUNCTION(Log() << "Executing protocols");

        bool executed = false;
        for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol) {
            ExecuteBase* eb = *(protocol);
            if (eb->Execute(data, m_dataMap)) {
                executed = true;
            }
        }
        //Now remove the marked nodes
        for (RemoveList::iterator it = m_ToRemove.begin(); it != m_ToRemove.end(); ++it) {
            RemoveData(it->first, it->second);
        }
        m_ToRemove.clear();
        return executed;
    }
    return false;
}

void DataManager::OnBeaconResponse(NodeInfo* info) {
    bool newData = AddData(info);
    NS_LOG_FUNCTION((newData ? "true " : "false ") << info->nodeId << " " << info->lastSeen);
}
void DataManager::OnNoLongerConforman(NodeInfo* info) {
    NodeDataCollection collection;
    if (GetNodeCollection(info, collection)) {
        NodeInfo* firstMessage = *(collection.begin());
        NodeInfo* lastMessage = *(collection.rbegin());
        //Use the time of the last valid message or of the current invalid one?
        lastMessage->totalTime = lastMessage->lastSeen - firstMessage->lastSeen;
        lastMessage->toRemove = true;
        lastMessage->lastMessage = true;
//			Mark the data for removal on the next execution
        m_ToRemove.push_back(std::make_pair(info->conformantDirection.getId(), info->nodeId));
    }
    delete info;
}
void DataManager::OnTimeOutNode(NodeInfo* info) {
    RemoveData(info->conformantDirection.getId(), info->nodeId);
    delete info;
}
void DataManager::OnLastMessageNode(NodeInfo* info) {
    NodeDataCollection collection;
    if (GetNodeCollection(info, collection)) {
        NodeInfo* firstMessage = *(collection.begin());
        info->totalTime = info->lastSeen - firstMessage->lastSeen;
    } else {
        info->totalTime = 0;
    }
    bool newData = AddData(info);
//			Mark the data for removal on the next execution
    m_ToRemove.push_back(std::make_pair(info->conformantDirection.getId(), info->nodeId));
    NS_LOG_FUNCTION((newData ? "true " : "false ") << info->nodeId << " " << info->lastSeen);
}

} /* namespace application */
} /* namespace protocol */
