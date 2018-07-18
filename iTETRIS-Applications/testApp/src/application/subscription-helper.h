/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
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

namespace testapp
{
	namespace application
	{

		/**
		 * Utility class to create the messages for the creation of the subscriptions
		 */
		class SubscriptionHelper
		{
			public:
				/**
				 * @brief To create a SetCamArea subscription
				 */
				static SubscriptionHolder * SetCamArea(const Circle & area);
				/**
				 * @brief To create a ReturnCarsInZone subscription
				 */
				static SubscriptionHolder * ReturnCarsInZone(const Circle & area);
				/**
				 * @brief To create a MobilityInformation subscription
				 * @param[in] ids Ids of the nodes for which get the information. Null means all nodes
				 */
				static SubscriptionHolder * GetMobilityInformation(const std::vector<int> * ids = NULL);
				/**
				 * @brief To create a TrafficLightInformation subscription
				 * @param[in] position Position of the semaphore. If Null it uses the node position
				 */
				static SubscriptionHolder * GetTrafficLightInformation(const Vector2D * position = NULL);
				/**
				 * @brief To create a AppCmdTrafficSim subscription to change the state of traffic light
				 * @param[in] trafficLightId Id of the traffic light
				 * @param[in] newStatus The new state of the traffic light
				 */
				static SubscriptionHolder * SetTrafficLight(const std::string & trafficLightId, const std::string & newStatus);
				/**
				 * @brief To create a AppMessageReceive subscription for unicast messages
				 * @param[in] destinationId Id of the destination node. Usually it is the node id
				 * @param[in] sourceIds Ids of the source nodes. Empty means every source node
				 */
				static SubscriptionHolder * ReceiveUnicast(const int destinationId, const std::vector<int> & sourceIds =
						std::vector<int>());
				/**
				 * @brief To create a AppMessageReceive subscription for unicast messages and receive only the messages from one source
				 * @param[in] destinationId Id of the destination node. Usually it is the node id
				 * @param[in] sourceId Id of the source node.
				 */
				static SubscriptionHolder * ReceiveUnicast(const int destinationId, const int sourceId);
				/**
				 * @brief To create a AppMessageReceive subscription for geobroadcast messages
				 * @param[in] messageCategory Category of the message. Used to filter the messages.
				 * @param[in] propagationCircle Reveice only the messages from this area. If Null any area is used
				 */
				static SubscriptionHolder * ReceiveGeobroadcast(const int messageCategory, const Circle * propagationCircle =
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
				static SubscriptionHolder * SendUnicast(const int sourceId, const short payloadLength, const int messageId,
						const std::vector<int> & destinationIds, const std::string & extra, const double time);
				/**
				 * @brief To create a AppMessageSend subscription to send a unicast message
				 * @param[in] sourceId Id of the source node.
				 * @param[in] payloadLength Simulated Length of the message
				 * @param[in] messageId Id of the message
				 * @param[in] destinationId Id of the destination node.
				 * @param[in] extra String shared with the recipient nodes. Used for the payload key obtained from PayloadStorage
				 * @param[in] time Time at which send the message
				 */
				static SubscriptionHolder * SendUnicast(const int sourceId, const short payloadLength, const int messageId,
						const int destinationId, const std::string & extra, const double time);
				/**
				 * @brief To create a AppMessageSend subscription to send a geobroadcast message
				 * @param[in] sourceId Id of the source node.
				 * @param[in] payloadLength Simulated Length of the message
				 * @param[in] messageCategory Category of the message
				 * @param[in] propagationCircle Area covered by the message.
				 * @param[in] extra String shared with the recipient nodes. Used for the payload key obtained from PayloadStorage
				 * @param[in] time Time at which send the message
				 */
				static SubscriptionHolder * SendGeobroadcast(const int sourceId, const short payloadLength,
						const int messageCategory, const Circle & propagationCircle, const std::string & extra, const double time);
				/**
				 * @brief To create a SumoTraciCommand subscription
				 * @param[in] executionId Id of the current request
				 * @param[in] commandStorage Message that will be sent to Traci
				 */
				static SubscriptionHolder * SumoTraciCommand(const int executionId, const tcpip::Storage & commandStorage);
				/**
				 * @brief To create a AppResultTrafficSim subscription to get the class of a node
				 * @param[in] nodeId Id of the node
				 */
				static SubscriptionHolder * GetNodeClass(const int nodeId);
			private:
				SubscriptionHelper();
				~SubscriptionHelper();
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* SUBSCRIPTION_HELPER_H_ */
