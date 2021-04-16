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

#include "fixed-station.h"
#include "subscription-helper.h"
#include "log/log.h"
#include "split.h"

namespace baseapp {
namespace application {

FixedStation::FixedStation(int id, BehaviourFactory* factory) :
    Node(id) {
    m_type = NT_RSU;
    m_rsuData = ProgramConfiguration::GetRsuData(id);
    m_trafficLight = NULL;
    m_positionUpdated = false;
    init(factory);
}

FixedStation::~FixedStation() {
    delete m_trafficLight;
    m_trafficLight = NULL;
}

void FixedStation::updateMobilityInformation(MobilityInfo* info) {
    if (!m_positionUpdated) {
        std::ostringstream oss;
        oss << "Rsu " << m_id << " position update: " << info->position;
        Log::WriteLog(oss);
        m_rsuData.position = info->position;
        m_positionUpdated = true;
    }
    delete info;
}

Vector2D FixedStation::getPosition() const {
    return m_rsuData.position;
}

Vector2D FixedStation::getVelocity() const {
    static Vector2D velocity;
    return velocity;
}


double FixedStation::getDirection() const {
    return DIR_INVALID;
}


float FixedStation::getSpeed() const {
    return 0.0;
}

int FixedStation::getLaneIndex() const {
    return -1;
}

float FixedStation::getAcceleration() const {
    return 0.0;
}

int stringToInt(std::string value) {
    int ret;
    std::stringstream str;
    str << value;
    str >> ret;
    return ret;
}

void FixedStation::trafficLightInformation(const bool error, const std::vector<std::string>& data) {
    std::vector<std::string> tokens;
    if (error) {
        split(tokens, data[0], ":");
        std::ostringstream oss;
        oss << "[TrafficLightInformation] error: " << tokens[1] << ". Will unsubscribe " << tokens[0];
        Log::Write(oss, LOG_ERROR);
        int subNo = stringToInt(tokens[0]);
        setToUnsubscribe(subNo);
    } else {
        for (int i = 0; i < data.size(); ++i) {
            split(tokens, data[i], " ");
            if (tokens.size() > 2) {
                int numLanes = stringToInt(tokens[2]);
                m_trafficLight = TrafficLight::CreateTrafficLight(i + 1, numLanes, data, tokens[0], m_rsuData);
                i += numLanes;
            }
            m_trafficLight->setState(tokens[1]);
            break; //Will use only the first traffic light from the subscription
        }
        std::ostringstream oss;
        oss << "[TrafficLightInformation] Traffic light status update: " << m_trafficLight->getState();
        Log::WriteLog(oss);
    }
}

void FixedStation::subscribeSendingCAMs() {
    m_toSubscribe.push(SubscriptionHelper::SetCamArea(m_rsuData.cam_area));
}

} /* namespace application */
} /* namespace protocol */
