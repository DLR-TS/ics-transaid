/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************/
/// @file    ns3-client.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author:  Jerome Haerri (jerome.haerri@eurecom.fr) and T.-T. Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM
 * version: 1.0
 ***************************************************************************************/
/****************************************************************************************
 * Copyright (c) 2016 EURECOM
 * This code has been developed in the context of the
 * SINETIC project
 * ....
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Additional permission under GNU GPL version 3 section 7
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgment: 'This product includes software developed by
 * EURECOM and its contributors'.
 * 4. Neither the name of EURECOM nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 ***************************************************************************************/
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

#include "ns3-client.h"
#include "../utilities.h"
#include "../ics.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"
#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
string Ns3Client::CAM_TYPE = "0";
string Ns3Client::DENM_TYPE = "1";

// ===========================================================================
// member method definitions
// ===========================================================================
Ns3Client::Ns3Client() : m_socket(nullptr), m_port(0)
{}

Ns3Client::~Ns3Client() {
    delete m_socket;
}

bool Ns3Client::Connect() {
    m_socket = new tcpip::Socket(m_host, m_port);

    for (int i = 0; i < 10; ++i) {
        try {
            cout << "iCS --> Trying " << i << " to connect ns-3 on port " << m_port << " and Host " << m_host << "..."
                 << endl;
            m_socket->connect();
            return true;
        } catch (tcpip::SocketException& e) {
            cout << "iCS --> No connection to ns-3; waiting..." << endl;
            Sleep(3000);
        }
    }

    return false;
}

int Ns3Client::Close() {
    if (m_socket != 0) {
        m_socket->close();
        delete m_socket;
        m_socket = 0;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool Ns3Client::CommandSimulationStep(int time) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

#ifdef LOG_ON
    IcsLog::LogLevel("Reached CommandSimulationStep... = ", kLogLevelInfo);
#endif

    if (m_socket == NULL) {
        cout << "iCS -->  #Error while sending command: no connection to server ns-3";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SIMSTEP);
    // simulation time
    outMsg.writeInt(time);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while sending command to ns-3: " << e.what();
        return false;
    }
#ifdef LOG_ON
    IcsLog::LogLevel("Message Sent...", kLogLevelInfo);
#endif
    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

#ifdef LOG_ON
    IcsLog::LogLevel("Message Received.", kLogLevelInfo);
#endif

    // validate result state
    if (!ReportResultState(inMsg, CMD_SIMSTEP)) {
        return false;
    }

    return true;

}

int Ns3Client::CommandUpdateNodePosition(int nodeId, float x, float y) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_UPDATENODE);
    // node id
    outMsg.writeInt(nodeId);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_UPDATENODE)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Ns3Client::CommandUpdateNodePosition2(int nodeId, float x, float y, float speed, float heading,
        std::string laneId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + laneId.length());
    // command id
    outMsg.writeUnsignedByte(CMD_UPDATENODE2);
    // node id
    outMsg.writeInt(nodeId);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // speed
    outMsg.writeFloat(speed);
    // heading
    outMsg.writeFloat(heading);
    // laneId
    outMsg.writeString(laneId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_UPDATENODE2)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Ns3Client::CommandCreateNode(float x, float y, std::vector<std::string> techList) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return -1;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (techList.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = techList.begin(); it < techList.end(); it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + stringSize);
    // command id
    cout << "jin <iCS/ns3-client.cc> CMD_CREATENODE=" << CMD_CREATENODE << endl;
    outMsg.writeUnsignedByte(CMD_CREATENODE);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // rat type
    outMsg.writeStringList(techList);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return -1;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return -1;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CREATENODE)) {
        return -1;
    }

    // length
    inMsg.readUnsignedByte();
    // command id
    inMsg.readUnsignedByte();
    int32_t nodeId = inMsg.readInt();

    return nodeId;
}

int Ns3Client::CommandCreateNode2(float x, float y, float speed, float heading, std::string laneId,
                                  std::vector<std::string> techList) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return -1;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (techList.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = techList.begin(); it < techList.end(); it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + 4 + 4 + laneId.length() + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_CREATENODE2);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // speed
    outMsg.writeFloat(speed);
    // heading
    outMsg.writeFloat(heading);
    // laneId
    outMsg.writeString(laneId);
    // rat type
    outMsg.writeStringList(techList);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return -1;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return -1;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CREATENODE2)) {
        return -1;
    }

    // length
    inMsg.readUnsignedByte();
    // command id
    inMsg.readUnsignedByte();
    int32_t nodeId = inMsg.readInt();

    return nodeId;
}

bool Ns3Client::CommandStartSendingCam(vector<string> sendersId, unsigned int payloadLength, float frequency) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin(); it < sendersId.end(); it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    outMsg.writeInt(4 + 1 + stringSize + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_START_CAM);
    // ns-3 ids of the nodes that has to start sending CAM
    outMsg.writeStringList(sendersId);
    // the length of information the CAM message transmits  --> TODO: the value shall not be hard coded!
    outMsg.writeInt(20);
    // the frequency in which the message will be sent
    outMsg.writeFloat(frequency);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_CAM)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStopSendingCam(vector<string> sendersId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin(); it < sendersId.end(); it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_STOP_CAM);
    // ns-3 ids of the nodes that has to start sending CAM
    outMsg.writeStringList(sendersId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_CAM)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandGetReceivedMessages(int nodeId, vector<ReceivedMessage>* receivedMessages, int timeResolution) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_GET_RECEIVED_MESSAGES);
    // node id
    outMsg.writeInt(nodeId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_GET_RECEIVED_MESSAGES)) {
        return false;
    }

    // length
    inMsg.readInt();
    // command id
    inMsg.readUnsignedByte();

    vector<string> receivedNodes = inMsg.readStringList();
    vector<string> types = inMsg.readStringList();
    vector<string> sentTimesteps = inMsg.readStringList();
    vector<string> seqNumb = inMsg.readStringList();

    vector<vector<unsigned char>*> packetTagContainerList;

    unsigned short packetTagContainerList_l = inMsg.readShort();  // used to check the coherence of the number of items

#ifdef LOG_ON
    stringstream log;
    log << "iCS --> [CommandGetReceivedMessages] lengths of the container list is " << packetTagContainerList_l;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    for (int i = 0; i < packetTagContainerList_l; i++) {
        unsigned short packetTagContainer_l = inMsg.readShort(); // read the length of the first embedded storage

        if (packetTagContainer_l > 0) {
            std::vector<unsigned char>* packetTagContainer_ptr = new std::vector<unsigned char>(packetTagContainer_l);
            for (unsigned int j = 0; j < packetTagContainer_l; j++) {
                unsigned char type = inMsg.readChar();
                (*packetTagContainer_ptr)[j] = type;
            }

            packetTagContainerList.push_back(packetTagContainer_ptr);
        }
    }

    if (!(receivedNodes.size() == types.size()) || !(sentTimesteps.size() == types.size())
            || !(seqNumb.size() == sentTimesteps.size())) {
        cout << "iCS --> #Error, the returned values size do not match" << endl;
        return false;
    }

    // If the simulator did not return any message do not process
    if (receivedNodes.size() == 0) {

        stringstream log;
        log << "iCS --> [CommandGetReceivedMessages] 0 messages were received";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    } else {
        vector<string>::iterator resultIt;
        int index = 0;
        for (resultIt = receivedNodes.begin(); resultIt != receivedNodes.end(); resultIt++) {
            string senderId = *resultIt;
            string type = types.at(index);
            string timestep = sentTimesteps.at(index);
            string seqNum = seqNumb.at(index);

            ReceivedMessage receivedMessage;
            receivedMessage.senderId = utils::Conversion::string2Int(senderId);
            receivedMessage.messageType = ParseMessageType(type);
#ifdef _DEBUG_ON
            stringstream log;
            log << "iCS --> [CommandGetReceivedMessages] keeping message at index " << index;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            int tmptimestep = utils::Conversion::string2Int(timestep) - timeResolution;
            receivedMessage.timeStep = tmptimestep - tmptimestep % timeResolution;
            receivedMessage.sequenceNumber = utils::Conversion::string2Int(seqNum);
#ifdef _DEBUG_ON
            stringstream log;
            log << "iCS --> [CommandGetReceivedMessages] message number " << receivedMessage.sequenceNumber << " type "
                << receivedMessage.messageType;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            if (packetTagContainerList.size() == 0) {
                vector<unsigned char>* empty_ptr = new vector<unsigned char>();
                receivedMessage.packetTagContainer = empty_ptr;
            } else {
                // JHNOTE(13/11/2013): was the location of a major bug...
                // the packetTagContainer was never initialized (as size is zero...)
                if (packetTagContainerList.size() >= index) {
                    receivedMessage.packetTagContainer = packetTagContainerList.at(index);
                }
            }

            receivedMessages->push_back(receivedMessage);
            index++;
        }
    }

    return true;
}

ics_types::messageType_t Ns3Client::ParseMessageType(string& messageTypeString) {
    if (messageTypeString == "CAM") {
        return CAM;
    }
    if (messageTypeString == DENM_TYPE) {
        return DENM;
    }
    if (messageTypeString == "serviceIdUnicast") {
        return UNICAST;
    }
    if (messageTypeString == "serviceIdGeobroadcast") {
        return GEOBROADCAST;
    }
    if (messageTypeString == "serviceIdTopobroadcast") {
        return TOPOBROADCAST;
    }
    return CAM;	// TODO do not return CAM by default...need to return an error
}

bool Ns3Client::CommandGetAllReceivedMessages(map<int, vector<Message> >* receivedMessages, int timeResolution) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_GET_ALL_RECEIVED_MESSAGES);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_GET_ALL_RECEIVED_MESSAGES)) {
        return false;
    }

    // length
    inMsg.readInt();
    // command id
    inMsg.readUnsignedByte();
    int numNodes = inMsg.readInt();
#ifdef LOG_ON
    ostringstream oss;
    oss << "[CommandGetAllReceivedMessages] " << numNodes << " nodes received messages.";
#endif
    for (int i = 0; i < numNodes; ++i) {
        int nodeId = inMsg.readInt();
        int numMessages = inMsg.readInt();
#ifdef LOG_ON
        oss << "\nNode " << nodeId << " received " << numMessages << " msgs. [";
#endif
        for (int j = 0; j < numMessages; ++j) {
            Message message;
            message.receiverNs3Id = nodeId;
            message.senderNs3Id = inMsg.readInt();
            message.messageId = inMsg.readInt();
            string mgsType = inMsg.readString();
            message.messageType = ParseMessageType(mgsType);
            int ts = inMsg.readInt();
            message.timeStep = ts - ts % timeResolution;
            message.sequenceNumber = inMsg.readInt();
            int size = inMsg.readShort();

            vector<unsigned char>* container = new vector<unsigned char>(size);
            for (int k = 0; k < size; ++k) {
                container->operator[](k) = inMsg.readChar();
            }
            message.packetTagContainer = container;
#ifdef LOG_ON
            oss << message.messageId << "->" << message.senderNs3Id << ", ";
#endif
            receivedMessages->operator [](nodeId).push_back(message);
        }
#ifdef LOG_ON
        oss << "]";
#endif
    }
#ifdef LOG_ON
    IcsLog::LogLevel((oss.str()).c_str(), kLogLevelInfo);
#endif

    return true;
}

bool Ns3Client::CommandStartTopoTxon(std::vector<std::string> sendersId, std::string serviceId,
                                     unsigned char commProfile, std::vector<std::string> technologyList, float frequency, unsigned int payloadLength,
                                     float msgRegenerationTime, unsigned int msgLifetime, unsigned int numHops,
                                     std::vector<unsigned char>* genericContainer) {

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    outMsg.writeInt(
        4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 2
        + genericContainer->size()); // Not sure if this works
    outMsg.writeUnsignedByte(CMD_START_TOPO_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeInt(numHops);
    outMsg.writeShort(genericContainer->size());
    outMsg.writePacket(genericContainer->data(), genericContainer->size());
    delete genericContainer;
    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_TOPO_TXON)) {
        return false;
    }

    return true;
}

int Ns3Client::CommandClose() {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == nullptr) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_CLOSE);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CLOSE)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool Ns3Client::ReportResultState(tcpip::Storage& inMsg, int command) {
    int cmdLength;
    int cmdId;
    int resultType;
    int cmdStart;
    std::string msg;
    ostringstream oss;

    try {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();

        if (cmdId != command) {
            cout << "ns-3 --> iCS #Error: received status response to command: " << cmdId << " but expected: " << command
                 << endl;
            return false;
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument e) {
        cout << "ns-3 --> iCS #Error: an exception was thrown while reading result state message" << endl;
        return false;
    }

    switch (resultType) {
        case NS3_RTYPE_ERR: {
            cout << ".. ns-3 answered with error to command (" << cmdId << "), [description: " << msg << "]" << endl;
            return false;
        }
        case NS3_RTYPE_NOTIMPLEMENTED:
            cout << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << endl;
            return false;
        case NS3_RTYPE_OK:
#ifdef LOG_ON
            oss << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]" << endl;
            IcsLog::LogLevel((oss.str()).c_str(), kLogLevelInfo);
#endif
            break;
        default:
            cout << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId << "), [description: "
                 << msg << "]" << endl;
            return false;
    }

    if ((cmdStart + cmdLength) != inMsg.position()) {
        cout << "ns-3 --> iCS #Error: command at position " << cmdStart << " has wrong length " << cmdLength << "H"
             << endl;
        return false;
    }

    return true;
}

int Ns3Client::CalculateStringListByteSize(vector<string> list) {
    // TODO check this...
    if (list.size() == 0) {
        return 4;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (list.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = list.begin(); it < list.end(); it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    return stringSize;
}

bool Ns3Client::CommandStartIpCiuTxon(std::vector<std::string> sendersId, std::string serviceId, float frequency,
                                      unsigned int payloadLength, unsigned int destination, float msgRegenerationTime,
                                      std::vector<unsigned char>* genericContainer) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 4 + 4 + 4 + 4 + 2 + genericContainer->size());
    outMsg.writeUnsignedByte(CMD_START_IPCIU_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeInt(destination);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeShort(genericContainer->size());
    outMsg.writePacket(genericContainer->data(), genericContainer->size());

    delete genericContainer;
    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_IPCIU_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStartIdBasedTxon(std::vector<std::string> sendersId, std::string serviceId,
                                        unsigned char commProfile, std::vector<std::string> technologyList, float frequency, unsigned int payloadLength,
                                        unsigned int destination, float msgRegenerationTime, unsigned int msgLifetime, double time, int messageId,
                                        std::vector<unsigned char>* genericContainer) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    // Reading inside genericContainer,which include the techno perference
    if (genericContainer != NULL)
        outMsg.writeInt(
            4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 2
            + genericContainer->size());
    else {
        outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 2);
    }

    outMsg.writeUnsignedByte(CMD_START_ID_BASED_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeDouble(time);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeInt(destination);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeInt(messageId);
    if (genericContainer != NULL) {
        outMsg.writeShort(genericContainer->size());
        if (genericContainer->size() > 0) {
            outMsg.writePacket(genericContainer->data(), genericContainer->size());
        }
    } else {
        outMsg.writeShort(0);
    }
    delete genericContainer;
    // send request message

    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }
#ifdef LOG_ON
    stringstream log;
    log << "[iCS] CommandStartIdBasedTxon() received from ns3";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
    // validate result state
    if (!ReportResultState(inMsg, CMD_START_ID_BASED_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStartMWTxon(std::vector<std::string> sendersId, std::string serviceId,
                                   unsigned char commProfile, std::vector<std::string> technologyList, CircularGeoAddress destination,
                                   float frequency, uint32_t payloadLength, double msgRegenerationTime, uint8_t msgLifetime,
                                   std::vector<unsigned char>* genericContainer) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    outMsg.writeInt(
        4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 2
        + genericContainer->size());
    outMsg.writeUnsignedByte(CMD_START_MW_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeShort(genericContainer->size());
    outMsg.writePacket(genericContainer->data(), genericContainer->size());

    delete genericContainer;
    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_MW_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStopServiceTxon(std::vector<std::string> sendersId, std::string serviceId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStopIpCiuServiceTxon(std::vector<std::string> sendersId, std::string serviceId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_IPCIU_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_IPCIU_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandStopMWServiceTxon(std::vector<std::string> sendersId, std::string serviceId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_MW_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_MW_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::StartGeobroadcastTxon(std::vector<std::string> sendersId, std::string serviceId,
                                      unsigned char commProfile, std::vector<std::string> technList, CircularGeoAddress destination, float frequency,
                                      unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime, double time, int messageId,
                                      std::vector<unsigned char>* genericContainer) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technList);
    outMsg.writeInt(
        4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 2
        + genericContainer->size());
    outMsg.writeUnsignedByte(CMD_START_GEO_BROAD_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technList);
    outMsg.writeDouble(time);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeInt(messageId);
    outMsg.writeShort(genericContainer->size());
    outMsg.writePacket(genericContainer->data(), genericContainer->size());
    delete genericContainer;
    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_GEO_BROAD_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::StartGeoanycastTxon(std::vector<std::string> sendersId, std::string serviceId,
                                    unsigned char commProfile, std::vector<std::string> technList, CircularGeoAddress destination, float frequency,
                                    unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime,
                                    std::vector<unsigned char>* genericContainer) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technList);

    outMsg.writeInt(
        4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 2
        + genericContainer->size());
    outMsg.writeUnsignedByte(CMD_START_GEO_ANY_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technList);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeShort(genericContainer->size());
    outMsg.writePacket(genericContainer->data(), genericContainer->size());

    delete genericContainer;
    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_GEO_ANY_TXON)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandActivateNode(const vector<int>& sendersId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 * sendersId.size());
    // command id
    outMsg.writeUnsignedByte(CMD_ACTIVATE_NODE);
    // number of ns-3 ids of the nodes to be activated
    outMsg.writeInt(sendersId.size());
    // write every id
    for (vector<int>::const_iterator it = sendersId.begin(); it != sendersId.end(); ++it) {
        outMsg.writeInt(*it);
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_ACTIVATE_NODE)) {
        return false;
    }

    return true;
}

bool Ns3Client::CommandDeactivateNode(const vector<int>& sendersId) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 * sendersId.size());
    // command id
    outMsg.writeUnsignedByte(CMD_DEACTIVATE_NODE);
    // number of ns-3 ids of the nodes to be activated
    outMsg.writeInt(sendersId.size());
    // write every id
    for (vector<int>::const_iterator it = sendersId.begin(); it != sendersId.end(); ++it) {
        outMsg.writeInt(*it);
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_DEACTIVATE_NODE)) {
        return false;
    }

    return true;
}

}
