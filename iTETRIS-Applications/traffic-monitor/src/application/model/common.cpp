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

#include "common.h"
#include <sstream>

namespace protocol {
namespace application {

bool IsVehicle(NodeType type) {
    return type & NT_VEHICLE;
}

bool IsNodeType(NodeType lval, NodeType rval) {
    if (rval == NT_ALL) {
        return true;
    } else if (rval == NT_RSU || rval == NT_VEHICLE) {
        return lval & rval;
    } else {
        return lval == rval;
    }
}

double AngleDifference(const double& angle1, const double& angle2) {
    double difference = angle2 - angle1;
    while (difference < -180) {
        difference += 360;
    }
    while (difference > 180) {
        difference -= 360;
    }
    return difference;
}

double GetDistance(const Vector2D& pos1, const Vector2D& pos2) {
    return std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2));
}

double NormalizeDirection(const double direction) {
    double ret = direction;
    if (direction < -180) {
        ret += 360;
    } else if (direction > 180) {
        ret -= 360;
    }
    return ret;
}

std::string ToString(ProtocolId pid) {
    switch (pid) {
        case PID_SPEED:
            return "Protocol Speed";
        default:
            return "???";
    }
}

std::string ToString(TypeBehaviour type) {
    switch (type) {
        case TYPE_BEHAVIOUR:
            return "Behaviour";
        case TYPE_BEHAVIOUR_RSU:
            return "BehaviourRsu";
        case TYPE_BEHAVIOUR_NODE:
            return "BehaviourNode";
        case TYPE_DATA_MANAGER:
            return "DataManager";
    }
}

std::string ToString(VehicleMovement direction) {
    switch (direction) {
        case APPROACHING:
            return "Approaching";
        case LEAVING:
            return "Leaving";

    }
}

const std::string VehicleDirection::getId() const {
    std::ostringstream oss;
    oss << dir << ":";
    switch (vMov) {
        case LEAVING:
            oss << "l";
            break;
        case APPROACHING:
            oss << "a";
            break;
    }
    return oss.str();
}

bool VehicleDirection::operator==(const VehicleDirection& other) const {
    return dir == other.dir && vMov == other.vMov;
}

bool VehicleDirection::operator!=(const VehicleDirection& other) const {
    return !operator ==(other);
}

std::ostream& operator<<(std::ostream& os, const VehicleDirection& direction) {
    os << direction.dir << " " << ToString(direction.vMov);
    return os;
}

} /* namespace application */
} /* namespace protocol */

