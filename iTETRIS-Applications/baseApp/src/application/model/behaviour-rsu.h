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
#ifndef BEHAVIOUR_RSU_H_
#define BEHAVIOUR_RSU_H_

#include "behaviour.h"
#include "random-variable.h"
#include "scheduler.h"
#include "program-configuration.h"
#include <map>

namespace baseapp
{
	namespace application
	{
		struct VehicleDirectionOrdering
		{
				bool operator()(const VehicleDirection& left, const VehicleDirection& right);
		};

		static const double BIG_VALUE = 1000 * 1000; //Big arbitrary value
		struct FlowStatus
		{
				FlowStatus(VehicleDirection direction) :
						dir(direction)
				{
					Reset();
				}
				VehicleDirection dir;
				unsigned quantity;
				double maxDistance;
				double minDistance;
				double avgSpeed;
				double avgTime;
				unsigned numLastMessage;
				void Reset()
				{
					numLastMessage = avgSpeed = avgTime = quantity = maxDistance = 0;
					minDistance = BIG_VALUE;
				}
		};

		class CommHeader;
		class BeaconResponseHeader;

		/**
		 * Message exchange behavior installed on a rsu node.
		 * It contains the message logic only.
		 * The storage of the messages is delegated to the data-manager class
		 */
		class BehaviourRsu: public Behaviour
		{
			public:
				static bool Enabled;
				static uint16_t TimeBeaconMin;
				static uint16_t TimeBeacon;
				static uint16_t TimeCheck;
				static uint16_t Timeout;

				BehaviourRsu(iCSInterface* controller);
				virtual ~BehaviourRsu();

				/**
				 * @brief Called by the ics interface to add the relevant direction
				 * @brief that have to be polled to get informations
				 */
				void AddDirections(std::vector<Direction>);
				const std::vector<VehicleDirection>& GetDirections() const;
				void Start();
				void Stop();

				virtual bool IsSubscribedTo(ProtocolId pid) const;
				virtual void Receive(server::Payload *payload, double snr);
				virtual bool Execute(const int currentTimeStep, DirectionValueMap &data);


				TypeBehaviour GetType() const
				{
					return Type();
				}

				static TypeBehaviour Type()
				{
					return TYPE_BEHAVIOUR_RSU;
				}

			protected:
                ns3::UniformVariable m_rnd;
                uint16_t m_responseTimeSpacing;
                event_id m_eventResponse;
				std::vector<VehicleDirection> m_directions;

				//Configuration
				uint16_t m_timeBeaconMin;
				uint16_t m_timeBeacon;
				uint16_t m_timeCheck;
				uint16_t m_timeOut;

				//Events
				event_id m_eventBeacon;
				event_id m_eventCheck;

				void EventBeacon(int position);
				void EventCheck();

			private:
				double m_beaconInterval;

				typedef std::map<const VehicleDirection, int, VehicleDirectionOrdering> DirMap;
				typedef std::map<const int, DirMap> TimeoutMap;
				TimeoutMap m_nodeLastSeen;
				/**
				 * @brief Called to update the last seen time of a node the a message is received
				 */
				void UpdateLastSeen(NodeInfo *);
				/**
				 * @brief Called to remove the node if it has send a message with the last message flag set
				 * @brief or a no longer conformant message
				 */
				void RemoveLastSeen(NodeInfo *);
				/**
				 * @brief Periodically check if a node has timed out
				 */
				void CheckTimeout();


				bool m_executeAtThisStep;

				/**
				 * @brief Called when a beacon response is received
				 */
				void OnBeaconResponse(CommHeader*, BeaconResponseHeader*);
				/**
				 * @brief Trace invoked when the rsu has received a beacon response from a node
				 */
				TracedCallback<NodeInfo*> m_traceBeaconResponse;
				/**
				 * @brief Called when a no longer conformant is received
				 */
				void OnNoLongerConformant(CommHeader*, NoLongerConformantHeader*);
				/**
				 * @brief Trace invoked when the rsu has received a nolongerconformat message
				 */
				TracedCallback<NodeInfo*> m_traceNoLongerConforman;

				//More events
				/**
				 * @brief Called when a noded times out
				 */
				TracedCallback<NodeInfo*> m_traceTimeOutNode;
				/**
				 * @brief Called when the message received from the node is its last one for the current direction
				 * @brief Only used in the case IcsInterface::UseSink is true
				 */
				TracedCallback<NodeInfo*> m_traceLastMessageNode;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_RSU_H_ */
