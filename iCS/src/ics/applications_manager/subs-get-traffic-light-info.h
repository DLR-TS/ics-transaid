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

#ifndef SUBS_GET_TRAFFIC_LIGHT_INFO_H_
#define SUBS_GET_TRAFFIC_LIGHT_INFO_H_

#include "subscription.h"
#include <set>

namespace ics {
class AppMessageManager;

class SubsGetTrafficLightInfo: public ics::Subscription {
public:
    SubsGetTrafficLightInfo(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize);
    virtual ~SubsGetTrafficLightInfo();
    int InformApp(AppMessageManager* messageManager);

private:
    void SelectTrafficLight(Point2D position);

    bool m_error;
    bool m_sentAllInfo;
    std::string m_message;
    std::set<std::string> m_trafficLightsIds;
};

} /* namespace ics */

#endif /* SUBS_GET_TRAFFIC_LIGHT_INFO_H_ */
