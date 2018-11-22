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

#ifndef BEHAVIOUR_TEST_RSU_H_
#define BEHAVIOUR_TEST_RSU_H_

#include <map>
#include "application/model/behaviour-rsu.h"
#include "application/helper/scheduler.h"
#include "application/helper/random-variable.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace testapp
{
	namespace application
	{
        /**
         * Behaviour for rsu in test cases. Inherits from BehaviourNode to have the random response offset variables at hand.
         */
		class BehaviourTestRSU: public BehaviourRsu
		{
			public:
				BehaviourTestRSU(iCSInterface* controller);
				~BehaviourTestRSU();

				void Start();

				bool IsSubscribedTo(ProtocolId pid) const;
				void Receive(server::Payload *payload, double snr);
				bool Execute(const int currentTimeStep, DirectionValueMap &data);
				void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);
				void processTraCIResult(const int result, const Command& command);

                /**
                 * @brief Called after a random timeout when a test message is received, @see Receive()
                 * @input[in] sender The source of the received message
                 */
                void EventSendResponse(TestHeader::ResponseInfo response);

                void RSUBroadcastCommSimple2();


                void abortBroadcast();

                TypeBehaviour GetType() const
                {
                    return Type();
                }

                static TypeBehaviour Type()
                {
                    return TYPE_BEHAVIOUR_TEST_RSU;
                }

			private:

                /// @name Flags to be used by test cases
                /// @{
                bool m_firstBroadcast;
                bool m_broadcastActive;
                int m_broadcastInterval;
                bool m_mobilitySubscription;
                bool m_trafficLightSubscription;
                bool m_setCAMareaSubscription;
                bool m_subReceiveMessage;
                /// @}


                /// @name Events
                /// @{
                /// @brief used to refer to abort event scheduled at start
                event_id m_eventBroadcast;
                event_id m_eventAbortBroadcast;
                /// @}

		};

	} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_TEST_RSU_H_ */
