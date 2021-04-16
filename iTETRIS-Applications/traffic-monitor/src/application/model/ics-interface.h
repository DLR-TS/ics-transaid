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
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <map>
#include "payload.h"
#include "fatal-error.h"
#include "trace-manager.h"
#include "headers.h"
#include "structs.h"
#include "traci-helper.h"

namespace protocol {
namespace application {

class Behaviour;
class Node;
class NodeSampler;

/**
 * Class used as interface between the actual protocol logic and the
 * application framework that interacts with iCS
 */
class iCSInterface: public TraceManager {
public:
    static double DirectionTolerance;
    static uint16_t AverageSpeedSampleSmall;
    static uint16_t AverageSpeedSampleHigh;
    static bool UseSink;

    iCSInterface(Node*, NodeType);
    virtual ~iCSInterface();
    /**
     * @brief Get the node instance on which it is installed this interface.
     * @brief Used to interact directly with with the iCS interface
     */
    Node* GetNode() const;
    /**
     * @brief Get the sampler instance. It introduces errors to the position to simulate the imprecision of the devices
     */
    NodeSampler* GetNodeSampler() const;
    /**
     * @brief Get a Behaviour from its type
     */
    Behaviour* GetBehaviour(TypeBehaviour type) const;

    /************************
     * Utility methods
     */
    /**
     * @brief Get the id  of the node
     */
    int GetId() const;
    /**
     * @brief If the node is active
     */
    bool IsActive() const;
    NodeType GetNodeType() const;
    Vector2D GetPosition() const;
    double GetDirection() const;
    /**
     * @brief Name of the node. For logging
     */
    std::string NodeName() const;
    /**
     * @brief Activates the node
     */
    void Activate();
    /**
     * @brief Deactivates the node
     */
    void Deactivate();
    /**
     * @brief Check if the node is going on a particular direction, using the set tolerance
     */
    bool IsConformantDirection(double direction);
    /**
     * @brief Check if the node is going on a particular direction, using the set tolerance
     * @brief If also check if the node is arriving at the intersection of leaving it
     */
    bool IsConformantDirectionAndMovement(const VehicleDirection direction, const Vector2D rsuPosition);
    /**
     * @brief Static method to check if two direction differ by less than the tolerance
     */
    static bool CheckDirections(const double& dir1, const double& dir2, const double& tolerance);
    /**
     * @brief Static method to check if two direction differ by less than the tolerance
     * @brief and if the movement of the node is the same as the one specified in directionFromRsu
     */
    static bool CheckDirectionAndMovement(const double& nodeDirection, const VehicleDirection& directionFromRsu,
                                          const double& tolerance, const Vector2D& nodePosition, const Vector2D& rsuePosition);
    /**
     * @brief Utility method to retrieve a header from a payload
     */
    template<typename T>
    void GetHeader(server::Payload* payload, server::Position position, T& header) const {
        Header* tmp = payload->getHeader(position);
        header = dynamic_cast<T>(tmp);
        if (header == NULL) {
            NS_FATAL_ERROR(LogNode() << "Wrong header type");
        }
    }

    /************************
     * iCS interaction methods
     */
    /**
     * @brief Used to send a Geobroadcast message.
     * @param[in] dstType type of the destination
     * @param[in] header Message to send
     * @param[in] pid Protocol id
     */
    void Send(NodeType dstType, Header* header, ProtocolId pid);
    /**
     * @brief Used to send a Unicast message.
     * @param[in] destId Id of the destination node
     * @param[in] header Message to send
     * @param[in] pid Protocol id
     */
    void SendTo(int destId, Header* header, ProtocolId pid);
    /**
     * @brief Called by the node class when the node has received the message from iCS
     * @param[in] payload Message received
     * @return true if the message was for the node. False if the message has been discarded
     */
    bool Receive(server::Payload* payload);
    /**
     * @brief Called by the node classwhen iCS ask the application to execute.
     * @param[out] data Data to send back to iCS. The application has to fill the map
     * @return Whatever the application executed. If true data will be sent to iCS. If false data is discarded
     */
    bool Execute(DirectionValueMap& data);
    /**
     * @brief Called by the node class when the reply from a sumo command is received.
     * @param[in] executionId The id of the command.
     * @param[in] storage The reply from traci.
     */
    void TraciCommandResult(const int executionId, tcpip::Storage& traciReply);


private:
    /**
     * @brief Add a new behaviour to the node
     */
    void SubscribeBehaviour(Behaviour* behaviour);
    /**
     * @brief Utility method to get the node name. Used in the logs
     */
    std::string LogNode() const;
    /**
     * @brief Common code called by Send and SendTo
     */
    Header* DoSend(NodeType, Header*, ProtocolId, int);

    Node* m_node;
    NodeSampler* m_nodeSampler;
    NodeType m_nodeType;
    bool m_active;
    double m_direction_tolerance;

    // behaviours collection
    typedef std::map<TypeBehaviour, Behaviour*> BehaviourMap;
    BehaviourMap m_behaviours;

    TracedCallback<bool> m_traceStateChange;
    TracedCallback<server::Payload*> m_traceReceive;
    TracedCallback<server::Payload*> m_traceSend;

    /*
     * Example of usage of the SumoTraciCommand subscription
     */
    int traciGetSpeed;
    int traciSetMaxSpeed;
    void AddTraciSubscription();
};

} /* namespace application */
} /* namespace protocol */

#endif /* CONTROLLER_H_ */
