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

#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include <memory>
#include "trace-manager.h"
#include "headers.h"
#include "fatal-error.h"
#include "structs.h"
#include "libsumo/TraCIDefs.h"

namespace baseapp
{
	namespace server
	{
		class Payload;
	}

	namespace application
	{

		class iCSInterface;
		class Node;
		struct Command;

        /// @brief Structure to hold the TraCI responses for all GET-commands
        /// @brief maps: objID -> CMD -> Response-object (time x value)
        /// @todo  Consider including the domain as top level. (Problem: it is not sent to the app currently)
		typedef std::map<std::string, std::map<int, std::pair <int, std::shared_ptr<libsumo::TraCIResult> > > > TraCIResponseMap;

		/**
		 * Abstract behaviour class
		 */
		class Behaviour: public TraceManager
		{
		    public:
                static uint16_t DefaultResponseTimeSpacing;
			public:
				Behaviour(iCSInterface* controller);
				virtual ~Behaviour();

				virtual bool IsActiveOnStart(void) const;
				bool IsRunning() const;
				/**
				 * @brief Contains the actions to be executed when the behavior starts
				 */
				virtual void Start();
				/**
				 * @brief Contains the actions to be executed when the behavior stops
				 */
				virtual void Stop();


				/** @brief OnAddSubscriptions is called at the begin of the app-interaction phase of the iCS simstep.
				 */
				virtual void OnAddSubscriptions();

				/**
				 * @brief If a message of the specified pid should be forwarded to the class
				 * @param[in] pid the ProtocolId of the message
				 * @return true if the class is interested messages of said pid. False otherwise
				 */
				virtual bool IsSubscribedTo(ProtocolId pid) const = 0;
				/**
				 * @brief Called by the ics-interface if a message is received by the node
				 * @brief and its pid is relevant to the behavior
				 * @param[in] payload The received message
				 * @param[in] snr The snr of the reception from ns3
				 */
				virtual void Receive(server::Payload *payload, double snr) = 0;

				/**
				 * @brief Called by ics-interface to get data to send back to iCS.
				 * @brief If the class does not returns data it has to return false.
				 * @param[out] data Data to send back to iCS. The application has to fill the map
				 * @return Whatever the application executed. If true data will be sent to iCS. If false data is discarded
				 */
				virtual bool Execute(DirectionValueMap &data);

                /**
                 * @brief Called by the ics-interface if a CAM message is received by the node
                 * @param[in] the CAM received message
                 */
				virtual void processCAMmessagesReceived(const int nodeID , const std::vector<CAMdata> & receivedCAMmessages);

				/// @brief Called whenever a GET-command result is returned from TraCI.
				///        Writes the according information into TraCIResponses
				/// @note if this is overridden, the call to Behaviour::processTraCIResult should be
				///       included in the derived method to store the response
				virtual void processTraCIResult(const int result, const Command& command);
				virtual void processTraCIResult(const double result, const Command& command);
				virtual void processTraCIResult(const std::string result, const Command& command);
				virtual void processTraCIResult(const std::vector<std::string> result, const Command& command);
		        void processTraCIResult(std::shared_ptr<libsumo::TraCIColor> color, const Command& command);
                void processTraCIResult(std::shared_ptr<libsumo::TraCILeaderDistance> leaderDist, const Command& command);

				virtual TypeBehaviour GetType() const
				{
					return Type();
				}

				static TypeBehaviour Type()
				{
					return TYPE_BEHAVIOUR;
				}

                /// @brief return time and result object for the last TraCI response received for the given object and variable
                /// returns noResponse if no entry exists in TraCIResponses
                static const std::pair<int, std::shared_ptr<libsumo::TraCIResult> >& GetLastTraCIResponse(std::string objID, int variableID);

			protected:
				virtual std::string Log() const;
				iCSInterface* GetController() const;

                /// @brief noResponse is returned by @getLastTraCIResponse if there is no entry for the requested
                /// object/command in TraCIResponses
                static std::pair<int, std::shared_ptr<libsumo::TraCIResult> > noResponse;

				bool m_enabled;

			private:
                /// @brief Stores TraCI response in TraCIResponses
                virtual void storeTraCIResult(const int time, const std::shared_ptr<libsumo::TraCIResult> result, const Command& command);

                iCSInterface* m_controller;
				bool m_running;

				// trace sources
				TracedCallback<bool> m_traceStartToggle;

                /// @brief Structure to hold the TraCI responses for all GET-commands
                /// @todo  Consider including the domain as top level. (Problem: it is not sent to the app currently)
                /// maps: objID -> CMD -> Response (time x value)
                static TraCIResponseMap TraCIResponses;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_H_ */
