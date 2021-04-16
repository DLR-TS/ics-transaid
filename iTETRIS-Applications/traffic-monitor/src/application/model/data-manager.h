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
#ifndef SRC_APPLICATION_MODEL_DATA_MANAGER_H_
#define SRC_APPLICATION_MODEL_DATA_MANAGER_H_

#include "behaviour.h"
#include "scheduler.h"
#include <map>
#include <set>
#include <vector>

namespace protocol {
namespace application {
class ExecuteBase;

struct NodeInfoOrdering {
    bool operator()(const NodeInfo* left, const NodeInfo* right);
};

/**
 * Data structures
 * NodeDataCollection set of information about a node. The last position contains the most recent data
 * NodeDataMap map of the node information of the nodes for a direction. key is the node id. value is the information for the node
 * DataMap map of information for the different direction. key is the id of a direction. value is the data known about that direction
 */
typedef std::set<NodeInfo*, NodeInfoOrdering> NodeDataCollection;
typedef std::map<const int, NodeDataCollection> NodeDataMap;
typedef std::map<const std::string, NodeDataMap> DataMap;

/**
 * Class which saves the information about the nodes. Installed only on he rsu
 * It received the data from the class BehavioutRsu through the traces
 */
class DataManager: public Behaviour {
public:
    static uint16_t ExecuteTime;
    static bool EnableCentralizedProtocol;
    static bool Enabled;

    DataManager(iCSInterface*);
    virtual ~DataManager();

    void Start();
    void Stop();
    /**
     * @brief Always returns false.
     */
    virtual bool IsSubscribedTo(ProtocolId pid) const;
    /**
     * @brief Never called.
     */
    virtual void Receive(server::Payload* payload, double snr);
    /**
     * @brief Calls every protocol so in can execute
     */
    virtual bool Execute(DirectionValueMap& data);

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_DATA_MANAGER;
    }
private:
    bool AddData(NodeInfo*);
    bool GetNodeCollection(NodeInfo*, NodeDataCollection&);
    void RemoveData(const std::string&, const int&);
    void RemoveData(const NodeDataMap::iterator&);
    void RemoveAll();

    //Events
    /**
     * @brief Called when a repose message from a node is received
     */
    void OnBeaconResponse(NodeInfo*);
    /**
     * @brief Called when a no longer conformant message from a node is received
     */
    void OnNoLongerConforman(NodeInfo*);
    /**
     * @brief Called when a node times out
     */
    void OnTimeOutNode(NodeInfo*);
    /**
     * @brief Called when the last repose message from a node is received
     */
    void OnLastMessageNode(NodeInfo*);

    DataMap m_dataMap;

    event_id m_eventExecute;
    void EventExecute();
    bool m_executeAtThisStep;

    typedef std::vector<std::pair<std::string, int> > RemoveList;
    RemoveList m_ToRemove;

    /**
     * @brief List of protocol that can process the data
     */
    typedef std::vector<ExecuteBase*> ProtocolList;
    ProtocolList m_protocols;
};


} /* namespace application */
} /* namespace protocol */

#endif /* SRC_APPLICATION_MODEL_DATA_MANAGER_H_ */
