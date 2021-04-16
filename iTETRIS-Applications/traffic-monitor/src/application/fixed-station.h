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

#ifndef STATION_H_
#define STATION_H_

#include "node.h"
#include "structs.h"
#include "vector.h"
#include "program-configuration.h"
#include "traffic-light.h"
#include <map>

namespace protocol {
namespace application {

class FixedStation: public Node {
public:
    FixedStation(int id);
    virtual ~FixedStation();

    //void applicationExecute();
    Vector2D getPosition();
    Vector2D getVelocity();
    double getDirection();
    void updateMobilityInformation(MobilityInfo* info);
    void mobilityInformationHasRun();

    /**
     * @brief Called with the updated information about the traffic light.
     * @brief The first time it is called it also contains the lanes controlled by the traffic light.
     */
    void trafficLightInformation(const bool error, const std::vector<std::string>& data);

protected:
    void addSubscriptions();
private:
    bool m_mobilitySubscription;
    bool m_trafficLightSubscription;
    bool m_positionUpdated;
    RsuData m_rsuData;
    TrafficLight* m_trafficLight;
};

} /* namespace application */
} /* namespace protocol */

#endif /* STATION_H_ */
