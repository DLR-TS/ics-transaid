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
 * Author Enrico Zamagni
 * University of Bologna
 ***************************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#include "vector.h"
#include "cmath"
#include <stdint.h>

namespace protocol {
namespace application {

//TODO add other vehicle types
typedef enum {
    NT_RSU = 0x01,
    NT_VEHICLE = 0x02,
    NT_VEHICLE_FULL = 0x06,
    NT_VEHICLE_MEDIUM = 0x0A,
    NT_ALL = 0xFF,
    NT_VEHICLE_SHADOW = 0x00
} NodeType;

typedef enum {
    PID_SPEED = 0x01
} ProtocolId;

enum Type_Behaviour {
    TYPE_BEHAVIOUR, TYPE_BEHAVIOUR_RSU, TYPE_BEHAVIOUR_NODE, TYPE_DATA_MANAGER

} typedef TypeBehaviour;

typedef enum {
    MT_RSU_BEACON = 0x01, MT_BEACON_RESPONSE = 0x02, MT_NO_LONGHER_CONFORMANT = 0x03
} MessageType;

typedef enum {
    APPROACHING = 0x01, LEAVING = 0x02
} VehicleMovement;

const unsigned int ID_ALL = 0xffffffff;
const double DIR_INVALID = 1000;
const double SPD_INVALID = -1000;

struct VehicleDirection {
    VehicleDirection() {
        dir = DIR_INVALID;
        vMov = APPROACHING;
        time = 0;
    }
    VehicleDirection(double direction, VehicleMovement movement) :
        dir(direction), vMov(movement) {
        time = 0;
    }
    VehicleDirection(double direction, VehicleMovement movement, uint16_t beaconTime) :
        dir(direction), vMov(movement), time(beaconTime) {
    }
    VehicleDirection(const VehicleDirection& other) {
        dir = other.dir;
        vMov = other.vMov;
        time = other.time;
    }
    double dir;
    VehicleMovement vMov;
    uint16_t time;
    const std::string getId() const;
    bool operator==(const VehicleDirection& other) const;
    bool operator!=(const VehicleDirection& other) const;
};
std::ostream& operator<<(std::ostream& stream, const VehicleDirection& direction);

struct NodeInfo {
    NodeInfo() {
        nodeId = lastSeen = totalTime = -1;
        lastMessage = toRemove = false;
        direction = DIR_INVALID;
        distance = currentSpeed = avgSpeedHigh = avgSpeedSmall = 0;
    }
    NodeInfo(int id, Vector2D pos, double dir, VehicleDirection cDir, double cSpd, double avgSpdS, double avgSpdH,
             int last, bool lMess) :
        nodeId(id), position(pos), direction(dir), conformantDirection(cDir), currentSpeed(cSpd), avgSpeedSmall(avgSpdS), avgSpeedHigh(
            avgSpdH), lastSeen(last), lastMessage(lMess), totalTime(0), distance(0) {
        toRemove = false;
    }
    int nodeId;
    int lastSeen;
    int totalTime;
    Vector2D position;
    VehicleDirection conformantDirection;
    double direction;
    double currentSpeed;
    double avgSpeedSmall;
    double avgSpeedHigh;
    double distance;
    bool lastMessage;
    bool toRemove;
};

bool IsVehicle(NodeType type);
bool IsNodeType(NodeType lval, NodeType rval);
double AngleDifference(const double& angle1, const double& angle2);
double GetDistance(const Vector2D& pos1, const Vector2D& pos2);
double NormalizeDirection(const double direction);
std::string ToString(ProtocolId pid);
std::string ToString(TypeBehaviour type);
std::string ToString(VehicleMovement direction);

} /* namespace application */
} /* namespace protocol */

#endif /* COMMON_H_ */
