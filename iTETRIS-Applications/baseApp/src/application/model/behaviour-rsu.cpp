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

#include "behaviour-rsu.h"
#include "current-time.h"
#include "ics-interface.h"
#include <app-commands-subscriptions-constants.h>
#include "log/console.h"

namespace baseapp
{
	namespace application
	{
		bool VehicleDirectionOrdering::operator ()(const VehicleDirection& left, const VehicleDirection& right)
		{
			if (left.dir == right.dir)
				return left.vMov < right.vMov;
			return left.dir < right.dir;
		}

		bool BehaviourRsu::Enabled = true;
		uint16_t BehaviourRsu::TimeBeaconMin = 250;
		uint16_t BehaviourRsu::TimeBeacon = 1000;
		uint16_t BehaviourRsu::TimeCheck = 1000;
		uint16_t BehaviourRsu::Timeout = 3000;

		BehaviourRsu::BehaviourRsu(iCSInterface* controller) :
				Behaviour(controller)
		{
			m_enabled = Enabled;
			m_timeBeacon = TimeBeacon;
			m_timeCheck = TimeCheck;
			m_timeOut = Timeout;
			m_timeBeaconMin = TimeBeaconMin;
            m_responseTimeSpacing = Behaviour::DefaultResponseTimeSpacing;

			m_beaconInterval = m_timeBeacon;
			m_eventBeacon = m_eventCheck = m_eventResponse = 0;
			m_executeAtThisStep = false;

			RegisterTrace("NodeReceiveData", m_traceBeaconResponse);
			RegisterTrace("NodeTimeOut", m_traceTimeOutNode);
			RegisterTrace("NodeLastMessage", m_traceLastMessageNode);
			RegisterTrace("NodeNoLongerConforman", m_traceNoLongerConforman);
		}

		BehaviourRsu::~BehaviourRsu()
		{
			Scheduler::Cancel(m_eventBeacon);
            Scheduler::Cancel(m_eventCheck);
		}

		void BehaviourRsu::AddDirections(std::vector<Direction> directions)
		{
			NS_LOG_FUNCTION(Log());
			bool wasRunning = false;
			if (IsRunning())
			{
				wasRunning = true;
				Stop();
			}
			if (directions.size() == 0)
				NS_FATAL_ERROR(Log()<<"the RSU can't have 0 directions");
			m_directions.clear();
			for (std::vector<Direction>::const_iterator it = directions.begin(); it != directions.end(); ++it)
			{
				if (!it->approaching && !it->leaving)
				{
					NS_LOG_WARN(Log() << "direction "<< it->direction << " approaching=false and leaving=false");
					continue;
				}
				if (it->approaching)
				{
					VehicleDirection dir(it->direction, APPROACHING, it->approachingTime);
					m_directions.push_back(dir);
				}
				if (it->leaving)
				{
					VehicleDirection dir(it->direction, LEAVING, it->leavingTime);
					m_directions.push_back(dir);
				}
			}

			if (wasRunning)
			{
				Start();
			}
		}

		void BehaviourRsu::Start()
		{
			if (!m_enabled)
				return;
			Behaviour::Start();
		}

		void BehaviourRsu::Stop()
		{
			Scheduler::Cancel(m_eventBeacon);
			Scheduler::Cancel(m_eventCheck);
			Behaviour::Stop();
		}

		const std::vector<VehicleDirection>& BehaviourRsu::GetDirections() const
		{
			return m_directions;
		}

		bool BehaviourRsu::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_SPEED;
		}

		void BehaviourRsu::Receive(server::Payload *payload, double snr)
		{
			NS_LOG_FUNCTION(Log());
			if (!m_enabled)
				return;
			CommHeader* commHeader;
			GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
			switch (commHeader->getMessageType())
			{
			case MT_BEACON_RESPONSE:
				BeaconResponseHeader* resposeHeader;
				GetController()->GetHeader(payload, server::PAYLOAD_END, resposeHeader);
				OnBeaconResponse(commHeader, resposeHeader);
				break;
			case MT_NO_LONGHER_CONFORMANT:
				NoLongerConformantHeader * header;
				GetController()->GetHeader(payload, server::PAYLOAD_END, header);
				OnNoLongerConformant(commHeader, header);
				break;
			default:
				NS_LOG_WARN(Log() << "Received unknown message type: "<<commHeader->getMessageType());
			}
		}

		void BehaviourRsu::UpdateLastSeen(NodeInfo * info)
		{
			TimeoutMap::iterator node = m_nodeLastSeen.find(info->nodeId);
			if (node == m_nodeLastSeen.end())
				node = m_nodeLastSeen.insert(std::make_pair(info->nodeId, DirMap())).first;
			node->second[info->conformantDirection] = info->lastSeen;
		}

		void BehaviourRsu::RemoveLastSeen(NodeInfo *info)
		{
			TimeoutMap::iterator node = m_nodeLastSeen.find(info->nodeId);
			if (node != m_nodeLastSeen.end())
				node->second.erase(info->conformantDirection);
			if (node->second.size() == 0)
				m_nodeLastSeen.erase(node);
		}

		void BehaviourRsu::OnBeaconResponse(CommHeader * commHeader, BeaconResponseHeader * resposeHeader)
		{
			NS_LOG_FUNCTION(Log());
			NodeInfo * info = new NodeInfo();
			info->nodeId = commHeader->getSourceId();
			info->position = commHeader->getSourcePosition();
			info->distance = GetDistance(GetController()->GetPosition(), info->position);
			info->direction = resposeHeader->getSourceDirection();
			info->conformantDirection.dir = resposeHeader->getConformantDirection();
			info->conformantDirection.vMov = resposeHeader->getVehicleMovement();
			info->currentSpeed = resposeHeader->getCurrentSpeed();
			info->avgSpeedSmall = resposeHeader->getAvgSpeedLow();
			info->avgSpeedHigh = resposeHeader->getAvgSpeedHigh();
			info->lastSeen = CurrentTime::Now();
			info->lastMessage = resposeHeader->getLastMessage();

			NS_LOG_INFO(
					Log() << "received info for node " << info->nodeId << ": pos=" << info->position<< " " << PrintHeader(resposeHeader));
			if (info->lastMessage)
			{
				RemoveLastSeen(info);
				m_traceLastMessageNode(info);
			} else
			{
				UpdateLastSeen(info);
				m_traceBeaconResponse(info);
			}
		}

		void BehaviourRsu::OnNoLongerConformant(CommHeader * commHeader, NoLongerConformantHeader * noLongerConformantHeader)
		{
			NS_LOG_FUNCTION(Log());
			NodeInfo * info = new NodeInfo();
			info->nodeId = commHeader->getSourceId();
			info->position = commHeader->getSourcePosition();
			info->distance = GetDistance(GetController()->GetPosition(), info->position);
			info->lastSeen = CurrentTime::Now();
			info->conformantDirection.dir = noLongerConformantHeader->getConformantDirection();
			info->conformantDirection.vMov = noLongerConformantHeader->getVehicleMovement();
			info->direction = noLongerConformantHeader->getSourceDirection();
			NS_LOG_INFO(Log()<<"node "<<info->nodeId<<" no longer conformant");
			RemoveLastSeen(info);
			m_traceNoLongerConforman(info);
		}

		void BehaviourRsu::EventBeacon(int position)
		{
			VehicleDirection direction = m_directions[position];
			NS_LOG_FUNCTION(Log() << "direction=" << direction);

			BeaconHeader *header = new BeaconHeader();
			header->setDirection(direction.dir);
			header->setVehicleMovement(direction.vMov);
			header->setMaxResponseTime(direction.time);
			GetController()->Send(NT_VEHICLE, header, PID_SPEED, PROTOCOL_MESSAGE);
			NS_LOG_DEBUG(Log() << "Sent beacon for direction " << direction);

			int nextPosition = position + 1 == m_directions.size() ? 0 : position + 1;
			m_eventBeacon = Scheduler::Schedule(direction.time, &BehaviourRsu::EventBeacon, this, nextPosition);
		}

		void BehaviourRsu::EventCheck()
		{
			NS_LOG_FUNCTION(Log());
			m_eventCheck = Scheduler::Schedule(m_timeCheck, &BehaviourRsu::EventCheck, this);
			CheckTimeout();
		}

		void BehaviourRsu::CheckTimeout()
		{
			int currentTime = CurrentTime::Now();
			for (TimeoutMap::iterator node = m_nodeLastSeen.begin(); node != m_nodeLastSeen.end();)
			{
				for (DirMap::iterator dir = node->second.begin(); dir != node->second.end();)
				{
					if (currentTime - dir->second > m_timeOut)
					{
						NS_LOG_INFO(
								Log()<<"Node " << node->first << " timeout in direction " << dir->first.getId()<< ". Was last seen " << dir->second);
						NodeInfo * info = new NodeInfo();
						info->nodeId = node->first;
						info->conformantDirection = dir->first;
						info->lastSeen = dir->second;
						m_traceTimeOutNode(info);
						node->second.erase(dir++);
					} else
						++dir;
				}
				if (node->second.size() == 0)
					m_nodeLastSeen.erase(node++);
				else
					++node;
			}
		}

		bool BehaviourRsu::Execute(DirectionValueMap &data)
		{
			return false;
		}
	} /* namespace application */
} /* namespace protocol */
