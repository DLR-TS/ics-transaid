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


#ifndef SUBSCRIPTION_HELPER_H_
#define SUBSCRIPTION_HELPER_H_

#include "tcpip/storage.h"
#include "program-configuration.h"
#include "structs.h"
#include <vector>
#include "vector.h"

namespace protocol {
namespace application {

/**
 * Utility class to create the messages for the creation of the subscriptions
 */
class SubscriptionHelper {
public:
    /**
     * @brief To create a SetCamArea subscription
     */
    static SubscriptionHolder* SetCamArea(const Circle& area);
    /**
     * @brief To create a ReturnCarsInZone subscription
     */
    static SubscriptionHolder* ReturnCarsInZone(const Circle& area);
    /**
     * @brief To create a MobilityInformation subscription
     * @param[in] ids Ids of the nodes for which get the information. Null means all nodes
     */
    static SubscriptionHolder* GetMobilityInformation(const std::vector<int>* ids = NULL);
    /**
     * @brief To create a TrafficLightInformation subscription
     * @param[in] position Position of the semaphore. If Null it uses the node position
     */
    static SubscriptionHolder* GetTrafficLightInformation(const Vector2D* position = NULL);
    /**
     * @brief To create a AppCmdTrafficSim subscription to change the state of traffic light
     * @param[in] trafficLightId Id of the traffic light
     * @param[in] newStatus The new state of the traffic light
     */
    static SubscriptionHolder* SetTrafficLight(const std::string& trafficLightId, const std::string& newStatus);
    /**
     * @brief To create a AppMessageReceive subscription for unicast messages
     * @param[in] destinationId Id of the destination node. Usually it is the node id
     * @param[in] sourceIds Ids of the source nodes. Empty means every source node
     */
    static SubscriptionHolder* ReceiveUnicast(const int destinationId, const std::vector<int>& sourceIds =
                std::vector<int>());
    /**
     * @brief To create a AppMessageReceive subscription for unicast messages and receive only the messages from one source
     * @param[in] destinationId Id of the destination node. Usually it is the node id
     * @param[in] sourceId Id of the source node.
     */
    static SubscriptionHolder* ReceiveUnicast(const int destinationId, const int sourceId);
    /**
     * @brief To create a AppMessageReceive subscription for geobroadcast messages
     * @param[in] messageCategory Category of the message. Used to filter the messages.
     * @param[in] propagationCircle Reveice only the messages from this area. If Null any area is used
     */
    static SubscriptionHolder* ReceiveGeobroadcast(const int messageCategory, const Circle* propagationCircle =
                NULL);
    /**
     * @brief To create a AppMessageSend subscription to send a unicast message
     * @param[in] sourceId Id of the source node.
     * @param[in] payloadLength Simulated Length of the message
     * @param[in] messageId Id of the message
     * @param[in] destinationIds List of destination nodes.
     * @param[in] extra String shared with the recipient nodes. Used for the payload key obtained from PayloadStorage
     * @param[in] time Time at which send the message
     */
    static SubscriptionHolder* SendUnicast(const int sourceId, const short payloadLength, const int messageId,
                                           const std::vector<int>& destinationIds, const std::string& extra, const double time);
    /**
     * @brief To create a AppMessageSend subscription to send a unicast message
     * @param[in] sourceId Id of the source node.
     * @param[in] payloadLength Simulated Length of the message
     * @param[in] messageId Id of the message
     * @param[in] destinationId Id of the destination node.
     * @param[in] extra String shared with the recipient nodes. Used for the payload key obtained from PayloadStorage
     * @param[in] time Time at which send the message
     */
    static SubscriptionHolder* SendUnicast(const int sourceId, const short payloadLength, const int messageId,
                                           const int destinationId, const std::string& extra, const double time);
    /**
     * @brief To create a AppMessageSend subscription to send a geobroadcast message
     * @param[in] sourceId Id of the source node.
     * @param[in] payloadLength Simulated Length of the message
     * @param[in] messageCategory Category of the message
     * @param[in] propagationCircle Area covered by the message.
     * @param[in] extra String shared with the recipient nodes. Used for the payload key obtained from PayloadStorage
     * @param[in] time Time at which send the message
     */
    static SubscriptionHolder* SendGeobroadcast(const int sourceId, const short payloadLength,
            const int messageCategory, const Circle& propagationCircle, const std::string& extra, const double time);
    /**
     * @brief To create a SumoTraciCommand subscription
     * @param[in] executionId Id of the current request
     * @param[in] commandStorage Message that will be sent to Traci
     */
    static SubscriptionHolder* SumoTraciCommand(const int executionId, const tcpip::Storage& commandStorage);
    /**
     * @brief To create a AppResultTrafficSim subscription to get the class of a node
     * @param[in] nodeId Id of the node
     */
    static SubscriptionHolder* GetNodeClass(const int nodeId);
private:
    SubscriptionHelper();
    ~SubscriptionHelper();
};

} /* namespace application */
} /* namespace protocol */

#endif /* SUBSCRIPTION_HELPER_H_ */
