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

#include "traffic-light.h"
#include "log/log.h"
#include "log/ToString.h"
#include "split.h"

namespace baseapp {
namespace application {

///TrafficLightLane implementation
TrafficLightLane::TrafficLightLane() {
    m_currentStatus = UNKNOWN;
    m_direction = NO_DIRECTION;
}
TrafficLightLane::~TrafficLightLane() {
}
std::string TrafficLightLane::getControlledLane() const {
    return m_controlledLane;
}
std::string TrafficLightLane::getFollowingLane() const {
    return m_followingLane;
}
TLStatus TrafficLightLane::getStatus() const {
    return m_currentStatus;
}
void TrafficLightLane::setStatus(TLStatus newStatus) {
    m_currentStatus = newStatus;
}
void TrafficLightLane::setControlledLane(std::string& contrLane) {
    m_controlledLane = contrLane;
}
void TrafficLightLane::setFollowingLane(std::string& followLane) {
    m_followingLane = followLane;
}
double TrafficLightLane::getDirection() const {
    return m_direction;
}
void TrafficLightLane::setDirection(double direction) {
    m_direction = direction;
}
void TrafficLightLane::setDirection(const std::vector<TLLane>& lanes) {
    for (std::vector<TLLane>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
        if (m_controlledLane == it->controlledLane && m_followingLane == it->followingLane) {
            m_direction = it->dir;
            m_friendlyName = it->friendlyName;
            return;
        }
    }
    m_direction = NO_DIRECTION;
    m_friendlyName = "No direction found";
}
const std::string& TrafficLightLane::getFriendlyName() const {
    return m_friendlyName;
}

void TrafficLightLane::setFriendlyName(const std::string& friendlyName) {
    m_friendlyName = friendlyName;
}

///TrafficLight implementation
TrafficLight::TrafficLight(std::string trafficLightID) {
    m_id = trafficLightID;
}
TrafficLight::~TrafficLight() {
    for (std::vector<TrafficLightLane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); ++it) {
        delete *it;
    }
    m_lanes.clear();
    m_lanesDirection.clear();
}
std::string TrafficLight::getId() const {
    return m_id;
}
const std::vector<TrafficLightLane*>& TrafficLight::getLanes() const {
    return m_lanes;
}
const std::vector<TrafficLightLane*> TrafficLight::getLanesWithDirection(double direction) const {
    std::map<double, std::vector<TrafficLightLane*> >::const_iterator it = m_lanesDirection.find(direction);
    if (it == m_lanesDirection.end()) {
        return std::vector<TrafficLightLane*>();
    }
    return it->second;
}
const std::string& TrafficLight::getState() const {
    return m_state;
}
bool TrafficLight::setState(const std::string& state) {
    if (state.size() != m_lanes.size()) {
        return false;
    }
    m_state = state;
    for (int i = 0; i < m_lanes.size(); ++i) {
        m_lanes[i]->setStatus(GetStatusFromChar(m_state[i]));
    }
    return true;
}
void TrafficLight::addTrafficLightLane(TrafficLightLane* newLane) {
    m_lanes.push_back(newLane);
    std::map<double, std::vector<TrafficLightLane*> >::iterator it = m_lanesDirection.find(newLane->getDirection());
    if (it == m_lanesDirection.end()) {
        std::vector<TrafficLightLane*> v;
        it = m_lanesDirection.insert(std::make_pair(newLane->getDirection(), v)).first;
    }
    it->second.push_back(newLane);
}
TLStatus TrafficLight::GetStatusFromChar(char statusChar) {
    switch (statusChar) {
        case 'r':
        case 'R':
            return RED;
        case 'y':
        case 'Y':
            return YELLOW;
        case 'g':
        case 'G':
            return GREEN;
        case 'o':
        case 'O':
            return OFF;
        default:
            return UNKNOWN;
    }
}
TrafficLight* TrafficLight::CreateTrafficLight(const int index, const int numLanes,
        const std::vector<std::string>& data, const std::string& id, const RsuData& rsuData) {
    TrafficLight* tl = new TrafficLight(id);
    std::ostringstream oss;
    oss << "[CreateTrafficLight] New traffic light with id " << id;
    Log::WriteLog(oss);
    std::vector<std::string> tokens;
    for (int i = index; i < index + numLanes; ++i) {
        split(tokens, data[i], " ");
        TrafficLightLane* lane = new TrafficLightLane();
        lane->setControlledLane(tokens[1]);
        lane->setFollowingLane(tokens[2]);
        lane->setDirection(rsuData.lanes);
        tl->addTrafficLightLane(lane);
        std::ostringstream oss;
        if (lane->getDirection() == NO_DIRECTION) {
            oss << "[CreateTrafficLight] Added new lane to the traffic light. No match found. Controlled lane: " << lane->getControlledLane() << " Following: " << lane->getFollowingLane();
        } else
            oss << "[CreateTrafficLight] Added new lane to the traffic light. Direction: " << toString(lane->getDirection())
                << ". Friendly name: " << lane->getFriendlyName();
        Log::WriteLog(oss);
    }
    return tl;
}

} /* namespace application */
} /* namespace protocol */
