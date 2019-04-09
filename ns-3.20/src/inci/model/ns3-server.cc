/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          CBT
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Julen Maneros <jmaneros@cbt.es>, Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <Fatma.Hrizi@eurecom.fr>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#include "ns3-server.h"
#include "ns3-commands.h"
#include "ns3-comm-constants.h"
#include "message-schedule.h"
#include "ns3/channel-tag.h"
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include "ns3/log.h"
#include <string>
#include "ns3/config.h"
#include "ns3/iTETRIS-Results.h"



using namespace std;
using namespace tcpip;

NS_LOG_COMPONENT_DEFINE ("Ns3Server");

namespace ns3
{

	Ns3Server* Ns3Server::instance_ = 0;
	bool Ns3Server::closeConnection_ = false;
	int Ns3Server::targetTime_ = 0;
	ofstream Ns3Server::myfile;
    ofstream Ns3Server::outfileLogPacketsRx;
    ofstream Ns3Server::outfileLogPacketsTx;
    ofstream Ns3Server::outfileLogNAR;
    ofstream Ns3Server::outfileLogNIR;
	string Ns3Server::CAM_TYPE = "0";
	string Ns3Server::DNEM_TYPE = "1";
	bool Ns3Server::logActive_ = false;

	Ns3Server::Ns3Server(int port, iTETRISNodeManager *node_manager, PacketManager *packetManager)
	{
		myDoingSimStep = false;
		closeConnection_ = false;
		my_nodeManagerPtr = node_manager;
		my_packetManagerPtr = packetManager;

		// Log results TransAID
        outfileLogPacketsRx.open("ReceivedPackets.txt"); // Added by A Correa
        outfileLogPacketsTx.open("TransmittedPackets.txt" ); // Added by A Correa
        outfileLogNAR.open("NAR.txt" ); // Added by A Correa
        outfileLogNIR.open("NIR.txt" ); // Added by A Correa

        my_resultsManager = new iTETRISResults(); // Added by A Correa
        //

		try
		{
			socket_ = new ServerSocket(port);
			socket_->accept();
		} catch (SocketException &e)
		{
			cerr << "ns-3 server --> #Error while creating socket: " << e.what() << endl;
		}
	}

	void Ns3Server::processCommandsUntilSimStep(int port, string logfile, iTETRISNodeManager *node_manager,
			PacketManager *packetManager)
	{

		if (logfile.empty())
		{
			cout << "[ns3] WARNING - Log file name not specified!" << endl;
			logActive_ = false;
		} else
		{
			myfile.open(logfile.c_str());
			logActive_ = true;
		}


		try
		{
			if (instance_ == 0)
			{
				if (!closeConnection_)
				{
					instance_ = new ns3::Ns3Server(port, node_manager, packetManager);
				} else
				{
					return;
				}
			}

			// Simulation should run until
			// 1. end time reached or
			// 2. got CMD_CLOSE or
			// 3. Client closes socket connection
			if (instance_->myDoingSimStep)
			{
				instance_->myDoingSimStep = false;
			}

			while (!closeConnection_)
			{

				if (!instance_->myInputStorage.valid_pos())
				{
					if (instance_->myOutputStorage.size() > 0)
					{
						// send out all answers as one storage
						instance_->socket_->sendExact(instance_->myOutputStorage);
					}
					instance_->myInputStorage.reset();
					instance_->myOutputStorage.reset();
					// Read a message
					instance_->socket_->receiveExact(instance_->myInputStorage);
				}

				while (instance_->myInputStorage.valid_pos() && !closeConnection_)
				{
					// dispatch each command
					if (instance_->dispatchCommand() == CMD_SIMSTEP)
					{
						instance_->myDoingSimStep = true;
					}
				}
			}
			if (closeConnection_ && instance_->myOutputStorage.size() > 0)
			{
				// send out all answers as one storage
				instance_->socket_->sendExact(instance_->myOutputStorage);
			}
		} catch (std::invalid_argument &e)
		{
			cerr << "ns-3 server --> Invalid argument in ns3-server.cc, processCommandsUntilSimStep() " << e.what() << endl;

			stringstream log;
			log << "[processCommandsUntilSimStep] [ERROR] Invalid argument: " << e.what();
			Log((log.str()).c_str());

		} catch (SocketException &e)
		{
			cerr << "ns-3 server --> Socket exception in ns3-server.cc, processCommandsUntilSimStep() " << e.what() << endl;

			stringstream log;
			log << "[processCommandsUntilSimStep] [ERROR] Socket exception: " << e.what();
			Log((log.str()).c_str());

		}

		cout << "ns-3 server --> Finishing server.... " << endl;

		if (instance_ != NULL)
		{
			delete instance_;
			instance_ = 0;
			closeConnection_ = true;
		}

		if (logActive_)
			myfile.close();


        // Close log files //Added by A Correa
        outfileLogPacketsTx.close();
        outfileLogPacketsRx.close();
        outfileLogNAR.close();
        outfileLogNIR.close();
        //

    }

	int Ns3Server::dispatchCommand()
	{        
		int commandStart = myInputStorage.position();
		int commandLength = myInputStorage.readInt();
		int commandId = myInputStorage.readUnsignedByte();
		bool success = false;

		ostringstream log;
		// dispatch commands
		switch (commandId)
		{
		case CMD_SIMSTEP:
#ifdef _DEBUG
			log <<"ns-3 server --> CMD_SIMSTEP received";
			Log((log.str()).c_str());

#endif
			success = RunSimStep(myInputStorage.readInt());
			return commandId;
		case CMD_UPDATENODE:
#ifdef _DEBUG
			log << "ns-3 server --> CMD_UPDATENODE received";
			Log((log.str()).c_str());
#endif
			success = UpdateNodePosition();
			break;
		case CMD_UPDATENODE2:
#ifdef _DEBUG
			log << "ns-3 server --> CMD_UPDATENODE2 received";
			Log((log.str()).c_str());
#endif
			success = UpdateNodePosition2();
			break;
		case CMD_CREATENODE:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_CREATENODE received";
			Log((log.str()).c_str());
#endif
			success = CreateNode();
			break;
		case CMD_CREATENODE2:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_CREATENODE2 received";
			Log((log.str()).c_str());
#endif
			success = CreateNode2();
			break;
		case CMD_START_CAM:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_CAM received";
			Log((log.str()).c_str());
#endif
			success = StartSendingCam();
			break;
		case CMD_STOP_CAM:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_STOP_CAM received";
			Log((log.str()).c_str());
#endif
			success = StopSendingCam();
			break;
		case CMD_GET_RECEIVED_MESSAGES:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_GET_RECEIVED_MESSAGES received";
			Log((log.str()).c_str());
#endif
			success = GetReceivedMessages();
			break;
		case CMD_GET_ALL_RECEIVED_MESSAGES:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_GET_ALL_RECEIVED_MESSAGES received";
			Log((log.str()).c_str());
#endif
			success = GetAllReceivedMessages();
			break;
		case CMD_CLOSE:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_CLOSE received";
			Log((log.str()).c_str());
#endif
			success = Close();
			break;
		case CMD_START_TOPO_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_TOPO_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartTopoTxon();
			break;
		case CMD_START_ID_BASED_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_ID_BASED_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartIdBasedTxon();
			break;
		case CMD_START_MW_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_MW_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartMWTxon();
			break;
		case CMD_START_IPCIU_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_IPCIU_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartIpCiuTxon();
			break;
		case CMD_STOP_SERVICE_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_STOP_SERVICE_TXON received";
			Log((log.str()).c_str());
#endif
			success = StopServiceTxon();
			break;
		case CMD_STOP_MW_SERVICE_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_STOP_MW_SERVICE_TXON received";
			Log((log.str()).c_str());
#endif
			success = StopMWServiceTxon();
			break;
		case CMD_STOP_IPCIU_SERVICE_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_STOP_IPCIU_SERVICE_TXON received";
			Log((log.str()).c_str());
#endif
			success = StopIpCiuServiceTxon();
			break;
		case CMD_START_GEO_BROAD_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_GEO_BROAD_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartGeobroadcastTxon();
			break;
		case CMD_START_GEO_ANY_TXON:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_START_GEO_ANY_TXON received";
			Log((log.str()).c_str());
#endif
			success = StartGeoanycastTxon();
			break;
		case CMD_ACTIVATE_NODE:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_ACTIVATE_NODE received";
			Log((log.str()).c_str());
#endif
			success = ActivateNode();
			break;
		case CMD_DEACTIVATE_NODE:
#ifdef _DEBUG
			log<< "ns-3 server --> CMD_DEACTIVATE_NODE received";
			Log((log.str()).c_str());
#endif
			success = DeactivateNode();
			break;
		default:
			writeStatusCmd(commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in ns3");
		}

		if (!success)
		{
			while (myInputStorage.valid_pos() && myInputStorage.position() < (unsigned int) (commandStart + commandLength))
			{
				myInputStorage.readChar();
			}
		}

		if (myInputStorage.position() != (unsigned int) (commandStart + commandLength))
		{
			ostringstream msg;
			msg << "Wrong position in requestMessage after dispatching command.";
			msg << " Expected command length was " << commandLength;
			msg << " but " << myInputStorage.position() - commandStart << " Bytes were read.";
			writeStatusCmd(commandId, RTYPE_ERR, msg.str());
			closeConnection_ = true;
		}

		return commandId;
	}

	void Ns3Server::writeStatusCmd(int commandId, int status, std::string description)
	{
#ifdef _DEBUG
        stringstream log;
#endif
        if (status == RTYPE_ERR)
		{
            cout << "ns-3 server --> Answered with error to command " << commandId << ": " << description;
		} else if (status == RTYPE_NOTIMPLEMENTED)
		{
            cout << "ns-3 server --> Requested command not implemented (" << commandId << "): " << description;
		}

#ifdef _DEBUG
        log << "ns-3 server --> Requested command (" << commandId << "): " << description;
        Log((log.str()).c_str());
#endif

        myOutputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));
		// command type
		myOutputStorage.writeUnsignedByte(commandId);
		// status
		myOutputStorage.writeUnsignedByte(status);
		// description
		myOutputStorage.writeString(description);

		return;
	}

	bool Ns3Server::RunSimStep(int time)
	{
		NS_LOG_INFO(Simulator::Now().GetSeconds() << " RunSimStep " << time);
		targetTime_ = time;

        Simulator::Stop(MilliSeconds(time) - Simulator::Now());
        Simulator::Run();

        my_resultsManager->LogAwarenessRatio(my_nodeManagerPtr->GetItetrisNodes()); // Added by A Correa

        //Simulator::RunOneEvent();
        writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "RunSimStep()");
        return true;
	}

	bool Ns3Server::CreateNode(void)
    {
#ifdef _DEBUG
        stringstream log;
#endif
        bool success = false;
		float x = myInputStorage.readFloat();
		float y = myInputStorage.readFloat();
		Vector pos = Vector(x, y, 0);
		vector<string> listOfCommModules = myInputStorage.readStringList();
		vector<string>::iterator moduleIt;

		int32_t nodeId = my_nodeManagerPtr->CreateItetrisNode(pos);


		for (moduleIt = listOfCommModules.begin(); moduleIt < listOfCommModules.end(); moduleIt++)
		{
			string cadena = *moduleIt;
#ifdef _DEBUG
			log<< "ns-3 server --> Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			my_nodeManagerPtr->InstallCommunicationModule(*moduleIt);
			success = true;
		}
#ifdef _DEBUG
		log<< "ns-3 server --> Node with ID "<<nodeId<<" created successfully in Position "<<x<<" "<<y<< endl;
		Log((log.str()).c_str());
#endif



        // Log results TransAID // Added by A Correa
        std::ostringstream resultString;
        resultString << "/NodeList/"
                     << nodeId
                     << "/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDist";

        Config::Connect (resultString.str (), MakeCallback( &iTETRISResults::LogPacketsTx,my_resultsManager) );

        resultString.str("");

        resultString << "/NodeList/"
                     << nodeId
                     << "/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxEndDist";

         Config::Connect (resultString.str (),MakeCallback( &iTETRISResults::LogPacketsRx,my_resultsManager) );
        //



		writeStatusCmd(CMD_CREATENODE, RTYPE_OK, "CreateNode()");
		myOutputStorage.writeUnsignedByte(1 + 1 + 4);
		myOutputStorage.writeUnsignedByte(CMD_CREATENODE);
		myOutputStorage.writeInt(nodeId);




		return success;
	}

	bool Ns3Server::CreateNode2(void)
	{
        bool success = false;
		float x = myInputStorage.readFloat();
		float y = myInputStorage.readFloat();
		float speed = myInputStorage.readFloat();
		float heading = myInputStorage.readFloat();
		string laneId = myInputStorage.readString();
		Vector pos = Vector(x, y, 0);
		vector<string> listOfCommModules = myInputStorage.readStringList();
		vector<string>::iterator moduleIt;

		int32_t nodeId = my_nodeManagerPtr->CreateItetrisNode(pos, speed, heading, laneId);

#ifdef _DEBUG
		stringstream log;
#endif

		for (moduleIt = listOfCommModules.begin(); moduleIt < listOfCommModules.end(); moduleIt++)
		{
			string cadena = *moduleIt;
			NS_LOG_UNCOND(Simulator::Now().GetSeconds() << " Install on node " << nodeId << " com " << cadena);
#ifdef _DEBUG
			log<< "ns-3 server --> Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			my_nodeManagerPtr->InstallCommunicationModule(*moduleIt);
			success = true;
		}

#ifdef _DEBUG
		log<< "ns-3 server --> Node with ID "<< nodeId<<" created successfully in Position "<<x<<" "<<y<< endl;
		Log((log.str()).c_str());
#endif

        // Log results TransAID // Added by A Correa
        std::ostringstream resultString;
        resultString << "/NodeList/"
                     << nodeId
                     << "/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDist";

        Config::Connect (resultString.str (),MakeCallback( &iTETRISResults::LogPacketsTx,my_resultsManager) );

        resultString.str("");

        resultString << "/NodeList/"
                     << nodeId
                     << "/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxEndDist";

       Config::Connect (resultString.str (),MakeCallback( &iTETRISResults::LogPacketsRx,my_resultsManager) );

        //

		writeStatusCmd(CMD_CREATENODE2, RTYPE_OK, "CreateNode2()");

		myOutputStorage.writeUnsignedByte(1 + 1 + 4);
		myOutputStorage.writeUnsignedByte(CMD_CREATENODE2);
		myOutputStorage.writeInt(nodeId);

		return success;
	}

	bool Ns3Server::UpdateNodePosition()
	{
		uint32_t nodeId = myInputStorage.readInt();
		float x = myInputStorage.readFloat();
		float y = myInputStorage.readFloat();

		Vector pos = Vector(x, y, 0);

		my_nodeManagerPtr->UpdateNodePosition(nodeId, pos); // call the iTETRISNodeManager function to update the node's position

#ifdef _DEBUG
		stringstream log;
		log<< "ns-3 server --> Node with ID "<<nodeId<<" has updated its position x="<<x<<" y="<<y<< endl;
		Log((log.str()).c_str());
#endif

		writeStatusCmd(CMD_UPDATENODE, RTYPE_OK, "UpdateNodePosition()");

		return true;
	}

	bool Ns3Server::UpdateNodePosition2()
	{
		uint32_t nodeId = myInputStorage.readInt();
		float x = myInputStorage.readFloat();
		float y = myInputStorage.readFloat();
		float speed = myInputStorage.readFloat();
		float heading = myInputStorage.readFloat();
		string laneId = myInputStorage.readString();
		Vector pos = Vector(x, y, 0);

		my_nodeManagerPtr->UpdateNodePosition(nodeId, pos, speed, heading, laneId); // call the iTETRISNodeManager function to update the node position

#ifdef _DEBUG
		stringstream log;
		log<< "ns-3 server --> Node with ID "<<nodeId<<" has updated its position x="<<x<<" y="<<y<< endl;
		Log((log.str()).c_str());
#endif

		writeStatusCmd(CMD_UPDATENODE2, RTYPE_OK, "UpdateNodePosition2()");

		return true;
	}

	bool Ns3Server::ActivateNode(void)
	{
		int number = myInputStorage.readInt();
		for (int i = 0; i < number; ++i)
		{
			int nodeId = myInputStorage.readInt();

#ifdef _DEBUG
			stringstream logNode;
			logNode << "[CMD_ACTIVATE_NODE] Activating node [ns3-ID] [" << nodeId << "]";
			Log((logNode.str()).c_str());
			logNode.str("");
#endif

			my_nodeManagerPtr->ActivateNode(nodeId);
		}
		writeStatusCmd(CMD_ACTIVATE_NODE, RTYPE_OK, "ActivateNode()");
		return true;
	}

	bool Ns3Server::DeactivateNode(void)
	{
		int number = myInputStorage.readInt();
		for (int i = 0; i < number; ++i)
		{
			int nodeId = myInputStorage.readInt();

#ifdef _DEBUG
			stringstream logNode;
			logNode << "[CMD_DEACTIVATE_NODE] Deactivating node [ns3-ID] [" << nodeId << "]";
			Log((logNode.str()).c_str());
			logNode.str("");
#endif

			my_nodeManagerPtr->DeactivateNode(nodeId);
		}
		writeStatusCmd(CMD_DEACTIVATE_NODE, RTYPE_OK, "DeactivateNode()");
		return true;
	}

	bool Ns3Server::StartSendingCam()
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		int payloadLength = myInputStorage.readInt();
		float frequency = myInputStorage.readFloat();

		//TODO add id to messages
		int messageId = 1;
		string ids;
		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log << "[StartSendingCam] Node Starts Sending CAM [ns3-ID|Frequency|Payload Length] [";
			log << cadena << "|" << frequency << "|" << payloadLength << "]";
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;

			my_packetManagerPtr->ActivateCamTxon(nodeId, frequency, payloadLength, messageId);
		}

		if (senderIdCollection.size() == 0)
		{

#ifdef _DEBUG
            stringstream log;
			log << "[StartSendingCam] There are not nodes to start sending CAM";
			Log((log.str()).c_str());
#endif
		}

		writeStatusCmd(CMD_START_CAM, RTYPE_OK, "StartSendingCam()");

		return true;
	}

	bool Ns3Server::StopSendingCam()
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();

		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log<< "ns-3 server --> Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			stringstream logNode;

#ifdef _DEBUG
			logNode << "[STOPSENDINGCAM] Stopping CAM of Node [ns3-ID] [" << nodeId << "]";
			Log((logNode.str()).c_str());
			logNode.str("");
#endif

			my_packetManagerPtr->DeactivateCamTxon(nodeId);
		}

		writeStatusCmd(CMD_STOP_CAM, RTYPE_OK, "StopSendingCam()");

		return true;
	}

	bool Ns3Server::GetReceivedMessages()
	{
		vector<string> sender;
		vector<string> type;
		vector<string> sentTimestep;
		vector<string> sequenceNumber;

		vector<tcpip::Storage*> genericTagStorage;

		int nodeId = myInputStorage.readInt();

#ifdef _DEBUG
		stringstream logNode;
		logNode << "[GetReceivedMessages] Retrieve Messages of Node [ns3-ID] [" << nodeId << "]";
		Log((logNode.str()).c_str());
		logNode.str("");
#endif

		/**
		 * JHNote (04/09/2013): enhanced ReceivedInciPacket with RSSI and SNR sampled from the PHY and Link layer.
		 * These two parameters may be omitted. In that case, they will take -1 values (RSSI: [0-127] and SNR > 0)
		 * Also, it is no longer limited to CAM
		 *
		 * The struct InciPacket::ReceivedInciPacket contains the data fields of a generic ns-3 packet
		 *
		 *   uint64_t senderId
		 *   std::string msgType
		 *   uint32_t ts
		 *   uint32_t tsSeqNo
		 *   tcpip::Storage* genericTagContainer
		 *
		 * The function PacketManager::GetReceivedPacket (uint32_t recvNodeId, struct InciPacket::ReceivedInciPacket &inciPacket)
		 * accepts as parameters the id of the node ('recvNodeId') from which we want to retrieve the received packets and a reference
		 * struct (InciPacket::ReceivedInciPacket) with the data fields of the last received packet. The function returns a bool
		 * indicating if more packets have been received by the node.
		 *
		 */

		struct InciPacket::ReceivedInciPacket inciPacket;

		while (my_packetManagerPtr->GetReceivedPacket(nodeId, inciPacket))
		{
			sender.push_back(Int2String(inciPacket.senderId));
			type.push_back(inciPacket.msgType);
			sentTimestep.push_back(Int2String(inciPacket.ts));
			sequenceNumber.push_back(Int2String(inciPacket.tsSeqNo));
			genericTagStorage.push_back(inciPacket.genericTagContainer);

#ifdef _DEBUG
			stringstream log;
			log << "[GetReceivedMessages] Message Info of node [ns3-ID|Sender ns3-ID|MessageType|Sent Timestep|SeqNum] ["
			<< nodeId << "|" << inciPacket.senderId << "|" << inciPacket.msgType << "|" << inciPacket.ts << "|" << inciPacket.tsSeqNo <<"]";
			Log((log.str()).c_str());
#endif
		}
		writeStatusCmd(CMD_GET_RECEIVED_MESSAGES, RTYPE_OK, "GetReceivedMessages()");

#ifdef _DEBUG
        stringstream log;
		log << "*[ns-3][Ns3Server::GetReceivedMessages] got tags back";
		Log((log.str()).c_str());
#endif
		tcpip::Storage tmpStorage;
		for (unsigned int i = 0; i < genericTagStorage.size(); i++)
		{
			tcpip::Storage* store = genericTagStorage[i];
			tmpStorage.writeShort(store->size());  // needed to decode the vector of storage at the iCS
			if (store->size() > 0)
			{
				tmpStorage.writeStorage(*store);  //dereferencing it to write the Storage and not the pointer
			}
			// get memory back
			delete store;
		}
#ifdef _DEBUG
		log << "*[ns-3][Ns3Server::GetReceivedMessages] moved here";
        Log((log.str()).c_str());
#endif
		myOutputStorage.writeInt(
				4 + 1 + CalculateStringListByteSize(sender) + CalculateStringListByteSize(type)
						+ CalculateStringListByteSize(sentTimestep) + CalculateStringListByteSize(sequenceNumber) + 2
						+ tmpStorage.size());
		myOutputStorage.writeUnsignedByte(CMD_GET_RECEIVED_MESSAGES);
		myOutputStorage.writeStringList(sender);
		myOutputStorage.writeStringList(type);
		myOutputStorage.writeStringList(sentTimestep);
		myOutputStorage.writeStringList(sequenceNumber);
		myOutputStorage.writeShort(genericTagStorage.size());
		if (genericTagStorage.size() > 0)
			myOutputStorage.writeStorage(tmpStorage);

#ifdef _DEBUG
		log << "*[ns-3][Ns3Server::GetReceivedMessages] moved here";
        Log((log.str()).c_str());
#endif
		return true;
	}

	bool Ns3Server::GetAllReceivedMessages()
	{
		NodeContainer nodeList = my_nodeManagerPtr->GetItetrisNodes();
		tcpip::Storage messageStorage;
		int numNodesWithMessages = 0;
#ifdef _DEBUG
		std::ostringstream oss;
		oss << "Received messages [";
#endif
		for (NodeContainer::Iterator it = nodeList.Begin(); it != nodeList.End(); ++it)
		{
			int nodeId = (*it)->GetId();
			int numMessages = my_packetManagerPtr->GetNumberOfReceivedPackets(nodeId);
			if (numMessages > 0)
			{
#ifdef _DEBUG
                oss << nodeId << ":" << numMessages << ", ";
#endif
				++numNodesWithMessages;
				messageStorage.writeInt(nodeId);
				messageStorage.writeInt(numMessages);
				InciPacket::ReceivedInciPacket inciPacket;
				while (my_packetManagerPtr->GetReceivedPacket(nodeId, inciPacket))
				{
					messageStorage.writeInt(inciPacket.senderId);
					messageStorage.writeInt(inciPacket.messageId);
					messageStorage.writeString(inciPacket.msgType);
					messageStorage.writeInt(inciPacket.ts);
					messageStorage.writeInt(inciPacket.tsSeqNo);
					messageStorage.writeShort(inciPacket.genericTagContainer->size());
					messageStorage.writeStorage(*inciPacket.genericTagContainer);
					//Get memory back
					delete inciPacket.genericTagContainer;
				}
			}
		}

#ifdef _DEBUG
        oss << "]";
		Log(oss.str().c_str());
#endif
		writeStatusCmd(CMD_GET_ALL_RECEIVED_MESSAGES, RTYPE_OK, "GetReceivedMessages()");
		myOutputStorage.writeInt(4 + 1 + 4 + messageStorage.size());
		myOutputStorage.writeUnsignedByte(CMD_GET_ALL_RECEIVED_MESSAGES);
		myOutputStorage.writeInt(numNodesWithMessages);
		myOutputStorage.writeStorage(messageStorage);

		return true;
	}

	std::string Ns3Server::Int2String(int n)
	{
		std::stringstream res;
		res << n;
		return (res.str());
	}

	std::string Ns3Server::Double2String(double n)
	{
		std::stringstream res;
		res << n;
		return (res.str());

	}

	bool Ns3Server::Close()
	{



		closeConnection_ = true;
		writeStatusCmd(CMD_CLOSE, RTYPE_OK, "Close()");
		return true;
	}

	int Ns3Server::CalculateStringListByteSize(vector<string> list)
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

	string getTime()
	{
		struct timeval tim;
		gettimeofday(&tim, NULL);
		char buffer[21];
		strftime(buffer, 21, "%Y-%m-%dT%H:%M:%S.", localtime((time_t*) &tim.tv_sec));
		string mytime(buffer);
		long milli = tim.tv_usec / 1000;
		ostringstream oss;
		if (milli < 10)
			oss << "00";
		else if (milli < 100)
			oss << "0";
		oss << milli;
		return mytime += oss.str();
	}

	string getTimeStep()
	{
		ostringstream oss;
		oss << (Ns3Server::targetTime_ / 1000) << ",";
		int milli = Ns3Server::targetTime_ % 1000;
		if (milli < 10)
			oss << "00";
		else if (milli < 100)
			oss << "0";
		oss << milli;
		return oss.str();
	}

	bool Ns3Server::Log(const char* message)
	{
		return false;
		if (logActive_)
		{
			myfile << "[" << getTime() << "] " << "[" << getTimeStep() << "] " << message << endl;
			return true;
		}
		return false;
	}

	bool Ns3Server::StartTopoTxon(void)
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		string serviceId = myInputStorage.readString();
		int commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
		vector<string> technologies = myInputStorage.readStringList();
		float frequency = myInputStorage.readFloat();
		int payloadLength = myInputStorage.readInt();
		float msgRegenerationTime = myInputStorage.readFloat();
		int msgLifetime = myInputStorage.readInt();
		int numHops = myInputStorage.readInt();
		short container_l = myInputStorage.readShort();
		std::vector<unsigned char> genericContainer;
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				genericContainer.push_back(myInputStorage.readChar());

		//TODO set message id
		int messageId = 1;
		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StartTopoTxon] Node starts topobroadcast transmission [ns3-ID|ServiceId|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime|NumHops] [";
			log << cadena << "|" << serviceId << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "|" << numHops << "]";
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->ActivateTopoBroadcastTxon(nodeId, serviceId, commProfile, technologies, frequency,
					payloadLength, msgRegenerationTime, msgLifetime, numHops, messageId, genericContainer);
		}

		if (senderIdCollection.size() == 0)
		{
			stringstream log;
            log << "[ns-3][Ns3Server::StartTopoTxon] There are no nodes to start topobroadcast transmission";
			Log((log.str()).c_str());
		}

		writeStatusCmd(CMD_START_TOPO_TXON, RTYPE_OK, "StartTopoTxon ()");
		return true;
	}

	bool Ns3Server::StartIdBasedTxon(void)
	{ 
		MessageScheduleUnicast * msu = new MessageScheduleUnicast();
		msu->senderIdCollection = myInputStorage.readStringList();
		msu->serviceId = myInputStorage.readString();
		msu->commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
		msu->technologies = myInputStorage.readStringList();
		msu->time = myInputStorage.readDouble();
		msu->frequency = myInputStorage.readFloat();
		msu->payloadLength = myInputStorage.readInt();
		msu->destination = myInputStorage.readInt();
		msu->msgRegenerationTime = myInputStorage.readFloat();
		msu->msgLifetime = myInputStorage.readInt();
		msu->messageId = myInputStorage.readInt();
		short container_l = myInputStorage.readShort();
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				msu->genericContainer.push_back(myInputStorage.readChar());
		NS_LOG_INFO(
				Simulator::Now().GetSeconds() << " StartIdBasedTxon." << " Scheduled message at "
						<< NanoSeconds(msu->time * 1000000).GetSeconds() << " From " << msu->senderIdCollection[0]);
		Simulator::Schedule(NanoSeconds(msu->time * 1000000) - Simulator::Now(), &MessageSchedule::DoInvoke, msu);
		writeStatusCmd(CMD_START_ID_BASED_TXON, RTYPE_OK, "StartIdBasedTxon ()");
		return true;
	}

bool Ns3Server::StartMWTxon(void)
    {
		vector<string> senderIdCollection = myInputStorage.readStringList();
		string serviceId = myInputStorage.readString();
		int commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
		vector<string> technologies = myInputStorage.readStringList();
		CircularGeoAddress destination;
		destination.lat = myInputStorage.readInt();
		destination.lon = myInputStorage.readInt();
		destination.areaSize = myInputStorage.readInt();
		float frequency = myInputStorage.readFloat();
		int payloadLength = myInputStorage.readInt();
		float msgRegenerationTime = myInputStorage.readFloat();
		int msgLifetime = myInputStorage.readInt();
		short container_l = myInputStorage.readShort();
		std::vector<unsigned char> genericContainer;
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				genericContainer.push_back(myInputStorage.readChar());

		//TODO add message id
		int messageId = 1;
		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;
			// TODO add a check on the node ID: It must be a TMC...need a method: isTMC(cadena)

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StartMWTxon] Node (TMC) starts MW transmission [ns3-ID|ServiceId|TechnologyList|CenterPointLat|CenterPointLon|AreaSize|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime] [";
			log << cadena << "|" << serviceId << "|";
			for (vector<string>::iterator techsIt = technologies.begin (); techsIt < technologies.end (); techsIt++)
			{
				string techTmp = *techsIt;
				if (techsIt != technologies.begin ())
				{
					log << "-";
				}
				log << techTmp;
			}
			log << "|" << destination.lat << "|" << destination.lon << "|" << destination.areaSize << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "]";
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->InitiateMWTxon(nodeId, serviceId, commProfile, technologies, destination, frequency,
					payloadLength, msgRegenerationTime, msgLifetime, messageId, genericContainer/*, IPv4*/);
		}

		if (senderIdCollection.size() == 0)
		{

			stringstream log;
			log << "[ns-3][Ns3Server::StartMWTxon] There are not TMC node to start MW transmission";
			Log((log.str()).c_str());

			writeStatusCmd(CMD_START_ID_BASED_TXON, RTYPE_ERR, "StartMWTxon ()");
		}

		writeStatusCmd(CMD_START_ID_BASED_TXON, RTYPE_OK, "StartMWTxon ()");
		return true;
	}

bool Ns3Server::StartIpCiuTxon(void)
{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		string serviceId = myInputStorage.readString();
		float frequency = myInputStorage.readFloat();
		int payloadLength = myInputStorage.readInt();
		int destination = myInputStorage.readInt();
		float msgRegenerationTime = myInputStorage.readFloat();
		short container_l = myInputStorage.readShort();
		std::vector<unsigned char> genericContainer;
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				genericContainer.push_back(myInputStorage.readChar());

		//TODO add message id
		int messageId = 1;
		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StartIpCiuTxon] CIU node starts IP-based transmission [ns3-ID|ServiceId|Frequency|PayloadLength|Destination|MsgRegenerationTime] [";
			log << cadena << "|" << serviceId << "|" << frequency << "|" << payloadLength << "|" << destination << "|" << msgRegenerationTime << "]";
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->InitiateIPCIUTxon(nodeId, serviceId, frequency, payloadLength, destination,
					msgRegenerationTime, messageId, genericContainer);

		}

		if (senderIdCollection.size() == 0)
		{
			stringstream log;
			log << "[ns-3][Ns3Server::StartIpCiuTxon] There are not CIU nodes to start IP-based transmission";
			Log((log.str()).c_str());
		}

		writeStatusCmd(CMD_START_IPCIU_TXON, RTYPE_OK, "StartIpCiuTxon ()");
		return true;
	}

	bool Ns3Server::StopServiceTxon(void)
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		vector<string>::iterator senderIt;
		string serviceId = myInputStorage.readString();
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StopServiceTxon] Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->DeactivateServiceTxon(nodeId, serviceId);
		}
		writeStatusCmd(CMD_STOP_SERVICE_TXON, RTYPE_OK, "StopServiceTxon()");
		return true;
	}

	bool Ns3Server::StopMWServiceTxon(void)
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		vector<string>::iterator senderIt;
		string serviceId = myInputStorage.readString();
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;
			// TODO add a check that nodeID (cadena) is a TMC - method isTMC(cadena) required

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StopMWServiceTxon] Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->DeactivateMWServiceTxon(nodeId, serviceId);
		}

		if (senderIdCollection.size() == 0)
		{

			stringstream log;
			log << "[ns-3][Ns3Server::StopMWServiceTxon] There are not TMC node to stop MW Services";
			Log((log.str()).c_str());

			writeStatusCmd(CMD_STOP_MW_SERVICE_TXON, RTYPE_ERR, "StopMWServiceTxon()");
		}

		writeStatusCmd(CMD_STOP_MW_SERVICE_TXON, RTYPE_OK, "StopMWServiceTxon()");
		return true;
	}

	bool Ns3Server::StopIpCiuServiceTxon(void)
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		vector<string>::iterator senderIt;
		string serviceId = myInputStorage.readString();
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log<< "[ns-3][Ns3Server::StopIpCiuServiceTxon] Nodes IDs: " << cadena;
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->DeactivateIPCIUServiceTxon(nodeId, serviceId);
		}
		writeStatusCmd(CMD_STOP_IPCIU_SERVICE_TXON, RTYPE_OK, "StopIpCiuServiceTxon()");
		return true;
	}

	bool Ns3Server::StartGeobroadcastTxon(void)
	{
		MessageScheduleGeoBroadcast * msgb = new MessageScheduleGeoBroadcast();
		msgb->senderIdCollection = myInputStorage.readStringList();
		msgb->serviceId = myInputStorage.readString();
		msgb->commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
		msgb->technologies = myInputStorage.readStringList();
		msgb->time = myInputStorage.readDouble();
		msgb->destination.lat = myInputStorage.readInt();
		msgb->destination.lon = myInputStorage.readInt();
		msgb->destination.areaSize = myInputStorage.readInt();
		msgb->frequency = myInputStorage.readFloat();
		msgb->payloadLength = myInputStorage.readInt();
		msgb->msgRegenerationTime = myInputStorage.readFloat();
		msgb->msgLifetime = myInputStorage.readInt();
		msgb->messageId = myInputStorage.readInt();
		;

		short container_l = myInputStorage.readShort();
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				msgb->genericContainer.push_back(myInputStorage.readChar());
		NS_LOG_INFO(
				Simulator::Now().GetSeconds() << " StartGeobroadcastTxon." << " Scheduled message at "
						<< NanoSeconds(msgb->time * 1000000).GetSeconds() << " From " << msgb->senderIdCollection[0]);
		Simulator::Schedule(NanoSeconds(msgb->time * 1000000) - Simulator::Now(), &MessageSchedule::DoInvoke, msgb);
		writeStatusCmd(CMD_START_GEO_BROAD_TXON, RTYPE_OK, "StartGeobroadcastTxon ()");
		return true;
	}

	bool Ns3Server::StartGeoanycastTxon(void)
	{
		vector<string> senderIdCollection = myInputStorage.readStringList();
		string serviceId = myInputStorage.readString();
		int commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
		vector<string> technologies = myInputStorage.readStringList();
		CircularGeoAddress destination;
		destination.lat = myInputStorage.readInt();
		destination.lon = myInputStorage.readInt();
		destination.areaSize = myInputStorage.readInt();
		float frequency = myInputStorage.readFloat();
		int payloadLength = myInputStorage.readInt();
		float msgRegenerationTime = myInputStorage.readFloat();
		int msgLifetime = myInputStorage.readInt();
		short container_l = myInputStorage.readShort();
		std::vector<unsigned char> genericContainer;
		if (container_l > 0)
			for (int i = 0; i < container_l; i++)
				genericContainer.push_back(myInputStorage.readChar());

		//TODO add message id
		int messageId = 1;
		vector<string>::iterator senderIt;
		for (senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end(); senderIt++)
		{
			string cadena = *senderIt;

#ifdef _DEBUG
			stringstream log;
			log << "[ns-3][Ns3Server::StartGeoanycastTxon] Node starts geoanycast transmission [ns3-ID|ServiceId|CenterPointLat|CenterPointLon|AreaSize|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime] [";
			log << cadena << "|" << serviceId << "|" << destination.lat << "|" << destination.lat << "|" << destination.lon << "|" << destination.areaSize << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "]";
			Log((log.str()).c_str());
#endif

			stringstream temp;
			uint32_t nodeId;
			temp << cadena;
			temp >> nodeId;
			my_packetManagerPtr->InitiateGeoAnycastTxon(nodeId, serviceId, commProfile, technologies, destination, frequency,
					payloadLength, msgRegenerationTime, msgLifetime, messageId, genericContainer);
		}

		if (senderIdCollection.size() == 0)
		{
			stringstream log;
			log << "[ns-3][Ns3Server::StartGeoanycastTxon] There are not nodes to start geoanycast transmission";
			Log((log.str()).c_str());
		}

		writeStatusCmd(CMD_START_GEO_ANY_TXON, RTYPE_OK, "StartGeoanycastTxon ()");
		return true;
	}

}
