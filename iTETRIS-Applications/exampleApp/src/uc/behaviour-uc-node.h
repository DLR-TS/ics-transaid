/****************************************************************************************
 * Copyright (c) 205 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-20-8, grant agreement no. 38622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * . Redistributions of source code must retain the above copyright notice,
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

#ifndef BEHAVIOUR_UC_NODE_H_
#define BEHAVIOUR_UC_NODE_H_

#include "application/model/behaviour-node.h"
#include "application/helper/scheduler.h"
#include "application/message-scheduler-helper.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp
{
	namespace application
	{
		struct cam_message_t
		{
			double frequency;
			event_id eventId;

			bool autoSend;
			bool initialize;
			bool highlight;
			bool enable;
		};

		/**
		* Behaviour for mobile nodes in uc cases.
		*/
		class BehaviourUCNode : public BehaviourNode
		{
		public:
			BehaviourUCNode(iCSInterface *controller, const bool use_ns3);
			~BehaviourUCNode();

			void Start();

			void OnAddSubscriptions();
			bool IsSubscribedTo(ProtocolId pid) const;
			void Receive(server::Payload *payload, double snr);
			bool Execute(DirectionValueMap &data);

			TypeBehaviour GetType() const { return Type(); }

			// in transaid/iTETRIS-Applications/baseApp/src/application/model/common.h
			static TypeBehaviour Type() { return TYPE_BEHAVIOUR_UC4_NODE; }

		private:
			/** @brief Regular broadcast of CAM-like messages (schedules itself)
    		*  @note Awaits fix of CAM mechanism
    		*/
			void sendCAMInfoRepeated();

			/** @brief Sends current vehicle state
    		*  @note Awaits fix of CAM mechanism
    		*/
			void sendCAMInfo();

		private:
			MessageScheduler* m_msgScheduler;
			cam_message_t cam_message;

			int m_node_id;

			bool m_use_ns3;
			bool m_info_set;
			bool m_optimizeForSpeed;
			bool m_debugCamMessages;

			std::string m_vehID;
		};

	} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_NODE_H_ */
