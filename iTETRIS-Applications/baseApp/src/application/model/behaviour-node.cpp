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

#include "behaviour-node.h"
#include "ics-interface.h"
#include "node-sampler.h"

namespace testapp
{
	namespace application
	{

		///BehaviourNode implementation
		bool BehaviourNode::Enabled = true;
		uint16_t BehaviourNode::ResponseTimeSpacing = 10;
		double BehaviourNode::SinkThreshold = 20;

		BehaviourNode::BehaviourNode(iCSInterface* controller) :
				Behaviour(controller)
		{
			m_enabled = Enabled;
			m_responseTimeSpacing = ResponseTimeSpacing;

			m_rnd = ns3::UniformVariable();
			m_eventResponse = 0;

			RegisterTrace("NodeSendData", m_traceSendData);
		}

		BehaviourNode::~BehaviourNode()
		{
			Scheduler::Cancel(m_eventResponse);
		}

		void BehaviourNode::Start()
		{
			if (!m_enabled)
				return;
			Behaviour::Start();
		}
		void BehaviourNode::Stop()
		{
			Scheduler::Cancel(m_eventResponse);
			Behaviour::Stop();
		}

		bool BehaviourNode::IsSubscribedTo(ProtocolId pid) const
		{
			return pid == PID_SPEED;
		}

		void BehaviourNode::Receive(server::Payload *payload, double snr)
		{
		}

		bool BehaviourNode::Execute(const int currentTimeStep, DirectionValueMap &data)
		{
			return false;
		}

        void BehaviourNode::processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages)
        {

        }


	} /* namespace application */
} /* namespace protocol */