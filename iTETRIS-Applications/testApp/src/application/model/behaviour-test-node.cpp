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

#include "behaviour-test-node.h"
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

		///BehaviourTestNode implementation
		BehaviourTestNode::BehaviourTestNode(iCSInterface* controller) :
				BehaviourNode(controller)
		{
            m_waitForRSUAcknowledgement = true;
            m_eventAbortWaitingForRSU = 0;
		}

		void BehaviourTestNode::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetTestCase()==TEST_CASE_EXECUTE) {
                // do nothing
            } else if (ProgramConfiguration::GetTestCase()==TEST_CASE_SETVTYPE) {
                GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_TYPE);
                tcpip::Storage type;
                type.writeString("type0");
                GetController()->AddTraciSubscription(CMD_SET_VEHICLE_VARIABLE, VAR_TYPE, TYPE_STRING, &type);
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                // Time to abort waiting for a response after insertion.
                // Will be ineffective if communication runs as intended.
                // Used for testing purposes before random offsets were assigned to messages.
                // (=> abort at 12000, as the test vehicle is inserted at t=2000)
                const int endWaitingTime = 10000;
                m_eventAbortWaitingForRSU = Scheduler::Schedule(endWaitingTime, &BehaviourTestNode::abortWaitingForRSUResponse, this);
                // TODO: Subscribe to receive CAMs?
            }

		}

		bool BehaviourTestNode::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_UNKNOWN;
		}

		void BehaviourTestNode::Receive(server::Payload *payload, double snr)
		{
            NS_LOG_FUNCTION(Log());
            if (!m_enabled)
                return;
            CommHeader* commHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);
            if (commHeader->getMessageType() != MT_RSU_TEST)
            {
                NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
                return;
            }
            NodeInfo rsu;
            rsu.nodeId = commHeader->getSourceId();
            rsu.position = commHeader->getSourcePosition();


            TestHeader* testHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_END, testHeader);

            NS_LOG_INFO(Log() << "Received a test message with content: " << testHeader->getMessage());

            if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                // TODO: Respond to test message a limited number of times, removal could be scheduled ... (see behaviours)
                Header * receivedHeader = payload->getHeader(testapp::server::PAYLOAD_END);
                TestHeader* receivedTestHeader = dynamic_cast<TestHeader*>(receivedHeader);
                if (m_waitForRSUAcknowledgement && receivedTestHeader->getMessage() == "RSU regular broadcast message") {
                    TestHeader * newHeader = new TestHeader(PID_UNKNOWN, MT_BEACON_RESPONSE, "Vehicle RSU broadcast acknowledgement");
//                     TODO: Send with random offset
//                    GetController()->Send(NT_ALL, newHeader, PID_UNKNOWN, MSGCAT_TESTAPP);
//                        SendTo(header->getSourceId(), newHeader, PID_UNKNOWN, MSGCAT_TESTAPP);
                } else if (receivedTestHeader->getMessage() == "RSU Vehicle stop advice") {
                    Scheduler::Cancel(m_eventAbortWaitingForRSU);
                    m_waitForRSUAcknowledgement = false;
                }
            }

//
//
//
//
//            // Add a random time offset to the response submission
//            double nextTime = m_rnd.GetValue(m_responseTimeSpacing,
//                    testHeader->getMaxResponseTime() - m_responseTimeSpacing);
//
//            Scheduler::Cancel(m_eventResponse);
//            m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourTestNode::EventSendResponse, this, rsu);
//            NS_LOG_INFO(Log() << "scheduled a test response in " << nextTime);
		}

		bool BehaviourTestNode::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
            if (ProgramConfiguration::GetTestCase() == TEST_CASE_SETVTYPE) {
                if (currentTimeStep == 10000) {
                    // check vType at time 10.
                    GetController()->AddTraciSubscription(CMD_GET_VEHICLE_VARIABLE, VAR_TYPE);
                }
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_INDUCTIONLOOP) {
                // constantly query induction loop status via RSU
                if (GetController()->GetNode()->isFixed()) {
                    GetController()->AddTraciSubscription("WC", CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER);
                }
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                // After t=8000, vehicle starts broadcasting until its broadcast is acknowledged or aborted at t = 12000
                if (currentTimeStep > 8000 && m_waitForRSUAcknowledgement){
                    TestHeader * header = new TestHeader(PID_UNKNOWN, MT_TEST_RESPONSE, "Vehicle regular broadcast");
                    // TODO: Send with random offset
                    GetController()->SendTo(5000, header, PID_UNKNOWN, MSGCAT_TESTAPP);
                }
            }
			return false;
		}


        void BehaviourTestNode::EventSendResponse(NodeInfo rsu)
        {
            NS_LOG_FUNCTION(Log());


            NS_LOG_DEBUG(
                    Log() << "Sent test response to RSU " << rsu.nodeId);
        }

        void BehaviourTestNode::abortWaitingForRSUResponse()
        {
            NS_LOG_FUNCTION(Log());
            m_waitForRSUAcknowledgement = false;
            NS_LOG_DEBUG(Log() << "Aborted waiting for RSU response");
        }


	} /* namespace application */
} /* namespace protocol */
