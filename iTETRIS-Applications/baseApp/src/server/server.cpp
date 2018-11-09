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

#include "server.h"
#include "log/console.h"
#include "log/log.h"
#include "log/ToString.h"
#include "program-configuration.h"
#include "app-commands-subscriptions-constants.h"
#include "current-time.h"
#include <climits>
#include <sstream>
#include "scheduler.h"
#include <cstring>

namespace baseapp
{
	namespace server
	{

		using namespace std;
		using namespace tcpip;

		Server* Server::m_instance;
		bool Server::m_closeConnection;

		Server::Server(application::BehaviourFactory* factory)
		{
			try
			{
				m_closeConnection = false;
				m_currentTimeStep = INT_MIN;
				m_nodeHandler = new NodeHandler(factory);
				m_socket = new Socket(ProgramConfiguration::GetSocketPort());
				m_socket->accept();
				Console::Log("Server listening on port: ", ProgramConfiguration::GetSocketPort());
			} catch (SocketException &e)
			{
				Console::Error(e.what());
			}
		}

		Server::~Server()
		{
			delete m_socket;
			delete m_nodeHandler;
		}

		void Server::RunServer(application::BehaviourFactory* factory)
		{
			try
			{
				if (m_instance == NULL)
				{
					if (m_closeConnection)
						return;
					m_instance = new Server(factory);
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
				Console::Error("Invalid argument: ", e.what());

			} catch (SocketException &e)
			{
				Console::Error("Socket exception: ", e.what());
			}

			if (m_instance != NULL)
			{
				delete m_instance;
				m_instance = NULL;
				m_closeConnection = true;
			}
		}

		NodeHandler* Server::GetNodeHandler()
		{
			if (m_instance == NULL)
				return NULL;
			return m_instance->m_nodeHandler;
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
			if (commandId != CMD_APP_CLOSE)
				updateTimeStep(m_inputStorage.readInt());

			bool success = false;
			// dispatch commands
			switch (commandId)
			{
			case CMD_CREATE_MOBILE_NODE:
				success = createMobileNode();
				break;
			case CMD_ASK_FOR_SUBSCRIPTION:
				success = askForSubscription();
				break;
			case CMD_END_SUBSCRIPTION:
				success = endSubscription();
				break;
			case CMD_MOBILITY_INFORMATION:
				success = mobilityInformation();
				break;
			case CMD_TRAFFIC_LIGHT_INFORMATION:
				success = trafficLightInformation();
				break;
			case CMD_APP_MSG_RECEIVE:
				success = applicationMessageReceive();
				break;
			case CMD_APP_MSG_SEND:
			case CMD_APP_CMD_TRAFF_SIM:
				success = applicationConfirmSubscription(commandId);
				break;
			case CMD_NOTIFY_APP_EXECUTE:
				success = applicationExecute(m_currentTimeStep);
				break;
			case CMD_APP_CLOSE:
				m_closeConnection = true;
				writeStatusCmd(CMD_APP_CLOSE, APP_RTYPE_OK, "Closing");
				break;
			case CMD_SUMO_TRACI_COMMAND:
				success = sumoTraciCommand();
                break;
            case CMD_RECEIVED_CAM_INFO:
                success = getReceivedCAMinfo();
                break;

			default:
				writeStatusCmd(commandId, APP_RTYPE_NOTIMPLEMENTED, "Command not implemented");
			}
			if (!success)
			{
				while (m_inputStorage.valid_pos() && m_inputStorage.position() < commandStart + commandLength)
				{
					m_inputStorage.readChar();
				}
			}

			if (m_inputStorage.position() != commandStart + commandLength)
			{
				ostringstream msg;
				msg << "Wrong position in requestMessage after dispatching command.";
				msg << " Expected command length was " << commandLength;
				msg << " but " << m_inputStorage.position() - commandStart << " Bytes were read.";
				Console::Warning(msg.str());
			}

			return commandId;
		}

		void Server::updateTimeStep(int current)
		{
			if (m_currentTimeStep != current)
			{
				CurrentTime::m_currentTime = current;
				Log::WriteLog(ostringstream("#@# Current timestep " + toString(current)));
				NS_LOG_DEBUG("#@# Current timestep " << current);
				m_currentTimeStep = current;
				checkNodeToRemove();
				m_nodeHandler->updateTimeStep(current);
				application::Scheduler::Notify(current);
			}
		}

		void Server::writeStatusCmd(int commandId, int status, const std::string & description)
		{
			if (status == APP_RTYPE_ERR)
			{
				ostringstream msg;
				msg << "Answered with error to command " << commandId << " " << description;
				Console::Error(msg.str());
			} else if (status == APP_RTYPE_NOTIMPLEMENTED)
			{
				ostringstream msg;
				msg << "Requested command not implemented " << commandId << " " << description;
				Console::Error(msg.str());
			}

#if DEBUG
			if (status == APP_RTYPE_OK)
			{
				ostringstream msg;
				msg << "Debug command " << commandId << " " << description;
				Console::Log(msg.str());
			}
#endif

			description.length();
			// command length
			m_outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));
			// command type
			m_outputStorage.writeUnsignedByte(commandId);
			// status
			m_outputStorage.writeUnsignedByte(status);
			// description
			m_outputStorage.writeString(description);
		}

		bool Server::createMobileNode()
		{
			int nodeId = m_inputStorage.readInt();
			int ns3NodeId = m_inputStorage.readInt();
			std::string sumoNodeId = m_inputStorage.readString();
			std::string sumoType = m_inputStorage.readString();
			std::string sumoClass = m_inputStorage.readString();
			if (!m_nodeHandler->createMobileNode(nodeId, ns3NodeId, sumoNodeId, sumoType, sumoClass))
			{
				writeStatusCmd(CMD_CREATE_MOBILE_NODE, APP_RTYPE_ERR, "Node " + toString(nodeId) + " already exists");
				return false;
			}
			writeStatusCmd(CMD_CREATE_MOBILE_NODE, APP_RTYPE_OK, "Create node " + toString(nodeId));
			return true;
		}

		bool Server::askForSubscription()
		{
			int nodeId = m_inputStorage.readInt();
			int subscriptionId = m_inputStorage.readInt();

			writeStatusCmd(CMD_ASK_FOR_SUBSCRIPTION, APP_RTYPE_OK, "Ask For Subscription node " + toString(nodeId));
			Storage * subscription;
			if (m_nodeHandler->askForSubscription(nodeId, subscriptionId, subscription))
			{
				m_outputStorage.writeStorage(*subscription);
			} else
			{
				m_outputStorage.writeUnsignedByte(1 + 1 + 1);
				m_outputStorage.writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
				m_outputStorage.writeUnsignedByte(CMD_END_SUBSCRIPTION_REQUEST);
			}
			return true;
		}

		bool Server::endSubscription()
		{
			int nodeId = m_inputStorage.readInt();
			int subscriptionType = m_inputStorage.readUnsignedByte();
			int subscriptionId = m_inputStorage.readInt();
			bool drop = m_nodeHandler->endSubscription(nodeId, subscriptionId, subscriptionType);
			writeStatusCmd(CMD_END_SUBSCRIPTION, APP_RTYPE_OK,
					"Subscription " + toString(subscriptionId) + (drop ? " dropped" : " kept"));
			m_outputStorage.writeUnsignedByte(1 + 1 + 1);
			m_outputStorage.writeUnsignedByte(CMD_END_SUBSCRIPTION);
			m_outputStorage.writeUnsignedByte(drop ? CMD_DROP_SUBSCRIPTION : CMD_RENEW_SUBSCRIPTION);
			return true;
		}

		bool Server::mobilityInformation()
		{
			int nodeId = m_inputStorage.readInt();
			short numStations = m_inputStorage.readShort();
			vector<MobilityInfo*> stations;
			stations.reserve(numStations);
			ostringstream oss;
			oss << "Mobility info on nodes: ";
			for (short i = 0; i < numStations; ++i)
			{
				MobilityInfo * m = new MobilityInfo(m_inputStorage);
				stations.push_back(m);
				oss << m->id << " ";
			}
			Log::WriteLog(oss);
			int newStations = m_nodeHandler->mobilityInformation(nodeId, stations);
			writeStatusCmd(CMD_MOBILITY_INFORMATION, APP_RTYPE_OK,
					"CMD_MOBILITY_INFORMATION: Added " + toString(newStations) + " stations");
			return true;
		}

		bool Server::applicationMessageReceive()
		{
			vector<Message> messages;
			int numMessages = m_inputStorage.readInt();
			for (int i = 0; i < numMessages; ++i)
			{
				Message message;
				message.m_destinationId = m_inputStorage.readInt();
				message.m_messageId = m_inputStorage.readInt();
				message.m_extra = m_inputStorage.readString();

				short size = m_inputStorage.readShort();
				ostringstream log;
				if (size >= 14)
				{

					//RSSI TAG
					m_inputStorage.readUnsignedByte();
					short rssi = m_inputStorage.readShort();
					//TAG_SNR
					m_inputStorage.readUnsignedByte();
					double snr = m_inputStorage.readDouble();
					message.m_snr = snr;
					//TAG_TXPOWER
					m_inputStorage.readUnsignedByte();
					unsigned int power = m_inputStorage.readUnsignedByte();
					log << "[" << message.m_destinationId << "|" << message.m_messageId << "|" << message.m_extra << "] ";
					log << "RSSI= " << rssi << " SNR= " << snr << " POWER= " << power;
					int remaing = size - 14;
					if (size >= 19)
					{
						//TAG_MSGID
						m_inputStorage.readUnsignedByte();
						int messageId = m_inputStorage.readInt();
						log << " MSGID=" << messageId;
						remaing = size - 19;
					}
					log << " [";
					for (int tmp = 0; tmp < remaing; ++tmp)
					{
						log << (int) m_inputStorage.readChar() << ",";
					}
					log << "]";

				} else
				{
					log << "[" << message.m_destinationId << "|" << message.m_messageId << "|" << message.m_extra << "] ";
					log << "size= " << size << " [";
					for (int tmp = 0; tmp < size; tmp++)
					{
						log << (int) m_inputStorage.readChar() << ",";
					}
					log << "]";
				}
				messages.push_back(message);
				Log::WriteLog(log);
			}
			m_nodeHandler->applicationMessageReceive(messages);
			writeStatusCmd(CMD_APP_MSG_RECEIVE, APP_RTYPE_OK, "CMD_APP_MSG_RECEIVE");
			return true;
		}

		bool Server::applicationConfirmSubscription(int commandId)
		{
			int nodeId = m_inputStorage.readInt();
			bool status = m_inputStorage.readUnsignedByte() == 0 ? false : true;
			int subscriptionId = m_inputStorage.readInt();
			m_nodeHandler->ConfirmSubscription(nodeId, subscriptionId, status);
			writeStatusCmd(commandId, APP_RTYPE_OK,
					commandId == CMD_APP_MSG_SEND ? "CMD_APP_MSG_SEND" : "CMD_APP_CMD_TRAFF_SIM");
			return true;
		}

		bool Server::applicationExecute(const int currentTimeStep)
		{
			int nodeId = m_inputStorage.readInt();
			//Reset the last seen counter
			std::map<int, int>::iterator it = m_lastSeenNodes.find(nodeId);
			if (it == m_lastSeenNodes.end())
				m_lastSeenNodes.insert(std::make_pair(nodeId, 0));
			else
				it->second = 0;
			// create reply message
			writeStatusCmd(CMD_NOTIFY_APP_EXECUTE, APP_RTYPE_OK, "CMD_NOTIFY_APP_EXECUTE");
			DirectionValueMap data;
			if (m_nodeHandler->applicationExecute(nodeId, currentTimeStep, data))
			{
				Storage dataStorage;
				dataStorage.writeUnsignedByte(data.size());
				for (DirectionValueMap::const_iterator dirIt = data.begin(); dirIt != data.end(); ++dirIt)
				{
					dataStorage.writeString(dirIt->first);
					dataStorage.writeUnsignedByte(dirIt->second.size());
					for (ValueMap::const_iterator valueIt = dirIt->second.begin(); valueIt != dirIt->second.end(); ++valueIt)
					{
						dataStorage.writeUnsignedByte(valueIt->first);
						dataStorage.writeDouble(valueIt->second);
					}
				}
				//data
				Storage tmpStorage;
				tmpStorage.writeInt(1); //Num of result
				tmpStorage.writeUnsignedByte(TRAFFIC_INFO); //Command type
				tmpStorage.writeString("TrafficData"); //Tag
				tmpStorage.writeInt(dataStorage.size()); //size of data
				tmpStorage.writeStorage(dataStorage); //data

				m_outputStorage.writeInt(4 + 1 + 1 + tmpStorage.size());
				m_outputStorage.writeUnsignedByte(CMD_NOTIFY_APP_EXECUTE);
				m_outputStorage.writeUnsignedByte(APP_RESULT_ON);
				m_outputStorage.writeStorage(tmpStorage);
			} else
			{
				m_outputStorage.writeInt(4 + 1 + 1);
				m_outputStorage.writeUnsignedByte(CMD_NOTIFY_APP_EXECUTE);
				m_outputStorage.writeUnsignedByte(APP_RESULT_OFF);
			}
			return true;
		}

		bool Server::trafficLightInformation()
		{
			int nodeId = m_inputStorage.readInt();
			bool error = m_inputStorage.readUnsignedByte() == 0 ? false : true;
			std::vector<std::string> data = m_inputStorage.readStringList();
			m_nodeHandler->trafficLightInformation(nodeId, error, data);
			writeStatusCmd(CMD_TRAFFIC_LIGHT_INFORMATION, APP_RTYPE_OK, "CMD_TRAFFIC_LIGHT_INFORMATION");
			return true;
		}

		bool Server::sumoTraciCommand()
		{
			int nodeId = m_inputStorage.readInt();
			int subscriptionId = m_inputStorage.readInt();
			int executionId = m_inputStorage.readInt();
			Storage storage;
			while (m_inputStorage.valid_pos())
				storage.writeChar(m_inputStorage.readChar());
			m_nodeHandler->sumoTraciCommandResult(nodeId, executionId, storage);
			m_nodeHandler->setToUnsubscribe(nodeId, subscriptionId);
			writeStatusCmd(CMD_SUMO_TRACI_COMMAND, APP_RTYPE_OK, "CMD_SUMO_TRACI_COMMAND");
			return true;
		}

		void Server::checkNodeToRemove()
		{
			for (std::map<int, int>::iterator it = m_lastSeenNodes.begin(); it != m_lastSeenNodes.end();)
			{
				if (it->second >= MAX_NODE_TIMESTEP)
				{
					m_nodeHandler->deleteNode(it->first);
					ostringstream oss;
					oss << "Deleted node " << it->first;
					Log::WriteLog(oss);
					m_lastSeenNodes.erase(it++);
				} else
				{
					++(it->second);
					++it;
				}
			}
		}

        bool Server::getReceivedCAMinfo()
        {
            int nodeID = m_inputStorage.readInt();
            float posX = m_inputStorage.readFloat();
            float posY = m_inputStorage.readFloat();
            int numberCAMreceived = m_inputStorage.readInt();

            vector<CAMdata> receivedCAMmessages;

            for (int it=0; it<numberCAMreceived; it++){
                CAMdata receivedCAMmessage;
                receivedCAMmessage.senderID = m_inputStorage.readInt();
                double x = m_inputStorage.readFloat();
                double y = m_inputStorage.readFloat();
                receivedCAMmessage.position = application::Vector2D(x, y);
                receivedCAMmessage.generationTime = m_inputStorage.readInt();
                receivedCAMmessage.stationType = m_inputStorage.readInt();
                receivedCAMmessage.speed = m_inputStorage.readFloat();
                receivedCAMmessage.angle = m_inputStorage.readFloat();
                receivedCAMmessage.acceleration = m_inputStorage.readFloat();
                receivedCAMmessage.length = m_inputStorage.readFloat();
                receivedCAMmessage.width = m_inputStorage.readFloat();
                receivedCAMmessage.ligths = m_inputStorage.readInt();
                receivedCAMmessage.laneID = m_inputStorage.readString();
                receivedCAMmessage.edgeID = m_inputStorage.readString();
                receivedCAMmessage.junctionID = m_inputStorage.readString();
                receivedCAMmessages.push_back(receivedCAMmessage);
            }
            m_nodeHandler->processCAMmessagesReceived(nodeID, receivedCAMmessages);
            writeStatusCmd(CMD_RECEIVED_CAM_INFO, APP_RTYPE_OK, "CMD_RECEIVED_CAM_INFO");
            return true;
        }

	} /* namespace server */
} /* namespace protocol */
