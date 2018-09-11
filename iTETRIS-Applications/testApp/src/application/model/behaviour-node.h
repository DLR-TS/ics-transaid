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

#ifndef BEHAVIOUR_NODE_H_
#define BEHAVIOUR_NODE_H_

#include "behaviour.h"
#include "scheduler.h"
#include "random-variable.h"
#include <map>
#include "structs.h"

namespace testapp
{
	namespace application
	{

		struct RSU
		{
				RSU()
				{
					id = 0;
					muted = false;
				}
				RSU(int rsuId, VehicleDirection conformantDirection)
				{
					id = rsuId;
					dir = conformantDirection;
					muted = false;
				}
				int id;
				bool muted;
				VehicleDirection dir;
		};

		/**
		 * Base class implementing some base functionality for the children classes
		 */
		class BehaviourNode: public Behaviour
		{
			public:
				static bool Enabled;
				static uint16_t ResponseTimeSpacing;
				static double SinkThreshold;

				BehaviourNode(iCSInterface* controller);
				virtual ~BehaviourNode();

				void Start();
				void Stop();

				virtual bool IsSubscribedTo(ProtocolId pid) const;
				virtual void Receive(server::Payload *payload, double snr);
				virtual bool Execute(const int currentTimeStep, DirectionValueMap &data);
                virtual void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);


				TypeBehaviour GetType() const
				{
					return Type();
				}

				static TypeBehaviour Type()
				{
					return TYPE_BEHAVIOUR_NODE;
				}

			protected:
				ns3::UniformVariable m_rnd;
				//Configuration
				uint16_t m_responseTimeSpacing;

				// Events
				event_id m_eventResponse;

				TracedCallback<NodeInfo&> m_traceSendData;
		};

		/**
		 * Installed on the nodes if IcsInterface::UseSink is set to true
		 * It uses a threshold to communicate to the rsu that it has reached the center of the
		 * intersection and that the current message will be its last one the current direction
		 */
		class BehaviourNodeWithSink: public BehaviourNode
		{
			public:
				BehaviourNodeWithSink(iCSInterface * controller);
				virtual ~BehaviourNodeWithSink();

				virtual void Receive(server::Payload *payload, double snr);

			private:
				/**
				 * @brief Called by the receive after a random timeout when a beacon message is received
				 * @brief The node send a BeaconResponse message
				 */
				void EventSendResponse(NodeInfo);
				double m_sinkThreshold;
				RSU m_muteRsu;
		};

		/**
		 * Installed on the nodes if IcsInterface::UseSink is set to false
		 * It uses a no longer conformant message to communicate to the rsu that it
		 * has passed the center of the intersection
		 */
		class BehaviourNodeWithoutSink: public BehaviourNode
		{
			public:
				BehaviourNodeWithoutSink(iCSInterface * controller);
				virtual ~BehaviourNodeWithoutSink();

				virtual void Receive(server::Payload *payload, double snr);

			private:
				/**
				 * @brief Called by the receive after a random timeout when a beacon message is received
				 */
				void EventSendResponse(NodeInfo);
				/**
				 * @brief Called by the EventSendResponse if the node has not yet passed the intersection
				 * @brief The node send a BeaconResponse message
				 */
				void SendRespose(NodeInfo);
				/**
				 * @brief Called by the EventSendResponse if the node has passed the intersection
				 * @brief The node send a NoLongerConformant message
				 */
				void SendNoLongerConformant(NodeInfo);
				std::map<std::string, bool> m_activeDirections;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_NODE_H_ */
