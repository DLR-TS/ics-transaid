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

#include "traci-helper.h"
#include "log/log.h"
#include <sstream>

namespace baseapp
{
	namespace application
	{
		TraciHelper TraciHelper::m_instance;
		int TraciHelper::m_executionIdCounter = 0;
		std::map<const int, int> TraciHelper::m_valueMap = TraciHelper::createValueMap();

		TraciHelper::TraciHelper()
		{
		}

		TraciHelper::~TraciHelper()
		{
		}

		int TraciHelper::AddCommand(const Command & command)
		{
			int current = ++m_executionIdCounter;
			m_instance.m_commandList[current] = command;
			return current;
		}

		int TraciHelper::AddGetCommand(const int commandId, const int variableId, const std::string & objId)
		{
			Command c(commandId, variableId, objId, GET_COMMAND);
			return AddCommand(c);
		}

		int TraciHelper::AddSetCommand(const int commandId, const int variableId, const std::string & objId)
		{
		    CommandType t = SET_COMMAND;
		    if (variableId == DISTANCE_REQUEST) {
		        // Hack for retrieving driving distance response.
		        // TODO: allow parameters for GET_COMMANDS, @see AddTraciSubscription()
		        t = GET_COMMAND;
		    }
			Command c(commandId, variableId, objId, t);
			return AddCommand(c);
		}

		bool TraciHelper::RemoveCommand(const int executionId)
		{
			return m_instance.m_commandList.erase(executionId) > 0;
		}

		bool TraciHelper::HasCommand(const int executionId)
		{
			return m_instance.m_commandList.find(executionId) != m_instance.m_commandList.end();
		}

		bool TraciHelper::GetCommand(const int executionId, Command & command)
		{
			if (HasCommand(executionId))
			{
				command = m_instance.m_commandList[executionId];
				return true;
			}
			return false;
		}

		bool TraciHelper::GetCommandId(const int executionId, int & commandId)
		{
			Command tmp;
			if (GetCommand(executionId, tmp))
			{
				commandId = tmp.commandId;
				return true;
			}
			return false;
		}

		bool TraciHelper::GetCommandType(const int executionId, CommandType & type)
		{
			Command tmp;
			if (GetCommand(executionId, tmp))
			{
				type = tmp.type;
				return true;
			}
			return false;
		}

		bool TraciHelper::IsGetCommand(const int executionId)
		{
			Command tmp;
			if (GetCommand(executionId, tmp))
				return tmp.type == GET_COMMAND;
			return false;
		}

		bool TraciHelper::IsGetCommand(const Command & command)
		{
			return command.type == GET_COMMAND;
		}

		bool TraciHelper::IsSetCommand(const int executionId)
		{
			Command tmp;
			if (GetCommand(executionId, tmp))
				return tmp.type == SET_COMMAND;
			return false;
		}

		bool TraciHelper::IsSetCommand(const Command & command)
		{
			return command.type == SET_COMMAND;
		}

		bool TraciHelper::VerifyCommand(const int executionId, tcpip::Storage & sumoReply)
		{
			Command command;
			if (!GetCommand(executionId, command))
			{
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

			try
			{
				cmdStart = sumoReply.position();
				cmdLength = sumoReply.readUnsignedByte();
				cmdId = sumoReply.readUnsignedByte();
				if (cmdId != command.commandId)
				{
					std::ostringstream oss;
					oss << "SUMO #Error: received status response to command: " << Log::toHex(cmdId,2) << " but expected: "
							<< command.commandId;
					Log::Write(oss, kLogLevelError);
					return false;
				}
				resultType = sumoReply.readUnsignedByte();
				msg = sumoReply.readString();
			} catch (std::invalid_argument &e)
			{
				Log::Write("SUMO #Error: an exception was thrown while reading result state message", kLogLevelError);
				return false;
			}

			switch (resultType)
			{
			case SUMO_RTYPE_ERR:
			{
				std::ostringstream oss;
				oss << "SUMO answered with error to command (" << Log::toHex(cmdId,2) << "), [description: " << msg << "]";
				Log::Write(oss, kLogLevelError);
				return false;
			}
			case SUMO_RTYPE_NOTIMPLEMENTED:
			{
				std::ostringstream oss;
				oss << "SUMO: Sent command is not implemented (" << Log::toHex(cmdId,2) << "), [description: " << msg << "]";
				Log::Write(oss, kLogLevelError);
				return false;
			}
			case SUMO_RTYPE_OK:
			{
				std::ostringstream oss;
				oss << "SUMO Command acknowledged (" << Log::toHex(cmdId,2) << "), [description: " << msg << "]";
				Log::WriteLog(oss);
				break;
			}
			default:
			{
				std::ostringstream oss;
				oss << "SUMO Answered with unknown result code(" << resultType << ") to command(" << Log::toHex(cmdId,2)
						<< "), [description: " << msg << "]";
				Log::WriteLog(oss);
				return false;
			}
			}

			if ((cmdStart + cmdLength) != sumoReply.position())
			{
				std::ostringstream oss;
				oss << "SUMO #Error: command at position " << cmdStart << " has wrong length";
				Log::Write(oss, kLogLevelError);
				return false;
			}
			return true;
		}

		bool TraciHelper::BeginValueRetrievalFromCommand(const int executionId, tcpip::Storage & sumoReply, int& variableId,
				std::string & objId, int& valueType, bool alsoVerify)
		{
			if (alsoVerify && !VerifyCommand(executionId, sumoReply))
				return false;
			Command command;
			GetCommand(executionId, command);
			int length = sumoReply.readUnsignedByte();
			if (length == 0)
				length = sumoReply.readInt();

			//int respLength = inMsg.readInt();
			int cmdId = sumoReply.readUnsignedByte();
			if (cmdId != (command.commandId + 0x10))
			{
				std::ostringstream oss;
				oss << "#Error: received response with command id: " << Log::toHex(cmdId,2) << " but expected: "
						<< Log::toHex(command.commandId + 0x10,2);
				Log::Write(oss, kLogLevelError);
				return false;
			}
			variableId = sumoReply.readUnsignedByte(); // variable id
			objId = sumoReply.readString(); // object id
			valueType = sumoReply.readUnsignedByte(); // value type
			return true;
		}

		void TraciHelper::ValueGetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
				const std::string & objId)
		{
			sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + objId.length()); // command length
			sumoQuery.writeUnsignedByte(commandId); // command id
			sumoQuery.writeUnsignedByte(variableId); // variable id
			sumoQuery.writeString(objId); // object id
		}

		int TraciHelper::AddValueGetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
				const std::string & objId)
		{
			ValueGetStorage(sumoQuery, commandId, variableId, objId);
			return AddGetCommand(commandId, variableId, objId);
		}

		void TraciHelper::ValueSetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
				const std::string & objId, const int newValueType, tcpip::Storage & newValueStorage) {
			sumoQuery.writeUnsignedByte(1 + 1 + 1 + 4 + objId.length() + 1 + newValueStorage.size()); // command length
			sumoQuery.writeUnsignedByte(commandId); // command id
			sumoQuery.writeUnsignedByte(variableId); // variable id
			sumoQuery.writeString(objId); // object id
			sumoQuery.writeUnsignedByte(newValueType); //type of the value
			sumoQuery.writeStorage(newValueStorage);	//new value inside a storage
		}

		int TraciHelper::AddValueSetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
				const std::string & objId, const int newValueType, tcpip::Storage & newValueStorage)
		{
			ValueSetStorage(sumoQuery, commandId, variableId, objId, newValueType, newValueStorage);
			return AddSetCommand(commandId, variableId, objId);
		}


        int TraciHelper::getValueType(int varID) {
            if(m_valueMap.find(varID) == m_valueMap.end()) {
                return 0;
            }
            return m_valueMap[varID];
        }

	} /* namespace application */
} /* namespace protocol */
