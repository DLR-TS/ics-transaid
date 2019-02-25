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
#include "../../utils/log/ToString.h"
#include "program-configuration.h"
#include "behaviour-factory.h"
#include "headers.h"
#include "node.h"
#include "node-sampler.h"
#include "output-helper.h"
#include "ics-interface.h"
#include "server.h"
#include "../../app-commands-subscriptions-constants.h"
#include <libsumo/TraCIDefs.h>
//#include "traci-server/TraCIConstants.h"
//#include <foreign/tcpip/storage.h>

namespace baseapp
{
	namespace application
	{
		double iCSInterface::DirectionTolerance = 8.0;
		uint16_t iCSInterface::AverageSpeedSampleSmall = 5;
		uint16_t iCSInterface::AverageSpeedSampleHigh = 15;
		bool iCSInterface::UseSink = false;

		iCSInterface::iCSInterface(Node* node, NodeType nodeClass, BehaviourFactory* factory)
		{
			m_active = false;
			m_node = node;
			m_direction_tolerance = DirectionTolerance;
            m_nodeSampler = NULL;

			RegisterTrace("StateChange", m_traceStateChange);
			RegisterTrace("Receive", m_traceReceive);
			RegisterTrace("Send", m_traceSend);

			if (nodeClass == NT_RSU)
			{
				m_nodeType = NT_RSU;
				factory->createRSUBehaviour(this, node);
			} else
			{
				m_nodeType = nodeClass;
				m_nodeSampler = new NodeSampler(this);
				factory->createNodeBehaviour(this, node);
			}
			if (OutputHelper::Instance() != NULL)
				OutputHelper::Instance()->RegisterNode(this);
			Activate();
		}

		iCSInterface::~iCSInterface()
		{
			if (OutputHelper::Instance() != NULL)
				OutputHelper::Instance()->RemoveNode(this);
			if (m_nodeSampler != NULL){
			    delete m_nodeSampler;
			}
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

		void iCSInterface::Send(NodeType dstType, Header* header, ProtocolId pid, const int messageCategory)
		{
			if (!m_active)
				return;
			Header* commHeader = DoSend(dstType, header, pid, ID_ALL, messageCategory);

			NS_LOG_DEBUG(
                    LogNode() << "[" << Scheduler::GetCurrentTime() << "] Sent packet. Its Header: " << PrintHeader(commHeader) << ", and following " << header->Name() << ": " << PrintHeader(header));
		}

        /// @brief Let the node receive geobroadcast messages
        void iCSInterface::startReceivingGeobroadcast(const int messageId) {
            m_node->subscribeToGeobroadcastReception(messageId);
        }

		void iCSInterface::SendTo(int destId, Header* header, ProtocolId pid, const int messageCategory)
		{
			if (!m_active)
				return;
			Header* commHeader = DoSend(NT_ALL, header, pid, destId, messageCategory);

			NS_LOG_DEBUG(
                    LogNode() << "[" << Scheduler::GetCurrentTime() << "] Sent packet to node " << destId << ". Its Header:" << PrintHeader(commHeader) << ", and following " << header->Name() << ": " << PrintHeader(header));
		}

		Header* iCSInterface::DoSend(NodeType dstType, Header* header, ProtocolId pid, int destinationId, const int messageCategory)
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
			m_node->send(payload, Scheduler::GetCurrentTime(), messageCategory);
			//else
			//  m_node->sendTo(dest, payload);
			return commHeader;
		}

		void iCSInterface::startReceivingUnicast(){
		    m_node->subscribeToUnicastReception();
		}

		void iCSInterface::StartSendingCAMs() {
		    StartSendingCAMs(0, nullptr, PID_UNKNOWN, 0);
		}

        void iCSInterface::startReceivingCAMs() {
            m_node->subscribeToCAMInfo();
        }

        void iCSInterface::StartSendingCAMs(int /*destId*/, Header* /*header*/, ProtocolId /*pid*/, const int /*messageCategory*/)
        {
            m_node->subscribeSendingCAMs();
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
					LogNode() << "iCSInferface::TraciCommandResult executionId " << executionId << " command " << Log::toHex(command.commandId,2) << " type " << (command.type == GET_COMMAND ? "GET" : "SET"));
			if (TraciHelper::IsGetCommand(command))
			{
				int varId;
				std::string objId;
				int varType;
				if (TraciHelper::BeginValueRetrievalFromCommand(executionId, traciReply, varId, objId, varType))
				{
//					The varType is the type of the variable to be read
				    switch (varType) {
				    case libsumo::TYPE_DOUBLE:
				    {
				        processTraCIResult(traciReply.readDouble(), command);
				    }
                    break;
                    case libsumo::TYPE_STRING:
                    {
                        processTraCIResult(traciReply.readString(), command);
                    }
                    break;
                    case libsumo::TYPE_INTEGER:
                    {
                        processTraCIResult(traciReply.readInt(), command);
                    }
                    break;
                    case libsumo::TYPE_BYTE:
                    {
                        processTraCIResult((int) traciReply.readByte(), command);
                    }
                    break;
                    case libsumo::TYPE_UBYTE:
                    {
                        processTraCIResult((int) traciReply.readUnsignedByte(), command);
                    }
                    break;
                    case libsumo::TYPE_STRINGLIST:
                    {
                        processTraCIResult(traciReply.readStringList(), command);
                    }
                    break;
                    case libsumo::TYPE_COLOR:
                    {
                        processTraCIResult(readColor(traciReply), command);
                    }
                    break;
				    default:
				        NS_LOG_ERROR(LogNode() <<"iCSInferface::TraciCommandResult unknown/unimplemented return type " << Log::toHex(varType));
				    }
				} else
				{
					NS_LOG_WARN(LogNode() <<"iCSInferface::TraciCommandResult Error in BeginValueRetrievalFromCommand");
				}
			} else if (command.type == SET_COMMAND)
			{
				if (TraciHelper::VerifyCommand(executionId, traciReply))
				{
					NS_LOG_INFO(LogNode() <<"iCSInferface::TraciCommandResult set command success");
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

		std::shared_ptr<libsumo::TraCIColor>
		iCSInterface::readColor(tcpip::Storage& inputStorage) {
		    std::shared_ptr<libsumo::TraCIColor> res = std::make_shared<libsumo::TraCIColor>();
		    res->r = static_cast<unsigned char>(inputStorage.readUnsignedByte());
		    res->g = static_cast<unsigned char>(inputStorage.readUnsignedByte());
		    res->b = static_cast<unsigned char>(inputStorage.readUnsignedByte());
		    res->a = static_cast<unsigned char>(inputStorage.readUnsignedByte());
		    return res;
		}


        void iCSInterface::AddTraciSubscription(const int cmdID, const int varID, const int varTypeID, tcpip::Storage * value)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(m_node->getSumoId(), cmdID, varID, varTypeID, value);
            }
        }


        void iCSInterface::AddTraciStop(const std::string edgeID, const double endPos,
                const int laneIndex, const double duration, const int flags,
                const double startPos, const double until)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_STOP;
                int varTypeID = libsumo::TYPE_COMPOUND;

                tcpip::Storage content;
                content.writeInt(7);
                content.writeUnsignedByte(libsumo::TYPE_STRING);
                content.writeString(edgeID);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(endPos);
                content.writeUnsignedByte(libsumo::TYPE_BYTE);
                content.writeByte(laneIndex);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(duration);
                content.writeUnsignedByte(libsumo::TYPE_BYTE);
                content.writeByte(flags);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(startPos);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(until);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(cmdID, varID, varTypeID, &content);
            }
        }


        void iCSInterface::commandTraciOpenGap(const double newTimeHeadway, const double newSpaceHeadway,
                const double duration, const double changeRate, const double maxDecel, const std::string& referenceVehicleID)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_OPENGAP;
                int varTypeID = libsumo::TYPE_COMPOUND;

                const bool referenceVehicleGiven = referenceVehicleID == "";
                const int length = 6 - int(referenceVehicleGiven);

                tcpip::Storage content;
                content.writeInt(length);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(newTimeHeadway);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(newSpaceHeadway);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(duration);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(changeRate);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(maxDecel);
                if (referenceVehicleGiven) {
                    content.writeUnsignedByte(libsumo::TYPE_STRING);
                    content.writeString(referenceVehicleID);
                }
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::updateBestLanes() {
            std::string objID = m_node->getSumoId();
            if (objID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_UPDATE_BESTLANES;
                tcpip::Storage sumoQuery;
                sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + objID.length()); // command length
                sumoQuery.writeUnsignedByte(cmdID); // command id
                sumoQuery.writeUnsignedByte(varID); // variable id
                sumoQuery.writeString(objID); // object id

                m_node->traciCommand(TraciHelper::AddSetCommand(cmdID, varID, objID), sumoQuery);
            }
        }

        void iCSInterface::setVehicleClass(std::string vClass) {
            tcpip::Storage vehicleClass;
            vehicleClass.writeString(vClass);
            AddTraciSubscription(libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_VEHICLECLASS, libsumo::TYPE_STRING, &vehicleClass);
        }

        void iCSInterface::commandTraciChangeLane(const int laneIndex, const double duration)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_CHANGELANE;
                int varTypeID = libsumo::TYPE_COMPOUND;

                const int length = 2;

                tcpip::Storage content;
                content.writeInt(length);
                content.writeUnsignedByte(libsumo::TYPE_BYTE);
                content.writeByte(laneIndex);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(duration);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::commandTraciGetDrivingDistance(const std::string edgeID, const double pos, const int laneIndex)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::DISTANCE_REQUEST;
                int varTypeID = libsumo::TYPE_COMPOUND;

                const int length = 2;

                tcpip::Storage content;
                content.writeInt(length);
                content.writeUnsignedByte(libsumo::POSITION_ROADMAP);
                content.writeString(edgeID);
                content.writeDouble(pos);
                content.writeUnsignedByte(laneIndex);
                content.writeUnsignedByte(libsumo::REQUEST_DRIVINGDIST);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::SetTraciParameter(const std::string key, const std::string value, const std::string vehID)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;
            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_PARAMETER;
                int varTypeID = libsumo::TYPE_COMPOUND;
                tcpip::Storage content;
                content.writeInt(2);
                content.writeUnsignedByte(libsumo::TYPE_STRING);
                content.writeString(key);
                content.writeUnsignedByte(libsumo::TYPE_STRING);
                content.writeString(value);
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }


        void iCSInterface::SetTraCIColor(const std::string& vehID, std::shared_ptr<libsumo::TraCIColor> color) {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;
            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_COLOR;
                int varTypeID = libsumo::TYPE_COLOR;
                tcpip::Storage content;
                content.writeUnsignedByte(color->r);
                content.writeUnsignedByte(color->g);
                content.writeUnsignedByte(color->b);
                content.writeUnsignedByte(color->a);
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::requestDepartedVehicles() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_DEPARTED_VEHICLES_IDS);
        }

        void iCSInterface::requestArrivedVehicles() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_ARRIVED_VEHICLES_IDS);
        }

        void iCSInterface::requestToC(const std::string vehID, const double timeTillMRM)
        {
            // Set parameter for ToC model
            SetTraciParameter("device.toc.requestToC", toString(timeTillMRM), vehID);
        }

        void iCSInterface::AddTraciSubscription(const std::string objID, const int cmdID, const int varID, const int varTypeID, tcpip::Storage * value)
        {
            // TODO: get command with value != 0 (e.g. DISTANCE_REQUEST for getDrivingDistance())
            tcpip::Storage sumoQuery;
            if (value == 0) {
                const int execID = TraciHelper::AddValueGetStorage(sumoQuery, cmdID, varID, objID);
                m_node->traciCommand(execID, sumoQuery);
            } else {
//                int type = TraciHelper::getValueType(varID);
                const int execID = TraciHelper::AddValueSetStorage(sumoQuery, cmdID, varID,
                        objID, varTypeID, *value);
                m_node->traciCommand(execID, sumoQuery);
            }
        }

        void iCSInterface::OnAddSubscriptions() {
//            NS_LOG_INFO(LogNode() << ": OnAddSubscriptions()");
            // activate behaviours if requested
            for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
            {
                if (it->second->IsRunning()) {
                    it->second->OnAddSubscriptions();
                }
            }
        }

        void iCSInterface::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {
            for (BehaviourMap::const_iterator it = m_behaviours.begin(); it != m_behaviours.end(); ++it)
            {
                if (!it->second->IsRunning())
                    continue;
                it->second->processCAMmessagesReceived(nodeID, receivedCAMmessages);
            }
        }

        void iCSInterface::requestMobilityInfo()
        {
            //TODO: specify for which nodes to get the info, as function parameter
            m_node->nodeGetMobilityInformation();
        }

        void iCSInterface::requestTrafficLightInfo(){
            m_node->subscribeTrafficLightInformation();
        }

        int iCSInterface::getSUMOStepLength() {
            return baseapp::server::Server::getSUMOStepLength();
        }

	} /* namespace application */
} /* namespace protocol */
