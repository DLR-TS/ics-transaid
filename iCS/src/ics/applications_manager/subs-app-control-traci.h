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
/****************************************************************************/
/// @file    subs-app-control-traci.h
/// @author  Jerome Haerri (EURECOM)
/// @date    March 31st, 2015
/// @version $Id:
///

/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

#ifndef SUBS_APP_CONTROL_TRACI_H_
#define SUBS_APP_CONTROL_TRACI_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#include "foreign/tcpip/storage.h"
#include "app-message-manager.h"
#include "subscription.h"

namespace ics {

// ===========================================================================
// class declarations
// ===========================================================================
//class SubscriptionsHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsAppResultTraffSim
 * @brief Manages the subscription to information from Traffic Simulator at the Application Level
 */
class SubsAppControlTraci: public ics::Subscription {
public:

    /**
     * @brief Constructor
     * @param[in] appId ID of the instantiated subscription.
     * @param[in] stationId Station that owns the subscription.
     * @param[in] msg Command fields to subscribe to the traffic simulator.
     * @param[in] msgSize Length of the command fields Command fields to subscribe to the traffic simulator.
     */
    SubsAppControlTraci(int subId, ics_types::stationID_t stationId, SyncManager* syncManager, unsigned char* msg, int msgSize);

    /**
     * @brief Destructor
     */
    virtual ~SubsAppControlTraci();

    /**
     * @brief Send a set of commands to retrieve data from TraCI.
     *
     * The required command and related informations are coded according to the Type-Length-Value method.
     *
     */
    void pull(SyncManager* syncManager);

    /**
     * @brief retrieve the return data from TraCI, without interpreting it.
     *
     * The required command and related informations are coded according to the Type-Length-Value method.
     *
     * @return data in a generic tcpip::Storage, without interpreting them
     */
    tcpip::Storage& returnValues();

    /**
       * @brief retrieve the return data from TraCI, without interpreting it.
       *
       * The required command and related informations are coded according to the Type-Length-Value method.
       *
       * @return Success in case successful transmission on the socket, failure otherwise.
     */
    int InformApp(AppMessageManager* messageManager);


    /**
     * @brief Deletes the subscription according to the input parameters.
     * @param[in] subscriptions Collection of subscriptions to delete
     * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
     */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);
    /**
     * Get the TraCI message
     */
    void getMsg(tcpip::Storage* outMsg);
    std::pair<float, float>  getPositionFromMsg();
    void printGetSpeedMessage();

private:

    tcpip::Storage in_msg;
    tcpip::Storage out_msg;
};

}

#endif /* SUBS_APP_CONTROL_TRACI_H_ */
