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
#include <cmath>
#include <sstream>
#include "log/log.h"
#include "program-configuration.h"
#include "behaviour-node.h"
#include "behaviour-rsu.h"
#include "data-manager.h"
#include "node.h"
#include "node-sampler.h"
#include "output-helper.h"
#include "ics-interface.h"

namespace testapp
{
	namespace application
	{
		double iCSInterface::DirectionTolerance = 8.0;
		uint16_t iCSInterface::AverageSpeedSampleSmall = 5;
		uint16_t iCSInterface::AverageSpeedSampleHigh = 15;
		bool iCSInterface::UseSink = false;

		iCSInterface::iCSInterface(Node* node, NodeType nodeClass)
		{
			m_active = false;
			m_node = node;
			m_direction_tolerance = DirectionTolerance;

			RegisterTrace("StateChange", m_traceStateChange);
			RegisterTrace("Receive", m_traceReceive);
			RegisterTrace("Send", m_traceSend);

			if (nodeClass == NT_RSU)
			{
				m_nodeType = NT_RSU;
				m_nodeSampler = NULL;

				BehaviourRsu* rsu = new BehaviourRsu(this);
				RsuData data = ProgramConfiguration::GetRsuData(node->getId());

				rsu->AddDirections(data.directions);
				SubscribeBehaviour(rsu);
				SubscribeBehaviour(new DataManager(this));
			} else
			{
				m_nodeType = nodeClass;
				m_nodeSampler = new NodeSampler(this);
				if (UseSink)
					SubscribeBehaviour(new BehaviourNodeWithSink(this));
				else
					SubscribeBehaviour(new BehaviourNodeWithoutSink(this));

			}
			if (OutputHelper::Instance() != NULL)
				OutputHelper::Instance()->RegisterNode(this);
			Activate();
		}

		iCSInterface::~iCSInterface()
		{
			if (OutputHelper::Instance() != NULL)
				OutputHelper::Instance()->RemoveNode(this);
			delete m_nodeSampler;
			for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
				delete it->second;
		}

		Node* iCSInterface::GetNode() const
		{
			return m_node;
		}

		int iCSInterface::GetId() const
		{
			return m_node->getId();
		}

		NodeSampler* iCSInterface::GetNodeSampler() const
		{
			return m_nodeSampler;
		}

		Behaviour* iCSInterface::GetBehaviour(TypeBehaviour type) const
		{
			BehaviourMap::const_iterator it = m_behaviours.find(type);
			if (it == m_behaviours.end())
				return NULL;
			return it->second;
		}

		bool iCSInterface::IsActive() const
		{
			return m_active;
		}

		NodeType iCSInterface::GetNodeType() const
		{
			return m_nodeType;
		}

		Vector2D iCSInterface::GetPosition() const
		{
			if (m_nodeType == NT_RSU)
			{
				return m_node->getPosition();
			} else
				return m_nodeSampler->GetPosition();
		}

		double iCSInterface::GetDirection() const
		{
			if (!IsVehicle(m_nodeType))
				return DIR_INVALID;
			return m_nodeSampler->GetDirection();
		}

		bool iCSInterface::IsConformantDirection(double direction)
		{
			double myDir = GetDirection();
			if (myDir == DIR_INVALID)
				return false;

			return CheckDirections(myDir, direction, m_direction_tolerance);
		}

		bool iCSInterface::IsConformantDirectionAndMovement(const VehicleDirection direction, const Vector2D rsuPosition)
		{
			return CheckDirectionAndMovement(GetDirection(), direction, m_direction_tolerance, GetPosition(), rsuPosition);
		}

		bool iCSInterface::CheckDirectionAndMovement(const double &nodeDirection, const VehicleDirection &directionFromRsu,
				const double &tolerance, const Vector2D& nodePosition, const Vector2D& rsuPosition)
		{
			if (!CheckDirections(nodeDirection, directionFromRsu.dir, tolerance))
				return false;
			Vector2D segmentRsuVehicle(rsuPosition.x - nodePosition.x, rsuPosition.y - nodePosition.y);
			double segmentDir = atan2(segmentRsuVehicle.y, segmentRsuVehicle.x) * 180.0 / M_PI;

			double min = NormalizeDirection(directionFromRsu.dir - 90);
			double max = NormalizeDirection(directionFromRsu.dir + 90);
			bool result;
			if (max > min)
				result = segmentDir >= min && segmentDir <= max;
			else
				result = segmentDir >= min || segmentDir <= max;
			if (directionFromRsu.vMov == LEAVING)
				result = !result;

			return result;
		}

		bool iCSInterface::CheckDirections(const double &dir1, const double &dir2, const double &tolerance)
		{
			if (dir1 == DIR_INVALID || dir2 == DIR_INVALID)
				return false;

			double diff = dir2 - dir1;
			diff = NormalizeDirection(diff);

			return std::abs(diff) <= tolerance;
		}

		std::string iCSInterface::NodeName() const
		{
			std::ostringstream logstr;
			logstr << "(";

			if (m_nodeType == NT_RSU)
				logstr << "RSU";
			else if (IsNodeType(m_nodeType, NT_VEHICLE))
				logstr << "Node";
			else
				logstr << "??";

			logstr << " " << m_node->getId() << ")";
			return logstr.str();
		}

		std::string iCSInterface::LogNode() const
		{
			std::ostringstream outstr;
			outstr << NodeName();
			outstr << ": ";
			return outstr.str();
		}

		void iCSInterface::Activate()
		{
			NS_LOG_FUNCTION(LogNode());
			NS_ASSERT(m_node != 0);

			if (m_active)
				return;
			m_active = true;
			NS_LOG_INFO(LogNode() << ": Activated");
			// activate behaviours if requested
			for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
			{
				if (it->second->IsActiveOnStart())
					it->second->Start();
			}
			m_traceStateChange(true);
			//Example use of a traci command subscription
			AddTraciSubscription();
		}

		void iCSInterface::Deactivate()
		{
			if (!m_active)
				return;
			m_active = false;
			NS_LOG_INFO(LogNode() << ": Deactivated");
			// deactivate all behaviours
			for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
			{
				if (it->second->IsRunning())
					it->second->Stop();
			}
			m_traceStateChange(false);
		}

		void iCSInterface::SubscribeBehaviour(Behaviour* behaviour)
		{
			//  add behaviour to collection
			m_behaviours.insert(std::make_pair(behaviour->GetType(), behaviour));
			NS_LOG_DEBUG(LogNode() << "Added behaviour " << ToString(behaviour->GetType()));
			//  activate behaviour if we are running
			if (m_active && behaviour->IsActiveOnStart())
			{
				behaviour->Start();
			}
		}

		bool iCSInterface::Receive(server::Payload * payload)
		{
			if (!m_active)
				return false;

			// receive packet
			m_traceReceive(payload);

			// read its header
			CommHeader* header;
			GetHeader(payload, server::PAYLOAD_FRONT, header);

			double snr = 110;
			if (!std::isnan(payload->snr))
				snr = payload->snr;
			// discard unwanted packets
			if (!IsNodeType(GetNodeType(), header->getDestinationType()))
			{
				NS_LOG_INFO(LogNode() << "Receive: Failed node type check");
				return false; // NODE TYPE CHECK
			}
			if (header->getDestinationId() != ID_ALL && header->getDestinationId() != m_node->getId())
			{
				//NS_LOG_INFO(LogNode() << "Receive: Failed Id check");
				return false; // ID CHECK
			}

			NS_LOG_DEBUG(LogNode() << "Received packet snr=" << snr << " ItsHeader: " << PrintHeader(header));

			// call protocol subscribers
			ProtocolId pid = header->getProtocolId();
			for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
			{
				if (!it->second->IsRunning() || !it->second->IsSubscribedTo(pid))
					continue;
				NS_LOG_DEBUG(LogNode() << "Forwarded packet to " << ToString(it->second->GetType()));
				it->second->Receive(payload, snr);
			}
			return true;
		}

		void iCSInterface::Send(NodeType dstType, Header* header, ProtocolId pid)
		{
			if (!m_active)
				return;
			Header* commHeader = DoSend(dstType, header, pid, ID_ALL);

			NS_LOG_DEBUG(
					LogNode() << "[" << Scheduler::GetCurrentTime() << "]Sent packet. ItsHeader:" << PrintHeader(commHeader) << ", and following " << header->Name() << ": " << PrintHeader(header));
		}

		void iCSInterface::SendTo(int destId, Header* header, ProtocolId pid)
		{
			if (!m_active)
				return;
			Header* commHeader = DoSend(NT_ALL, header, pid, destId);

			NS_LOG_DEBUG(
					LogNode() << "[" << Scheduler::GetCurrentTime() << "]Sent packet to node " << destId << ". ItsHeader:" << PrintHeader(commHeader) << ", and following " << header->Name() << ": " << PrintHeader(header));
		}

		Header* iCSInterface::DoSend(NodeType dstType, Header* header, ProtocolId pid, int destinationId)
		{
			// prepare its header
			CommHeader* commHeader = new CommHeader();
			commHeader->setProtocolId(pid);
			commHeader->setSourceType(m_nodeType);
			commHeader->setSourceId(m_node->getId());
			commHeader->setSourcePosition(GetPosition());
			commHeader->setDestinationType(dstType);
			commHeader->setDestinationId(destinationId);
			commHeader->setMessageType(header->getMessageType());

			// prepare packet. 16 was set by Zamagni
			server::Payload* payload = new server::Payload(16);
			payload->addHeader(header);
			payload->addHeader(commHeader);

			// send packet
			m_traceSend(payload);
			//Like in the original protocol use only grobroadcast messages
			//if (dest == ID_ALL)
			m_node->send(payload, Scheduler::GetCurrentTime());
			//else
			//  m_node->sendTo(dest, payload);
			return commHeader;
		}

		bool iCSInterface::Execute(DirectionValueMap &data)
		{
			bool retVal = false;
			for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
			{
				if (it->second->Execute(data))
					retVal = true;
			}
			return retVal;
		}
		void iCSInterface::TraciCommandResult(int executionId, tcpip::Storage & traciReply)
		{
			Command command;
			if (!TraciHelper::GetCommand(executionId, command))
			{
				NS_LOG_WARN(LogNode() <<"iCSInferface::TraciCommandResult executionId " << executionId << " unknown");
				return;
			}
			NS_LOG_INFO(
					LogNode() << "iCSInferface::TraciCommandResult executionId " << executionId << " command " << command.commandId << " type " << (command.type == GET_COMMAND ? "GET" : "SET"));
			if (executionId == traciGetSpeed)
			{

				int varId;
				std::string objId;
				int varType;
				if (TraciHelper::BeginValueRetrievalFromCommand(executionId, traciReply, varId, objId, varType))
				{
//					The varType is the type of the variable to be read
					double speed = traciReply.readDouble();
					NS_LOG_INFO(LogNode() <<"iCSInferface::TraciCommandResult Speed of " << objId << " is " << speed);
				} else
				{
					NS_LOG_WARN(LogNode() <<"iCSInferface::TraciCommandResult Error in BeginValueRetrievalFromCommand");
				}
			} else if (executionId == traciSetMaxSpeed)
			{
				if (TraciHelper::VerifyCommand(executionId, traciReply))
				{
					NS_LOG_INFO(LogNode() <<"iCSInferface::TraciCommandResult max speed correctly set");
				} else
				{
					NS_LOG_WARN(LogNode() <<"iCSInferface::TraciCommandResult set command error");
				}
			} else
			{
				NS_LOG_WARN(LogNode() <<"iCSInferface::TraciCommandResult executionId not recognized");
			}
//			Remove the executionId from TraciHelper
			TraciHelper::RemoveCommand(executionId);
//			For a more general approach it can be used Command.type to check if the command was a value retrieval or
//			a state change
		}
		void iCSInterface::AddTraciSubscription()
		{
			if (m_node->getSumoId() != INVALID_STRING)
			{
//				example: get the speed of a node
				tcpip::Storage cmd1;
				traciGetSpeed = TraciHelper::AddValueGetStorage(cmd1, GET_VEHICLE_VARIABLE, VARIABLE_SPEED,
						m_node->getSumoId());
				m_node->traciCommand(traciGetSpeed, cmd1);

//				example: set the max speed of a node
				tcpip::Storage cmd2;
				tcpip::Storage maxSpeed;
				maxSpeed.writeDouble(20);
				traciSetMaxSpeed = TraciHelper::AddValueSetStorage(cmd2, CHANGE_VEHICLE_STATE, VARIABLE_MAX_SPEED,
						m_node->getSumoId(), TYPE_DOUBLE, maxSpeed);
				m_node->traciCommand(traciSetMaxSpeed, cmd2);
			}
		}
	} /* namespace application */
} /* namespace protocol */
