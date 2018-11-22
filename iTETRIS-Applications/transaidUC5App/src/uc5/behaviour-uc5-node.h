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

#ifndef BEHAVIOUR_UC5_NODE_H_
#define BEHAVIOUR_UC5_NODE_H_

#include <map>
#include "application/model/behaviour-node.h"
#include "application/helper/scheduler.h"
#include "application/helper/random-variable.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace uc5app
{
	namespace application
	{
		/**
		 * Behaviour for mobile nodes in uc5 cases.
		 */
		class BehaviourUC5Node: public BehaviourNode
		{
		public:
		    BehaviourUC5Node(iCSInterface* controller);
		    ~BehaviourUC5Node();

		    void Start();

		    virtual bool IsSubscribedTo(ProtocolId pid) const;
		    virtual void Receive(server::Payload *payload, double snr);
		    virtual bool Execute(const int currentTimeStep, DirectionValueMap &data);
            void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);
            void processTraCIResult(const int result, const Command& command);
            void processTraCIResult(const double result, const Command& command);
            void processTraCIResult(const std::string result, const Command& command);
            void processTraCIResult(const std::vector<std::string> result, const Command& command);

		    void abortWaitingForRSUResponse();

		    TypeBehaviour GetType() const
		    {
		        return Type();
		    }

		    static TypeBehaviour Type()
		    {
		        return TYPE_BEHAVIOUR_UC5_NODE;
		    }

		};

	} /* namespace application */
} /* namespace uc5app */

#endif /* BEHAVIOUR_UC5_NODE_H_ */
