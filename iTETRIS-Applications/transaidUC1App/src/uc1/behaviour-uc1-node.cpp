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

#include "behaviour-uc1-node.h"
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

		///BehaviourUC1Node implementation
		BehaviourUC1Node::BehaviourUC1Node(iCSInterface* controller) :
				BehaviourNode(controller)
		{
            m_waitForRSUAcknowledgement = true;
            m_vehicleStopScheduled = false;
            m_firstBroadcast = true;
            m_eventAbortWaitingForRSU = 0;
            m_eventBroadcast = 0;
            m_broadcastInterval = 1000;
		}

        BehaviourUC1Node::~BehaviourUC1Node() {
            Scheduler::Cancel(m_eventAbortWaitingForRSU);
            Scheduler::Cancel(m_eventBroadcast);
        }

		void BehaviourUC1Node::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetUC1Case()=="simpleExecute") {
                // do nothing
            } else if (ProgramConfiguration::GetUC1Case()=="setVType") {
                GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_TYPE);
                tcpip::Storage type;
                type.writeString("t2");
                GetController()->AddTraciSubscription(CMD_SET_VEHICLE_VARIABLE, VAR_TYPE, TYPE_STRING, &type);
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple") {
                // Time to abort waiting for a response after insertion.
                // Will be ineffective if communication runs as intended (uc1 case commSimple2).
                // Used for uc1ing purposes before random offsets were assigned to messages. (uc1 case commSimple)
                // (=> abort at 12000, as the uc1 vehicle is inserted at t=2000)
                const int endWaitingTime = 10000;
                m_eventAbortWaitingForRSU = Scheduler::Schedule(endWaitingTime, &BehaviourUC1Node::abortWaitingForRSUResponse, this);
                // TODO: Subscribe to receive CAMs?
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
                // After t=8000, vehicle starts broadcasting until its broadcast is acknowledged or aborted at t = 12000
                const int endWaitingTime = 10000;
                const int insertionAccordingToRoutresFile = 2000;
                m_eventAbortWaitingForRSU = Scheduler::Schedule(endWaitingTime, &BehaviourUC1Node::abortWaitingForRSUResponse, this);
                NS_LOG_INFO(Log() << "Vehicle scheduled abort waiting for RSU acknowledgement at " << endWaitingTime + insertionAccordingToRoutresFile);
            }

		}

		bool BehaviourUC1Node::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_UNKNOWN;
		}

		void BehaviourUC1Node::Receive(server::Payload *payload, double snr)
		{
            NS_LOG_FUNCTION(Log());
            if (!m_enabled)
                return;
            CommHeader* commHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
            if (commHeader->getMessageType() != MT_RSU_UC1)
            {
                NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
                return;
            }
            NodeInfo rsu;
            rsu.nodeId = commHeader->getSourceId();
            rsu.position = commHeader->getSourcePosition();


            UC1Header* uc1Header;
            GetController()->GetHeader(payload, server::PAYLOAD_END, uc1Header);
            Header * receivedHeader = payload->getHeader(uc1app::server::PAYLOAD_END);
            UC1Header* receivedUC1Header = dynamic_cast<UC1Header*>(receivedHeader);

            NS_LOG_INFO(Log() << "Received a uc1 message with content: " << uc1Header->getMessage());

            if (ProgramConfiguration::GetUC1Case() == "commSimple") {
                if (m_waitForRSUAcknowledgement && receivedUC1Header->getMessage() == "RSU Vehicle acknowledgement") {
                    Scheduler::Cancel(m_eventAbortWaitingForRSU);
                    abortWaitingForRSUResponse();
                    NS_LOG_DEBUG(Log() << "On reception of RSU Vehicle acknowledgement.");
                }
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
                // Random offset for responseTime
                double responseTime = m_rnd.GetValue(m_responseTimeSpacing, uc1Header->getMaxResponseTime() - m_responseTimeSpacing);
                UC1Header::ResponseInfo response;
                response.targetID = commHeader->getSourceId();
                if (m_waitForRSUAcknowledgement && receivedUC1Header->getMessage() == "RSU regular broadcast message") {
                    NS_LOG_DEBUG(Log() << "Vehicle " << GetController()->GetNode()->getId() << "  and responds.");
                    response.message = "Vehicle response to RSU broadcast";
                    Scheduler::Cancel(m_eventResponse);
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourUC1Node::EventSendResponse, this, response);
                } else if (m_waitForRSUAcknowledgement && receivedUC1Header->getMessage() == "RSU Vehicle acknowledgement") {
                    Scheduler::Cancel(m_eventAbortWaitingForRSU);
                    abortWaitingForRSUResponse();
                    NS_LOG_DEBUG(Log() << "On reception of RSU Vehicle acknowledgement.");
                    // Send response
                    response.message = "Vehicle response to RSU Vehicle acknowledgement";
                    Scheduler::Cancel(m_eventResponse);
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourUC1Node::EventSendResponse, this, response);
                    NS_LOG_INFO(Log() << "Scheduled a uc1 response in " << responseTime);
                } else if (receivedUC1Header->getMessage() == "RSU Stop advice" && !m_vehicleStopScheduled) {
                    NS_LOG_DEBUG(Log() << "On reception of RSU Stop advice.");
                    std::string stopEdge = receivedUC1Header->getStopEdge();
                    double stopPosition = receivedUC1Header->getStopPosition();
                    GetController()->AddTraciStop(stopEdge, stopPosition, 0, 3.);
                    m_vehicleStopScheduled = true;
                    NS_LOG_INFO(Log() << "Added a stop on edge " << stopEdge << " at position" << stopPosition);
                }
            }
		}

		bool BehaviourUC1Node::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
            if (ProgramConfiguration::GetUC1Case() == "setVType") {
                if (currentTimeStep == 10000) {
                    // check vType at time 10.
                    GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_TYPE);
                }
            } else if (ProgramConfiguration::GetUC1Case() == "inductionLoop") {
                // Vehicle does nothing
						} else if (ProgramConfiguration::GetUC1Case() == "uc1Trajectory") {
							  //QUESTION: This seems to work only on specifi values. Why ?
							  if (currentTimeStep % 1000 == 0.0) {
								  	// retrieve speed every 10[sec].
									  GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED);
										// retrieve lane id every 10[sec].
									  GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_LANE_ID);
										// retrieve 1D position every lane at 10[sec].
									  GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_LANEPOSITION);
							  }
						} else if (ProgramConfiguration::GetUC1Case() == "uc1ToC") {
							  // TODO: instead of time, trigger ToC via lane ID and position
						  	if (currentTimeStep == 10000 ) {
										// Requesting ToC at 10[sec].
										GetController()->requestToC("veh0",4.0);
								}
						} else if (ProgramConfiguration::GetUC1Case() == "uc1Mobility") {
								if (currentTimeStep == 12000 ) {
										// Requesting Mobility Info at 12[sec].
										GetController()->requestMobilityInfo();
								}
            } else if (ProgramConfiguration::GetUC1Case() == "commSimple") {
                // After t=8000, vehicle starts broadcasting until its broadcast is acknowledged or aborted at t = 12000
                if (currentTimeStep > 8000 && m_waitForRSUAcknowledgement){
                    UC1Header * header = new UC1Header(PID_UNKNOWN, MT_UC1_RESPONSE, "Vehicle regular broadcast");
                    GetController()->SendTo(5000, header, PID_UNKNOWN, MSGCAT_UC1APP);
                }
            } if (ProgramConfiguration::GetUC1Case() == "commSimple2") {
                // do nothing
            }
			return false;
		}


        void BehaviourUC1Node::EventSendResponse(UC1Header::ResponseInfo response)
        {
            NS_LOG_FUNCTION(Log());

            // React to perception of vehicle.
            UC1Header * responseHeader = new UC1Header(PID_UNKNOWN, MT_UC1_RESPONSE, response);
            GetController()->SendTo(response.targetID, responseHeader , PID_UNKNOWN, MSGCAT_UC1APP);

            NS_LOG_DEBUG(Log() << "Sent uc1 response to RSU " << response.targetID);
        }


        void BehaviourUC1Node::abortWaitingForRSUResponse()
        {
            NS_LOG_FUNCTION(Log());
            m_waitForRSUAcknowledgement = false;
            NS_LOG_DEBUG(Log() << "Aborted waiting for RSU response");
        }


        void BehaviourUC1Node::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
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


	} /* namespace application */
} /* namespace protocol */
