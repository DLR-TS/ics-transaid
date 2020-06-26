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
#include <app-commands-subscriptions-constants.h>
//#include <libsumo/Helper.h>
#include <utils/common/RGBColor.h>
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

		double iCSInterface::GetSpeed() const
		{
			if (m_nodeType == NT_RSU)
			{
				return 0;
			} else
				return m_nodeSampler->GetSpeed(1);
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

			//std::cout << "Payload content " << payload << std::endl;

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
				        processTraCIResult(traciReply.readDouble(), command, executionId);
				    }
                    break;
                    case libsumo::TYPE_STRING:
                    {
                        processTraCIResult(traciReply.readString(), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_INTEGER:
                    {
                        processTraCIResult(traciReply.readInt(), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_BYTE:
                    {
                        processTraCIResult((int) traciReply.readByte(), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_UBYTE:
                    {
                        processTraCIResult((int) traciReply.readUnsignedByte(), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_STRINGLIST:
                    {
                        processTraCIResult(traciReply.readStringList(), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_COLOR:
                    {
                        processTraCIResult(readColor(traciReply), command, executionId);
                    }
                    break;
                    case libsumo::POSITION_2D:
                    {
                        processTraCIResult(readPosition2D(traciReply), command, executionId);
                    }
                    break;
                    case libsumo::TYPE_COMPOUND:
                    {
                        switch (varId)
                        {
                        case libsumo::VAR_LEADER:
                        {
                            processTraCIResult(readLeaderDistance(traciReply), command, executionId);
                        }
                        break;
                        case libsumo::VAR_NEXT_STOPS:
                        {
                            processTraCIResult(readNextStopDataVector(traciReply), command, executionId);
                        }
                        break;
                        case libsumo::CMD_CHANGELANE:
                        {
                            processTraCIResult(readPair2Int(traciReply), command, executionId);
                        }
                        break;
                        case libsumo::VAR_NEIGHBORS:
                        {
                            processTraCIResult(readVectorPair(traciReply), command, executionId);
                        }
                        break;
                        case libsumo::VAR_PARAMETER_WITH_KEY:
                        {
                            processTraCIResult(readParameterWithKey(traciReply), command);
                        }
                        break;
                        default:
                            NS_LOG_ERROR(LogNode() << "iCSInferface::TraciCommandResult unknown/unimplemented TYPE_COMPOUND varID " << Log::toHex(varId));
                        }
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


        const std::pair<int, std::shared_ptr<libsumo::TraCIResult> >&
        iCSInterface::GetLastTraCIResponse(std::string objID, int variableID, std::string parameterKey) {
            return Behaviour::GetLastTraCIResponse(objID, variableID, parameterKey);
        }

        const std::pair<std::shared_ptr<CommandInfo>, std::shared_ptr<libsumo::TraCIResult>> &iCSInterface::getTraCIResponse(int executionId)
        {       
            return Behaviour::getTraCIResponse(executionId);
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

        void
        iCSInterface::writeColor(std::shared_ptr<libsumo::TraCIColor> color, tcpip::Storage& outputStorage) {
            outputStorage.writeUnsignedByte(color->r);
            outputStorage.writeUnsignedByte(color->g);
            outputStorage.writeUnsignedByte(color->b);
            outputStorage.writeUnsignedByte(color->a);
        }

        std::shared_ptr<libsumo::TraCIPosition> iCSInterface::readPosition2D(tcpip::Storage &inputStorage) {
            std::shared_ptr<libsumo::TraCIPosition> res = std::make_shared<libsumo::TraCIPosition>();
            res->x = static_cast<double>(inputStorage.readDouble());
            res->y = static_cast<double>(inputStorage.readDouble());
            res->z = 0;
            return res;
        }
        //-------------------------------------------------------------------------------------------
        //simulation
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::getDepartedIDList()
        {
            return AddTraciSubscriptionId("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_DEPARTED_VEHICLES_IDS);
        }

        //-------------------------------------------------------------------------------------------
        //simulation
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::getArrivedIDList()
        {
            return AddTraciSubscriptionId("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_ARRIVED_VEHICLES_IDS);
        }

        //-------------------------------------------------------------------------------------------
        //simulation
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::getStartingTeleportIDList()
        {
            return AddTraciSubscriptionId("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS);
        }

        //-------------------------------------------------------------------------------------------
        //simulation
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::getEndingTeleportIDList()
        {
            return AddTraciSubscriptionId("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_TELEPORT_ENDING_VEHICLES_IDS);
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetParameter(const std::string &vehID, const std::string &key)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_PARAMETER;
                int varTypeID = libsumo::TYPE_STRING;

                tcpip::Storage content;
                content.writeString(key);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                return AddTraciSubscriptionId(ID, cmdID, varID, true, varTypeID, &content);
            }

            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetParameter(const std::string &vehID, const std::string &key, const std::string &value)
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

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetLaneIndex(const std::string &vehID)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_LANE_INDEX;
                int varTypeID = libsumo::TYPE_INTEGER;

                return AddTraciSubscriptionId(ID, cmdID, varID, varTypeID);
            }

            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetLaneChangeMode(const std::string &vehID, const int lcm)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_LANECHANGE_MODE;
                int varTypeID = libsumo::TYPE_INTEGER;

                tcpip::Storage content;
                content.writeInt(lcm);

                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleChangeLane(const std::string &vehID, const int laneIndex, const double duration)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
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
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetSpeed(const std::string &vehID)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_SPEED;

                return AddTraciSubscriptionId(ID, cmdID, varID);
            }

            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetNeighbors(const std::string &vehID, const unsigned int mode)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_NEIGHBORS;
                int varTypeID = libsumo::TYPE_UBYTE;

                tcpip::Storage content;
                content.writeUnsignedByte(mode);

                return AddTraciSubscriptionId(ID, cmdID, varID, true, varTypeID, &content);
            }

            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetDrivingDistance(const std::string &vehID, const std::string &edgeID, const double pos, const int laneIndex)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
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

                return AddTraciSubscriptionId(ID, cmdID, varID, true, varTypeID, &content);
            }
            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::vehicleGetLaneChangeState(const std::string &vehID, const int direction)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_CHANGELANE;
                int varTypeID = libsumo::TYPE_INTEGER;

                tcpip::Storage content;
                content.writeInt(direction);

                return AddTraciSubscriptionId(ID, cmdID, varID, true, varTypeID, &content);
            }

            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetColor(const std::string &vehID, std::shared_ptr<libsumo::TraCIColor> color)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_COLOR;
                int varTypeID = libsumo::TYPE_COLOR;

                tcpip::Storage content;
                writeColor(color, content);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetToC(const std::string &vehID, const double timeTillMRM)
        {
            // Set parameter for ToC model
            vehicleSetParameter(vehID, "device.toc.requestToC", toString(timeTillMRM));
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetLcAssertive(const std::string &vehID, const double value)
        {
            // Set parameter for ToC model
            vehicleSetParameter(vehID, "laneChangeModel.lcAssertive", toString(value));
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetStop(const std::string &vehID, const std::string &edgeID, const double endPos,
                                          const int laneIndex, const double duration, const int flags, const double startPos, const double until)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
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
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetParkingAreaStop(const std::string &vehID, const std::string &edgeID, const double endPos,
                                                     const int laneIndex, const double duration, const int flags, const double startPos, const double until)
        {
            vehicleSetStop(vehID, edgeID, endPos, laneIndex, duration, flags | libsumo::STOP_PARKING_AREA, startPos, until);
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleOpenGap(const std::string &vehID, const double newTimeHeadway, const double newSpaceHeadway,
                                          const double duration, const double changeRate, const double maxDecel, const std::string &referenceVehID)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_OPENGAP;
                int varTypeID = libsumo::TYPE_COMPOUND;

                const bool referenceVehicleGiven = referenceVehID != "";
                const int length = 6 - int(!referenceVehicleGiven);

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

                if (referenceVehicleGiven)
                {
                    content.writeUnsignedByte(libsumo::TYPE_STRING);
                    content.writeString(referenceVehID);
                }

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleUpdateBestLanes(const std::string &vehID)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_UPDATE_BESTLANES;
                tcpip::Storage sumoQuery;
                sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + ID.length()); // command length
                sumoQuery.writeUnsignedByte(cmdID);                       // command id
                sumoQuery.writeUnsignedByte(varID);                       // variable id
                sumoQuery.writeString(ID);                                // object id

                m_node->traciCommand(TraciHelper::AddSetCommand(cmdID, varID, ID), sumoQuery);
            }
        }

        //-------------------------------------------------------------------------------------------
        //vehicle
        //-------------------------------------------------------------------------------------------
        void iCSInterface::vehicleSetClass(const std::string &vehID, const std::string &vClass)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;

            if (ID != INVALID_STRING)
            {
                tcpip::Storage vehicleClass;
                vehicleClass.writeString(vClass);

                AddTraciSubscription(ID, libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::VAR_VEHICLECLASS, libsumo::TYPE_STRING, &vehicleClass);
            }
        }

        //-------------------------------------------------------------------------------------------
        //poi
        //-------------------------------------------------------------------------------------------
        void iCSInterface::poiRemove(const std::string &poi, const int layer)
        {
            int cmdID = libsumo::CMD_SET_POI_VARIABLE;
            int varID = libsumo::REMOVE;
            int varTypeID = libsumo::TYPE_INTEGER;

            tcpip::Storage content;
            content.writeInt(layer);

            AddTraciSubscription(poi, cmdID, varID, varTypeID, &content);
        }

        //-------------------------------------------------------------------------------------------
        //poi
        //-------------------------------------------------------------------------------------------
        void iCSInterface::poiSetPosition(const std::string &poi, const double xPos, const double yPos)
        {
            int cmdID = libsumo::CMD_SET_POI_VARIABLE;
            int varID = libsumo::VAR_POSITION;
            int varTypeID = libsumo::POSITION_2D;

            tcpip::Storage content;
            content.writeDouble(xPos);
            content.writeDouble(yPos);

            AddTraciSubscription(poi, cmdID, varID, varTypeID, &content);
        }

        //-------------------------------------------------------------------------------------------
        //gui
        //-------------------------------------------------------------------------------------------
        void iCSInterface::guiTrackVehicle(const std::string &viewID, const std::string &vehID)
        {
            int cmdID = libsumo::CMD_SET_GUI_VARIABLE;
            int varID = libsumo::VAR_TRACK_VEHICLE;
            int varTypeID = libsumo::TYPE_STRING;

            tcpip::Storage content;
            content.writeString(vehID);

            AddTraciSubscription(viewID, cmdID, varID, varTypeID, &content);
        }

        //-------------------------------------------------------------------------------------------
        //gui
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::guiGetOffset(const std::string &viewID)
        {
            int cmdID = libsumo::CMD_GET_GUI_VARIABLE;
            int varID = libsumo::VAR_VIEW_OFFSET;
            int varTypeID = libsumo::POSITION_2D;

            return AddTraciSubscriptionId(viewID, cmdID, varID, varTypeID);
        }

        //-------------------------------------------------------------------------------------------
        //gui
        //-------------------------------------------------------------------------------------------
        void iCSInterface::guiSetOffset(const std::string &viewID, const double x, const double y)
        {
            int cmdID = libsumo::CMD_SET_GUI_VARIABLE;
            int varID = libsumo::VAR_VIEW_OFFSET;
            int varTypeID = libsumo::POSITION_2D;

            tcpip::Storage content;
            content.writeDouble(x);
            content.writeDouble(y);

            AddTraciSubscription(viewID, cmdID, varID, varTypeID, &content);
        }

        //-------------------------------------------------------------------------------------------
        //gui
        //-------------------------------------------------------------------------------------------
        void iCSInterface::guiSetZoom(const std::string &viewID, const double zoom)
        {
            int cmdID = libsumo::CMD_SET_GUI_VARIABLE;
            int varID = libsumo::VAR_VIEW_ZOOM;
            int varTypeID = libsumo::TYPE_DOUBLE;

            tcpip::Storage content;
            content.writeDouble(zoom);

            AddTraciSubscription(viewID, cmdID, varID, varTypeID, &content);
        }

        std::shared_ptr<libsumo::TraCILeaderDistance>
        iCSInterface::readLeaderDistance(tcpip::Storage& inputStorage) {
            std::shared_ptr<libsumo::TraCILeaderDistance> res = std::make_shared<libsumo::TraCILeaderDistance>();
            inputStorage.readInt(); // length (2)
            inputStorage.readUnsignedByte();  // libsumo::TYPE_STRING
            res->leaderID = static_cast<std::string>(inputStorage.readString());
            inputStorage.readUnsignedByte();  // libsumo::TYPE_DOUBLE
            res->dist = static_cast<double>(inputStorage.readDouble());
            return res;
        }

        std::shared_ptr<libsumo::TraCINextStopDataVector>
        iCSInterface::readNextStopDataVector(tcpip::Storage& inputStorage) {
            std::shared_ptr<libsumo::TraCINextStopDataVector> res = std::make_shared<libsumo::TraCINextStopDataVector>();
            inputStorage.readInt();  // compound length
            inputStorage.readUnsignedByte();  // libsumo::TYPE_INTEGER
            for (int length = inputStorage.readInt(); length > 0; --length) {
                libsumo::TraCINextStopData nsd;
                inputStorage.readUnsignedByte();  // libsumo::TYPE_STRING
                nsd.lane = static_cast<std::string>(inputStorage.readString());
                inputStorage.readUnsignedByte();  // libsumo::TYPE_DOUBLE
                nsd.endPos = static_cast<double>(inputStorage.readDouble());
                inputStorage.readUnsignedByte();  // libsumo::TYPE_STRING
                nsd.stoppingPlaceID = static_cast<std::string>(inputStorage.readString());
                inputStorage.readUnsignedByte();  // libsumo::TYPE_INTEGER
                nsd.stopFlags = static_cast<int>(inputStorage.readInt());
                inputStorage.readUnsignedByte();  // libsumo::TYPE_DOUBLE
                nsd.duration = static_cast<double>(inputStorage.readDouble());
                inputStorage.readUnsignedByte();  // libsumo::TYPE_DOUBLE
                nsd.until = static_cast<double>(inputStorage.readDouble());
                res->value.push_back(nsd);
            }
            return res;
        }

        std::shared_ptr<baseapp::TraCIPair2Int>
        iCSInterface::readPair2Int(tcpip::Storage& inputStorage) {
            std::shared_ptr<baseapp::TraCIPair2Int> res = std::make_shared<baseapp::TraCIPair2Int>();
            inputStorage.readInt(); // length (2)
            inputStorage.readUnsignedByte();  // libsumo::TYPE_INTEGER
            res->value.first = static_cast<int>(inputStorage.readInt());
            inputStorage.readUnsignedByte();  // libsumo::TYPE_INTEGER
            res->value.second = static_cast<int>(inputStorage.readInt());
            return res;
        }

        std::shared_ptr<baseapp::TraCIVectorPair>
        iCSInterface::readVectorPair(tcpip::Storage& inputStorage) {
            std::shared_ptr<baseapp::TraCIVectorPair> res = std::make_shared<baseapp::TraCIVectorPair>();

            for (int length = inputStorage.readInt(); length > 0; --length) {
                std::pair<std::string, double> p;
                p.first = static_cast<std::string>(inputStorage.readString());
                p.second = static_cast<double>(inputStorage.readDouble());
                res->value.push_back(p);
            }

            return res;
        }

        std::shared_ptr<baseapp::TraCIParameterWithKey>
        iCSInterface::readParameterWithKey(tcpip::Storage& inputStorage) {
            std::shared_ptr<baseapp::TraCIParameterWithKey> res = std::make_shared<baseapp::TraCIParameterWithKey>();
            inputStorage.readInt(); // length (2)
            inputStorage.readUnsignedByte();  // libsumo::TYPE_STRING
            res->key = static_cast<std::string>(inputStorage.readString());
            inputStorage.readUnsignedByte();  // libsumo::TYPE_STRING
            res->value = static_cast<std::string>(inputStorage.readString());
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

                const bool referenceVehicleGiven = referenceVehicleID != "";
                const int length = 6 - int(!referenceVehicleGiven);

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

        void iCSInterface::commandTraciChangeLane(const std::string vehID, const int laneIndex, const double duration)
        {
            if (vehID != INVALID_STRING)
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
                AddTraciSubscription(vehID, cmdID, varID, varTypeID, &content);
            }
        }

				void iCSInterface::commandTraciLaneChangeMode(const std::string vehID, const int lcm)
        {
            if (vehID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_LANECHANGE_MODE;
								int varTypeID = libsumo::TYPE_INTEGER;

                tcpip::Storage content;
                content.writeInt(lcm);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(vehID, cmdID, varID, varTypeID, &content);
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

        void iCSInterface::commandTraciGetLeader(const double dist)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_LEADER;
                int varTypeID = libsumo::TYPE_DOUBLE;

                tcpip::Storage content;
                content.writeDouble(dist);

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::commandTraciGetNextStops(const std::string& vehID)
        {
            std::string ID = (vehID == "" ? m_node->getSumoId() : vehID);
            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_NEXT_STOPS;

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID);
            }
        }

        void iCSInterface::GetTraciParameterWithKey(const int cmdID, const std::string key, const std::string objID)
        {
            if (cmdID != libsumo::CMD_GET_VEHICLE_VARIABLE && cmdID != libsumo::CMD_GET_SIM_VARIABLE) {
                return;
            }

            std::string ID = ((objID == "" && cmdID == libsumo::CMD_GET_VEHICLE_VARIABLE) ? m_node->getSumoId() : objID);

            if (ID != INVALID_STRING) {
                int varID = libsumo::VAR_PARAMETER_WITH_KEY;
                int varTypeID = libsumo::TYPE_STRING;
                tcpip::Storage content;
                content.writeString(key);
                // Add traci subscriptions without explicitely given objectID for
                // mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
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
                writeColor(color, content);
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::SetTraCIParkingAreaStop(const std::string& vehID, const std::string& stopID, const double duration, const double until, const int flags)
        {
            std::string ID = vehID == "" ? m_node->getSumoId() : vehID;
            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
                int varID = libsumo::CMD_STOP;
                int varTypeID = libsumo::TYPE_COMPOUND;
                double pos = 1.0;
                int laneIndex = 0;
                double startPos = libsumo::INVALID_DOUBLE_VALUE;

                tcpip::Storage content;
                content.writeInt(7);
                content.writeUnsignedByte(libsumo::TYPE_STRING);
                content.writeString(stopID);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(pos);
                content.writeUnsignedByte(libsumo::TYPE_BYTE);
                content.writeByte(laneIndex);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(duration);
                content.writeUnsignedByte(libsumo::TYPE_BYTE);
                content.writeByte(flags | libsumo::STOP_PARKING_AREA);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(startPos);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(until);
                
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::GetTraCIStopState(const std::string& vehID)
        {
            std::string ID = (vehID == "" ? m_node->getSumoId() : vehID);
            if (ID != INVALID_STRING)
            {
                int cmdID = libsumo::CMD_GET_VEHICLE_VARIABLE;
                int varID = libsumo::VAR_STOPSTATE;

                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID);
            }
        }

        void iCSInterface::Highlight(std::string colorDef, const double size, const int type, const double duration, const std::string& sumoPOI) {
            RGBColor c = RGBColor::parseColor(colorDef);
            auto color = std::make_shared<libsumo::TraCIColor>(c.red(), c.green(), c.blue(), c.alpha());
            Highlight(color, size, type, duration, sumoPOI);
        }

        void iCSInterface::Highlight(std::shared_ptr<libsumo::TraCIColor> color, const double size, const int type, const double duration, const std::string& sumoPOI) {
            std::string ID = m_node->getSumoId();
            int cmdID;
            if (ID != "" && sumoPOI != "") {
            	std::cerr << "Warning: iCSInterface::Highlight(): Ignoring given argument sumoPOI because this is a vehicle ('" << ID << "')" << std::endl;
            } else if (ID == "" && sumoPOI != "") {
                ID = sumoPOI;
                cmdID = libsumo::CMD_SET_POI_VARIABLE;
            } else {
                ID = m_node->getSumoId();
                cmdID = libsumo::CMD_SET_VEHICLE_VARIABLE;
            }
            // Highlight associated node if sumo id is known
            if (ID != INVALID_STRING)
            {
                int varID = libsumo::VAR_HIGHLIGHT;
                int varTypeID = libsumo::TYPE_COMPOUND;
                unsigned int alphaMax = 255;
                tcpip::Storage content;
                const int length = 5;
                content.writeInt(length);
                content.writeUnsignedByte(libsumo::TYPE_COLOR);
                writeColor(color, content);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(size);
                content.writeUnsignedByte(libsumo::TYPE_UBYTE);
                content.writeUnsignedByte(alphaMax);
                content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
                content.writeDouble(duration);
                content.writeUnsignedByte(libsumo::TYPE_UBYTE);
                content.writeUnsignedByte(type);
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                AddTraciSubscription(ID, cmdID, varID, varTypeID, &content);
            }
        }

        void iCSInterface::registerMessageReceptionListener(std::shared_ptr<server::NodeHandler::MessageReceptionListener> l) {
        	if (m_node->isFixed()) {
        		server::Server::GetNodeHandler()->addRSUMessageReceptionListener(l);
        	} else {
        		server::Server::GetNodeHandler()->addVehicleMessageReceptionListener(l);
        	}
        }

        void iCSInterface::requestDepartedVehicles() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_DEPARTED_VEHICLES_IDS);
        }

        void iCSInterface::requestArrivedVehicles() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_ARRIVED_VEHICLES_IDS);
        }

        void iCSInterface::requestParkingStartingVehiclesIDList() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_PARKING_STARTING_VEHICLES_IDS);
        }

        void iCSInterface::requestParkingEndingVehiclesIDList() {
            AddTraciSubscription("_SIM", libsumo::CMD_GET_SIM_VARIABLE, libsumo::VAR_PARKING_ENDING_VEHICLES_IDS);
        }

        void iCSInterface::requestToC(const std::string vehID, const double timeTillMRM)
        {
            // Set parameter for ToC model
            SetTraciParameter("device.toc.requestToC", toString(timeTillMRM), vehID);
        }

        void iCSInterface::AddTraciSubscription(const std::string objID, const int cmdID, const int varID, const int varTypeID, tcpip::Storage * value)
        {
            /// TODO: get command with value != 0 (e.g. DISTANCE_REQUEST for getDrivingDistance())
            /// Command type might be derived from varID
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

        //-------------------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::AddTraciSubscriptionId(const int cmdID, const int varID, const bool getCommand, const int varTypeID, tcpip::Storage *value)
        {
            if (m_node->getSumoId() != INVALID_STRING)
            {
                // Add traci subscriptions without explicitely given objectID for mobile nodes only
                return AddTraciSubscriptionId(m_node->getSumoId(), cmdID, varID, getCommand, varTypeID, value);
            }
            return -1;
        }

        //-------------------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------------------
        const int iCSInterface::AddTraciSubscriptionId(const std::string objID, const int cmdID, const int varID, const bool getCommand, const int varTypeID, tcpip::Storage *value)
        {
            tcpip::Storage sumoQuery;
            if (value == 0)
            {
                const int execID = TraciHelper::AddValueGetStorage(sumoQuery, cmdID, varID, objID);
                m_node->traciCommand(execID, sumoQuery);
                return execID;
            }
            else
            {
                const int execID = getCommand ? TraciHelper::AddValueSetStorageGet(sumoQuery, cmdID, varID, objID, varTypeID, *value) : TraciHelper::AddValueSetStorage(sumoQuery, cmdID, varID, objID, varTypeID, *value);
                m_node->traciCommand(execID, sumoQuery);
                return execID;
            }

            return -1;
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

        std::string iCSInterface::GetSumoID(int icsID) const
        {
            if (icsID == -1) {
                return GetNode()->getSumoId();
            }
            return baseapp::server::Server::getSumoID(icsID);
        }

        int iCSInterface::getICSID(std::string sumoID) const
        {
            int icsID;
            bool success = baseapp::server::Server::getICSID(sumoID, icsID);

            return success ? icsID : -1;
        }

        const server::NodeMap& iCSInterface::GetAllNodes() const {
            return server::Server::GetNodeHandler()->getNodes();
        }

	} /* namespace application */
} /* namespace protocol */
