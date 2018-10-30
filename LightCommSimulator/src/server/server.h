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
 *
 * Author Alejandro Correa Vila <acorrea@umh.es> Universidad Miguel Hernandez
 ***************************************************************************************/

#ifndef SERVER_H_
#define SERVER_H_

#include "tcpip/server-socket.h"
#include "tcpip/storage.h"
#include "scheduler.h"



namespace lightcomm
{
	namespace server
	{

		class Server
		{
			public:
				static void RunServer();
                static int CurrentTimeStep();


            private:
                Server();
				virtual ~Server();

				int dispatchCommand();

                void writeStatusCmd(int commandId, int status, std::string description);

                /**
                 * @brief In the Lightcomms there is are no task for the running sim step so this function only updates the timestep
                 */
                bool RunSimStep(int time);

                /**
                 * @brief Create a new node (vehicle or CIU) specifying its initial position
                 */
                bool CreateNode();

                /**
                 * @brief Create a new node (vehicle or CIU) specifying its initial position, speed, heading and laneId
                 */
                bool CreateNode2 ();

                /**
                 * @brief Update node's position
                 */
                bool UpdateNodePosition();

                /**
                 * @brief Update node's position, speed, heading and laneId
                 */
                bool UpdateNodePosition2 ();


                /**
                 * @brief Activate a node and all its communication modules, e.g. PHY layer
                 */
                bool ActivateNode (void);

                /**
                 * @brief Deactivate a node and all its communication modules, e.g. PHY layer
                 */
                bool DeactivateNode (void);

                /**
                 * @brief Start sending CAM in the nodes indicated in a list of nodes
                 */
                bool StartSendingCam();

                /**
                 * @brief Stop sending CAM in the nodes indicated in a list of nodes
                 */
                bool StopSendingCam();

                /**
                 * @brief Retrieve the messages that have been received by the simulated nodes
                 */
                bool GetReceivedMessages();

                /**
                     * @brief Retrieve all the messages that have been received by the all the simulated nodes
                */
                bool GetAllReceivedMessages();


                bool Close();
                int CalculateStringListByteSize(std::vector<std::string> list);

                bool StartTopoTxon (void);

                /**
                 * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
                 */
                bool StartIdBasedTxon (void);

                /**
                 * @brief Activate a transmision from the TMC based on a geographic area. This txon mode can be used to activate any kind of transmission supported by the Technology Selector according the ServiceID rules.
                */
                bool StartMWTxon (void);

                /**
                 * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.)
                 */
                bool StartIpCiuTxon (void);

                /**
                 * @brief Activate a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
                 */
                bool StartGeobroadcastTxon (void);

                /**
                 * @brief Activate a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
                 */
                bool StartGeoanycastTxon (void);

                /**
                 * @brief Deactivate the txon of a service running in a vehicle or a RSU.
                 */
                bool StopServiceTxon (void);

                /**
                 * @brief Deactivate the txon of a service running in IP-based base station or CIU.
                 */
                bool StopIpCiuServiceTxon (void);

                /**
                 * @brief Deactivate the MWtxon of a MW service running on a TMC.
                 */
                bool StopMWServiceTxon (void);




			private:
				static Server* m_instance;
				static bool m_closeConnection;


                tcpip::ServerSocket* m_socket;
				tcpip::Storage m_inputStorage;
				tcpip::Storage m_outputStorage;
				int m_currentTimeStep;
                event_id m_eventBroadcast;

                // replicate nodeID of NS-2 in the lightcomm simulator
                int nodeId = 0;

                struct Message
                {
                        Message()
                        {
                //			Can delete it even if I've never assigned it
                            packetTagContainer = NULL;
                        }
                        int senderId;
                        int messageId;
                        std::string messageType;
                        int timeStep;
                        int sequenceNumber;
                        //std::vector<unsigned char>* packetTagContainer;
                        tcpip::Storage* packetTagContainer;
                        float frequency;

                }typedef Message;


                struct NodeData {
                    float posX;
                    float posY;
                };

                // Table that stores all the received messages
                std::map<int, std::vector<Message> > m_GeneralReceivedMessageMap;

                // Table that stores the nodeId and the position of the node
                std::map<int, NodeData> m_NodeMap;

                // Table that stores the event_id of the CAM scheduled
                std::map<int, event_id> m_CAMeventIDMap;

                void ScheduleMessageTx(Message msg);
                std::vector<int> GetReceivers(int nodeId);
		};

	} /* namespace server */
} /* namespace lightcomm */

#endif /* SERVER_H_ */
