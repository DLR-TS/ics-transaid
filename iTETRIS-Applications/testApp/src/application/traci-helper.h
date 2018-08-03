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

#ifndef SRC_APPLICATION_TRACI_HELPER_H_
#define SRC_APPLICATION_TRACI_HELPER_H_

#include <map>
#include "tcpip/storage.h"

// result type: Ok
#define SUMO_RTYPE_OK 0x00
// result type: not implemented
#define SUMO_RTYPE_NOTIMPLEMENTED 0x01
// result type: error
#define SUMO_RTYPE_ERR 0xFF
// unsigned byte
#define TYPE_UBYTE 0x07
// signed byte
#define TYPE_BYTE 0x08
// 32 bit integer
#define TYPE_INTEGER 0x09
// float
#define TYPE_FLOAT 0x0A
// double
#define TYPE_DOUBLE 0x0B
// string
#define TYPE_STRING 0x0C
// command: get vehicle variable
#define GET_VEHICLE_VARIABLE 0xa4
// variable: speed
#define VARIABLE_SPEED 0x40
// command: Change Vehicle State
#define CHANGE_VEHICLE_STATE 0xc4
// variable: max speed
#define VARIABLE_MAX_SPEED 0x41

namespace testapp
{
	namespace application
	{
		typedef enum
		{
			SET_COMMAND, GET_COMMAND
		} CommandType;

		struct Command
		{
				Command() {};
				Command(int id, CommandType t)
				{
					commandId = id;
					type = t;
				}
				int commandId;
				CommandType type;

		}typedef Command;

		/**
		 * Helper class for the subscription SumoTraciCommand
		 * It contains a map to identify which is the current reply
		 */
		class TraciHelper
		{
			public:
				/**
				 * @brief Adds a command the the map and returns an id to identify is later
				 */
				static int AddCommand(const Command & command);
				/**
				 * @brief Adds a value retrieval command the the map and returns an id to identify is later
				 */
				static int AddGetCommand(const int commandId);
				/**
				 * @brief Adds a state changing command the the map and returns an id to identify is later
				 */
				static int AddSetCommand(const int commandId);
				/**
				 * @brief Removes the id from the map
				 */
				static bool RemoveCommand(const int executionId);
				/**
				 * @brief Checks if the id is inside the map
				 */
				static bool HasCommand(const int executionId);
				/**
				 * @brief Get a command
				 * @param[in] executionId Id of the command
				 * @param[out] command The command if the methods returns true
				 * @return true if the command was found. false otherwise
				 */
				static bool GetCommand(const int executionId, Command & command);
				/**
				 * @brief Get a commandId
				 * @param[in] executionId Id of the command
				 * @param[out] commandId The commandId if the methods returns true
				 * @return true if the command was found. false otherwise
				 */
				static bool GetCommandId(const int executionId, int & commandId);
				/**
				 * @brief Get a command type
				 * @param[in] executionId Id of the command
				 * @param[out] type The type of the command if the methods returns true
				 * @return true if the command was found. false otherwise
				 */
				static bool GetCommandType(const int executionId, CommandType & type);
				/**
				 * @brief Checks if the id is a value retrieval command
				 */
				static bool IsGetCommand(const int executionId);
				/**
				 * @brief Checks if the command is a value retrieval command
				 */
				static bool IsGetCommand(const Command & command);
				/**
				 * @brief Checks if the id is a state change command
				 */
				static bool IsSetCommand(const int executionId);
				/**
				 * @brief Checks if the command is a state change command
				 */
				static bool IsSetCommand(const Command & command);
				/**
				 * @brief Verifies the sumo reply of a command
				 * @param[in] executionId Id of the command
				 * @param[in] sumoReply The reply from sumo
				 * @return true if the verify succeeded. false otherwise
				 */
				static bool VerifyCommand(const int executionId, tcpip::Storage & sumoReply);
				/**
				 * @brief Starts the retrieve process if a value retrieval command
				 * @brief After the successful execution of this method in sumoReply can be read the data returned by sumo
				 * @param[in] executionId Id of the command
				 * @param[in] sumoReply The reply from sumo
				 * @param[out] variableId The id of the variable retrieved
				 * @param[out] objId The id of the object
				 * @param[out] valueType The type of the variable to retrieve
				 * @param[in] alsoVerify If the method should call VerifyCommand. True by defaul
				 * @return true if the method succeeded. false otherwise
				 */
				static bool BeginValueRetrievalFromCommand(const int executionId, tcpip::Storage & sumoReply, int& variableId,
						std::string & objId, int& valueType, bool alsoVerify = true);
				/**
				 * @brief Creates the request for a value retrieval command to send to sumo via traci
				 * @param[out] sumoQuery The request to be sent to sumo
				 * @param[in] commandId The commandId of the request
				 * @param[in] variableId The variableId of the request
				 * @param[in] objId The objectIf of the request
				 */
				static void ValueGetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
						const std::string & objId);
				/**
				 * @brief As ValueGetStorage but it also adds the command to the map
				 * @brief Creates the request for a value retrieval command to send to sumo via traci
				 * @param[out] sumoQuery The request to be sent to sumo
				 * @param[in] commandId The commandId of the request
				 * @param[in] variableId The variableId of the request
				 * @param[in] objId The objectIf of the request
				 * @return The id of the command inside the map
				 */
				static int AddValueGetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
						const std::string & objId);
				/**
				 * @brief Creates the request for a state change to send to sumo via traci
				 * @param[out] sumoQuery The request to be sent to sumo
				 * @param[in] commandId The commandId of the request
				 * @param[in] variableId The variableId of the request
				 * @param[in] objId The objectIf of the request
				 * @param[in] newValueType The type of the valiable to be set
				 * @param[in] newValueStorage The new value to be set, already codified in a storage
				 */
				static void ValueSetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
						const std::string & objId, const int newValueType, const tcpip::Storage & newValueStorage);
				/**
				 * @brief As ValueSetStorage but it also adds the command to the map
				 * @brief Creates the request for a state change to send to sumo via traci
				 * @param[out] sumoQuery The request to be sent to sumo
				 * @param[in] commandId The commandId of the request
				 * @param[in] variableId The variableId of the request
				 * @param[in] objId The objectIf of the request
				 * @param[in] newValueType The type of the valiable to be set
				 * @param[in] newValueStorage The new value to be set, already codified in a storage
				 * @return The id of the command inside the map
				 */
				static int AddValueSetStorage(tcpip::Storage & sumoQuery, const int commandId, const int variableId,
						const std::string & objId, const int newValueType, const tcpip::Storage & newValueStorage);


                /// @brief return the value type of the given traci variable
				/// @todo  If a new variable type is to be used, it has to be added in createValueMap()
                static int getValueType(int varID);

			private:
				TraciHelper();
				virtual ~TraciHelper();
				static TraciHelper m_instance;

				std::map<const int, Command> m_commandList;

		        static int m_executionIdCounter;

		        static std::map<const int, int> createValueMap() {
		            std::map<const int, int> ret;
                    ret[VARIABLE_SPEED] = TYPE_DOUBLE;
                    ret[VARIABLE_MAX_SPEED] = TYPE_DOUBLE;
		            return ret;
		        }

                static std::map<const int, int> m_valueMap;
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* SRC_APPLICATION_TRACI_HELPER_H_ */
