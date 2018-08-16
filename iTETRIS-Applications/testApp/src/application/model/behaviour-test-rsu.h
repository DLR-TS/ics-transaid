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

#ifndef BehaviourTestRSUBEHAVIOUR_TEST_RSU_H_
#define BehaviourTestRSUBEHAVIOUR_TEST_RSU_H_

#include "behaviour-node.h"
#include "scheduler.h"
#include "random-variable.h"
#include <map>

namespace testapp
{
	namespace application
	{
        /**
         * Behaviour for rsu in test cases. Inherits from BehaviourNode to have the random response offset variables at hand.
         */
		class BehaviourTestRSU: public BehaviourNode
		{
			public:
				BehaviourTestRSU(iCSInterface* controller);

				void Start();

				virtual bool IsSubscribedTo(ProtocolId pid) const;
				virtual void Receive(server::Payload *payload, double snr);
				virtual bool Execute(const int currentTimeStep, DirectionValueMap &data);
                /**
                 * @brief Called after a random timeout when a test message is received, @see Receive()
                 * @input[in] sender The source of the received message
                 */
                void EventSendResponse(TestHeader::ResponseInfo response);

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



                /// @}
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* BehaviourTestRSUBEHAVIOUR_TEST_RSU_H_ */
