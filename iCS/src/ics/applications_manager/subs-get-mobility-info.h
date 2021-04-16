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

#ifndef SUBS_GET_MOBILITY_INFO_H_
#define SUBS_GET_MOBILITY_INFO_H_

#include "subscription.h"
#include "subscriptions-type-constants.h"
#include "utils/ics/iCStypes.h"
#include <vector>

namespace ics {

class AppMessageManager;

class SubsGetMobilityInfo: public Subscription {
public:
    SubsGetMobilityInfo(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize);
    virtual ~SubsGetMobilityInfo();

    int InformApp(AppMessageManager* messageManager);
private:
    int m_mode;
    std::vector<int> m_listIds;
    Circle m_zone; //area where to get the list of vehicles

    std::vector<TMobileStationDynamicInfo>* GetAllID();
    std::vector<TMobileStationDynamicInfo>* GetListId();
    std::vector<TMobileStationDynamicInfo>* GetZoneId();
};

} /* namespace ics */

#endif /* SUBS_GET_MOBILITY_INFO_H_ */
