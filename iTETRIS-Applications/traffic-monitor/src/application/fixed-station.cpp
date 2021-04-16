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

namespace protocol {
namespace application {

FixedStation::FixedStation(int id) :
    Node(id) {
    m_type = NT_RSU;
    m_mobilitySubscription = m_positionUpdated = m_trafficLightSubscription = false;
    m_rsuData = ProgramConfiguration::GetRsuData(id);
    m_trafficLight = NULL;
    init();
}

FixedStation::~FixedStation() {
    delete m_trafficLight;
    m_trafficLight = NULL;
}

void FixedStation::addSubscriptions() {
    //Subscribe to receive messages
    Node::addSubscriptions();
    if (!m_mobilitySubscription) {
        m_mobilitySubscription = true;
        m_toSubscribe.push(SubscriptionHelper::GetMobilityInformation());
    }
    if (!m_trafficLightSubscription) {
        m_trafficLightSubscription = true;
        m_toSubscribe.push(SubscriptionHelper::GetTrafficLightInformation());
    }
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

void FixedStation::mobilityInformationHasRun() {
}

Vector2D FixedStation::getPosition() {
    return m_rsuData.position;
}

Vector2D FixedStation::getVelocity() {
    static Vector2D velocity;
    return velocity;
}

double FixedStation::getDirection() {
    return DIR_INVALID;
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

} /* namespace application */
} /* namespace protocol */
