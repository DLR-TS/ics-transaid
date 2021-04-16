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
#include "protocols.h"
#include "scheduler.h"
#include "log/log.h"

namespace protocol {
namespace application {

///ExecuteBase
TraceMap& ExecuteBase::GetTracedCallbacks() {
    return m_traceMap;
}

///CentralizedProtocol
std::string CentralizedProtocol::TraceName = "FlowCheck";
double CentralizedProtocol::SpaceThreshold = 50;
bool CentralizedProtocol::ReturnData = false;

CentralizedProtocol::CentralizedProtocol() {
    m_traceMap[TraceName] = &m_traceFlows;
}

CentralizedProtocol::~CentralizedProtocol() {
}

bool CentralizedProtocol::CompluteSpeed(const NodeInfo& latest, const NodeInfo& first, double& speedAll,
                                        double& speedFilter) {
    double distance = GetDistance(latest.position, first.position);
    speedAll = distance / (latest.lastSeen - first.lastSeen) * 1000;
    if (distance >= SpaceThreshold) {
        speedFilter = speedAll;
        return true;
    }
    return false;
}

void CentralizedProtocol::AggregateDataForDirection(const NodeDataMap& data, ValueMap& valueMap) {
    double speed = 0;
    double spaceMeanSpeed = 0;
    double speedTime = 0;
    double speedTimeFilter = 0;
    double speedTimeSingle = 0;
    int number = 0;
    int numberSpeedFilter = 0;
    double maxd = 0;
    double mind = 1000 * 1000;
    double time = 0;
    int numLastMex = 0;
    std::ostringstream oss;
    for (NodeDataMap::const_iterator node = data.begin(); node != data.end(); ++node) {
        //The latest message is in the last position of the set
        NodeInfo info = *(node->second.rbegin().operator * ());
        //Do not count the nodes which have sent a no longer conformant message
        if (!info.toRemove) {
            ++number;
            speed += info.currentSpeed;
            //Space Mean Speed
            spaceMeanSpeed += 1 / info.currentSpeed;
            oss << info.currentSpeed << " ";
            //Speed from messages
            if (node->second.size() > 1) {
                double spd, spdF;
                NodeInfo first = *(node->second.begin().operator * ());
                if (CompluteSpeed(info, first, spd, spdF)) {
                    ++numberSpeedFilter;
                    speedTimeFilter += spdF;
                }
                speedTime += spd;
                NodeDataCollection::reverse_iterator rit = node->second.rbegin();
                ++rit;
                NodeInfo last = *(rit.operator * ());
                CompluteSpeed(info, last, spd, spdF);
                speedTimeSingle += spd;
            }
            if (info.distance > maxd) {
                maxd = info.distance;
            }
            if (info.distance < mind) {
                mind = info.distance;
            }
        }
        //Use also the data from no longer conformant nodes
        if (info.lastMessage && info.totalTime > 0) {
            ++numLastMex;
            time += info.totalTime;
        }
    }
    if (numLastMex > 0) {
        time /= numLastMex;
    } else {
        time = -1;
    }
    if (number > 0) {
        speed /= number;
        //Space Mean Speed
        spaceMeanSpeed = number / spaceMeanSpeed;
        speedTime /= number;
        speedTimeSingle /= number;
        std::string tmp = oss.str();
        NS_LOG_INFO("SpaceMeanSpeed " << spaceMeanSpeed << " n=" << number << " list=" << tmp);
    } else {
        mind = 0;
        speed = -1;
        spaceMeanSpeed = -1;
        speedTime = -1;
        speedTimeSingle = -1;
    }
    if (numberSpeedFilter > 0) {
        speedTimeFilter /= number;
    } else {
        speedTimeFilter = -1;
    }
    valueMap[NUMBER] = number;
    valueMap[SPEED] = speed;
    valueMap[TIME] = time;
    valueMap[MAX_DISTANCE] = maxd;
    valueMap[MIN_DISTANCE] = mind;
    valueMap[SPACE_MEAN_SPEED] = spaceMeanSpeed;
    valueMap[SPEED_TIME] = speedTime;
    valueMap[SPEED_TIME_FILTER] = speedTimeFilter;
    valueMap[SPEED_TIME_SINGLE] = speedTimeSingle;

}

bool CentralizedProtocol::Execute(DirectionValueMap& data, const DataMap& dataMap) {
    NS_LOG_FUNCTION("");
    std::vector<std::string> flows;
    std::stringstream str;
    int totNum = 0;
    for (DataMap::const_iterator dir = dataMap.begin(); dir != dataMap.end(); ++dir) {
        ValueMap valueMap;
        AggregateDataForDirection(dir->second, valueMap);
        totNum += valueMap[NUMBER];
        std::ostringstream out;
        out << dir->first << "=" << valueMap[NUMBER] << ":" << valueMap[MAX_DISTANCE] << ":" << valueMap[SPEED] << ":"
//						<< valueMap[TIME] << ":" << valueMap[MIN_DISTANCE] << ":" << valueMap[SPACE_MEAN_SPEED] << ":"
            << valueMap[TIME] << ":" << valueMap[MIN_DISTANCE] << ":" << valueMap[SPEED_TIME_SINGLE] << ":"
            << valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER];
        flows.push_back(out.str());
        if (valueMap[SPEED] == -1) {
            valueMap[SPEED] = 0;
        }
        if (valueMap[TIME] == -1) {
            valueMap[TIME] = 0;
        }
        data[dir->first] = valueMap;
        str << "dir=" << dir->first << " num=" << valueMap[NUMBER] << " spd=" << valueMap[SPEED] << " sms="
//						<< valueMap[SPACE_MEAN_SPEED] << " st=" << valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER]
            << valueMap[SPEED_TIME_SINGLE] << " st=" << valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER]
            << " ";
    }
    std::ostringstream out;
    out << "totc=" << totNum;
    flows.insert(flows.begin(), out.str());
    NS_LOG_INFO("CentralizedProtocol " << str.str());
    m_traceFlows(flows);
    return ReturnData;
}

} /* namespace application */
} /* namespace protocol */
