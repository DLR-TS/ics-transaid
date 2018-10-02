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

#include "behaviour-test-rsu.h"
#include "ics-interface.h"
#include "program-configuration.h"
#include "node.h"
#include "../../app-commands-subscriptions-constants.h"
#include "current-time.h"
#include "log/console.h"


namespace testapp
{
	namespace application
	{

		///BehaviourTestRSU implementation
		BehaviourTestRSU::BehaviourTestRSU(iCSInterface* controller) :
				BehaviourNode(controller), m_firstBroadcast(true), m_broadcastInterval(1000),
				m_broadcastActive(true), m_eventBroadcast(0), m_eventAbortBroadcast(0)
		{
		}

		BehaviourTestRSU::~BehaviourTestRSU() {
            Scheduler::Cancel(m_eventBroadcast);
            Scheduler::Cancel(m_eventAbortBroadcast);
        }

		void BehaviourTestRSU::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetTestCase()=="simpleExecute") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase()=="setVType") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase() == "commSimple") {
                // TODO: Subscribe to receive CAMs?
           } else if (ProgramConfiguration::GetTestCase() == "commSimple2") {
               // Start broadcasting at 5000
               int broadcastStart = 5000;
               int broadcastEnd = 10000;
               m_eventBroadcast = Scheduler::Schedule(broadcastStart, &BehaviourTestRSU::RSUBroadcastCommSimple2, this);
               m_eventAbortBroadcast = Scheduler::Schedule(broadcastEnd, &BehaviourTestRSU::abortBroadcast, this);
               NS_LOG_INFO(Log() << "RSU scheduled broadcast start at " << broadcastStart << " and broadcastEnd at " << broadcastEnd);
          }

		}

		bool BehaviourTestRSU::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_UNKNOWN;
		}

		void BehaviourTestRSU::Receive(server::Payload *payload, double snr)
		{
            NS_LOG_FUNCTION(Log());
            if (!m_enabled)
                return;
            CommHeader* commHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
            if (commHeader->getMessageType() != MT_TEST_RESPONSE)
            {
                NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
                return;
            }
            TestHeader* testHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_END, testHeader);
            Header * receivedHeader = payload->getHeader(testapp::server::PAYLOAD_END);
            TestHeader* receivedTestHeader = dynamic_cast<TestHeader*>(receivedHeader);

            NS_LOG_INFO(Log() << "Received a test message with content: " << testHeader->getMessage());

            if (ProgramConfiguration::GetTestCase() == "commSimple2") {
                double responseTime = m_rnd.GetValue(m_responseTimeSpacing, testHeader->getMaxResponseTime() - m_responseTimeSpacing);
                Scheduler::Cancel(m_eventResponse);
                if (receivedTestHeader->getMessage() == "Vehicle response to RSU Vehicle acknowledgement") {
                    // Random offset for responseTime
                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends stop advice response on reception of Vehicle response to RSU Vehicle acknowledgement.");
                    TestHeader::ResponseInfo response;
                    response.message = "RSU Stop advice";
                    response.targetID = commHeader->getSourceId();
                    response.stopEdge = "CE";
                    response.stopPosition = 50;
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestRSU::EventSendResponse, this, response);
                    NS_LOG_INFO(Log() << "scheduled a test response to Vehicle response to RSU Vehicle acknowledgement in " << responseTime);
                } else if (receivedTestHeader->getMessage() == "Vehicle response to RSU broadcast") {
                    // Random offset for responseTime
                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends acknowledgement response on reception of vehicle's first response.");
                    TestHeader::ResponseInfo response;
                    response.message = "RSU Vehicle acknowledgement";
                    response.targetID = commHeader->getSourceId();
                    m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestRSU::EventSendResponse, this, response);
                    NS_LOG_INFO(Log() << "scheduled a test response to Vehicle regular broadcast in " << responseTime);
                }
            }
		}

		bool BehaviourTestRSU::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
            if (ProgramConfiguration::GetTestCase() == "setVType") {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase() == "inductionLoop") {
                // constantly query induction loop status via RSU
                GetController()->AddTraciSubscription("WC", CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER);
            } else if (ProgramConfiguration::GetTestCase() == "commSimple") {
                // RSU constantly broadcasts for 5 secs (starting at t=5000)
                if (currentTimeStep < 10000) {
                    TestHeader * header = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU regular broadcast message");
                    GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_TESTAPP);
                }
            } else if (ProgramConfiguration::GetTestCase() == "commSimple2") {
                // do nothing
            }
			return false;
		}


        void BehaviourTestRSU::EventSendResponse(TestHeader::ResponseInfo response)
        {
            NS_LOG_FUNCTION(Log());

            // React to perception of vehicle.
            TestHeader * responseHeader = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, response);
            GetController()->SendTo(response.targetID, responseHeader , PID_UNKNOWN, MSGCAT_TESTAPP);

            NS_LOG_DEBUG(Log() << "Sent test response to vehicle " << response.targetID);
        }


        void BehaviourTestRSU::RSUBroadcastCommSimple2()
        {
            if (m_firstBroadcast) {
                NS_LOG_FUNCTION(Log());
                m_firstBroadcast = false;
                NS_LOG_DEBUG(Log() << "Starting vehicle broadcast");
            }

            if (!m_broadcastActive)
                return;

            TestHeader * header = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU regular broadcast message");
            GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

            // Schedule next broadcast with random offset
            double nextTime = m_rnd.GetValue(m_broadcastInterval, m_broadcastInterval+100);
            NS_LOG_DEBUG(Log() << "Scheduled next RSU broadcast at time " << nextTime);
            m_eventBroadcast = Scheduler::Schedule(nextTime, &BehaviourTestRSU::RSUBroadcastCommSimple2, this);
        }


        void BehaviourTestRSU::abortBroadcast()
        {
            NS_LOG_FUNCTION(Log());
            m_broadcastActive = false;
            Scheduler::Cancel(m_eventBroadcast);
            NS_LOG_DEBUG(Log() << "RSU aborted broadcasting");
        }


        void BehaviourTestRSU::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {
            NS_LOG_FUNCTION(Log());
            if (ProgramConfiguration::GetTestCase() == "CAMsimple") {
                NS_LOG_DEBUG(Log() << "Node " << nodeID <<   " received CAM messages");
                for (std::vector<CAMdata>::const_iterator it = receivedCAMmessages.begin(); it != receivedCAMmessages.end(); ++it)
                {
                    NS_LOG_INFO(Log() << "CAM message received from node " << it->senderID << " at time " << it->generationTime);
                }
            }
        }



	} /* namespace application */
} /* namespace protocol */
