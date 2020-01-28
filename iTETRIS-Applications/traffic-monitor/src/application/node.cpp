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

#include "node.h"
#include <sstream>
#include "log/log.h"
#include "server.h"
#include "node-handler.h"
#include <app-commands-subscriptions-constants.h>
#include "model/ics-interface.h"
#include "payload.h"
#include "subscription-helper.h"
#include "traci-helper.h"

namespace protocol
{
	namespace application
	{
		using namespace std;

		double Node::ProbabilityFull = 1;
		double Node::ProbabilityMedium = 0;
		float Node::PropagationRagiusRsu = 200;
		float Node::PropagationRagiusFull = 200;
		float Node::PropagationRagiusMedium = 150;
		ns3::UniformVariable Node::m_random = ns3::UniformVariable();

		Node::Node(int id) :
				m_id(id)
		{
			m_firstAskSubscription = true;
			m_subReceiveMessage = false;
			m_controller = NULL;
			m_ns3Id = INVALID_INT;
			m_sumoId = INVALID_STRING;
			m_sumoClass = INVALID_STRING;
			m_sumoType = INVALID_STRING;
			m_type = NT_VEHICLE_SHADOW;
		}

		Node::~Node()
		{
			if (m_subscriptions.size() > 0)
			{
				for (map<int, Subscription*>::const_iterator it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it)
					delete it->second;
				m_subscriptions.clear();
			}
			while (!m_toSubscribe.empty())
			{
				delete m_toSubscribe.front();
				m_toSubscribe.pop();
			}
			delete m_controller;
		}

		void Node::init()
		{
			if (m_type != NT_VEHICLE_SHADOW)
				m_controller = new iCSInterface(this, m_type);
		}

		void Node::addSubscription(Subscription * subscription)
		{
			ostringstream log;
			log << "[None " << m_id << "] added subscription " << subscription->m_id << " type "
					<< subscription->m_subscriptionType;
			Log::WriteLog(log);
			m_subscriptions.insert(make_pair(subscription->m_id, subscription));
		}
		void Node::addSubscription(const int subscriptionId, const int subscriptionType, const bool toUnsubscribe)
		{
			addSubscription(new Subscription(subscriptionId, m_id, subscriptionType, toUnsubscribe));
		}
		bool Node::removeSubscription(const int subscriptionId)
		{
			map<int, Subscription*>::iterator it = m_subscriptions.find(subscriptionId);
			if (it == m_subscriptions.end())
				return false;
			ostringstream log;
			log << "[None " << m_id << "] removed subscription " << it->first;
			Log::WriteLog(log);
			delete it->second;
			m_subscriptions.erase(it);
			return true;
		}
		bool Node::removeSubscription(const Subscription * subscription)
		{
			return removeSubscription(subscription->m_id);
		}
		bool Node::findSubscription(const int subscriptionId, Subscription * & subscription) const
		{
			map<int, Subscription*>::const_iterator it = m_subscriptions.find(subscriptionId);
			if (it == m_subscriptions.end())
				return false;
			subscription = it->second;
			return true;
		}
		bool Node::setToUnsubscribe(const int subscriptionId, const bool toUnsibscribe)
		{
			Subscription * sub;
			if (findSubscription(subscriptionId, sub))
			{
				sub->m_toUnsubscribe = toUnsibscribe;
				return true;
			}
			return false;
		}
		bool Node::isToUnsubscribe(const int subscriptionId) const
		{
			Subscription * sub;
			if (findSubscription(subscriptionId, sub))
			{
				return sub->m_toUnsubscribe;
			}
			//If I don't have the subscription with this id something went wrong so I'll unsubscribe it
			return true;
		}

		bool Node::askForSubscription(const int subscriptionId, tcpip::Storage * & request)
		{
			if (m_type == NT_VEHICLE_SHADOW)
				return false;
			if (m_firstAskSubscription)
			{
				m_firstAskSubscription = false;
				addSubscriptions();
			} else
			{
				//  remove the subscription I used the last time. I delete it now otherwise I can't delete the request storage
				delete m_toSubscribe.front();
				m_toSubscribe.pop();
			}
			if (m_toSubscribe.empty())
			{
				m_firstAskSubscription = true;
				return false;
			}
			SubscriptionHolder* subscription = m_toSubscribe.front();
			request = subscription->m_request;
			addSubscription(subscriptionId, subscription->m_subscriptionType, subscription->m_toUnsubscribe);
			return true;
		}

		void Node::applicationMessageReceive(int messageId, server::Payload * payload)
		{
			if (payload != NULL)
			{
				ostringstream log;
				log << "Node " << m_id << " received message " << messageId << " payload " << payload->getId() << " timestep "
						<< payload->getTimeStep();
				Log::WriteLog(log);
				m_controller->Receive(payload);
			} else
				Log::WriteLog("MessageReceive: payload is NULL");
		}

		bool Node::applicationExecute(DirectionValueMap &data)
		{
			if (m_type == NT_VEHICLE_SHADOW)
				return false;
			return m_controller->Execute(data);
		}

		void Node::send(server::Payload * payload, double time)
		{
			//Add the payload to the local storage
			std::string key = server::Server::GetNodeHandler()->insertPayload(payload, false);
			ostringstream oss;
			oss << "[Node " << m_id << "] send to all. Key=" << key << ". Time=" << time;
			Log::WriteLog(oss);
			//Schedule the creation of the subscription.
			m_toSubscribe.push(
					SubscriptionHelper::SendGeobroadcast(m_id, payload->size(), PROTOCOL_MESSAGE,
							Circle(getPosition(), getPropagationRadius()), key, time));
		}

		void Node::sendTo(const int destinationId, server::Payload * payload, double time)
		{
			std::string key = server::Server::GetNodeHandler()->insertPayload(payload, true);
			ostringstream oss;
			oss << "[Node " << m_id << "]send to " << destinationId << ". Key=" << key << ". Time=" << time;
			Log::WriteLog(oss);
			m_toSubscribe.push(
					SubscriptionHelper::SendUnicast(m_id, payload->size(), PROTOCOL_MESSAGE, destinationId, key, time));
		}

		void Node::traciCommand(const int executionId, tcpip::Storage & commandStorage)
		{
			m_toSubscribe.push(SubscriptionHelper::SumoTraciCommand(executionId, commandStorage));
		}

		void Node::addSubscriptions()
		{
			if (!m_subReceiveMessage)
			{
				//Subscribe to both
				m_toSubscribe.push(SubscriptionHelper::ReceiveUnicast(m_id));
				m_toSubscribe.push(SubscriptionHelper::ReceiveGeobroadcast(PROTOCOL_MESSAGE));
				m_subReceiveMessage = true;
			}
		}

		float Node::getPropagationRadius() const
		{
			switch (m_type)
			{
			case NT_RSU:
				return PropagationRagiusRsu;
			case NT_VEHICLE_FULL:
				return PropagationRagiusFull;
			case NT_VEHICLE_MEDIUM:
				return PropagationRagiusMedium;
			default:
				return 0;
			}
		}

		NodeType Node::GetRandomNodeType()
		{
			double vaule = m_random.GetValue();
			if (vaule < ProbabilityFull)
				return NT_VEHICLE_FULL;
			else if (vaule < ProbabilityFull + ProbabilityMedium)
				return NT_VEHICLE_MEDIUM;
			else
				return NT_VEHICLE_SHADOW;
		}

		void Node::sumoTraciCommandResult(const int executionId, tcpip::Storage & storage)
		{
			m_controller->TraciCommandResult(executionId, storage);
		}

	} /* namespace application */
} /* namespace protocol */
