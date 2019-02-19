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

#ifndef SERVER_H_
#define SERVER_H_

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "node-handler.h"

namespace baseapp
{
	namespace server
	{

		class Server
		{
			public:
				static void RunServer(application::BehaviourFactory* factory);
				static NodeHandler* GetNodeHandler();
				static int CurrentTimeStep();
				static int getSUMOStepLength();

			private:
				Server(application::BehaviourFactory* factory);
				virtual ~Server();

				int dispatchCommand();
				void updateTimeStep(int current);
				void checkNodeToRemove();
				void writeStatusCmd(int commandId, int status, const std::string & description);

				/// @brief Reads SUMO step length info from input storage.
				bool storeSUMOStepLength();

				bool createMobileNode();
				bool removeMobileNode();
				bool askForSubscription();
				bool endSubscription();
				//bool carsInZone();
				bool mobilityInformation();
				bool applicationMessageReceive();
				bool applicationConfirmSubscription(int commandId);
				bool applicationExecute();
				bool trafficLightInformation();
				bool sumoTraciCommand();
                bool getReceivedCAMinfo();

				//Don't need it
				//  bool commandTrafficSimulation();
				//  bool resultTrafficSimulation();
				//  bool xApplicationData();
				//  bool notifyApplicationMessageStatus();

			private:
				static Server* m_instance;
				static bool m_closeConnection;
				tcpip::Socket* m_socket;
				tcpip::Storage m_inputStorage;
				tcpip::Storage m_outputStorage;
				int m_currentTimeStep;
				NodeHandler * m_nodeHandler;
				std::map<int, int> m_lastSeenNodes;
				static const int MAX_NODE_TIMESTEP = 5;
				static int SUMO_STEPLENGTH;
		};

	} /* namespace server */
} /* namespace protocol */

#endif /* SERVER_H_ */
