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
				BehaviourNode(controller)
		{
		}

		void BehaviourTestRSU::Start()
		{
			if (!m_enabled)
				return;
			BehaviourNode::Start();

            //Example use of a traci command subscription
            if (ProgramConfiguration::GetTestCase()==TEST_CASE_EXECUTE) {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase()==TEST_CASE_SETVTYPE) {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                 // TODO: Subscribe to receive CAMs?
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
            if (commHeader->getMessageType() != MT_RSU_TEST)
            {
                NS_LOG_WARN(Log()<< "Received an unknown message "<< commHeader->getMessageType());
                return;
            }

            TestHeader* testHeader;
            GetController()->GetHeader(payload, server::PAYLOAD_END, testHeader);

            NS_LOG_INFO(Log() << "Received a test message with content: " << testHeader->getMessage());

            if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                    // TODO: react to registration of vehicle: send UNICAST response for scheduling a stop.
                    TestHeader * newHeader = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU Vehicle stop advice");
                    // TODO: Send with random offset
//                    GetController()->SendTo(commHeader->getSourceId(), newHeader, PID_UNKNOWN, MSGCAT_TESTAPP);
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
//            m_eventResponse = Scheduler::Schedule(nextTime, &BehaviourTestRSU::EventSendResponse, this, rsu);
//            NS_LOG_INFO(Log() << "scheduled a test response in " << nextTime);
		}

		bool BehaviourTestRSU::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
            // TODO: Insert test case specific code from iCSInterface::Execute()

            if (ProgramConfiguration::GetTestCase() == TEST_CASE_SETVTYPE) {
                // rsu does nothing
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_INDUCTIONLOOP) {
                // constantly query induction loop status via RSU
                GetController()->AddTraciSubscription("WC", CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER);
            } else if (ProgramConfiguration::GetTestCase() == TEST_CASE_COMMSIMPLE) {
                // RSU constantly broadcasts for 5 secs (starting at t=5000)
                if (currentTimeStep < 10000) {
                    TestHeader * header = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU regular broadcast message");
                    // TODO: Send with random offset
                    GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_TESTAPP);
                }
            }
			return false;
		}


        void BehaviourTestRSU::EventSendResponse(NodeInfo rsu)
        {
            NS_LOG_FUNCTION(Log());


            NS_LOG_DEBUG(
                    Log() << "Sent test response to RSU " << rsu.nodeId);
        }


	} /* namespace application */
} /* namespace protocol */
