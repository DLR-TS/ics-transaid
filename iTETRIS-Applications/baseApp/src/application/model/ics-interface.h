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
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <map>
#include <limits>
#include <memory>
#include "payload.h"
#include "fatal-error.h"
#include "trace-manager.h"
#include "headers.h"
#include "structs.h"
#include "traci-helper.h"
#include "behaviour.h"
#include "node-handler.h"
#include "foreign/tcpip/storage.h"

namespace libsumo {
    class TraCIColor;
}

namespace baseapp
{
	namespace application
	{

		class BehaviourFactory;
		class Node;
		class NodeSampler;
		class Subscription;

		/**
		 * Class used as interface between the actual protocol logic and the
		 * application framework that interacts with iCS
		 */
		class iCSInterface: public TraceManager
		{
		    friend class Behaviour; // To access the node
			public:
				static double DirectionTolerance;
				static uint16_t AverageSpeedSampleSmall;
				static uint16_t AverageSpeedSampleHigh;
				static bool UseSink;

				iCSInterface(Node*, NodeType, BehaviourFactory* factory);
				virtual ~iCSInterface();
				/**
				 * @brief Get the node instance on which it is installed this interface.
				 * @brief Used to interact directly with with the iCS interface
				 */
				Node* GetNode() const;
				/**
				 * @brief Get the sampler instance. It introduces errors to the position to simulate the imprecision of the devices
				 */
				NodeSampler* GetNodeSampler() const;
				/**
				 * @brief Get a Behaviour from its type
				 */
				Behaviour * GetBehaviour(TypeBehaviour type) const;

				/************************
				 * Utility methods
				 */
				/**
				 * @brief Get the id  of the node
				 */
                int GetId() const;
                std::string GetSumoID(int icsID = -1) const;


                /** @brief get a reference to the map icsID -> Node*
                 *         containing all nodes known to the application.
                 */
                const server::NodeMap& GetAllNodes() const;

				/**
				 * @brief If the node is active
				 */
				bool IsActive() const;
				NodeType GetNodeType() const;
				Vector2D GetPosition() const;
				double GetDirection() const;
				double GetSpeed() const;

				/**
				 * @brief Name of the node. For logging
				 */
				std::string NodeName() const;
				/**
				 * @brief Activates the node
				 */
				void Activate();
				/**
				 * @brief Deactivates the node
				 */
				void Deactivate();
				/**
				 * @brief Check if the node is going on a particular direction, using the set tolerance
				 */
				bool IsConformantDirection(double direction);
				/**
				 * @brief Check if the node is going on a particular direction, using the set tolerance
				 * @brief If also check if the node is arriving at the intersection of leaving it
				 */
				bool IsConformantDirectionAndMovement(const VehicleDirection direction, const Vector2D rsuPosition);
				/**
				 * @brief Static method to check if two direction differ by less than the tolerance
				 */
				static bool CheckDirections(const double &dir1, const double &dir2, const double &tolerance);
				/**
				 * @brief Static method to check if two direction differ by less than the tolerance
				 * @brief and if the movement of the node is the same as the one specified in directionFromRsu
				 */
				static bool CheckDirectionAndMovement(const double &nodeDirection, const VehicleDirection &directionFromRsu,
						const double &tolerance, const Vector2D& nodePosition, const Vector2D& rsuePosition);
				/**
				 * @brief Utility method to retrieve a header from a payload
				 */
				template<typename T>
				void GetHeader(server::Payload * payload, server::Position position, T & header) const
				{
					Header* tmp = payload->getHeader(position);
					header = dynamic_cast<T>(tmp);
					if (header == NULL)
						NS_FATAL_ERROR(LogNode() << "Wrong header type");
				}

				/// @brief Get SUMO simulation step length (in milliseconds) from server
				static int getSUMOStepLength();

				/************************
				 * iCS interaction methods
				 */
				/**
				 * @brief Used to send a Geobroadcast message.
				 * @param[in] dstType type of the destination
				 * @param[in] header Message to send
				 * @param[in] pid Protocol id
				 */
				void Send(NodeType dstType, Header* header, ProtocolId pid, const int messageCategory);

                /// @brief Let the node receive geobroadcast messages
                void startReceivingGeobroadcast(const int messageId);

                /**
                 * @brief Used to send a Unicast message.
                 * @param[in] destId Id of the destination node
                 * @param[in] header Message to send
                 * @param[in] pid Protocol id
                 * @param[in] messageCategory message category (used to filter received geobroadcast
                 *                            messages on iCS side), @see SubscriptionHelper::ReceiveGeobroadcast()
                 */
                void SendTo(int destId, Header* header, ProtocolId pid, const int messageCategory);

                /// @brief Let the node receive unicast messages
                void startReceivingUnicast();

                /**
                 * @brief Used to start sending CAM messages.
                 * @todo Revise. This uses a simplified implementation on the node's side,
                 *       although this declaration (which doesn't have an implementation)
                 *       suggests a more complex method.
                 * @param[in] destId Id of the destination node
                 * @param[in] header Message to send
                 * @param[in] pid Protocol id
                 * @param[in] messageCategory message category (used to filter received geobroadcast
                 *                            messages on iCS side), @see SubscriptionHelper::ReceiveGeobroadcast()
                 */
                void StartSendingCAMs(int destId, Header* header, ProtocolId pid, const int messageCategory);
                /// @brief Shortcut to address the simplified implementation.
                void StartSendingCAMs();

                /// @brief Let the node receive CAMs
                void startReceivingCAMs();

				/**
				 * @brief Called by the node class when the node has received the message from iCS
				 * @param[in] payload Message received
				 * @return true if the message was for the node. False if the message has been discarded
				 */
				bool Receive(server::Payload * payload);

                /**
                 * @brief Called by the node class when iCS asks the application to execute.
                 * @param[out] data Data to send back to iCS. The application has to fill the map
                 * @return Whatever the application executed. If true data will be sent to iCS. If false data is discarded
                 */
                bool Execute(DirectionValueMap &data);

				/**
				 * @brief Called by the node class when the reply from a sumo command is received.
				 * @param[in] executionId The id of the command.
				 * @param[in] storage The reply from traci.
				 */
				void TraciCommandResult(const int executionId, tcpip::Storage & traciReply);

                /// @brief schedule a traci command to be executed
                /// @param[in] cmdID traci command id
                /// @param[in] varID traci variable id
                /// @param[in] varTypeID traci type id (only for set commands)
                /// @param[in] value contents for a set-command, if default (=nullptr) is given, the command is treated as a get-command
                void AddTraciSubscription(const int cmdID, const int varID, const int varTypeID = 0, tcpip::Storage* value = 0);

                /// @brief helper function that adds GetMobilityInfo subscription
                void requestMobilityInfo();

                /// @brief helper function that adds TrafficLightInfo subscription
                void requestTrafficLightInfo();

                /// @brief schedule a traci command to be executed
                /// @param[in] objID ID for the object to be addressed
                /// @param[in] cmdID traci command id
                /// @param[in] varID traci variable id
                /// @param[in] varTypeID traci type id (only for set commands)
                /// @param[in] value contents for a set-command, if default (=nullptr) is given, the command is treated as a get-command
                void AddTraciSubscription(const std::string objID, const int cmdID, const int varID, const int varTypeID = 0, tcpip::Storage* value = 0);


                /// @brief Schedule a stop for a mobile node
                ///        see http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State
                void AddTraciStop(const std::string edgeID, const double endPos=1.,
                        const int laneIndex=0, const double duration=std::numeric_limits<double>::max(),
                        const int flags=0, const double startPos=std::numeric_limits<double>::min(),
                        const double until=-1);

                /// @brief Request a vehicle to open a gap
                ///        see http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State
                void commandTraciOpenGap(const double newTimeHeadway, const double newSpaceHeadway,
                        const double duration, const double changeRate, const double maxDecel, const std::string& referenceVehicleID = "");


                /// @brief request to update best lanes structure for vehicle
                void updateBestLanes();

                /// @brief set the vehicles sumo vehicleClass
                void setVehicleClass(std::string vClass);

                /// @brief Request a lane change
                ///        see http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State
                void commandTraciChangeLane(const std::string vehID, const int laneIndex, const double duration);

                /// @brief Sets the vehicle's lane change mode as a bitset.
                ///        see https://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State
                void commandTraciLaneChangeMode(const std::string vehID, const int lcm);

                /// @brief Get the driving distance to given edge and position
                ///        see http://sumo.dlr.de/wiki/TraCI/Vehicle_Value_Retrieval
                void commandTraciGetDrivingDistance(const std::string edgeID, const double pos, const int laneIndex = 0);

                /// @brief Set parameter for ToC model
                void SetTraciParameter(const std::string key, const std::string value, const std::string vehID = "");

                /// @brief Set vehicle color via TraCI
                void SetTraCIColor(const std::string& vehID, std::shared_ptr<libsumo::TraCIColor> color);

                /// @brief Request IDs of vehicles that entered the simulation
                void requestDepartedVehicles();
                /// @brief Request IDs of vehicles that left the simulation
                void requestArrivedVehicles();

                /// @brief Helper function for setting the required parameters for ToC
                void requestToC(const std::string vehID, const double timeTillMRM);


                /// @brief Add a subscription
                void AddSubscription(Subscription * sub);

                /// @brief OnAddSubscriptions is called at the begin of each simstep when the iCS asks for new subscriptions.
                ///        It calls the corresponding method for all installed behaviors. In particular, this allows to issue
                ///        a TraCI GET call, whose result will be delivered in the same simulation step when subscription results
                ///        are sent to the app.
                void OnAddSubscriptions();

                /**
                 * @brief Called by the node class when the node has received CAM messages
                 * @param[in] CAM Messages received
                 */
                void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);

				/**
				 * @brief Add a new behaviour to the node
				 */
				void SubscribeBehaviour(Behaviour* behaviour);
				/**
				 * @brief Utility method to get the node name. Used in the logs
				 */
				std::string LogNode() const;

			private:

				/**
				 * @name TraCI result processing
				 * @brief App-specific traci result processing goes into the behavior processors
				 */
				/// @{
                template<typename T> void processTraCIResult(const T result, const Command& command) {
					for (const auto& it: m_behaviours) {
						it.second->processTraCIResult(result, command);
					}
				}

                static std::shared_ptr<libsumo::TraCIColor> readColor(tcpip::Storage& inputStorage);
                ///@}

				/**
				 * @brief Common code called by Send and SendTo
				 */
				Header* DoSend(NodeType, Header*, ProtocolId, int, const int messageCategory);

				Node * m_node;
				NodeSampler * m_nodeSampler;
				NodeType m_nodeType;

				bool m_active;
				double m_direction_tolerance;

				// behaviours collection
				typedef std::map<TypeBehaviour, Behaviour*> BehaviourMap;
				BehaviourMap m_behaviours;

				TracedCallback<bool> m_traceStateChange;
				TracedCallback<server::Payload *> m_traceReceive;
				TracedCallback<server::Payload *> m_traceSend;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* CONTROLLER_H_ */
