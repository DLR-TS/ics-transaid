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

#include "traci-helper.h"
#include "log/log.h"
#include <sstream>

namespace baseapp {
namespace application {
TraciHelper TraciHelper::m_instance;
int TraciHelper::m_executionIdCounter = 0;
std::map<const int, int> TraciHelper::m_valueMap = TraciHelper::createValueMap();

TraciHelper::TraciHelper() {
}

TraciHelper::~TraciHelper() {
}

int TraciHelper::AddCommand(const Command& command) {
    int current = ++m_executionIdCounter;
    m_instance.m_commandList[current] = command;
    return current;
}

int TraciHelper::AddGetCommand(const int commandId, const int variableId, const std::string& objId) {
    Command c(commandId, variableId, objId, GET_COMMAND);
    return AddCommand(c);
}

int TraciHelper::AddSetCommand(const int commandId, const int variableId, const std::string& objId) {
    CommandType t = SET_COMMAND;
    if (commandId == libsumo::CMD_GET_VEHICLE_VARIABLE ||
            commandId == libsumo::CMD_GET_SIM_VARIABLE) {
        /// Hack for retrieving responses with input params (e.g., driving distance).
        /// TODO: allow parameters for GET_COMMANDS, @see AddTraciSubscription()
        t = GET_COMMAND;
    }
    Command c(commandId, variableId, objId, t);
    return AddCommand(c);
}

bool TraciHelper::RemoveCommand(const int executionId) {
    return m_instance.m_commandList.erase(executionId) > 0;
}

bool TraciHelper::HasCommand(const int executionId) {
    return m_instance.m_commandList.find(executionId) != m_instance.m_commandList.end();
}

bool TraciHelper::GetCommand(const int executionId, Command& command) {
    if (HasCommand(executionId)) {
        command = m_instance.m_commandList[executionId];
        return true;
    }
    return false;
}

bool TraciHelper::GetCommandId(const int executionId, int& commandId) {
    Command tmp;
    if (GetCommand(executionId, tmp)) {
        commandId = tmp.commandId;
        return true;
    }
    return false;
}

bool TraciHelper::GetCommandType(const int executionId, CommandType& type) {
    Command tmp;
    if (GetCommand(executionId, tmp)) {
        type = tmp.type;
        return true;
    }
    return false;
}

bool TraciHelper::IsGetCommand(const int executionId) {
    Command tmp;
    if (GetCommand(executionId, tmp)) {
        return tmp.type == GET_COMMAND;
    }
    return false;
}

bool TraciHelper::IsGetCommand(const Command& command) {
    return command.type == GET_COMMAND;
}

bool TraciHelper::IsSetCommand(const int executionId) {
    Command tmp;
    if (GetCommand(executionId, tmp)) {
        return tmp.type == SET_COMMAND;
    }
    return false;
}

bool TraciHelper::IsSetCommand(const Command& command) {
    return command.type == SET_COMMAND;
}

bool TraciHelper::VerifyCommand(const int executionId, tcpip::Storage& sumoReply) {
    Command command;
    if (!GetCommand(executionId, command)) {
        std::ostringstream oss;
        oss << "ExecutionId " << executionId << " not found";
        Log::Write(oss, kLogLevelError);
        return false;
    }
    int cmdLength;
    int cmdId;
    int resultType;
    int cmdStart;
    std::string msg;

    try {
        cmdStart = sumoReply.position();
        cmdLength = sumoReply.readUnsignedByte();
        cmdId = sumoReply.readUnsignedByte();
        if (cmdId != command.commandId) {
            std::ostringstream oss;
            oss << "SUMO #Error: received status response to command: " << Log::toHex(cmdId, 2) << " but expected: "
                << command.commandId;
            Log::Write(oss, kLogLevelError);
            return false;
        }
        resultType = sumoReply.readUnsignedByte();
        msg = sumoReply.readString();
    } catch (std::invalid_argument& e) {
        Log::Write("SUMO #Error: an exception was thrown while reading result state message", kLogLevelError);
        return false;
    }

    switch (resultType) {
        case SUMO_RTYPE_ERR: {
            std::ostringstream oss;
            oss << "SUMO answered with error to command (" << Log::toHex(cmdId, 2) << "), [description: " << msg << "]";
            Log::Write(oss, kLogLevelError);
            return false;
        }
        case SUMO_RTYPE_NOTIMPLEMENTED: {
            std::ostringstream oss;
            oss << "SUMO: Sent command is not implemented (" << Log::toHex(cmdId, 2) << "), [description: " << msg << "]";
            Log::Write(oss, kLogLevelError);
            return false;
        }
        case SUMO_RTYPE_OK: {
            std::ostringstream oss;
            oss << "SUMO Command acknowledged (" << Log::toHex(cmdId, 2) << "), [description: " << msg << "]";
            Log::WriteLog(oss);
            break;
        }
        default: {
            std::ostringstream oss;
            oss << "SUMO Answered with unknown result code(" << resultType << ") to command(" << Log::toHex(cmdId, 2)
                << "), [description: " << msg << "]";
            Log::WriteLog(oss);
            return false;
        }
    }

    if ((cmdStart + cmdLength) != sumoReply.position()) {
        std::ostringstream oss;
        oss << "SUMO #Error: command at position " << cmdStart << " has wrong length";
        Log::Write(oss, kLogLevelError);
        return false;
    }
    return true;
}

bool TraciHelper::BeginValueRetrievalFromCommand(const int executionId, tcpip::Storage& sumoReply, int& variableId,
        std::string& objId, int& valueType, bool alsoVerify) {
    if (alsoVerify && !VerifyCommand(executionId, sumoReply)) {
        return false;
    }
    Command command;
    GetCommand(executionId, command);
    int length = sumoReply.readUnsignedByte();
    if (length == 0) {
        length = sumoReply.readInt();
    }

    //int respLength = inMsg.readInt();
    int cmdId = sumoReply.readUnsignedByte();
    if (cmdId != (command.commandId + 0x10)) {
        std::ostringstream oss;
        oss << "#Error: received response with command id: " << Log::toHex(cmdId, 2) << " but expected: "
            << Log::toHex(command.commandId + 0x10, 2);
        Log::Write(oss, kLogLevelError);
        return false;
    }
    variableId = sumoReply.readUnsignedByte(); // variable id
    objId = sumoReply.readString(); // object id
    valueType = sumoReply.readUnsignedByte(); // value type
    return true;
}

void TraciHelper::ValueGetStorage(tcpip::Storage& sumoQuery, const int commandId, const int variableId,
                                  const std::string& objId) {
    sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + objId.length()); // command length
    sumoQuery.writeUnsignedByte(commandId); // command id
    sumoQuery.writeUnsignedByte(variableId); // variable id
    sumoQuery.writeString(objId); // object id
}

int TraciHelper::AddValueGetStorage(tcpip::Storage& sumoQuery, const int commandId, const int variableId,
                                    const std::string& objId) {
    ValueGetStorage(sumoQuery, commandId, variableId, objId);
    return AddGetCommand(commandId, variableId, objId);
}

void TraciHelper::ValueSetStorage(tcpip::Storage& sumoQuery, const int commandId, const int variableId,
                                  const std::string& objId, const int newValueType, tcpip::Storage& newValueStorage) {
    sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + objId.length() + 1 + newValueStorage.size()); // command length
    sumoQuery.writeUnsignedByte(commandId); // command id
    sumoQuery.writeUnsignedByte(variableId); // variable id
    sumoQuery.writeString(objId); // object id
    sumoQuery.writeUnsignedByte(newValueType); //type of the value
    sumoQuery.writeStorage(newValueStorage);	//new value inside a storage
}

int TraciHelper::AddValueSetStorage(tcpip::Storage& sumoQuery, const int commandId, const int variableId,
                                    const std::string& objId, const int newValueType, tcpip::Storage& newValueStorage) {
    ValueSetStorage(sumoQuery, commandId, variableId, objId, newValueType, newValueStorage);
    return AddSetCommand(commandId, variableId, objId);
}

int TraciHelper::AddValueSetStorageGet(tcpip::Storage& sumoQuery, const int commandId, const int variableId,
                                       const std::string& objId, const int newValueType, tcpip::Storage& newValueStorage) {
    ValueSetStorage(sumoQuery, commandId, variableId, objId, newValueType, newValueStorage);
    return AddGetCommand(commandId, variableId, objId);
}

int TraciHelper::getValueType(int varID) {
    if (m_valueMap.find(varID) == m_valueMap.end()) {
        return 0;
    }
    return m_valueMap[varID];
}

bool TraciHelper::isStoppedParking(int stopState) {
    return (stopState & 2) == 2;
}

} /* namespace application */
} /* namespace protocol */
