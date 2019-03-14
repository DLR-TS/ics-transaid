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
 * Author Leonhard Luecken <leonhard.luecken@dlr.de>
 *
  * Author Alejandro Correa Vila <acorrea@umh.es> Universidad Miguel Hernandez
 ***************************************************************************************/

#include "server.h"
#include "log/console.h"
#include "log/log.h"
#include "log/ToString.h"
#include "program-configuration.h"
#include "current-time.h"
#include <climits>
#include <sstream>
#include "scheduler.h"
#include <cstring>
#include "lightcomm-constants.h"
#include <math.h>




namespace lightcomm
{
	namespace server
	{

        using namespace std;
        using namespace tcpip;

		Server* Server::m_instance;
		bool Server::m_closeConnection;

        Server::Server() : nodeId(0)
		{
			try
			{

				m_closeConnection = false;
				m_currentTimeStep = INT_MIN;

                m_socket = new ServerSocket(ProgramConfiguration::GetSocketPort());
				m_socket->accept();
            } catch (SocketException &e)
			{

			}
		}

		Server::~Server()
		{
			delete m_socket;
		}

		void Server::RunServer()
		{
			try
			{
				if (m_instance == NULL)
				{
					if (m_closeConnection)
						return;
					m_instance = new Server();
				}

				while (!m_closeConnection)
				{
					if (!m_instance->m_inputStorage.valid_pos())
					{
						if (m_instance->m_outputStorage.size() > 0)
						{
							// send out all answers as one storage
							m_instance->m_socket->sendExact(m_instance->m_outputStorage);
						}
						m_instance->m_inputStorage.reset();
						m_instance->m_outputStorage.reset();
						// Read a message
						m_instance->m_socket->receiveExact(m_instance->m_inputStorage);
					}
					while (m_instance->m_inputStorage.valid_pos() && !m_closeConnection)
					{
						// dispatch each command
						m_instance->dispatchCommand();
					}
				}
				if (m_closeConnection && m_instance->m_outputStorage.size() > 0)
				{
					// send out all answers as one storage
					m_instance->m_socket->sendExact(m_instance->m_outputStorage);
				}
			} catch (std::invalid_argument & e)
			{


			} catch (SocketException &e)
			{

			}

			if (m_instance != NULL)
			{
				delete m_instance;
				m_instance = NULL;
				m_closeConnection = true;
			}
		}


		int Server::CurrentTimeStep()
		{
			return CurrentTime::Now();
		}

		int Server::dispatchCommand()
		{
			int commandStart = m_inputStorage.position();
			int commandLength = m_inputStorage.readInt();

			int commandId = m_inputStorage.readUnsignedByte();
            bool success = false;

            ostringstream log;
            // dispatch commands
            switch (commandId)
            {
            case CMD_SIMSTEP:

                success = RunSimStep(m_inputStorage.readInt());
                return commandId;
            case CMD_UPDATENODE:

                success = UpdateNodePosition();
                break;
            case CMD_UPDATENODE2:

                success = UpdateNodePosition2();
                break;
            case CMD_CREATENODE:

                success = CreateNode();
                break;
            case CMD_CREATENODE2:

                success = CreateNode2();
                break;
            case CMD_START_CAM:

                success = StartSendingCam();
                break;
            case CMD_STOP_CAM:

                success = StopSendingCam();
                break;
            case CMD_GET_RECEIVED_MESSAGES:

                success = GetReceivedMessages();
                break;
            case CMD_GET_ALL_RECEIVED_MESSAGES:

                success = GetAllReceivedMessages();
                break;
            case CMD_CLOSE:

                success = Close();
                break;
            case CMD_START_TOPO_TXON:

                success = StartTopoTxon();
                break;
            case CMD_START_ID_BASED_TXON:

                success = StartIdBasedTxon();
                break;
            case CMD_START_MW_TXON:

                success = StartMWTxon();
                break;
            case CMD_START_IPCIU_TXON:

                success = StartIpCiuTxon();
                break;
            case CMD_STOP_SERVICE_TXON:

                success = StopServiceTxon();
                break;
            case CMD_STOP_MW_SERVICE_TXON:

                success = StopMWServiceTxon();
                break;
            case CMD_STOP_IPCIU_SERVICE_TXON:

                success = StopIpCiuServiceTxon();
                break;
            case CMD_START_GEO_BROAD_TXON:

                success = StartGeobroadcastTxon();
                break;
            case CMD_START_GEO_ANY_TXON:

                success = StartGeoanycastTxon();
                break;
            case CMD_ACTIVATE_NODE:

                success = ActivateNode();
                break;
            case CMD_DEACTIVATE_NODE:

                success = DeactivateNode();
                break;
            default:
                writeStatusCmd(commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in ns3");
            }

            if (!success)
            {
                while (m_inputStorage.valid_pos() && m_inputStorage.position() < (unsigned int) (commandStart + commandLength))
                {
                    m_inputStorage.readChar();
                }
            }

            if (m_inputStorage.position() != (unsigned int) (commandStart + commandLength))
            {
                ostringstream msg;
                msg << "Wrong position in requestMessage after dispatching command.";
                msg << " Expected command length was " << commandLength;
                msg << " but " << m_inputStorage.position() - commandStart << " Bytes were read.";
                writeStatusCmd(commandId, RTYPE_ERR, msg.str());
                m_closeConnection = true;
            }


			return commandId;
		}


        void Server::writeStatusCmd(int commandId, int status, std::string description)
        {


            m_outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));
            // command type
            m_outputStorage.writeUnsignedByte(commandId);
            // status
            m_outputStorage.writeUnsignedByte(status);
            // description
            m_outputStorage.writeString(description);

            return;
        }

        bool Server::RunSimStep(int time)
        {
            if (m_currentTimeStep != time)
            {
                CurrentTime::m_currentTime = time;
                m_currentTimeStep = time;
                lightcomm::Scheduler::Notify(time);
            }
            writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "RunSimStep()");
            return true;
        }

        bool Server::CreateNode(void)
        {

            NodeData nodeData;
            nodeData.posX=m_inputStorage.readFloat();
            nodeData.posY=m_inputStorage.readFloat();
            nodeData.type = "RSU";

            m_inputStorage.readStringList(); // read list of Comm modules

            nodeId++;

            m_NodeMap.operator [](nodeId)  = nodeData;

            writeStatusCmd(CMD_CREATENODE, RTYPE_OK, "CreateNode()");
            m_outputStorage.writeUnsignedByte(1 + 1 + 4);
            m_outputStorage.writeUnsignedByte(CMD_CREATENODE);
            m_outputStorage.writeInt(nodeId);

            return true;
        }

        bool Server::CreateNode2(void)
        {
            NodeData nodeData;
            nodeData.posX=m_inputStorage.readFloat();
            nodeData.posY=m_inputStorage.readFloat();
            nodeData.type = "Vehicle";

            m_inputStorage.readFloat(); // read speed
            m_inputStorage.readFloat(); // read heading
            m_inputStorage.readString(); // read laneId
            m_inputStorage.readStringList(); // read list of Comm modules

            nodeId++;

            m_NodeMap.operator [](nodeId)  = nodeData;

            writeStatusCmd(CMD_CREATENODE2, RTYPE_OK, "CreateNode2()");
            m_outputStorage.writeUnsignedByte(1 + 1 + 4);
            m_outputStorage.writeUnsignedByte(CMD_CREATENODE2);
            m_outputStorage.writeInt(nodeId);

            return true;
        }


        bool Server::UpdateNodePosition()
        {
            int nodeId = m_inputStorage.readInt();
            NodeData nodeData;
            nodeData.posX=m_inputStorage.readFloat();
            nodeData.posY=m_inputStorage.readFloat();

            m_NodeMap.operator [](nodeId)  = nodeData;

            writeStatusCmd(CMD_UPDATENODE, RTYPE_OK, "UpdateNodePosition()");

            return true;
        }

        bool Server::UpdateNodePosition2()
        {
            int nodeId = m_inputStorage.readInt();
            NodeData nodeData;
            nodeData.posX=m_inputStorage.readFloat();
            nodeData.posY=m_inputStorage.readFloat();

            m_inputStorage.readFloat(); // read speed
            m_inputStorage.readFloat(); // read heading
            m_inputStorage.readString(); // read laneId

            m_NodeMap.operator [](nodeId)  = nodeData;

            writeStatusCmd(CMD_UPDATENODE2, RTYPE_OK, "UpdateNodePosition2()");

            return true;
        }

        bool Server::ActivateNode(void)
        {
            int number = m_inputStorage.readInt();
            for (int i = 0; i < number; ++i)
            {
                int nodeId = m_inputStorage.readInt();

            }
            writeStatusCmd(CMD_ACTIVATE_NODE, RTYPE_OK, "ActivateNode()");
            return true;
        }

        bool Server::DeactivateNode(void)
        {
            int number = m_inputStorage.readInt();
            for (int i = 0; i < number; ++i)
            {
                int nodeId = m_inputStorage.readInt();

            }
            writeStatusCmd(CMD_DEACTIVATE_NODE, RTYPE_OK, "DeactivateNode()");
            return true;
        }

        bool Server::StartSendingCam()
        {

            std::vector<std::string> senderIdCollection = m_inputStorage.readStringList();
            int payloadLength = m_inputStorage.readInt();
            float frequency = m_inputStorage.readFloat();


            //TODO add id to messages
            int messageId = 1;

            std::vector<std::string>::iterator senderIt;
            for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
            {

                std::string cadena = *senderIt;

                stringstream temp;
                int nodeId;
                temp << cadena;
                temp >> nodeId;

                Server::Message msg;

                msg.senderId = nodeId;
                msg.messageId = messageId;
                msg.messageType = "CAM";
                msg.frequency = frequency;



                ScheduleMessageTx(msg);


            }

            writeStatusCmd(CMD_START_CAM, RTYPE_OK, "StartSendingCam()");

            return true;
        }


        bool Server::StopSendingCam()
        {
            std::vector<std::string> senderIdCollection = m_inputStorage.readStringList();

            std::vector<std::string>::iterator senderIt;
            for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
            {
                std::string cadena = *senderIt;

                stringstream temp;
                int nodeId;
                temp << cadena;
                temp >> nodeId;

                Scheduler::Cancel(m_CAMeventIDMap.operator [](nodeId));
            }

            writeStatusCmd(CMD_STOP_CAM, RTYPE_OK, "StopSendingCam()");

            return true;
        }




        bool Server::GetReceivedMessages()
        {

            return true;
        }


        bool Server::GetAllReceivedMessages()
        {
            tcpip::Storage messageStorage;
            int numNodesWithMessages = 0;



            std::map<int, std::vector<Server::Message> >::iterator rx_it;
            for (rx_it = m_GeneralReceivedMessageMap.begin(); rx_it != m_GeneralReceivedMessageMap.end(); rx_it++)
            {

                int numMessages = rx_it->second.size();
                ++numNodesWithMessages;
                messageStorage.writeInt(rx_it->first);
                messageStorage.writeInt(numMessages);

                std::vector<Message>* receivedMessages = &rx_it->second;
                for (std::vector<Server::Message>::iterator receivedIterator = receivedMessages->begin();
                        receivedIterator != receivedMessages->end(); ++receivedIterator)
                {


                    Server::Message receivedMessage = *receivedIterator;

                    messageStorage.writeInt(receivedMessage.senderId);

                    messageStorage.writeInt(receivedMessage.messageId);

                    messageStorage.writeString(receivedMessage.messageType);

                    messageStorage.writeInt(receivedMessage.timeStep);

                    messageStorage.writeInt(receivedMessage.sequenceNumber);

                    receivedMessage.packetTagContainer =  new tcpip::Storage();

                    messageStorage.writeShort( receivedMessage.packetTagContainer->size());

                    messageStorage.writeStorage(*receivedMessage.packetTagContainer);

                    delete receivedMessage.packetTagContainer;
                }

            }

            writeStatusCmd(CMD_GET_ALL_RECEIVED_MESSAGES, RTYPE_OK, "GetReceivedMessages()");
            m_outputStorage.writeInt(4 + 1 + 4 + messageStorage.size());
            m_outputStorage.writeUnsignedByte(CMD_GET_ALL_RECEIVED_MESSAGES);
            m_outputStorage.writeInt(numNodesWithMessages);
            m_outputStorage.writeStorage(messageStorage);

            return true;
        }

        bool Server::Close()
        {
            m_closeConnection = true;
            writeStatusCmd(CMD_CLOSE, RTYPE_OK, "Close()");
            return true;
        }


        int Server::CalculateStringListByteSize(std::vector<string> list)
        {

            if (list.size() == 0)
                return 0;

            // 4 bytes for the length of the string list
            // 4 bytes for the length each member of the list
            int stringSize = 4 + (list.size() * 4);

            int totalAmountOfCharacters = 0;
            vector<string>::iterator it;
            for (it = list.begin(); it < list.end(); it++)
            {
                string chain = *it;
                size_t chainSize = chain.length();
                totalAmountOfCharacters += chainSize;
            }

            // Finally one byte per character
            stringSize = stringSize + totalAmountOfCharacters;

            return stringSize;
        }


        void Server::ScheduleMessageTx(Message msg){

            msg.timeStep = CurrentTimeStep();

            std::vector<int> receivers;
            receivers = GetReceivers(msg.senderId);


            for (vector<int>::iterator receivedIterator = receivers.begin(); receivedIterator != receivers.end(); ++receivedIterator)
            {
                int node = *receivedIterator;
                m_GeneralReceivedMessageMap.operator[](node).push_back(msg);

            }

            if (msg.frequency>0 && msg.messageType=="CAM") // Before was set to O for the CAM transmission
            {
                double nextTime = 1/msg.frequency;
                m_CAMeventIDMap.operator [](msg.senderId)  = Scheduler::Schedule(nextTime, &Server::ScheduleMessageTx, this, msg);
            }

        }


        std::vector<int> Server::GetReceivers(int nodeId){

            std::vector<int> receivers;

            NodeData txData;
            txData = m_NodeMap[nodeId];

            float range;
            if (txData.type == "RSU"){
			            range = 50000; // increased to assure all message from RSU are received
            } else{
            	range = 50000; // increased to assure all messages are received
            }

			float distance;

            for (std::map<int, NodeData>::iterator nodeIt = m_NodeMap.begin(); nodeIt != m_NodeMap.end(); ++nodeIt)
            {

            	if (nodeIt->first != nodeId){

					distance = sqrt( pow(txData.posX - nodeIt->second.posX,2) + pow(txData.posY - nodeIt->second.posY,2) );

					if (distance <= range){
						receivers.push_back(nodeIt->first);
					}
            	}
            }

            return receivers;
        }


        bool Server::StartTopoTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StartIdBasedTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StartMWTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StartIpCiuTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StopServiceTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StopMWServiceTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StopIpCiuServiceTxon(void) // ToDo
        {

            return true;
        }

        bool Server::StartGeobroadcastTxon(void) // ToDo
        {

        	std::vector<std::string> senderIdCollection = m_inputStorage.readStringList();
        	std::string serviceId = m_inputStorage.readString();
        	int commProfile  = m_inputStorage.readUnsignedByte();
        	std::vector<std::string> technologies = m_inputStorage.readStringList();
        	double time = m_inputStorage.readDouble();
    		int lat = m_inputStorage.readInt();
    		int lon = m_inputStorage.readInt();
    		int areaSize = m_inputStorage.readInt();
    		float frequency = m_inputStorage.readFloat();
    		int payloadLength = m_inputStorage.readInt();
    		float msgRegenerationTime = m_inputStorage.readFloat();
    		int msgLifetime = m_inputStorage.readInt();
    		int messageId = m_inputStorage.readInt();

    		std::vector<unsigned char> packetTagContainer;


    		short container_l = m_inputStorage.readShort();
    		if (container_l > 0){
    			for (int i = 0; i < container_l; i++)
    			{
    				packetTagContainer.push_back(m_inputStorage.readChar());
    			}
    		}
            std::vector<std::string>::iterator senderIt;
            for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
            {

                std::string cadena = *senderIt;

                std::cout << "Sender: " << cadena << " of " << senderIdCollection.size() << " with frequency " << frequency << std::endl;

                stringstream temp;
                int nodeId;
                temp << cadena;
                temp >> nodeId;

                Server::Message msg;

                msg.senderId = nodeId;
                msg.messageId = messageId;
                msg.messageType = "serviceIdGeobroadcast";
                msg.frequency = frequency;
                //msg.packetTagContainer->writePacket(packetTagContainer);
                msg.sequenceNumber = 0; // Todo update this if frequency >1

                ScheduleMessageTx(msg);
            }

    		writeStatusCmd(CMD_START_GEO_BROAD_TXON, RTYPE_OK, "StartGeobroadcastTxon ()");
    		return true;
        }

        bool Server::StartGeoanycastTxon(void) // ToDo
        {

            return true;
        }





	} /* namespace server */
} /* namespace lightcomm */
