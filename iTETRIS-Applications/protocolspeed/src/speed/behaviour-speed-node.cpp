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

#include "program-configuration.h"
#include "ics-interface.h"
#include "node.h"
#include "node-sampler.h"
#include "app-commands-subscriptions-constants.h"
#include "behaviour-speed-node.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp
{
	namespace application
	{

		///BehaviourNodeWithSink implementation

		BehaviourNodeWithSink::BehaviourNodeWithSink(iCSInterface * controller) :
				BehaviourNode(controller)
		{
			m_sinkThreshold = SinkThreshold;
		}

		BehaviourNodeWithSink::~BehaviourNodeWithSink()
		{
		}

		void BehaviourNodeWithSink::Receive(server::Payload *payload, double snr)
		{
			NS_LOG_FUNCTION(Log());
			if (!m_enabled)
				return;
			CommHeader* commHeader;
			GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
			if (commHeader->getMessageType() != MT_RSU_BEACON)
			{
				NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
				return;
			}
			BeaconHeader* beaconHeader;
			GetController()->GetHeader(payload, server::PAYLOAD_END, beaconHeader);
			int rsuId = commHeader->getSourceId();
			VehicleDirection dir(beaconHeader->getDirection(), beaconHeader->getVehicleMovement());
			if (m_muteRsu.id == rsuId && m_muteRsu.muted && m_muteRsu.dir == dir)
			{
				NS_LOG_INFO(
						Log() << "The sink threshold of " << m_sinkThreshold << " has been reached for the RSU " << rsuId << " direction " << dir);
				return;
			}
			//  if (GetController()->IsConformantDirection(beaconHeader->getDirection()))
			if (GetController()->IsConformantDirectionAndMovement(dir, commHeader->getSourcePosition()))
			{
				NodeInfo rsu;
				rsu.nodeId = rsuId;
				rsu.position = commHeader->getSourcePosition();
				rsu.conformantDirection = dir;
				if (m_muteRsu.id != rsuId && m_muteRsu.dir != dir)
					m_muteRsu = RSU(rsuId, dir);

				double nextTime = m_rnd.GetValue(m_responseTimeSpacing,
						beaconHeader->getMaxResponseTime() - m_responseTimeSpacing);
				Scheduler::Cancel(m_eventResponse);
				m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourNodeWithSink::EventSendResponse, this, rsu);
				NS_LOG_INFO(Log() << "scheduled a beacon response in " << nextTime);
			} else
			{
				NS_LOG_INFO(
						Log() << "beacon direction " << dir << " is not conformant with the current direction " << GetController()->GetDirection());
			}
		}

		void BehaviourNodeWithSink::EventSendResponse(NodeInfo rsu)
		{
			NS_LOG_FUNCTION(Log());
			//  if (!GetController()->IsConformantDirection(rsu.direction))
			if (!GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position))
			{
				NS_LOG_WARN(
						Log() << "The node has a direction no longer conformant. CurrDir=" << GetController()->GetDirection() << ", RsuDir=" << rsu.conformantDirection);
				return;
			}
			double distance;
			bool last = false;
			if ((distance = GetDistance(rsu.position, GetController()->GetPosition())) < m_sinkThreshold)
			{
				if (rsu.conformantDirection.vMov == APPROACHING)
				{
					NS_LOG_INFO(
							Log() << "The node has crossed the sink threshold. Current distance=" << distance << " sink=" << m_sinkThreshold << ". Last message sent.");
					last = true;
					m_muteRsu.muted = true;
				} else
				{
					NS_LOG_INFO(
							Log() << "The node is still too close to the rsu. No response will be sent. Dir=" << rsu.conformantDirection << " distance=" << distance << " sink=" << m_sinkThreshold);
					return;
				}
			}

			NodeInfo node;
			node.nodeId = rsu.nodeId; //NodeId of the source
			node.position = GetController()->GetPosition();
			node.direction = GetController()->GetDirection();
			node.conformantDirection = rsu.conformantDirection;
			node.currentSpeed = GetController()->GetNodeSampler()->GetSpeed(1);
			node.avgSpeedSmall = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleSmall);
			node.avgSpeedHigh = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleHigh);
			node.lastMessage = last;

			BeaconResponseHeader * responseHeader = new BeaconResponseHeader();
			responseHeader->setSourceDirection(node.direction);
			responseHeader->setCurrentSpeed(node.currentSpeed);
			responseHeader->setAvgSpeedLow(node.avgSpeedSmall);
			responseHeader->setAvgSpeedHigh(node.avgSpeedHigh);
			responseHeader->setConformantDirection(node.conformantDirection.dir);
			responseHeader->setVehicleMovement(node.conformantDirection.vMov);
			responseHeader->setLastMessage(last);

			GetController()->SendTo(rsu.nodeId, responseHeader, PID_SPEED, PROTOCOL_MESSAGE);
			m_traceSendData(node);
			NS_LOG_DEBUG(
					Log() << "Sent beacon response to RSU " << rsu.nodeId << " for direction=" << rsu.conformantDirection << " distance=" << distance);
		}

		///BehaviourNodeWithSink implementation

		BehaviourNodeWithoutSink::BehaviourNodeWithoutSink(iCSInterface * controller) :
				BehaviourNode(controller)
		{
		}

		BehaviourNodeWithoutSink::~BehaviourNodeWithoutSink()
		{
		}

		void BehaviourNodeWithoutSink::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetTestCase()=="") {
                // conserve behaviour for old demo app
                GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED);
                tcpip::Storage maxSpeed;
                maxSpeed.writeDouble(20);
                GetController()->AddTraciSubscription(CMD_SET_VEHICLE_VARIABLE, VAR_MAXSPEED, TYPE_DOUBLE, &maxSpeed);
            }

            //Subscribe to geobroadcast and unicast
            GetController()->startReceivingUnicast();
            GetController()->startReceivingGeobroadcast(PROTOCOL_MESSAGE);

		}

		void BehaviourNodeWithoutSink::Receive(server::Payload *payload, double snr)
		{
			NS_LOG_FUNCTION(Log());
			if (!m_enabled)
				return;
			CommHeader* commHeader;
			GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
			if (commHeader->getMessageType() != MT_RSU_BEACON)
			{
				NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
				return;
			}
			BeaconHeader* beaconHeader;
			GetController()->GetHeader(payload, server::PAYLOAD_END, beaconHeader);
			int rsuId = commHeader->getSourceId();
			VehicleDirection dir(beaconHeader->getDirection(), beaconHeader->getVehicleMovement());
			NodeInfo rsu;
			rsu.nodeId = rsuId;
			rsu.position = commHeader->getSourcePosition();
			rsu.conformantDirection = dir;
			std::map<std::string, bool>::iterator it = m_activeDirections.find(dir.getId());
			if (it == m_activeDirections.end())
				m_activeDirections.insert(std::make_pair(dir.getId(), false));
			if (GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position)
					|| m_activeDirections[rsu.conformantDirection.getId()])
			{

				double nextTime = m_rnd.GetValue(m_responseTimeSpacing,
						beaconHeader->getMaxResponseTime() - m_responseTimeSpacing);
				Scheduler::Cancel(m_eventResponse);
				m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourNodeWithoutSink::EventSendResponse, this, rsu);
				NS_LOG_INFO(Log() << "scheduled a beacon response in " << nextTime << " for direction " << dir);
			} else
			{
				NS_LOG_INFO(Log() << "direction "<< dir <<" not active");
			}
		}

		void BehaviourNodeWithoutSink::EventSendResponse(NodeInfo rsu)
		{
			NS_LOG_FUNCTION(Log());
			if (GetController()->IsConformantDirectionAndMovement(rsu.conformantDirection, rsu.position))
			{
				SendRespose(rsu);
			} else if (m_activeDirections[rsu.conformantDirection.getId()])
			{
				SendNoLongerConformant(rsu);
			}
		}

		void BehaviourNodeWithoutSink::SendRespose(NodeInfo rsu)
		{
			NS_LOG_FUNCTION(Log());
			double distance = GetDistance(rsu.position, GetController()->GetPosition());
			m_activeDirections[rsu.conformantDirection.getId()] = true;

			NodeInfo node;
			node.nodeId = rsu.nodeId; //NodeId of the source
			node.position = GetController()->GetPosition();
			node.direction = GetController()->GetDirection();
			node.conformantDirection = rsu.conformantDirection;
			node.currentSpeed = GetController()->GetNodeSampler()->GetSpeed(1);
			node.avgSpeedSmall = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleSmall);
			node.avgSpeedHigh = GetController()->GetNodeSampler()->GetSpeed(iCSInterface::AverageSpeedSampleHigh);
			node.lastMessage = false;

			BeaconResponseHeader * responseHeader = new BeaconResponseHeader();
			responseHeader->setSourceDirection(node.direction);
			responseHeader->setCurrentSpeed(node.currentSpeed);
			responseHeader->setAvgSpeedLow(node.avgSpeedSmall);
			responseHeader->setAvgSpeedHigh(node.avgSpeedHigh);
			responseHeader->setConformantDirection(node.conformantDirection.dir);
			responseHeader->setVehicleMovement(node.conformantDirection.vMov);
			responseHeader->setLastMessage(false);

			GetController()->SendTo(rsu.nodeId, responseHeader, PID_SPEED, PROTOCOL_MESSAGE);
			m_traceSendData(node);
			NS_LOG_DEBUG(
					Log() << "Sent beacon response to RSU " << rsu.nodeId << " for direction=" << rsu.conformantDirection << " distance=" << distance);
		}

		void BehaviourNodeWithoutSink::SendNoLongerConformant(NodeInfo rsu)
		{
			NS_LOG_FUNCTION(Log());
			m_activeDirections[rsu.conformantDirection.getId()] = false;
			NodeInfo node;
			node.nodeId = rsu.nodeId; //NodeId of the source
			node.position = GetController()->GetPosition();
			node.direction = GetController()->GetDirection();
			node.distance = GetDistance(rsu.position, node.position);
			node.conformantDirection = rsu.conformantDirection;
			node.currentSpeed = -10;
			node.avgSpeedSmall = -10;
			node.avgSpeedHigh = -10;
			node.lastMessage = false;

			NoLongerConformantHeader* header = new NoLongerConformantHeader();
			header->setConformantDirection(rsu.conformantDirection.dir);
			header->setVehicleMovement(rsu.conformantDirection.vMov);
			header->setSourceDirection(node.direction);

			GetController()->SendTo(rsu.nodeId, header, PID_SPEED, PROTOCOL_MESSAGE);
			m_traceSendData(node);
			NS_LOG_DEBUG(
					Log() << "Sent NoLongerConformant to RSU " << rsu.nodeId << ". My dir="<<node.direction<<", expected dir=" << rsu.conformantDirection<<". Dist="<<node.distance);
		}


	} /* namespace application */
} /* namespace protocol */
