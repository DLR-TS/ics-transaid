/****************************************************************************/
/// @file    app-message-manager.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright �� 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>

#include "app-message-manager.h"
#include "subscription.h"
#include "subs-return-cars-zone.h"
#include "subs-start-travel-time-calculation.h"
#include "subs-stop-travel-time-calculation.h"
#include "subs-get-received-cam-info.h"
#include "subs-calculate-travel-time.h"
#include "subs-set-cam-area.h"
#include "subs-get-facilities-info.h"
#include "subs-app-message-send.h"
#include "subs-app-message-receive.h"
#include "subs-app-result-traff-sim.h"
#include "subs-app-cmd-traff-sim.h"
#include "subs-x-application-data.h"
#include "subs-sumo-traci-command.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "app-result-container.h"
#include "../sync-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"
#include "subs-get-mobility-info.h"
#include "subs-get-traffic-light-info.h"
#include "subs-app-control-traci.h"

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include "../../config.h"
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;

namespace ics
{

	AppMessageManager::AppMessageManager(SyncManager* syncManager)
	{
		this->m_syncManager = syncManager;
	}

// ===========================================================================
// member method definitions
// ===========================================================================
	bool AppMessageManager::Connect(string host, int port)
	{
		m_socket = new Socket(host, port);
		m_socket->set_verbose(false);

		bool connected = false;
		for (int i = 0; i < 10; ++i)
		{
			try
			{
				cout << "iCS --> Trying " << i << " to connect Application on port " << port << "..." << endl;
				m_socket->connect();
				return true;
			} catch (exception& e)
			{
				cout << "iCS --> No connection to Application; waiting..." << endl;
				Sleep(3);
			}
		}

		return false;
	}

	bool AppMessageManager::Close()
	{
		if (m_socket != NULL)
		{
			m_socket->close();
		}

		return true;
	}

	int AppMessageManager::CommandClose()
	{
		Storage outMsg;
		Storage inMsg;
		std::stringstream msg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: no connection to server" << endl;
			return EXIT_FAILURE;
		}

		// command length
		outMsg.writeInt(4 + 1);
		// command id
		outMsg.writeUnsignedByte(CMD_APP_CLOSE);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> Error while sending command: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving command: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// validate result state
		if (!ReportResultState(inMsg, CMD_APP_CLOSE))
		{
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool AppMessageManager::CommandCreateVehicleNodeApplication(VehicleNode *node)
	{
		tcpip::Storage outMsg;
		tcpip::Storage tmpMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return false;
		}

		if (node == NULL)
		{
			cout << "iCS --> #Error while sending command: subscriptions is NULL" << endl;
			return false;
		}

		//Ics id
		tmpMsg.writeInt(node->m_icsId);
		//ns3 id
		tmpMsg.writeInt(node->m_nsId);
		//sumo id
		tmpMsg.writeString(node->m_tsId);
		//sumo type
		tmpMsg.writeString(node->m_SumoType);
		//sumo class
		tmpMsg.writeString(node->m_SumoClass);

		// command length
		outMsg.writeInt(4 + 1 + 4 + tmpMsg.size());
		// command id
		outMsg.writeUnsignedByte(CMD_CREATE_MOBILE_NODE);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);

		outMsg.writeStorage(tmpMsg);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_CREATE_MOBILE_NODE))
		{
			cout << "iCS --> #Error after ReportResultState" << endl;
			return false;
		}
		return true;
	}

	bool AppMessageManager::CommandGetNewSubscriptions(int nodeId, int appId, vector<Subscription*> *subscriptions,
			bool &noMoreSubs)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		// command length
		outMsg.writeInt(4 + 1 + 4 + 4 + 4);
		// command id
		outMsg.writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// node identifier
		outMsg.writeInt(nodeId);
		// subscription will have this id
		outMsg.writeInt(Subscription::m_subscriptionCounter + 1);

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_ASK_FOR_SUBSCRIPTION))
		{
			cout << "iCS --> #Error CommandGetNewSubscriptions: ReportResultState" << endl;
			return false;
		}

		int cmdLength;
		int cmdStart;
		int subscriptionCode;
		try
		{
			cmdStart = inMsg.position();
			cmdLength = inMsg.readUnsignedByte();
			if (inMsg.readUnsignedByte() != CMD_ASK_FOR_SUBSCRIPTION)
			{
				cout << "iCS --> #Error CommandGetNewSubscriptions: Not CMD_ASK_FOR_SUBSCRIPTION" << endl;
				return false;
			}

			subscriptionCode = inMsg.readUnsignedByte();

#ifdef LOG_ON
			stringstream log;
			log << "[INFO] AppMessageManager::CommandGetNewSubscriptions() subscription code is " << IcsLog::toHex(subscriptionCode,2);
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

			switch (subscriptionCode)
			{
			case SUB_RETURNS_CARS_IN_ZONE:
			{
				float baseX = inMsg.readFloat();
				float baseY = inMsg.readFloat();
				float radius = inMsg.readFloat();
#ifdef LOG_ON
				stringstream log;
				log << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Return Cars In Zone " << baseX
						<< " " << baseY << " " << radius;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				SubsReturnsCarInZone *subscription = new SubsReturnsCarInZone(appId, nodeId, baseX, baseY, radius);
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_SET_CAM_AREA:
			{
				float baseX = inMsg.readFloat();
				float baseY = inMsg.readFloat();
				float radius = inMsg.readFloat();
				float frequency = inMsg.readFloat();
				int infoType = inMsg.readUnsignedByte();
#ifdef LOG_ON
				stringstream log;
				log << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Set CAM Area " << baseX << " "
						<< baseY << " " << radius;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				SubsSetCamArea *subscription = new SubsSetCamArea(appId, nodeId, baseX, baseY, radius, frequency, infoType);
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_TRAVEL_TIME_ESTIMATION_START:
			{
				inMsg.readUnsignedByte(); // status (to be removed)
				inMsg.readInt(); // id of rsu
				float x, y, frequency, msgRegenerationTime;
				vector<Point2D> vertex = vector<Point2D>();
				x = inMsg.readFloat(); // point 1
				y = inMsg.readFloat();
				Point2D point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 2
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 3
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 4
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				frequency = inMsg.readFloat();
				msgRegenerationTime = inMsg.readFloat(); // message regeneration time
				int msgLifeTime = inMsg.readInt(); // message life time in seconds
				SubsStartTravelTimeCalculation* subscription = new SubsStartTravelTimeCalculation(appId, nodeId, vertex,
						frequency, msgRegenerationTime, msgLifeTime);
				subscriptions->push_back(subscription);
				noMoreSubs = false;
				break;
			}
			case SUB_TRAVEL_TIME_ESTIMATION_END:
			{
				inMsg.readUnsignedByte(); // status (to be removed)
				inMsg.readInt(); // id of rsu
				float x, y, frequency, msgRegenerationTime;
				vector<Point2D> vertex = vector<Point2D>();
				x = inMsg.readFloat(); // point 1
				y = inMsg.readFloat();
				Point2D point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 2
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 3
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				x = inMsg.readFloat(); // point 4
				y = inMsg.readFloat();
				point = Point2D(x, y);
				vertex.push_back(point);
				frequency = inMsg.readFloat();
				msgRegenerationTime = inMsg.readFloat(); // message regeneration time
				int msgLifeTime = inMsg.readInt(); // message life time in seconds
				SubsStopTravelTimeCalculation* subscription = new SubsStopTravelTimeCalculation(appId, nodeId, vertex,
						frequency, msgRegenerationTime, msgLifeTime);
				subscriptions->push_back(subscription);
				noMoreSubs = false;
				break;
			}
			case SUB_TRAVEL_TIME_ESTIMATION:
			{
				inMsg.readInt(); // vehicle ID
				SubsCalculateTravelTime* subscription = new SubsCalculateTravelTime(appId, nodeId);
				subscriptions->push_back(subscription);
				noMoreSubs = false;
				break;
			}
			case SUB_RECEIVED_CAM_INFO:
			{

#ifdef LOG_ON
                stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId
						<< " subscribed to get information about the received CAM messages.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				SubsGetReceivedCamInfo *subscription = new SubsGetReceivedCamInfo(appId, nodeId);
				subscriptions->push_back(subscription);
#ifdef LOG_ON
				log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_FACILITIES_INFORMATION:
			{

#ifdef LOG_ON
                stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get facilities information.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					unsigned char type = inMsg.readChar();
					packet[i - 3] = type;
				}
				SubsGetFacilitiesInfo *subscription = new SubsGetFacilitiesInfo(appId, nodeId, packet, cmdLength - 3); //packet);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				noMoreSubs = false;
				break;
			}
			case SUB_APP_MSG_SEND:
			{

#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to send an Application message.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsAppMessageSend *subscription = new SubsAppMessageSend(appId, nodeId, m_syncManager, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				noMoreSubs = false;
				break;
			}
			case SUB_APP_MSG_RECEIVE:
			{

#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to receive an Application message.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsAppMessageReceive *subscription = new SubsAppMessageReceive(appId, nodeId, m_syncManager, packet,
						cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_APP_CMD_TRAFF_SIM:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId
						<< " subscribed to send an Application CMD message to Traffic Simulator.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsAppCmdTraffSim *subscription = new SubsAppCmdTraffSim(appId, nodeId, m_syncManager, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				noMoreSubs = false;
				break;
			}
			case SUB_APP_RESULT_TRAFF_SIM:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId
						<< " subscribed to send an Application Result Message to Traffic Simulator.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsAppResultTraffSim *subscription = new SubsAppResultTraffSim(appId, nodeId, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_X_APPLICATION_DATA:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get cross-application data.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsXApplicationData *subscription = new SubsXApplicationData(appId, nodeId, m_syncManager, packet,
						cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_MOBILITY_INFORMATION:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get mobility information";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsGetMobilityInfo *subscription = new SubsGetMobilityInfo(appId, nodeId, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			case SUB_TRAFFIC_LIGHT_INFORMATION:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get traffic light information";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsGetTrafficLightInfo *subscription = new SubsGetTrafficLightInfo(appId, nodeId, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
            //New Generic TRACI subscription - direct TRACI subscription, no local Ics interpretation (23/04/2016)
			case SUB_CONTROL_TRACI:
			{

#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to control TraCI";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer

				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}

				SubsAppControlTraci *subscription = new SubsAppControlTraci(appId, nodeId, m_syncManager, packet, cmdLength - 3);

				delete[] packet;
				// JHNote (04/04/2015) - adding the subscription to the table makes the SUMO requests asynchronous to the application (it cannot get immediatly the results)
				//                       but we can also immediatly reply with the results (SUMO replies immediatly) so that the application can also have immediate reply.
				// JHNote (09/04/2015) - not useful to immediatly reply. Better simply let it come at the third stage with all other subscription results, for now...
				subscriptions->push_back(subscription);

				//  subscription->InformApp(this);


#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
			      break;
			    }

			case CMD_END_SUBSCRIPTION_REQUEST:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() No more requests from node " << nodeId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				noMoreSubs = true;
				break;
			}

			case SUB_SUMO_TRACI_COMMAND:
			{
#ifdef LOG_ON
				stringstream log;
				log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to execute a sumo command";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				// read the buffer
				unsigned char* packet = new unsigned char[cmdLength - 3];
				for (unsigned int i = 3; i < cmdLength; i++)
				{
					packet[i - 3] = inMsg.readChar();
				}
				SubsSumoTraciCommand *subscription = new SubsSumoTraciCommand(appId, nodeId, packet, cmdLength - 3);
				delete[] packet;
				subscriptions->push_back(subscription);
#ifdef LOG_ON
                log.clear();
                log << " New subscription ID is " << subscription->m_id << ".";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                noMoreSubs = false;
				break;
			}
			default:
			{
				cout << "App --> iCS #Error: No such subscription" << endl;
				noMoreSubs = true;
			}
			}
		} catch (std::invalid_argument e)
		{
			cout << "App --> iCS #Error: an exception was thrown while reading result state message. (Subscription code: "
					<< subscriptionCode << ")" << endl;
			cout << e.what() << endl;
            noMoreSubs = true;
			return false;
		}

		return true;
	}

	bool AppMessageManager::CommandUnsubscribe(int nodeId, vector<Subscription*>* subscriptions, bool &noMoreUnSubs)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		if (subscriptions == NULL)
		{
			cout << "iCS --> #Error while sending command: subscriptions is NULL" << endl;
			;
			return false;
		}

		// command length
		outMsg.writeInt(4 + 1 + 4 + 4);
		// command id
		outMsg.writeUnsignedByte(CMD_END_SUBSCRIPTION);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// node identifier
		outMsg.writeInt(nodeId);

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			noMoreUnSubs = true;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_END_SUBSCRIPTION))
		{
			cout << "iCS --> #Error after ReportResultState" << endl;
			noMoreUnSubs = true;
			return false;
		}
		if (!ValidateUnsubscriptions(inMsg, subscriptions, noMoreUnSubs))
		{
			noMoreUnSubs = true;
			return false;
		}
		return true;
	}

	int AppMessageManager::CommandUnsubscribe(int nodeId, Subscription* subscription)
	{

		if (subscription == NULL)
		{
			cerr << "iCS --> [ERROR] CommandUnsubscribe() node " << nodeId << " - subscription NULL!" << endl;
			return -1;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return -1;
		}

		if (subscription == NULL)
		{
			cout << "iCS --> #Error while sending command: subscriptions is NULL" << endl;
			;
			return -1;
		}

#ifdef LOG_ON
    stringstream log;
    log << "AppMessageManager::CommandUnsubcribe - ### Subscription id " << subscription->m_id;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		// command length
		outMsg.writeInt(4 + 1 + 4 + 4 + 1 + 4);
		// command id
		outMsg.writeUnsignedByte(CMD_END_SUBSCRIPTION);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// node identifier
		outMsg.writeInt(nodeId);
		// subscription to be maintained or dropped
		const type_info& typeinfo = typeid(*subscription);
		if (typeinfo == typeid(SubsSetCamArea))
			outMsg.writeUnsignedByte(SUB_SET_CAM_AREA);
		else if (typeinfo == typeid(SubsReturnsCarInZone))
			outMsg.writeUnsignedByte(SUB_RETURNS_CARS_IN_ZONE);
		else if (typeinfo == typeid(SubsGetReceivedCamInfo))
			outMsg.writeUnsignedByte(SUB_RECEIVED_CAM_INFO);
		else if (typeinfo == typeid(SubsGetFacilitiesInfo))
			outMsg.writeUnsignedByte(SUB_FACILITIES_INFORMATION);
		else if (typeinfo == typeid(SubsXApplicationData))
			outMsg.writeUnsignedByte(SUB_X_APPLICATION_DATA);
		else if (typeinfo == typeid(SubsAppMessageSend))
			outMsg.writeUnsignedByte(SUB_APP_MSG_SEND);
		else if (typeinfo == typeid(SubsAppMessageReceive))
			outMsg.writeUnsignedByte(SUB_APP_MSG_RECEIVE);
		else if (typeinfo == typeid(SubsAppCmdTraffSim))
			outMsg.writeUnsignedByte(SUB_APP_CMD_TRAFF_SIM);
		else if (typeinfo == typeid(SubsAppResultTraffSim))
			outMsg.writeUnsignedByte(SUB_APP_RESULT_TRAFF_SIM);
		else if (typeinfo == typeid(SubsGetMobilityInfo))
			outMsg.writeUnsignedByte(SUB_MOBILITY_INFORMATION);
		else if (typeinfo == typeid(SubsGetTrafficLightInfo))
			outMsg.writeUnsignedByte(SUB_TRAFFIC_LIGHT_INFORMATION);
		else if (typeinfo == typeid(SubsSumoTraciCommand))
			outMsg.writeUnsignedByte(SUB_SUMO_TRACI_COMMAND);
		else if (typeinfo == typeid(SubsAppControlTraci))
		      outMsg.writeUnsignedByte(SUB_CONTROL_TRACI);

		// the id of the subscription
		outMsg.writeInt(subscription->m_id);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return -1;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return -1;
		}

		// check out the status of the primitive
		if (!ReportResultState(inMsg, CMD_END_SUBSCRIPTION))
		{
			return -1;
		}

		// check out the subscription status requested by the app
		return ValidateUnsubscriptions(inMsg);
	}

	bool AppMessageManager::CommandSendSubscriptionCarsInZone(vector<VehicleNode*>* carsInZone, int nodeId, int m_id)
	{
		if (carsInZone == NULL)
		{
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		int numberOfCars = (int) carsInZone->size();
		int numberOfCarsSize = numberOfCars * (4 + 4 + 4 + 4 + 4 + 4);

		// command length
		outMsg.writeInt(4 + 1 + 4 + 4 + 4 + 4 + numberOfCarsSize);
		// command id
		outMsg.writeUnsignedByte(CMD_CARS_IN_ZONE);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// subscribed node id
		outMsg.writeInt(nodeId);
		// subscribed subscriptionId
		outMsg.writeInt(m_id); 
		// number of cars
		outMsg.writeInt(numberOfCars);

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		for (vector<VehicleNode*>::iterator  nodeIt = carsInZone->begin(); nodeIt < carsInZone->end(); nodeIt++)
		{
			VehicleNode* node = *nodeIt;
			//car identifier
			outMsg.writeInt(node->m_icsId);
			//car position x
			outMsg.writeFloat(node->GetPositionX());
			//car position y
			outMsg.writeFloat(node->GetPositionY());
			//car speed
			outMsg.writeFloat(node->GetSpeed());
			//car direction
			outMsg.writeFloat(node->GetDirection());
			//car acceleration
			outMsg.writeFloat(node->GetAcceleration());
#ifdef LOG_ON
			stringstream log;
			log << "[INFO] CommandSendSubscriptionCarsInZone() Node Id " << node->m_icsId << " X: " << node->GetPositionX()
					<< " Y: " << node->GetPositionY() << " Speed: " << node->GetSpeed() << " Direction: " << node->GetDirection()
					<< " Acceleration: " << node->GetAcceleration();
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_CARS_IN_ZONE))
			return false;

		return true;
	}

	int AppMessageManager::CommandSendSubcriptionCalculateTravelTimeFlags(int nodeId, int startStation, int stopStation)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		// command length
		outMsg.writeInt(1 + 1 + 4 + 4 + 4 + 4);
		// command id
		outMsg.writeUnsignedByte(CMD_TRAVEL_TIME_ESTIMATION);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// subscribed node id
		outMsg.writeInt(nodeId);
		// start station
		outMsg.writeInt(startStation);
		// stop station
		outMsg.writeInt(stopStation);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

#ifdef LOG_ON
		stringstream log;
		log << "Start and stop station info: " << startStation << " | " << stopStation;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_TRAVEL_TIME_ESTIMATION))
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	bool AppMessageManager::CommandSendSubscriptionReceivedCamInfo(vector<TCamInformation>* camInfo, int nodeId)
	{
		if (camInfo == NULL)
		{
			return false;
		}

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		// Compute the information length to be sent
		int numberOfSendersSize = 0;
		vector<TCamInformation>::iterator it;
		for (it = camInfo->begin(); it < camInfo->end(); it++)
		{
			numberOfSendersSize += it->camInfoBuffSize;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 4 /*nodeX*/
		+ 4 /*nodeY*/
		+ 4 /*NumCAMs*/
		+ numberOfSendersSize;

		outMsg.writeInt(totalLengthPacket);                 // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_RECEIVED_CAM_INFO);    // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);            // bytes: 5-8
		// subscribed node id
		outMsg.writeInt(nodeId);                            // bytes: 9-12
		// node's position when receiving
		Point2D pos = SyncManager::m_facilitiesManager->getStationPosition(nodeId);
		outMsg.writeFloat(pos.x());                         // bytes: 13-16
		outMsg.writeFloat(pos.y());                         // bytes: 17-20
		// number of cams
		outMsg.writeInt((int) camInfo->size());              // bytes: 21-24

		for (it = camInfo->begin(); it < camInfo->end(); it++)
		{
			TCamInformation currCamInfo = *it;
			// General basic CAM profile
			outMsg.writeInt(currCamInfo.senderID);             // sender identifier
			outMsg.writeFloat(currCamInfo.senderPosition.x()); // sender x position
			outMsg.writeFloat(currCamInfo.senderPosition.y()); // sender y position
			outMsg.writeInt(currCamInfo.generationTime);   // message generationTime
			outMsg.writeInt((int) currCamInfo.staType);        // station Type
			// Vehicle CAM profile
			outMsg.writeFloat(currCamInfo.speed);              // speed
			outMsg.writeFloat(currCamInfo.angle);              // angle
			outMsg.writeFloat(currCamInfo.acceleration);       // acceleration
			outMsg.writeFloat(currCamInfo.length);             // vehicle length
			outMsg.writeFloat(currCamInfo.width);              // vehicle width
			outMsg.writeInt(currCamInfo.lights);          // vehicle exterior lights
			// Location Referencing information
			outMsg.writeString(currCamInfo.laneID);            // laneID
			outMsg.writeString(currCamInfo.edgeID);            // edgeID
			outMsg.writeString(currCamInfo.junctionID);        // junctionID
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_RECEIVED_CAM_INFO))
			return false;

		return true;
	}

	bool AppMessageManager::CommandSendSubscriptionFacilitiesInfo(tcpip::Storage *facInfo, int nodeId)
	{
		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 1 /* size indication of facInfo */
		+ facInfo->size();

#ifdef LOG_ON
        stringstream log;
        log << "AppMesagerManager::CommandSendSubscriptionFacilitiesInfo() - facInfo size is" << facInfo->size() << endl;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_FACILITIES_INFORMATION);            // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
		// subscribed node id
		outMsg.writeInt(nodeId);                                      // bytes: 9-12
		// facilities information (expressed according to the Type-Length-Value syntax)
		outMsg.writeUnsignedByte(facInfo->size());
		if (facInfo->size() > 0)
		{
			outMsg.writeStorage(*facInfo); // bytes: 13-(13+facInfo.size())
		}
		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			cout << "iCS --> #Error sent " << totalLengthPacket << " bytes to application" << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_FACILITIES_INFORMATION))
			return false;

		return true;
	}

	int AppMessageManager::NotifyMessageStatus(int nodeId, vector<pair<int, stationID_t> >& receivedMessages)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		cout << "iCS --> AppMessageMananger: in NotifyMessageStatus..." << endl;

		int messagesSize = (receivedMessages.size() * 4) * 2;

		// command length
		outMsg.writeInt(4 + 1 + 4 + 4 + 4 + messagesSize);
		// command id
		outMsg.writeUnsignedByte(CMD_NOTIFY_APP_MESSAGE_STATUS);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// the node in which the applications is running
		outMsg.writeInt(nodeId);
		// number of received messages
		outMsg.writeInt(receivedMessages.size());
		int index = 0;
		for (vector<pair<int, stationID_t> >::iterator it = receivedMessages.begin(); it != receivedMessages.end(); ++it)
		{
			int messageId = (*it).first;
			int receiverId = (*it).second;
			outMsg.writeInt(messageId);
			outMsg.writeInt(receiverId);
			index++;

#ifdef LOG_ON
			stringstream log;
			log << "[INFO] NotifyMessageStatus() Receiver Id: " << receiverId << " Message Id: " << messageId
					<< " sent as ARRIVED to the App";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_NOTIFY_APP_MESSAGE_STATUS))
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	bool AppMessageManager::CommandSendSubscriptionAppMessageSend(bool status, int nodeId, int subscriptionId)
	{
		return SendStatus(CMD_APP_MSG_SEND, status, nodeId, subscriptionId);
	}

	bool AppMessageManager::SendStatus(int command, bool status, int nodeId, int subscriptionId)
	{
		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return false;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 1 /*scheduling status*/
		+ 4;/*subscription id*/

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(command);                      // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
		// subscribed node id
		outMsg.writeInt(nodeId);                                      // bytes: 9-12
		// scheduling status information (0x00 = NOT SCHEDULED, 0x01 = SCHEDULED)
		outMsg.writeUnsignedByte(status);                               // bytes: 13
		// the subscription id
		outMsg.writeInt(subscriptionId);
		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, command))
			return false;

		return true;
	}

	bool AppMessageManager::CommandSendSubscriptionAppMessageReceive(
			std::vector<std::pair<Message, stationID_t> >& msgInfo)
	{

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;
		tcpip::Storage tmpMsg;

		int rcvMsg = 0;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		for (std::vector<std::pair<Message, stationID_t> >::iterator it = msgInfo.begin(); it != msgInfo.end(); ++it)
		{

			if ((*it).second == 0)
			{
				cout << "Node id 0" << endl;
				continue;
			}
			rcvMsg++;
			// subscribed node id
			tmpMsg.writeInt((*it).second);                            // bytes: 9-12
			// the received application message ID
			tmpMsg.writeInt((*it).first.appMessageId);
			string extra = SyncManager::m_facilitiesManager->getReceivedMessagePayload(it->first.actionId)->m_extra;
			tmpMsg.writeString(extra);
			// JHNote: additional data from ns-3 as a generic TagContainer: (such as RSSI and SNR)
			tmpMsg.writeShort((short) (*it).first.packetTagContainer->size());
			tmpMsg.writePacket((*it).first.packetTagContainer->data(), (*it).first.packetTagContainer->size());

			delete it->first.packetTagContainer;

		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /* number of Rcv. Messages */
		+ tmpMsg.size(); /*contains sequences of nodeId and appMsgId enhanced with a generic TagContainer */

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_APP_MSG_RECEIVE);                   // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8

		// number of messages
		outMsg.writeInt(rcvMsg);

		// the tmp storage
		outMsg.writeStorage(tmpMsg);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_APP_MSG_RECEIVE))
		{
			cout << "iCS --> #Error while checking response state from Application: " << endl;
			//return false;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool AppMessageManager::CommandSendSubscriptionAppResultTraffSim(vector<unsigned char>& tsInfo, int nodeId,
			int subscriptionId)
	{
		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 4 // subscriptionId
				+ tsInfo.size();

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_APP_RESULT_TRAFF_SIM);              // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
		// subscribed node id
		outMsg.writeInt(nodeId);                       	               // bytes: 9-12
		// id of the subscription
		outMsg.writeInt(subscriptionId);                                // bytes: 13-16
		// facilities information (expressed according to the Type-Length-Value syntax)
		outMsg.writePacket(tsInfo); // bytes: 17-(17+tsInfo.size())

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_APP_RESULT_TRAFF_SIM))
			return false;

		return true;
	}

	bool AppMessageManager::CommandSendSubscriptionXApplicationData(vector<unsigned char>& xAppData, int nodeId,
			int subscriptionId)
	{
		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
			return false;
		}

		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			;
			return false;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 4 //subscriptionId
				+ xAppData.size();

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_X_APPLICATION_DATA);                // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
		// subscribed node id
		outMsg.writeInt(nodeId);                                      // bytes: 9-12
		// subscription id
		outMsg.writeInt(subscriptionId);
		// facilities information (expressed according to the Type-Length-Value syntax)
		outMsg.writePacket(xAppData); // bytes: 13-(13+tsInfo.size())

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_X_APPLICATION_DATA))
			return false;

		return true;
	}

	bool AppMessageManager::CommandSendSubscriptionAppCmdTraffSim(bool status, int nodeId, int subscriptionId)
	{
		return SendStatus(CMD_APP_CMD_TRAFF_SIM, status, nodeId, subscriptionId);
	}

	bool AppMessageManager::CommandApplicationToExecute(int nodeId, ResultContainer* resultContainer)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		// command length
		outMsg.writeInt(4 + 1 + 4 + 4); // Added by Ramon Bauza 29-09-10
		// command id
		outMsg.writeUnsignedByte(CMD_NOTIFY_APP_EXECUTE);
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);
		// the node in which the applications is running
		outMsg.writeInt(nodeId);

		if (nodeId == 0)
		{
			cout << "Node id 0" << endl;
		}

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return false;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);

		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return false;
		}

		if (!ReportResultState(inMsg, CMD_NOTIFY_APP_EXECUTE))
			return false;

		if (!resultContainer->ProcessResult(inMsg))
		{
			cout << "iCS --> #Error processing the result." << endl;
			return false;
		}

		return true;
	}

	int AppMessageManager::CommandSendSubscriptionMobilityInfo(std::vector<TMobileStationDynamicInfo> * information,
			int nodeId)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;
		tcpip::Storage tmpMsg;

		int messNum = 0;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		for (std::vector<TMobileStationDynamicInfo>::const_iterator it = information->begin(); it != information->end();
				++it)
		{
			messNum++;
			// subscribed node id. I used the timeStep field
			tmpMsg.writeInt(it->timeStep);                            // bytes: 9-12
            tmpMsg.writeInt(m_syncManager->GetNodeByIcsId(it->timeStep)->m_nsId);  // The ns3 ID for transparent subscriptions from APP
            tmpMsg.writeString(m_syncManager->GetNodeByIcsId(it->timeStep)->m_tsId); // The SUMO ID for transparent subscriptions from APP

			tmpMsg.writeFloat(it->positionX);
			tmpMsg.writeFloat(it->positionY);
#ifdef LOG_ON
            stringstream log;
            log<<"[CommandSendSubscriptionMobilityInfo] time step"<<SyncManager::m_simStep<<"nodeID:"<<m_syncManager->GetNodeByIcsId(it->timeStep)->m_tsId<<" position x"<<it->positionX<<":y:"<<it->positionY;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			tmpMsg.writeUnsignedByte(it->exteriorLights ? 1 : 0);
			if (it->exteriorLights)
			{
				tmpMsg.writeFloat(it->speed);
				tmpMsg.writeFloat(it->direction);
				tmpMsg.writeFloat(it->acceleration);
				tmpMsg.writeString(it->lane);
			}
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 2 /* number of messages */
		+ tmpMsg.size(); /*contains sequences of nodeId and appMsgId enhanced with a generic TagContainer */

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_MOBILITY_INFORMATION);                   // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8

		outMsg.writeInt(nodeId);
		// number of messages
		outMsg.writeShort(messNum);

		// the tmp storage
		outMsg.writeStorage(tmpMsg);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_MOBILITY_INFORMATION))
		{
			cout << "iCS --> #Error while checking response state from Application: " << endl;
			//return false;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}


	int AppMessageManager::CommandSendSubscriptionControlTraCI(int m_id, tcpip::Storage& proxyMsg, int nodeId)
	{
	  tcpip::Storage outMsg;
	  tcpip::Storage inMsg;

	  int messNum = 0;

	  if (m_socket == NULL)
	  {
	    cout << "iCS --> #Error while sending command: Socket is off" << endl;
	    return EXIT_FAILURE;
	  }


	  // command length
	  int totalLengthPacket = 4 /*length*/
	  + 1 /*commandId*/
	  + 4 /*timeStep*/
	  + 4 /*nodeId*/
	  + 4 /*subscriptionId*/
	  + proxyMsg.size(); /*contains data from TraCI, following a TraCI encoding */

	  outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
	  // command id
	  outMsg.writeUnsignedByte(CMD_CONTROL_TRACI);                   // bytes: 4   // RET_CONTROL_TRACI
	  // simulation timestep
	  outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
	  // the node that called for TraCI
	  outMsg.writeInt(nodeId);
	  outMsg.writeInt(m_id);
	  // the tmp storage (returned data from TraCI, without any interpretation
	  outMsg.writeStorage(proxyMsg);

	  // send request message
	  try
	  {
	    m_socket->sendExact(outMsg);
	  } catch (SocketException e)
	  {
	    cout << "iCS --> #Error while sending immediate command to Application: " << e.what() << endl;
	    return EXIT_FAILURE;
	  }

	  // receive answer message
	  try
	  {
	    m_socket->receiveExact(inMsg);
	  } catch (SocketException e)
	  {
	    cout << "iCS --> #Error while receiving immediate response from Application: " << e.what() << endl;
	    return EXIT_FAILURE;
	  }

	  if (!ReportResultState(inMsg, CMD_CONTROL_TRACI))
	  {
	    cout << "iCS --> #Error while checking response state from Application: " << endl;
	    return EXIT_FAILURE;
	  }

	  return EXIT_SUCCESS;
	}



	int AppMessageManager::CommandSendSubscriptionTrafficLightInfo(std::vector<std::string> & data, int nodeId,
			bool error)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;
		tcpip::Storage tmpMsg;

		if (m_socket == NULL)
		{
			cout << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		tmpMsg.writeStringList(data);

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 1 /* error */
		+ tmpMsg.size(); /*contains sequences of nodeId and appMsgId enhanced with a generic TagContainer */

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_TRAFFIC_LIGHT_INFORMATION);                   // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8

		outMsg.writeInt(nodeId);
		// error
		outMsg.writeUnsignedByte(error ? 1 : 0);

		// the tmp storage
		outMsg.writeStorage(tmpMsg);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_TRAFFIC_LIGHT_INFORMATION))
		{
			cout << "iCS --> #Error while checking response state from Application: " << endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool AppMessageManager::ReportResultState(tcpip::Storage& inMsg, int command)
	{
		int cmdLength;
		int cmdId;
		int resultType;
		int cmdStart;
		std::string msg;

		try
		{
			cmdStart = inMsg.position();
			cmdLength = inMsg.readUnsignedByte();
			cmdId = inMsg.readUnsignedByte();
			if (cmdId != command)
			{
				cout << "App --> iCS #Error: received status response to command: " << cmdId << " but expected: " << command
						<< endl;
				return false;
			}
			resultType = inMsg.readUnsignedByte();
			msg = inMsg.readString();
		} catch (std::invalid_argument e)
		{
			cout << "App --> iCS #Error: an exception was thrown while reading result state message. (command code: "
					<< command << ")" << endl;
			return false;
		}

		switch (resultType)
		{
		case APP_RTYPE_ERR:
		{
			cout << ".. APP answered with error to command (" << cmdId << "), [description: " << msg << "]" << endl;
			return false;
		}
		case APP_RTYPE_NOTIMPLEMENTED:
			cout << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << endl;
			return false;
		case APP_RTYPE_OK:
        {
#ifdef LOG_ON
            stringstream log;
            log << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			break;
        }
        default:
			cout << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId << "), [description: "
					<< msg << "]" << endl;
			return false;
		}

		if ((cmdStart + cmdLength) != inMsg.position())
		{
			cout << "App --> iCS #Error: command at position " << cmdStart << " has wrong length" << endl;
			return false;
		}
		return true;
	}

	bool AppMessageManager::ValidateUnsubscriptions(tcpip::Storage& inMsg, vector<Subscription*>* subscriptions,
			bool &noMoreUnSubs)
	{
		if (subscriptions == NULL)
		{
			cout << "iCS --> #Error while sending command: subsToDrop is NULL" << endl;
			return false;
		}

		int cmdId;
		int cmdLength;
		int cmdStart;

		while (inMsg.valid_pos())
		{
			cmdStart = inMsg.position();
			cmdLength = inMsg.readUnsignedByte();
			cmdId = inMsg.readUnsignedByte();

			if (cmdId != CMD_END_SUBSCRIPTION)
			{
				cout << "iCS --> #Error: received response with command id: " << cmdId << " but expected: "
						<< (int) CMD_END_SUBSCRIPTION << endl;
				return false;
			}
			try
			{
				int subscription = inMsg.readUnsignedByte();

				switch (subscription)
				{
				case SUB_RETURNS_CARS_IN_ZONE:
				{
					float baseX = inMsg.readFloat();
					float baseY = inMsg.readFloat();
					float radius = inMsg.readFloat();
					SubsReturnsCarInZone::Delete(baseX, baseY, radius, subscriptions);
#ifdef LOG_ON
                    stringstream log;
                    log << "iCS --> Unsubscribed to SUB_RETURNS_CARS_IN_ZONE with " << baseX << " " << baseY << " " << radius;
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
					noMoreUnSubs = true;
					break;
				}
				case SUB_SET_CAM_AREA:
				{

					float baseX = inMsg.readFloat();
					float baseY = inMsg.readFloat();
					float radius = inMsg.readFloat();
					float frequency = inMsg.readFloat();
					SubsSetCamArea::Delete(baseX, baseY, radius, frequency, subscriptions);
#ifdef LOG_ON
                    stringstream log;
                    log << "iCS --> Unsubscribed to SUB_SET_CAM_AREA with " << baseX << " " << baseY << " " << radius << " "<< frequency;
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
					break;
				}
				case SUB_TRAVEL_TIME_ESTIMATION_START:
				{
					inMsg.readUnsignedByte(); //status (to be removed)
					break;
				}
				case SUB_TRAVEL_TIME_ESTIMATION_END:
				{
					inMsg.readUnsignedByte(); // status (to be removed)
					break;
				}
				case SUB_APP_MSG_RECEIVE:
				{
                    //commented as stationID not defined here TODO: get it
                    //SubsAppMessageReceive::Delete(stationID, subscriptions);
					break;
				}
				case SUB_FACILITIES_INFORMATION:
				{
                    //commented as stationID not defined here TODO: get it
                    //SubsGetFacilitiesInfo::Delete(stationID, subscriptions);
					break;
				}
				case SUB_APP_MSG_SEND:
				{
                    //commented as stationID not defined here TODO: get it
                    //SubsAppMessageSend::Delete(stationID, subscriptions);
					break;
				}
				case SUB_APP_RESULT_TRAFF_SIM:
				{
                    //commented as stationID not defined here TODO: get it
                    //SubsAppResultTraffSim::Delete(stationID, subscriptions);
					break;
				}
				case SUB_APP_CMD_TRAFF_SIM:
				{
                    //commented as stationID not defined here TODO: get it
                    //SubsAppCmdTraffSim::Delete(stationID, subscriptions);
					break;
				}
				case CMD_END_SUBSCRIPTION_REQUEST:
				{
					noMoreUnSubs = true;
					int stringLength = inMsg.readInt(); // !!! why?
					break;
				}
				default:
					cout << "iCS --> #Error: received unknown position format" << endl;
					return false;
				}
			} catch (std::invalid_argument e)
			{
                cout<< "#Error while reading message:" << e.what() << endl;
				return false;
			}
		}

		return true;
	}

	int AppMessageManager::ValidateUnsubscriptions(Storage& inMsg)
	{
		int cmdId;
		int cmdLength;
		int cmdStart;

		while (inMsg.valid_pos())
		{
			cmdStart = inMsg.position();
			cmdLength = inMsg.readUnsignedByte();
			cmdId = inMsg.readUnsignedByte();
			if (cmdId != CMD_END_SUBSCRIPTION)
			{
				cout << "iCS --> #Error: received response with command id: " << cmdId << " but expected: "
						<< (int) CMD_END_SUBSCRIPTION << endl;
				return -1;
			}
			try
			{
				int subscriptionStatus = inMsg.readUnsignedByte();

				if (subscriptionStatus == CMD_DROP_SUBSCRIPTION)
				{
					return 1;
				}

				if (subscriptionStatus == CMD_RENEW_SUBSCRIPTION)
				{
					return 0;
				}

				return -1;

			} catch (std::invalid_argument e)
			{
                cout << "#Error while reading message:" << e.what() << endl;
				return -1;
			}
		}
	}

	int AppMessageManager::CommandSendSubscriptionSumoTraciCommand(const int nodeId, const int subscriptionId,
			const int executionId, tcpip::Storage & result)
	{
		tcpip::Storage outMsg;
		tcpip::Storage inMsg;

		if (m_socket == NULL)
		{
			cerr << "iCS --> #Error while sending command: Socket is off" << endl;
			return EXIT_FAILURE;
		}

		// command length
		int totalLengthPacket = 4 /*length*/
		+ 1 /*commandId*/
		+ 4 /*timeStep*/
		+ 4 /*nodeId*/
		+ 4 /*subscriptionId*/
		+ 4 /*executionId*/
		+ result.size(); /*contains sequences of nodeId and appMsgId enhanced with a generic TagContainer */

		outMsg.writeInt(totalLengthPacket);                            // bytes: 0-3
		// command id
		outMsg.writeUnsignedByte(CMD_SUMO_TRACI_COMMAND);                   // bytes: 4
		// simulation timestep
		outMsg.writeInt(SyncManager::m_simStep);                       // bytes: 5-8
		outMsg.writeInt(nodeId);
		outMsg.writeInt(subscriptionId);
		outMsg.writeInt(executionId);

		// the result storage
		outMsg.writeStorage(result);

		// send request message
		try
		{
			m_socket->sendExact(outMsg);
		} catch (SocketException e)
		{
			cerr << "iCS --> #Error while sending command to Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		// receive answer message
		try
		{
			m_socket->receiveExact(inMsg);
		} catch (SocketException e)
		{
			cerr << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
			return EXIT_FAILURE;
		}

		if (!ReportResultState(inMsg, CMD_SUMO_TRACI_COMMAND))
		{
			cerr << "iCS --> #Error while checking response state from Application: " << endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
}
