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

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "tcpip/storage.h"
#include "log/log.h"
#include "vector.h"
#include <cmath>
#include <stdint.h>
#include <map>

namespace testapp
{

	typedef enum
	{
		NUMBER = 0x01,
		SPEED = 0x02,
		TIME = 0x03,
		MAX_DISTANCE = 0x04,
		MIN_DISTANCE = 0x05,
		SPACE_MEAN_SPEED = 0x06,
		SPEED_TIME = 0x07,
		SPEED_TIME_FILTER = 0x8,
		SPEED_TIME_SINGLE = 0x9
	} DataId;

	/**
	 * Data returned to iCS
	 * ValueMap map of data to be returned for a direction. key what data it is. value the value of the data
	 * DirectionValueMap map of data for every direction. ket id of the direction. value the data for that direction
	 */
	typedef std::map<DataId, double> ValueMap;
	typedef std::map<std::string, ValueMap> DirectionValueMap;

	struct SubscriptionHolder
	{
			SubscriptionHolder(const int subscriptionType, tcpip::Storage * request, const bool toUnsubscribe) :
					m_subscriptionType(subscriptionType), m_toUnsubscribe(toUnsubscribe), m_request(request)
			{
			}
			SubscriptionHolder(const int subscriptionType, tcpip::Storage * request) :
					m_subscriptionType(subscriptionType), m_toUnsubscribe(false), m_request(request)
			{
			}
			~SubscriptionHolder()
			{
				delete m_request;
			}
			int m_subscriptionType;
			bool m_toUnsubscribe;
			tcpip::Storage * m_request;
	}typedef SubscriptionHolder;

	struct Circle
	{
			Circle() :
					x(0), y(0), radius(0)
			{
			}
			Circle(float a_x, float a_y, float a_radius) :
					x(a_x), y(a_y), radius(a_radius)
			{
			}
			Circle(application::Vector2D position, float a_radius) :
					radius(a_radius)
			{
				x = position.x;
				y = position.y;
			}

			float x;
			float y;
			float radius;
	}typedef Circle;

	const double INVALID_DIRECTION = 1000;

	struct MobilityInfo
	{
			MobilityInfo(const int id) :
					id(id)
			{
				speed = acceleration = 0;
				direction = INVALID_DIRECTION;
				isMobile = false;
			}

			MobilityInfo(tcpip::Storage & storage)
			{
				id = storage.readInt();
                nsId = storage.readInt(); //added the ns3 ID support
                tsId = storage.readString(); //added the SUMO ID support
				double x = storage.readFloat();
				double y = storage.readFloat();
				//Don not read inline x and y because it will read y before...
				position = application::Vector2D(x, y);
				isMobile = storage.readUnsignedByte() == 0 ? false : true;
				if (isMobile)
				{
					speed = storage.readFloat();
					direction = ConvertDirection(storage.readFloat());
					acceleration = storage.readFloat();
					lane = storage.readString();
				}
			}

			int id;
            int nsId;
            std::string tsId;
			application::Vector2D position;
			bool isMobile;
			float speed;
			float direction;
			float acceleration;
			std::string lane;
			static double ConvertDirection(const double direction)
			{
				double result = direction - 90.0;
				if (result <= -180.0)
					result += 360.0;
				return result;
			}
	}typedef MobilityInfo;

	struct Message
	{
			Message()
			{
				m_destinationId = m_messageId = 0;
				m_snr = NAN;
			}
			int m_destinationId;
			int m_messageId;
			std::string m_extra;
			double m_snr;
	}typedef Message;

    struct CAMdata
    {
        int senderID;
        application::Vector2D position;
        int generationTime;
        int stationType;
        float speed;
        float angle;
        float acceleration;
        float length;
        float width;
        int ligths;
        std::string laneID;
        std::string edgeID;
        std::string junctionID;
    }typedef CAMdata;

} /* namespace protocol */

#endif /* STRUCTS_H_ */
