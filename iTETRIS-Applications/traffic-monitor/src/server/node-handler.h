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

#ifndef NODEHANDLER_H_
#define NODEHANDLER_H_

#include <map>
#include "payload-storage.h"
#include "circular-buffer.h"
#include "structs.h"
#include "node.h"
#include "mobile-node.h"

namespace protocol {
namespace application {
class FixedStation;
}
namespace server {
typedef std::map<int, application::Node*> NodeMap;

class NodeHandler {
public:
    NodeHandler();
    virtual ~NodeHandler();
    void updateTimeStep(const int timeStep);
    bool getNode(const int id, application::Node*& node) const;
    bool asStation(const int nodeId, application::FixedStation*& station) const;

    inline bool hasNode(const int id) const {
        return m_nodes.count(id);
    }

    inline void addNode(application::Node* node) {
        m_nodes.insert(std::make_pair(node->getId(), node));
    }

    inline bool asMobileNode(application::Node* node, application::MobileNode*& mobileNode) const {
        mobileNode = dynamic_cast<application::MobileNode*>(node);
        return mobileNode != NULL;
    }

    inline NodeMap::const_iterator begin() const {
        return m_nodes.begin();
    }

    inline NodeMap::const_iterator end() const {
        return m_nodes.end();
    }

    //return whatever a new node was created
    bool createMobileNode(const int nodeId, const int ns3NodeId, const std::string& sumoNodeId,
                          const std::string& sumoType, const std::string& sumoClass);
    //returns whatever it has added a new subscription
    bool askForSubscription(const int nodeId, const int subscriptionId, tcpip::Storage*& request);
    //returns if a subscription needs to be dropped
    bool endSubscription(const int nodeId, const int subscriptionId, const int subscriptionType);
    //returns the new cars that have entered the zone
    int mobilityInformation(const int nodeId, const std::vector<MobilityInfo*>& info);
    void trafficLightInformation(const int nodeId, const bool error, const std::vector<std::string>& data);
    //returns the id of the payload
    std::string insertPayload(const Payload* payload, bool deleteOnRead = true);
    void applicationMessageReceive(const std::vector<Message>& messages);
    //returns if there is data to send to iCS
    bool applicationExecute(const int nodeId, DirectionValueMap& data);
    void ConfirmSubscription(const int nodeId, const int subscriptionId, const bool status);

    void deleteNode(int);
    void setToUnsubscribe(const int nodeId, const int subscriptionId);
    void sumoTraciCommandResult(const int nodeId, const int executionId, tcpip::Storage& storage);

    //  Don't need it
    //  bool commandTrafficSimulation();
    //  bool resultTrafficSimulation();
    //  bool xApplicationData();
    //  bool notifyApplicationMessageStatus();

private:
    NodeMap m_nodes;
    PayloadStorage* m_storage;
    CircularBuffer<int>* m_timeStepBuffer;
};

} /* namespace server */
} /* namespace protocol */

#endif /* NODEHANDLER_H_ */
