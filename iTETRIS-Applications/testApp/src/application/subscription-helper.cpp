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


#include "subscription-helper.h"
#include "../app-commands-subscriptions-constants.h"

namespace testapp
{
	namespace application
	{
		using namespace tcpip;

		SubscriptionHolder * SubscriptionHelper::SetCamArea(const Circle & area)
		{
			Storage* request = new Storage();

			request->writeUnsignedByte(1 + 1 + 1 + 4 + 4 + 4 + 4 + 1);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_SET_CAM_AREA);
			request->writeFloat(area.x);
			request->writeFloat(area.y);
			request->writeFloat(area.radius);
			request->writeFloat(1);
			request->writeUnsignedByte(0);

			return new SubscriptionHolder(SUB_SET_CAM_AREA, request);
		}

		SubscriptionHolder * SubscriptionHelper::ReturnCarsInZone(const Circle & area)
		{
			Storage* request = new Storage();

			request->writeUnsignedByte(1 + 1 + 1 + 4 + 4 + 4);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_RETURNS_CARS_IN_ZONE);
			request->writeFloat(area.x);
			request->writeFloat(area.y);
			request->writeFloat(area.radius);

			return new SubscriptionHolder(SUB_RETURNS_CARS_IN_ZONE, request);
		}

		SubscriptionHolder * SubscriptionHelper::GetMobilityInformation(const std::vector<int> * ids)
		{
			Storage* request = new Storage();
			if (ids == NULL)
				request->writeUnsignedByte(1 + 1 + 1 + 1);
			else
				request->writeUnsignedByte(1 + 1 + 1 + 1 + 2 + 4 * ids->size());
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_MOBILITY_INFORMATION);
			if (ids == NULL)
				request->writeUnsignedByte(VALUE__ALL_ID);
			else
			{
				request->writeUnsignedByte(VALUE__LIST_ID);
				request->writeShort(ids->size());
				for (std::vector<int>::const_iterator it = ids->begin(); it != ids->end(); ++it)
					request->writeInt(*it);
			}
			return new SubscriptionHolder(SUB_MOBILITY_INFORMATION, request);
		}

		SubscriptionHolder * SubscriptionHelper::GetTrafficLightInformation(const Vector2D * position)
		{
			Storage* request = new Storage();
			if (position == NULL)
				request->writeUnsignedByte(1 + 1 + 1 + 1);
			else
				request->writeUnsignedByte(1 + 1 + 1 + 1 + 2 * 4);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_TRAFFIC_LIGHT_INFORMATION);
			if (position == NULL)
				request->writeUnsignedByte(VALUE__NODE_POS);
			else
			{
				request->writeUnsignedByte(VALUE__SELECT_POS);
				request->writeFloat(position->x);
				request->writeFloat(position->y);
			}
			return new SubscriptionHolder(SUB_TRAFFIC_LIGHT_INFORMATION, request);
		}

		SubscriptionHolder * SubscriptionHelper::SetTrafficLight(const std::string & trafficLightId,
				const std::string & newStatus)
		{
			Storage* request = new Storage();
			int size = 2 * 4 + trafficLightId.size() + newStatus.size();
			request->writeUnsignedByte(1 + 1 + 1 + 1 + 1 + size);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_CMD_TRAFF_SIM);
			request->writeUnsignedByte(0x00); //HEADER__APP_MSG_TYPE not used
			request->writeUnsignedByte(VALUE_SET_TRAFFIC_LIGHT);
			request->writeString(trafficLightId);
			request->writeString(newStatus);
			return new SubscriptionHolder(SUB_APP_CMD_TRAFF_SIM, request);
		}

		SubscriptionHolder * SubscriptionHelper::ReceiveUnicast(const int destinationId, const int sourceId)
		{
			return ReceiveUnicast(destinationId, std::vector<int>(1, sourceId));
		}

		SubscriptionHolder * SubscriptionHelper::ReceiveUnicast(const int destinationId, const std::vector<int> & sourceIds)
		{
			Storage* request = new Storage();

			int variableSize = 0;
			if (sourceIds.size() > 0)
				variableSize = 2 + 4 * sourceIds.size();
			request->writeUnsignedByte(1 + 1 + 1 + 1 + 1 + 4 + 2 + 1 + 1 + variableSize);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_MSG_RECEIVE);
			// Message type, not used by iCS for unicast
			request->writeUnsignedByte(0x01);
			// Only destID
			request->writeUnsignedByte(0x04);
			//destination of the unicast message
			request->writeInt(destinationId);
			//payload length.. useless
			request->writeShort(0);
			request->writeUnsignedByte(EXT_HEADER_TYPE_UNICAST);
			if (sourceIds.size() > 0)
			{
				request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_IDs);
				request->writeShort(sourceIds.size());
				for (std::vector<int>::const_iterator it = sourceIds.begin(); it != sourceIds.end(); ++it)
					request->writeInt(*it);
			} else
			{
				//use a different value to select any source
				request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_IDs + 1);
			}

			return new SubscriptionHolder(SUB_APP_MSG_RECEIVE, request);
		}

		SubscriptionHolder * SubscriptionHelper::ReceiveGeobroadcast(const int messageId, const Circle * propagationCircle)
		{
			Storage* request = new Storage();
			int variableSize = 0;
			if (propagationCircle != NULL)
				variableSize = 1 + 1 + 4 + 4 + 4;
			request->writeUnsignedByte(1 + 1 + 1 + 1 + 1 + 2 + 1 + 1 + variableSize);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_MSG_RECEIVE);
			// Message type, used as message Id
			request->writeUnsignedByte(messageId);
			// Do not set anything
			request->writeUnsignedByte(0x00);
			//payload length.. useless
			request->writeShort(0);
			request->writeUnsignedByte(EXT_HEADER_TYPE_GEOBROADCAST);
			if (propagationCircle != NULL)
			{
				request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_AREAs);
				//number areas. Use only one
				request->writeUnsignedByte(0x01);
				request->writeUnsignedByte(EXT_HEADER__CIRCLE);
				request->writeFloat(propagationCircle->x);
				request->writeFloat(propagationCircle->y);
				request->writeFloat(propagationCircle->radius);
			} else
			{
				//use a different value to select any area
				request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_AREAs + 1);
			}
			delete propagationCircle;

			return new SubscriptionHolder(SUB_APP_MSG_RECEIVE, request);
		}

		SubscriptionHolder * SubscriptionHelper::SendUnicast(const int sourceId, const short payloadLength,
				const int messageId, const int destinationId, const std::string & extra, const double time)
		{
			return SendUnicast(sourceId, payloadLength, messageId, std::vector<int>(1, destinationId), extra, time);
		}

		SubscriptionHolder * SubscriptionHelper::SendUnicast(const int sourceId, const short payloadLength,
				const int messageId, const std::vector<int> & destinationIds, const std::string & extra, const double time)
		{
			Storage* request = new Storage();
			int payloadSize = 1 + 4 + extra.size();
			int variableSize = 0;
			if (destinationIds.size() > 0)
				variableSize = (4 + 2 + payloadSize) * destinationIds.size();
			request->writeUnsignedByte(1 + 1 + 1 + 1 + 1 + 1 + 1 + 4 + 1 + 8 + 2 + 4 + 1 + 1 + 1 + variableSize);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_MSG_SEND);
			// Message type, not used in a send
			request->writeUnsignedByte(33);
			// in bits, it is: 11111 : the preferred techno, comm profile, senderID, the message lifetime and the precise timestep
			request->writeUnsignedByte(0x1F);
			request->writeUnsignedByte(0xFF); //preferred techno
			request->writeUnsignedByte(0x00); //Comm profile. Set to 0 or it will not send messages of mobile nodes
			request->writeInt(sourceId);    //senderID
			request->writeUnsignedByte(2); //message lifetime
			request->writeDouble(time); //precise timestep
			request->writeShort(payloadLength);
			// Message sequence number. used as message id
			request->writeInt(messageId);
			request->writeUnsignedByte(EXT_HEADER_TYPE_UNICAST); // CommMode
			request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_IDs); // CommMode
			request->writeUnsignedByte(destinationIds.size());
			if (destinationIds.size() > 0)
			{
				for (std::vector<int>::const_iterator it = destinationIds.begin(); it != destinationIds.end(); ++it)
				{
					request->writeInt(*it);
					//payload size
					request->writeShort(payloadSize);
					request->writeUnsignedByte(EXT_HEADER__EXTRA);
					request->writeString(extra);
				}
			}

			return new SubscriptionHolder(SUB_APP_MSG_SEND, request);
		}

		SubscriptionHolder * SubscriptionHelper::SendGeobroadcast(const int sourceId, const short payloadLength,
				const int messageId, const Circle & propagationCircle, const std::string & extra, const double time)
		{
			Storage* request = new Storage();
			int payloadSize = 1 + 4 + extra.size();
			request->writeUnsignedByte(
					1 + 1 + 1 + 1 + 1 + 1 + 1 + 4 + 1 + 8 + 2 + 4 + 1 + 1 + 1 + 1 + 4 + 4 + 4 + 2 + payloadSize);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_MSG_SEND);
			// Message type, not used in a send
			request->writeUnsignedByte(33);
			// in bits, it is: 11111 : the preferred techno, comm profile, senderID, the message lifetime and the precise timestep
			request->writeUnsignedByte(0x1F);
			request->writeUnsignedByte(0xFF); //preferred techno
			request->writeUnsignedByte(0x00); //comm profile
			request->writeInt(sourceId);  //senderID
			request->writeUnsignedByte(2); //the message lifetime
			request->writeDouble(time); //precise timestep
			request->writeShort(payloadLength);
			// Message sequence number. used as message id
			request->writeInt(messageId);
			request->writeUnsignedByte(EXT_HEADER_TYPE_GEOBROADCAST);
			request->writeUnsignedByte(EXT_HEADER__VALUE_BLOCK_AREAs);
			// Number areas. Use only one
			request->writeUnsignedByte(0x01);
			request->writeUnsignedByte(EXT_HEADER__CIRCLE);
			request->writeFloat(propagationCircle.x);
			request->writeFloat(propagationCircle.y);
			request->writeFloat(propagationCircle.radius);
			//payload size
			request->writeShort(payloadSize);
			request->writeUnsignedByte(EXT_HEADER__EXTRA);
			request->writeString(extra);

			return new SubscriptionHolder(SUB_APP_MSG_SEND, request);
		}

		SubscriptionHolder * SubscriptionHelper::SumoTraciCommand(const int executionId,
				const tcpip::Storage & commandStorage)
		{
			Storage* request = new Storage();
			request->writeUnsignedByte(3 * 1 + 4 + commandStorage.size());
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_SUMO_TRACI_COMMAND);
			request->writeInt(executionId);
			request->writeStorage(commandStorage);
			return new SubscriptionHolder(SUB_SUMO_TRACI_COMMAND, request);
		}

		SubscriptionHolder * SubscriptionHelper::GetNodeClass(const int nodeId)
		{
			Storage* request = new Storage();
			request->writeUnsignedByte(5 * 1 + 4);
			request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
			request->writeUnsignedByte(SUB_APP_RESULT_TRAFF_SIM);
			request->writeUnsignedByte(0); 	//	APP_MSG_TYPE not used..
			request->writeUnsignedByte(VALUE_GET_VEHICLE_CLASS); //cmdMode
			request->writeInt(nodeId); //Command
			return new SubscriptionHolder(SUB_APP_RESULT_TRAFF_SIM, request);
		}

        SubscriptionHolder * SubscriptionHelper::GetReceivedCamInfo()
        {
            Storage* request = new Storage();

            request->writeUnsignedByte(1 + 1 + 1);
            request->writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
            request->writeUnsignedByte(SUB_RECEIVED_CAM_INFO);

            return new SubscriptionHolder(SUB_RECEIVED_CAM_INFO, request);
        }

	} /* namespace application */
} /* namespace protocol */
