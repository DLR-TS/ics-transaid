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
/// @file    subs-app-result-traff-sim.cpp
/// @author  Jerome Haerri (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <cstring>

#include "subs-app-result-traff-sim.h"
#include "subscriptions-helper.h"
#include "subscriptions-type-constants.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../itetris-node.h"

namespace ics {

// ===========================================================================
// Constants
// ===========================================================================

#define TYPE_ERROR			       			  0x00
#define TYPE_EDGE_TRAVELTIME       			  0x01
#define TYPE_EDGES_ID_FROM_ROAD_ID      	  0x02

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsAppResultTraffSim::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it = subscriptions->begin() ; it < subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsAppResultTraffSim*)) {
            SubsAppResultTraffSim* subsAppResultTraffSim = static_cast<SubsAppResultTraffSim*>(sub);
            if (subsAppResultTraffSim->m_nodeId == stationID) {
                delete subsAppResultTraffSim;
                delete sub;
                return EXIT_SUCCESS;
            }
        }
    }
    return EXIT_SUCCESS;
}

// ===========================================================================
// member method definitions
// ===========================================================================

SubsAppResultTraffSim::SubsAppResultTraffSim(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize) : Subscription(stationId), m_msg(msg, msgSize) {
    // Read parameters
    m_id = ++m_subscriptionCounter;

    m_name = "SEND A Command To the Traffic Simulator";

    m_appId = appId;

}

SubsAppResultTraffSim::~SubsAppResultTraffSim() {}

std::vector<unsigned char>
SubsAppResultTraffSim::pull(SyncManager* syncManager) {
    std::vector<unsigned char> info;
    unsigned int index = 0;

    unsigned char appMsgType = m_msg.readChar();       // APP_MSG_TYPE
    unsigned char cmdMode = m_msg.readChar();         // Defines the command mode and thus the way the rest of the msg will be read.

    switch (cmdMode) {
        case VALUE_GET_EDGE_TRAVELTIME: {
#ifdef LOG_ON
            stringstream log;
            log << "Command: GET_EDGE_TRAVELTIME";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            ics_types::roadElementID_t edgeID = "";

            ics_types::stationID_t m_iCS_ID = (ics_types::stationID_t) m_msg.readInt();
            int numWeights = m_msg.readInt();

            for (int i = 0; i < numWeights; i++) {
                unsigned char typeEdgeID = m_msg.readChar();
                if (typeEdgeID == TYPE_EDGE_TRAVELTIME) {
                    info.push_back(TYPE_EDGE_TRAVELTIME);
                    edgeID = m_msg.readString();
                    float edgeTravelTime = syncManager->m_trafficSimCommunicator->GetEdgeWeight(edgeID);
                    std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(edgeTravelTime);
                    info.insert(info.end(), tmp.begin(), tmp.end());
                } else {
                    IcsLog::LogLevel("To send a GET_EDGE_TRAVELTIME command a list of EdgeIDs must be  specified.", kLogLevelError);
                    info.push_back(TYPE_ERROR);
                    return info;
                }
            }

            break;
        }
        case VALUE_GET_ROUTE_VARIABLE: {
#ifdef LOG_ON
            stringstream log;
            log << "Command: GET ROUTE VARIABLE";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            ics_types::roadElementID_t routeID = "";

            ics_types::stationID_t m_iCS_ID = (ics_types::stationID_t) m_msg.readInt();
            unsigned int numRoute = (unsigned int) m_msg.readChar();

            for (unsigned int i = 0; i < numRoute; i++) {
                unsigned char typeRouteID = m_msg.readChar();
                if (typeRouteID == TYPE_EDGES_ID_FROM_ROAD_ID) {
                    info.push_back(TYPE_EDGES_ID_FROM_ROAD_ID);
                    routeID = m_msg.readString();
                    std::vector<std::string> routeEdges = syncManager->m_trafficSimCommunicator->GetRouteEdges(routeID);

                    for (unsigned int i = 0; i < routeEdges.size() - 1; i++) {
                        string tmpString = routeEdges.at(i);
                        std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpreteString(tmpString);
                        info.insert(info.end(), tmp.begin(), tmp.end());
                    }
                } else {
                    IcsLog::LogLevel("To send a GET_ROUTE_VARIABLE command a list of routeIDs must be  specified.", kLogLevelError);
                    info.push_back(TYPE_ERROR);
                    return info;
                }
            }

            break;

        }
        default: {
            IcsLog::LogLevel(" SubsAppResultTraffSim::pull() Impossible to send the command. Command not recognized.", kLogLevelError);
            info.push_back(TYPE_ERROR);
            return info;

        }

    }
    return info;

#ifdef LOG_ON
    IcsLog::LogLevel("[ICS - SubsAppResultTraffSim() - The message was correctly sent!!!", kLogLevelInfo);
#endif

}

} // end namespace ics
