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
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/
#include "headers.h"
#include <sstream>
#include "behaviour-rsu.h"

namespace baseapp {
namespace application {

std::string PrintHeader(Header* header) {
    return PrintHeader(*header);
}

std::string PrintHeader(Header& header) {
    std::ostringstream outstr;
    header.Print(outstr);
    return outstr.str();
}

// TransaidHeader
TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, CamInfo* message, int msgSize):
    m_camInfo(message), m_denmInfo(nullptr), m_cpmInfo(nullptr),
    m_mapInfo(nullptr), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(nullptr), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, DenmInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(message), m_cpmInfo(nullptr),
    m_mapInfo(nullptr), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(nullptr), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, CpmInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(nullptr), m_cpmInfo(message),
    m_mapInfo(nullptr), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(nullptr), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, MapInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(nullptr), m_cpmInfo(nullptr),
    m_mapInfo(message), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(nullptr), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, McmVehicleInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(nullptr), m_cpmInfo(nullptr),
    m_mapInfo(nullptr), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(message), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, McmRsuInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(nullptr), m_cpmInfo(nullptr),
    m_mapInfo(nullptr), m_mcmRsuInfo(message), m_mcmVehicleInfo(nullptr), m_iviInfo(nullptr),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::TransaidHeader(ProtocolId pid, MessageType msgType, IviInfo* message, int msgSize):
    m_camInfo(nullptr), m_denmInfo(nullptr), m_cpmInfo(nullptr),
    m_mapInfo(nullptr), m_mcmRsuInfo(nullptr), m_mcmVehicleInfo(nullptr), m_iviInfo(message),
    m_protocolId(pid), m_messageType(msgType), m_realMessageSize(msgSize)
{};

TransaidHeader::~TransaidHeader() {
    delete m_camInfo;
    delete m_denmInfo;
    delete m_cpmInfo;
    delete m_mapInfo;
    delete m_mcmRsuInfo;
    delete m_mcmVehicleInfo;
    delete m_iviInfo;
}

uint32_t TransaidHeader::GetSerializedSize(void) const {
    if (m_messageType == TRANSAID_CAM)  {
        return 2 + sizeof(m_camInfo);
    } else if (m_messageType == TRANSAID_DENM) {
        return 2 + sizeof(m_denmInfo);
    } else if (m_messageType == TRANSAID_CPM) {
        return 2 + sizeof(m_cpmInfo);
    } else if (m_messageType == TRANSAID_MAP) {
        return 2 + sizeof(m_mapInfo);
    } else if (m_messageType == TRANSAID_MCM_VEHICLE) {
        return 2 + sizeof(m_mcmVehicleInfo);
    } else if (m_messageType == TRANSAID_MCM_RSU) {
        return 2 + sizeof(m_mcmRsuInfo);
    } else if (m_messageType == TRANSAID_IVI) {
        return 2 + sizeof(m_iviInfo);
    } else {
        return 2;
    }
};

void TransaidHeader::Print(std::ostream& os) const {
    os << " [TransaidHeader] PId:" << m_protocolId << ", MsgTypeId:" << m_messageType << ", size:" << GetSerializedSize();
};

std::string TransaidHeader::Name() const {
    return "TransaidHeader";
};

MessageType TransaidHeader::getMessageType() const {
    return m_messageType;
}

const TransaidHeader::CamInfo* TransaidHeader::getCamInfo() const {
    return m_camInfo;
}

const TransaidHeader::DenmInfo* TransaidHeader::getDenmInfo() const {
    return m_denmInfo;
}

const TransaidHeader::CpmInfo* TransaidHeader::getCpmInfo() const {
    return m_cpmInfo;
}

const TransaidHeader::McmRsuInfo* TransaidHeader::getMcmRsuInfo() const {
    return m_mcmRsuInfo;
}

const TransaidHeader::McmVehicleInfo* TransaidHeader::getMcmVehicleInfo() const {
    return m_mcmVehicleInfo;
}

const TransaidHeader::MapInfo* TransaidHeader::getMapInfo() const {
    return m_mapInfo;
}

const TransaidHeader::IviInfo* TransaidHeader::getIviInfo() const {
    return m_iviInfo;
}

int TransaidHeader::getMessageRealSize() const {
    return m_realMessageSize;
}

// TestHeader
uint16_t TestHeader::maxResponseTime = 100;

TestHeader::TestHeader(ProtocolId pid, MessageType msgType, const ResponseInfo& response) :
    m_protocolId(pid), m_messageType(msgType), m_message(response.message),
    m_stopEdge(response.stopEdge), m_stopPosition(response.stopPosition)
{};

TestHeader::TestHeader(ProtocolId pid, MessageType msgType, const std::string& message) :
    m_protocolId(pid), m_messageType(msgType), m_message(message),
    m_stopEdge(""), m_stopPosition(0)
{};

uint32_t
TestHeader::GetSerializedSize(void) const {
    return 2 + m_message.size();
};
void
TestHeader::Print(std::ostream& os) const {
    os << " [TestHeader] PId:" << m_protocolId << ", MsgTypeId:" << m_messageType << ", message:" << m_message << ", size:" << GetSerializedSize();
};
std::string
TestHeader::Name() const {
    return "TestHeader";
};
MessageType
TestHeader::getMessageType() const {
    return m_messageType;
}
std::string
TestHeader::getMessage() const {
    return m_message;
}

std::string
TestHeader::getStopEdge() const {
    return m_stopEdge;
}

double
TestHeader::getStopPosition() const {
    return m_stopPosition;
}

uint16_t
TestHeader::getMaxResponseTime() const {
    return maxResponseTime;
}


///CommHeader

CommHeader::CommHeader() {
    m_sourceType = m_destinationType = NT_ALL;
    m_sourceId = m_destinationId = ID_ALL;
    m_sourcePosition = Vector2D();
    m_protocolId = PID_SPEED;
    m_messageType = MT_ALL;
}

uint32_t CommHeader::GetSerializedSize(void) const {
    return SERIALIZED_SIZE;
}

void CommHeader::Print(std::ostream& os) const {
    os << "Pid=" << ToString(m_protocolId);
    os << " SRC:";
    if (m_sourceType == NT_RSU) {
        os << " Rsu";
    } else if (IsVehicle(m_sourceType)) {
        os << " Vehicle";
    } else {
        os << " ???";
    }
    os << " Id=" << m_sourceId << " Pos=(" << m_sourcePosition << ")";
    os << " DEST:";
    if (m_destinationType == NT_RSU) {
        os << " Rsu";
    } else if (m_destinationType == NT_ALL) {
        os << " All";
    } else if (IsVehicle(m_destinationType)) {
        os << " Vehicle";
    } else {
        os << " ???";
    }
    if (m_destinationId == ID_ALL) {
        os << " Id=All";
    } else {
        os << " Id=" << m_destinationId;
    }
    //os << " Message type " << m_messageType;
}

///BeaconHeader
BeaconHeader::BeaconHeader() {
    m_maxResponseTime = BehaviourRsu::TimeBeaconMin;
    m_direction = DIR_INVALID;
}

uint32_t BeaconHeader::GetSerializedSize(void) const {
    return SERIALIZED_SIZE;
}

void BeaconHeader::Print(std::ostream& os) const {
    os << "Beacon dir=" << m_direction << " maxResp=" << m_maxResponseTime;
}

///BeaconHeader
BeaconResponseHeader::BeaconResponseHeader() {
    m_sourceDirection = m_conformantDirection = DIR_INVALID;
    m_currentSpeed = m_avgSpeedLow = m_avgSpeedHigh = SPD_INVALID;
    m_lastMessage = false;
}

uint32_t BeaconResponseHeader::GetSerializedSize(void) const {
    return SERIALIZED_SIZE;
}

void BeaconResponseHeader::Print(std::ostream& os) const {
    os << "dir=" << m_sourceDirection << " curSPD=" << m_currentSpeed << " avgSPDS=" << m_avgSpeedLow << " avgSPDH="
       << m_avgSpeedHigh << (m_lastMessage ? " last message." : "");
}

///BeaconHeader
NoLongerConformantHeader::NoLongerConformantHeader() {
    m_sourceDirection = m_conformantDirection = DIR_INVALID;
}

uint32_t NoLongerConformantHeader::GetSerializedSize(void) const {
    return SERIALIZED_SIZE;
}

void NoLongerConformantHeader::Print(std::ostream& os) const {
    os << "dir=" << m_sourceDirection;
}

} /* namespace application */
} /* namespace protocol */
