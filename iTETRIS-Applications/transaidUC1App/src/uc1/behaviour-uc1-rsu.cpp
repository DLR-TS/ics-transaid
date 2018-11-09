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

#include "behaviour-uc1-rsu.h"
#include "ics-interface.h"
#include "program-configuration.h"
#include "node.h"
#include "../../app-commands-subscriptions-constants.h"
#include "current-time.h"
#include "log/console.h"

using namespace baseapp;
using namespace baseapp::application;

namespace uc1app
{
	namespace application
	{

		///BehaviourUC1RSU implementation
		BehaviourUC1RSU::BehaviourUC1RSU(iCSInterface* controller) :
				BehaviourNode(controller), m_firstBroadcast(true), m_broadcastInterval(1000),
				m_broadcastActive(true), m_eventBroadcast(0), m_eventAbortBroadcast(0)
		{
		}

		BehaviourUC1RSU::~BehaviourUC1RSU() {
            Scheduler::Cancel(m_eventBroadcast);
            Scheduler::Cancel(m_eventAbortBroadcast);
        }

		void BehaviourUC1RSU::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetUC1Case()=="simpleExecute") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetUC1Case()=="setVType") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple") {
                // TODO: Subscribe to receive CAMs?
           } else if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
               // Start broadcasting at 5000
               int broadcastStart = 5000;
               int broadcastEnd = 10000;
               m_eventBroadcast = Scheduler::Schedule(broadcastStart, &BehaviourUC1RSU::RSUBroadcastCommSimple2, this);
               m_eventAbortBroadcast = Scheduler::Schedule(broadcastEnd, &BehaviourUC1RSU::abortBroadcast, this);
               NS_LOG_INFO(Log() << "RSU scheduled broadcast start at " << broadcastStart << " and broadcastEnd at " << broadcastEnd);
          }

		}

		bool BehaviourUC1RSU::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_UNKNOWN;
		}

		void BehaviourUC1RSU::Receive(server::Payload *payload, double snr)
		{
            NS_LOG_FUNCTION(Log());
            if (!m_enabled)
                return;
            CommHeader* commHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
            if (commHeader->getMessageType() != MT_UC1_RESPONSE)
            {
                NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
                return;
            }
            UC1Header* uc1Header;
            GetController()->GetHeader(payload, server::PAYLOAD_END, uc1Header);
            Header * receivedHeader = payload->getHeader(uc1app::server::PAYLOAD_END);
            UC1Header* receivedUC1Header = dynamic_cast<UC1Header*>(receivedHeader);

            NS_LOG_INFO(Log() << "Received a uc1 message with content: " << uc1Header->getMessage());

            if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
                double responseTime = m_rnd.GetValue(m_responseTimeSpacing, uc1Header->getMaxResponseTime() - m_responseTimeSpacing);
                Scheduler::Cancel(m_eventResponse);
                if (receivedUC1Header->getMessage() == "Vehicle response to RSU Vehicle acknowledgement") {
                    // Random offset for responseTime
                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends stop advice response on reception of Vehicle response to RSU Vehicle acknowledgement.");
                    UC1Header::ResponseInfo response;
                    response.message = "RSU Stop advice";
                    response.targetID = commHeader->getSourceId();
                    response.stopEdge = "CE";
                    response.stopPosition = 50;
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourUC1RSU::EventSendResponse, this, response);
                    NS_LOG_INFO(Log() << "scheduled a uc1 response to Vehicle response to RSU Vehicle acknowledgement in " << responseTime);
                } else if (receivedUC1Header->getMessage() == "Vehicle response to RSU broadcast") {
                    // Random offset for responseTime
                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends acknowledgement response on reception of vehicle's first response.");
                    UC1Header::ResponseInfo response;
                    response.message = "RSU Vehicle acknowledgement";
                    response.targetID = commHeader->getSourceId();
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourUC1RSU::EventSendResponse, this, response);
                    NS_LOG_INFO(Log() << "scheduled a uc1 response to Vehicle regular broadcast in " << responseTime);
                }
            }
		}

		bool BehaviourUC1RSU::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
            if (ProgramConfiguration::GetUC1Case() == "setVType") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetUC1Case() == "inductionLoop") {
                // constantly query induction loop status via RSU
                GetController()->AddTraciSubscription("WC", CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER);
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple") {
                // RSU constantly broadcasts for 5 secs (starting at t=5000)
                if (currentTimeStep < 10000) {
                    UC1Header * header = new UC1Header(PID_UNKNOWN, MT_RSU_UC1, "RSU regular broadcast message");
                    GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_UC1APP);
                }
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
                // do nothing
            }
			return false;
		}


        void BehaviourUC1RSU::EventSendResponse(UC1Header::ResponseInfo response)
        {
            NS_LOG_FUNCTION(Log());

            // React to perception of vehicle.
            UC1Header * responseHeader = new UC1Header(PID_UNKNOWN, MT_RSU_UC1, response);
            GetController()->SendTo(response.targetID, responseHeader , PID_UNKNOWN, MSGCAT_UC1APP);

            NS_LOG_DEBUG(Log() << "Sent uc1 response to vehicle " << response.targetID);
        }


        void BehaviourUC1RSU::RSUBroadcastCommSimple2()
        {
            if (m_firstBroadcast) {
                NS_LOG_FUNCTION(Log());
                m_firstBroadcast = false;
                NS_LOG_DEBUG(Log() << "Starting vehicle broadcast");
            }

            if (!m_broadcastActive)
                return;

            UC1Header * header = new UC1Header(PID_UNKNOWN, MT_RSU_UC1, "RSU regular broadcast message");
            GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_UC1APP);

            // Schedule next broadcast with random offset
            double nextTime = m_rnd.GetValue(m_broadcastInterval, m_broadcastInterval+100);
            NS_LOG_DEBUG(Log() << "Scheduled next RSU broadcast at time " << nextTime);
            m_eventBroadcast = Scheduler::Schedule(nextTime, &BehaviourUC1RSU::RSUBroadcastCommSimple2, this);
        }


        void BehaviourUC1RSU::abortBroadcast()
        {
            NS_LOG_FUNCTION(Log());
            m_broadcastActive = false;
            Scheduler::Cancel(m_eventBroadcast);
            NS_LOG_DEBUG(Log() << "RSU aborted broadcasting");
        }


        void BehaviourUC1RSU::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {
            NS_LOG_FUNCTION(Log());
            if (ProgramConfiguration::GetUC1Case() == "CAMsimple") {
                NS_LOG_DEBUG(Log() << "Node " << nodeID <<   " received CAM messages");
                for (std::vector<CAMdata>::const_iterator it = receivedCAMmessages.begin(); it != receivedCAMmessages.end(); ++it)
                {
                    NS_LOG_INFO(Log() << "CAM message received from node " << it->senderID << " at time " << it->generationTime);
                }
            }
        }

        void BehaviourUC1RSU::processTraCIResult(const int result, const Command& command) {
            Behaviour::processTraCIResult(result, command);
            if (ProgramConfiguration::GetUC1Case() == "inductionLoop") {
                if (command.commandId == CMD_GET_INDUCTIONLOOP_VARIABLE
                        && command.variableId == LAST_STEP_VEHICLE_NUMBER
                        && result > 0) {
                    GetController()->AddTraciSubscription(command.objId, CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_ID_LIST);
                    GetController()->AddTraciSubscription(command.objId, CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_MEAN_SPEED);
                    GetController()->AddTraciSubscription(command.objId, CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_OCCUPANCY);
                }
            }
        }


	} /* namespace application */
} /* namespace protocol */
