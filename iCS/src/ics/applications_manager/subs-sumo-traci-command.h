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

#ifndef ICS_APPLICATIONS_MANAGER_SUBS_SUMO_TRACI_COMMAND_H_
#define ICS_APPLICATIONS_MANAGER_SUBS_SUMO_TRACI_COMMAND_H_

#include "subscription.h"
#include "utils/ics/iCStypes.h"
#include "foreign/tcpip/storage.h"

namespace ics {
class AppMessageManager;

class SubsSumoTraciCommand: public Subscription {
public:
    SubsSumoTraciCommand(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize);
    virtual ~SubsSumoTraciCommand();

    int InformApp(AppMessageManager* messageManager);

private:
    bool ExecuteCommand();

    int m_executionId;
    tcpip::Storage m_request;
    tcpip::Storage m_result;
};

} /* namespace ics */

#endif /* ICS_APPLICATIONS_MANAGER_SUBS_SUMO_TRACI_COMMAND_H_ */
