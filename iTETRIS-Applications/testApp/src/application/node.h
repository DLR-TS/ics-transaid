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

#ifndef NODE_H_
#define NODE_H_

#include <map>
#include <queue>
#include "foreign/tcpip/storage.h"
#include "structs.h"
#include "vector.h"
#include "random-variable.h"
#include "common.h"

namespace testapp
{
	namespace server
	{
		class Payload;
	}
	namespace application
	{
		const int INVALID_INT = -1;
		const std::string INVALID_STRING = "";
		class iCSInterface;

		/**
		 * Represent a subscription of a node
		 */
		struct Subscription
		{

				Subscription(int id, int nodeId, int subscriptionType) :
						m_id(id), m_nodeId(nodeId), m_subscriptionType(subscriptionType)
				{
					m_toUnsubscribe = false;
				}

				Subscription(int id, int nodeId, int subscriptionType, bool toUnsubscribe) :
						m_id(id), m_nodeId(nodeId), m_subscriptionType(subscriptionType), m_toUnsubscribe(toUnsubscribe)
				{
				}
				int m_id;
				int m_nodeId;
				int m_subscriptionType;
				bool m_toUnsubscribe;
		}typedef Subscription;

		/**
		 * Represents a node. Abstract class
		 */
		class Node
		{
			public:
				static double ProbabilityFull;
				static double ProbabilityMedium;
				static float PropagationRagiusRsu;
				static float PropagationRagiusFull;
				static float PropagationRagiusMedium;

				virtual ~Node();

				/******************************
				 * Subscription management. Should not be used directly by the ics-interface.
				 * Check the send method to see how the ics-interface should add a subscription.
				 */
				/**
				 * @brief Adds a subscription
				 */
				void addSubscription(Subscription * subscription);
				/**
				 * @brief Adds a subscription from the arguments
				 */
				void addSubscription(const int subscriptionId, const int subscriptionType, const bool toUnsubscribe = false);
				/**
				 * @brief Removes the subscription with that id
				 */
				bool removeSubscription(const int subscriptionId);
				/**
				 * @brief Removes the subscription
				 */
				bool removeSubscription(const Subscription * subscription);
				/**
				 * @brief Gets a subscription from the id
				 */
				bool findSubscription(const int subscriptionId, Subscription * & subscription) const;
				/**
				 * @brief That subscription will be cancelled when iCS ask which subscriptions to remove
				 */
				bool setToUnsubscribe(const int subscriptionId, const bool toUnsibscribe = true);
				/**
				 * @brief Whatever a subscription is to remove
				 */
				bool isToUnsubscribe(const int subscriptionId) const;
				/**
				 * @brief Adds the scheduled subscriptions in m_toSubscribe
				 * @brief Called multiple times until if returns false.
				 * @param[in] subscriptionId id to use for the new subscription
				 * @param[out] request Request to be sent do iCS with all the information to create the selected subscription
				 * @return False if no new subscriptions are scheduled. True otherwise
				 */
				bool askForSubscription(const int subscriptionId, tcpip::Storage * & request);

				/******************************
				 * Methods that will call the ics-interface when the named action happens
				 */
				/**
				 * @brief The node has received a message. This method will call iCSInferface::Receive
				 */
				virtual void applicationMessageReceive(const int messageId, server::Payload * payload);
				/**
				 * @brief iCS asked the node to execute. This method will call iCSInferface::Execute
				 */
				virtual bool applicationExecute(const int currentTimeStep, DirectionValueMap &data);
				/**
				 * @brief The result of a sumo command has been received. This method will call iCSInferface::SumoTraciCommandResult
				 */
				virtual void sumoTraciCommandResult(const int executionId, tcpip::Storage & storage);
				/**
				 * @brief Push the updated position data to the iCSInferface.
				 * @brief This method in a mobile node will call iCSInferface::GetNodeSampler then NodeSampler::UpdatePosition
				 */
				virtual void updateMobilityInformation(MobilityInfo * info) = 0;

                /**
                 * @brief The node has received one or more CAM messages. This method will call iCSInferface::processCAMmessagesReceived
                 */
                virtual void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);

				/******************************
				 * Methods called by the iCSInferface to shedule the creation of the corresponding subscription
				 */
				/**
				 * @brief Send a geobroadcast message
				 */
				virtual void send(server::Payload * payload, double time, const int messageCategory);
				/**
				 * @brief Send a unicast message
				 */
				virtual void sendTo(const int destinationId, server::Payload * payload, double time, const int messageCategory);
				/**
				 * @brief Create the subscription to execute the command
				 */
				virtual void traciCommand(const int executionId, tcpip::Storage & commandStorage);

				/**
				 * @brief Create the subscription to get mobility info
				 */
				virtual void nodeGetMobilityInformation();


				/*****************************
				 * Utility methods
				 */
				virtual Vector2D getPosition() = 0;
				virtual Vector2D getVelocity() = 0;
				virtual double getDirection() = 0;
				virtual float getPropagationRadius() const;

				inline iCSInterface * getController() const
				{
					return m_controller;
				}
				inline int getId() const
				{
					return m_id;
				}
				inline bool isFixed() const
				{
					return m_type == NT_RSU;
				}
				inline NodeType getNodeType() const
				{
					return m_type;
				}
				inline const std::string & getSumoId() const
				{
					return m_sumoId;
				}
			protected:
				/**
				 * @brief Map of active subscription identified by id (the same used inside iCS)
				 */
				std::map<int, Subscription*> m_subscriptions;
				/**
				 * @brief Scheduled subscriptions that will be added when iCS asks for new subscriptions
				 */
				std::queue<SubscriptionHolder*> m_toSubscribe;
				const int m_id;
				NodeType m_type;
				iCSInterface * m_controller;
				int m_ns3Id;
				std::string m_sumoId;
				std::string m_sumoClass;
				std::string m_sumoType;

				Node(int id);
				void init();
				virtual void addSubscriptions();
				static NodeType GetRandomNodeType();
			private:
				static ns3::UniformVariable m_random;
				bool m_firstAskSubscription;
				bool m_subReceiveMessage;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* NODE_H_ */
