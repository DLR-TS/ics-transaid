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

#ifndef NODEHANDLER_H_
#define NODEHANDLER_H_

#include <map>
#include "payload-storage.h"
#include "circular-buffer.h"
#include "structs.h"
#include "node.h"
#include "mobile-node.h"

namespace testapp
{
	namespace application
	{
		class FixedStation;
	}
	namespace server
	{
		typedef std::map<int, application::Node*> NodeMap;

		class NodeHandler
		{
			public:
				NodeHandler();
				virtual ~NodeHandler();
				void updateTimeStep(const int timeStep);
				bool getNode(const int id, application::Node *& node) const;
				bool asStation(const int nodeId, application::FixedStation *& station) const;

				inline bool hasNode(const int id) const
				{
					return m_nodes.count(id);
				}

				inline void addNode(application::Node * node)
				{
					m_nodes.insert(std::make_pair(node->getId(), node));
				}

				inline bool asMobileNode(application::Node * node, application::MobileNode *& mobileNode) const
				{
					mobileNode = dynamic_cast<application::MobileNode*>(node);
					return mobileNode != NULL;
				}

				inline NodeMap::const_iterator begin() const
				{
					return m_nodes.begin();
				}

				inline NodeMap::const_iterator end() const
				{
					return m_nodes.end();
				}

				//return whatever a new node was created
				bool createMobileNode(const int nodeId, const int ns3NodeId, const std::string & sumoNodeId,
						const std::string & sumoType, const std::string & sumoClass);
				//returns whatever it has added a new subscription
				bool askForSubscription(const int nodeId, const int subscriptionId, tcpip::Storage * & request);
				//returns if a subscription needs to be dropped
				bool endSubscription(const int nodeId, const int subscriptionId, const int subscriptionType);
				//returns the new cars that have entered the zone
				int mobilityInformation(const int nodeId, const std::vector<MobilityInfo*> & info);
				void trafficLightInformation(const int nodeId, const bool error, const std::vector<std::string> & data);
				//returns the id of the payload
				std::string insertPayload(const Payload* payload, bool deleteOnRead = true);
				void applicationMessageReceive(const std::vector<Message> & messages);
				//returns if there is data to send to iCS
				bool applicationExecute(const int nodeId, const int currentTimeStep, DirectionValueMap &data);
				void ConfirmSubscription(const int nodeId, const int subscriptionId, const bool status);

				void deleteNode(int);
				void setToUnsubscribe(const int nodeId, const int subscriptionId);
				void sumoTraciCommandResult(const int nodeId, const int executionId, tcpip::Storage & storage);

				//  Don't need it
				//  bool commandTrafficSimulation();
				//  bool resultTrafficSimulation();
				//  bool xApplicationData();
				//  bool notifyApplicationMessageStatus();

			private:
				NodeMap m_nodes;
				PayloadStorage * m_storage;
				CircularBuffer<int> * m_timeStepBuffer;
		};

	} /* namespace server */
} /* namespace protocol */

#endif /* NODEHANDLER_H_ */
