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

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "tcpip/storage.h"
#include "log/log.h"
#include "vector.h"
#include <cmath>
#include <stdint.h>
#include <map>

namespace protocol {

typedef enum {
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

struct SubscriptionHolder {
    SubscriptionHolder(const int subscriptionType, tcpip::Storage* request, const bool toUnsubscribe) :
        m_subscriptionType(subscriptionType), m_toUnsubscribe(toUnsubscribe), m_request(request) {
    }
    SubscriptionHolder(const int subscriptionType, tcpip::Storage* request) :
        m_subscriptionType(subscriptionType), m_toUnsubscribe(false), m_request(request) {
    }
    ~SubscriptionHolder() {
        delete m_request;
    }
    int m_subscriptionType;
    bool m_toUnsubscribe;
    tcpip::Storage* m_request;
} typedef SubscriptionHolder;

struct Circle {
    Circle() :
        x(0), y(0), radius(0) {
    }
    Circle(float a_x, float a_y, float a_radius) :
        x(a_x), y(a_y), radius(a_radius) {
    }
    Circle(application::Vector2D position, float a_radius) :
        radius(a_radius) {
        x = position.x;
        y = position.y;
    }

    float x;
    float y;
    float radius;
} typedef Circle;

const double INVALID_DIRECTION = 1000;

struct MobilityInfo {
    MobilityInfo(const int id) :
        id(id) {
        speed = acceleration = 0;
        direction = INVALID_DIRECTION;
        isMobile = false;
    }

    MobilityInfo(tcpip::Storage& storage) {
        id = storage.readInt();
        nsId = storage.readInt(); //added the ns3 ID support
        tsId = storage.readString(); //added the SUMO ID support
        double x = storage.readFloat();
        double y = storage.readFloat();
        //Don not read inline x and y because it will read y before...
        position = application::Vector2D(x, y);
        isMobile = storage.readUnsignedByte() == 0 ? false : true;
        if (isMobile) {
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
    static double ConvertDirection(const double direction) {
        double result = direction - 90.0;
        if (result <= -180.0) {
            result += 360.0;
        }
        return result;
    }
} typedef MobilityInfo;

struct Message {
    Message() {
        m_destinationId = m_messageId = 0;
        m_snr = NAN;
    }
    int m_destinationId;
    int m_messageId;
    std::string m_extra;
    double m_snr;
} typedef Message;

} /* namespace protocol */

#endif /* STRUCTS_H_ */
