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

#include "node-handler.h"
#include "current-time.h"
#include "program-configuration.h"
#include "fixed-station.h"
#include "log/log.h"
#include "log/ToString.h"

#include "fixed-station.h"
#include <sstream>
#include <climits>

namespace baseapp
{
	namespace server
	{

		using namespace std;
		using namespace application;

		std::string NodeHandler::emptyString = "";

		NodeHandler::NodeHandler(BehaviourFactory* factory) : m_factory(factory)
		{
			m_storage = new PayloadStorage();
			m_timeStepBuffer = new CircularBuffer<int>(ProgramConfiguration::GetMessageLifetime());
		}

		NodeHandler::~NodeHandler()
		{
			if (m_nodes.size() > 0)
			{
				for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
					delete it->second;
				m_nodes.clear();
			}
			delete m_storage;
			delete m_timeStepBuffer;
		}

		void NodeHandler::updateTimeStep(const int timeStep)
		{
			int oldTimeStep;
			if (m_timeStepBuffer->addValue(timeStep, oldTimeStep))
				m_storage->expiredPayloadCleanUp(oldTimeStep);
		}

		bool NodeHandler::getNode(const int id, Node *& node) const
		{
			NodeMap::const_iterator it = m_nodes.find(id);
			if (it == m_nodes.end())
				return false;
			node = it->second;
			return true;
		}

		bool NodeHandler::asStation(const int nodeId, FixedStation *& station) const
		{
			Node * node;
			if (getNode(nodeId, node))
			{
				station = dynamic_cast<application::FixedStation*>(node);
				return station != NULL;
			}
			return false;
		}

        bool NodeHandler::createMobileNode(const int nodeId, const int ns3NodeId, const std::string & sumoNodeId,
                const std::string & sumoType, const std::string & sumoClass)
        {
            if (m_nodes.find(nodeId) != m_nodes.end())
                return false;
            Node * node = new MobileNode(nodeId, ns3NodeId, sumoNodeId, sumoType, sumoClass, m_factory);
            ostringstream oss;
            oss << "Added new mobile node with id " << nodeId << " ns3id " << ns3NodeId << " sumoId " << sumoNodeId
                    << " sumoType " << sumoType << " sumoClass " << sumoClass;
            Log::WriteLog(oss);
            addNode(node);
            return true;
        }


        bool NodeHandler::removeMobileNode(const int nodeId, const int ns3NodeId, const std::string & sumoNodeId)
        {
            auto nodeIt = m_nodes.find(nodeId);
            if (nodeIt == m_nodes.end()) {
                return false;
            } else {
                delete nodeIt->second;
                m_nodes.erase(nodeIt);
            }
            ostringstream oss;
            oss << "Removed mobile node with id " << nodeId << " ns3id " << ns3NodeId << " sumoId " << sumoNodeId;
            Log::WriteLog(oss);
            return true;
        }

		bool NodeHandler::askForSubscription(const int nodeId, const int subscriptionId, tcpip::Storage * & request)
		{
			Node * node;
			if (!getNode(nodeId, node))
			{
				if (ProgramConfiguration::IsRsu(nodeId))
				{
					node = new FixedStation(nodeId, m_factory);
					Log::WriteLog(ostringstream("Added new fixed station with id " + toString(nodeId)));
				} else
				{
					node = new MobileNode(nodeId, m_factory);
					Log::WriteLog(ostringstream("Added new mobile node with id " + toString(nodeId)));
				}
				addNode(node);
			}
			return node->askForSubscription(subscriptionId, request);
		}

		bool NodeHandler::endSubscription(const int nodeId, const int subscriptionId, const int subscriptionType)
		{
			Node * node;
			if (getNode(nodeId, node))
			{
				bool result = node->isToUnsubscribe(subscriptionId);
				if (result)
					node->removeSubscription(subscriptionId);
				return result;
			}
			return true;
		}

		int NodeHandler::mobilityInformation(const int nodeId, const vector<MobilityInfo*> & info)
		{
			int count = 0;
			for (vector<MobilityInfo*>::const_iterator it = info.begin(); it != info.end(); ++it)
			{
				Node * node;
				if (getNode((*it)->id, node))
				{
					node->updateMobilityInformation(*it);
				} else
				{
					if ((*it)->isMobile)
						node = new MobileNode(*it, m_factory);
					else
					{
						node = new FixedStation((*it)->id, m_factory);
						node->updateMobilityInformation(*it);
					}
					Log::WriteLog(ostringstream("Added new node with id " + toString((*it)->id)));
					++count;
					addNode(node);
				}
			}
			FixedStation * node;
			if (asStation(nodeId, node))
			{
				node->mobilityInformationHasRun();
			}
			return count;
		}

		string NodeHandler::insertPayload(const Payload* payload, bool deleteOnRead)
		{
			StoragePolicy policy = deleteOnRead ? kDeleteOnRead : kMultipleRead;
			return m_storage->insert(payload, policy);
		}

		void NodeHandler::applicationMessageReceive(const vector<Message> & messages)
		{
			for (vector<Message>::const_iterator it = messages.begin(); it != messages.end(); ++it)
			{
				Node * node;
				if (getNode(it->m_destinationId, node))
				{
					Payload * payload = NULL;
					if (m_storage->find(it->m_extra, payload))
						payload->snr = it->m_snr;
					node->applicationMessageReceive(it->m_messageId, payload);
					//The payload is deleted if necessary
					if (m_storage->asPolicy(it->m_extra) == kDeleteOnRead)
						delete payload;
				}
			}
		}

		bool NodeHandler::applicationExecute(const int nodeId, DirectionValueMap &data)
		{
			if (ProgramConfiguration::GetStartTime() >= CurrentTime::Now())
				return false;
			Node * node;
			if (getNode(nodeId, node))
			{
				return node->applicationExecute(data);
			}
			return false;
		}

		void NodeHandler::ConfirmSubscription(const int nodeId, const int subscriptionId, const bool status)
		{
			if (status)
			{
				Node * node;
				if (getNode(nodeId, node))
				{
					node->setToUnsubscribe(subscriptionId);
					ostringstream log;
					log << "[Node " << nodeId << "] Confirmed subscription " << subscriptionId
							<< ". Will be unsuscribed on the next timestep";
					Log::WriteLog(log);
				}
			} else
			{
				ostringstream log;
				log << "[Node " << nodeId << "] Error scheduling subscription " << subscriptionId;
				Log::Write(log, kLogLevelWarning);
			}
		}

		void NodeHandler::deleteNode(int nodeId)
		{
			NodeMap::iterator it = m_nodes.find(nodeId);
			if (it != m_nodes.end())
			{
				delete it->second;
				m_nodes.erase(it);
			}
		}

		void NodeHandler::trafficLightInformation(const int nodeId, const bool error, const std::vector<std::string> & data)
		{
			FixedStation * station;
			if (asStation(nodeId, station))
			{
				station->trafficLightInformation(error, data);
			}
		}

		void NodeHandler::setToUnsubscribe(const int nodeId, const int subscriptionId)
		{
			Node * station;
			if (getNode(nodeId, station))
			{
				station->setToUnsubscribe(subscriptionId);
			}
		}

		void NodeHandler::sumoTraciCommandResult(const int nodeId, const int executionId, tcpip::Storage & storage)
		{
			Node * station;
			if (getNode(nodeId, station))
			{
				station->sumoTraciCommandResult(executionId, storage);
			}
		}

        void NodeHandler::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {
                Node * node;
                if (getNode(nodeID, node))
                {
                    node->processCAMmessagesReceived(nodeID ,receivedCAMmessages);
                }
         }


        const std::string& NodeHandler::getSumoID(int icsID) const {
            auto nodeIt = m_nodes.find(icsID);
            if (nodeIt == m_nodes.end()) {
                return emptyString;
            }
            return nodeIt->second->getSumoId();
        }

	} /* namespace server */
} /* namespace protocol */
