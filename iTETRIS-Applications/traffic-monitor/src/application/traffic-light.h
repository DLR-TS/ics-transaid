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

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include <vector>
#include <string>
#include "program-configuration.h"

namespace protocol {
namespace application {
/**
 * Traffic light abstraction
 */

const double NO_DIRECTION = 1000;

enum TLStatus {
    GREEN, YELLOW, RED, OFF, UNKNOWN
};

class TrafficLightLane {
public:

    TrafficLightLane();
    virtual ~TrafficLightLane();

    std::string getControlledLane() const;
    void setControlledLane(std::string& contrLane);

    std::string getFollowingLane() const;
    void setFollowingLane(std::string& followLane);

    TLStatus getStatus() const;
    void setStatus(TLStatus newStatus);

    double getDirection() const;
    void setDirection(double direction);
    void setDirection(const std::vector<TLLane>& lanes);

    const std::string& getFriendlyName() const;
    void setFriendlyName(const std::string& friendlyName);

private:

    std::string m_controlledLane;
    std::string m_followingLane;
    std::string m_friendlyName;
    TLStatus m_currentStatus;
    double m_direction;
};

class TrafficLight {
public:

    TrafficLight(std::string trafficLightID);
    virtual ~TrafficLight();

    std::string getId() const;

    const std::vector<TrafficLightLane*>& getLanes() const;
    const std::vector<TrafficLightLane*> getLanesWithDirection(double direction) const;
    void addTrafficLightLane(TrafficLightLane* newLane);

    const std::string& getState() const;
    bool setState(const std::string& state);

    static TLStatus GetStatusFromChar(char statusChar);

    static TrafficLight* CreateTrafficLight(const int index, const int numLanes,
                                            const std::vector<std::string>& data, const std::string& id, const RsuData& rsuData);

private:

    std::string m_id;
    std::string m_state;
    std::vector<TrafficLightLane*> m_lanes;
    std::map<double, std::vector<TrafficLightLane*> > m_lanesDirection;
};

} /* namespace application */
} /* namespace protocol */

#endif /* TRAFFICLIGHT_H_ */

