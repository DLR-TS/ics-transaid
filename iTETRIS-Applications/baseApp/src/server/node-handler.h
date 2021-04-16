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
#include <set>
#include "payload-storage.h"
#include "circular-buffer.h"
#include "structs.h"
#include "node.h"
#include "mobile-node.h"

namespace baseapp {
namespace application {
class BehaviourFactory;
class FixedStation;
class TMCBehaviour;
}
namespace server {
typedef std::map<int, application::Node*> NodeMap;

class NodeHandler {
public:

    class MessageReceptionListener {
    public:
        /// @brief To be called, when a message is received
        /// @note  The payload pointer will be deleted externally after this call.
        virtual void ReceiveMessage(int receiverID, server::Payload* payload, double snr, bool mobileNode = false) = 0;
    };



    NodeHandler(application::BehaviourFactory* factory);
    virtual ~NodeHandler();
    void updateTimeStep(const int timeStep);
    bool getNode(const int id, application::Node*& node) const;
    bool asStation(const int nodeId, application::FixedStation*& station) const;

    inline bool hasNode(const int id) const {
        return m_nodes.count(id);
    }

    void addNode(application::Node* node);

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
    //return if the node was removed successfully
    bool removeMobileNode(const int nodeId, const int ns3NodeId, const std::string& sumoNodeId);
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

    void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages);

    const std::string& getSumoID(int icsID) const;

    /// @brief Looks up icsID for given sumo ID, returns false, if the sumoID is not known
    /// @param[in] sumoID sumo id of node for which the ics id should be retrieved
    /// @param[out] icsID here the corresponding ics ID will be written, if the node exists
    bool getICSID(std::string sumoID, int& icsID) const;

    const NodeMap& getNodes() const {
        return m_nodes;
    }

    void setTMCBehaviour(application::TMCBehaviour* b);

    /// @brief Adds a RSU message reception listener.
    void addRSUMessageReceptionListener(std::shared_ptr<MessageReceptionListener> l);

    /// @brief Adds a RSU message reception listener.
    void addVehicleMessageReceptionListener(std::shared_ptr<MessageReceptionListener> l);

    /// @brief Adds a RSU message reception listener.
    void removeRSUMessageReceptionListener(std::shared_ptr<MessageReceptionListener> l);

    /// @brief Adds a RSU message reception listener.
    void removeVehicleMessageReceptionListener(std::shared_ptr<MessageReceptionListener> l);

private:

    /// @brief This method controls the execution of the TMC Behaviour, if existent.
    void checkTMCExecution(const application::Node* node);

    /// @brief This method monitores the request for subscriptions by the TMC Behaviour, if existent.
    void checkTMCSubscriptionRequests(const application::Node* node);

    NodeMap m_nodes;
    std::map<std::string, int> m_sumoICSIDMap;

    /// @brief Whether the TMC was already asked for general subscriptions to be issued in this sim step
    bool askedTMCForSubscriptions;
    /// @brief Whether the TMC was already executed in this sim step
    int executedRSUs;
    /// @brief All RSU ids whrer the application is installed
    std::set<int> rsuIDs;


    PayloadStorage* m_storage;
    CircularBuffer<int>* m_timeStepBuffer;

    /// @brief Logic for the traffic management control, @see BehaviourFactory
    ///        The TMC Behaviour receives a copy of all received messages for the RSUs
    std::shared_ptr<application::TMCBehaviour> m_TMCBehaviour;
    application::BehaviourFactory* m_factory;
    static std::string emptyString;


    // Instances, that listen for message receptions at RSUs
    std::set<std::shared_ptr<MessageReceptionListener> > m_RSUMessageReceptionListeners;
    std::set<std::shared_ptr<MessageReceptionListener> > m_VehicleMessageReceptionListeners;
};

} /* namespace server */
} /* namespace protocol */

#endif /* NODEHANDLER_H_ */
