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

namespace protocol {
namespace application {

std::string PrintHeader(Header* header) {
    return PrintHeader(*header);
}

std::string PrintHeader(Header& header) {
    std::ostringstream outstr;
    header.Print(outstr);
    return outstr.str();
}

///CommHeader

CommHeader::CommHeader() {
    m_sourceType = m_destinationType = NT_ALL;
    m_sourceId = m_destinationId = ID_ALL;
    m_sourcePosition = Vector2D();
    m_protocolId = PID_SPEED;
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
