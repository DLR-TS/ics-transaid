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
/// @file    subs-return-cars-zone.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <math.h>

#include "subs-return-cars-zone.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "../ics.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../../utils/ics/geometric/Circle.h"

using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
int SubsReturnsCarInZone::Delete(float baseX, float baseY, float radius, vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it = subscriptions->begin(); it < subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsReturnsCarInZone*)) {
            SubsReturnsCarInZone* subsReturnCarInZone = static_cast<SubsReturnsCarInZone*>(sub);
            float x, y, r;
            x = subsReturnCarInZone->m_baseX;
            y = subsReturnCarInZone->m_baseY;
            r = subsReturnCarInZone->m_radius;
            if ((x == baseX) && (y == baseY) && (r == radius)) {
                delete subsReturnCarInZone;
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
SubsReturnsCarInZone::SubsReturnsCarInZone(int appId, stationID_t stationId, float baseX, float baseY, float radius) :
    Subscription(stationId) {
    m_id = ++m_subscriptionCounter;
    m_name = "RETURN CARS IN ZONE";
    m_appId = appId;
    m_baseX = baseX;
    m_baseY = baseY;
    m_radius = radius;
}

SubsReturnsCarInZone::~SubsReturnsCarInZone() {
}

vector<VehicleNode*>*
SubsReturnsCarInZone::GetCarsInZone(vector<VehicleNode*>* vehicles) {
#ifdef LOG_ON
    stringstream log;
#endif
    if (vehicles == NULL) {
        cout << "[GetCarsInZone] [ERROR] Input parameter is NULL" << endl;
        return NULL;
    }

    // Container to return the nodes in zone
    vector<VehicleNode*>* nodesInZone = new vector<VehicleNode*>();

    // Get the IDs of the nodes in area from the facilities
    Point2D point(m_baseX, m_baseY);
    Circle area(point, m_radius);
    vector<stationID_t>* nodesInArea = SyncManager::m_facilitiesManager->getMobileStationsIDsInArea(area);
    if (nodesInArea->size() == 0) {
#ifdef LOG_ON
        log << "[INFO] GetCarsInZone() There are 0 nodes of total " << vehicles->size()
            << " vehicles in the area according to the Facilities";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        return nodesInZone;
    } else {
#ifdef LOG_ON
        log << "[INFO] GetCarsInZone() There are " << nodesInArea->size() << " nodes in the area";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    }
#ifdef LOG_ON
    log << "[INFO] GetCarsInZone() Starting the Lookup ";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    // Process to get the pointer to the vehicle
    // Loop all the vehicles in iTETRIS
    for (vector<VehicleNode*>::iterator nodeIt = vehicles->begin(); nodeIt < vehicles->end(); nodeIt++) {
        VehicleNode* vehicle = *nodeIt;
#ifdef LOG_ON
        log << "[INFO] GetCarsInZone() Full Vehicle List: Vehicle ID " << vehicle->m_icsId;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        log << "[INFO] GetCarsInZone() Nodes in Area: Vehicles IDs ";
#endif
        // Loop the IDs of the vehicles in the area
        for (vector<stationID_t>::iterator areaNodesIt = nodesInArea->begin(); areaNodesIt < nodesInArea->end();
                areaNodesIt++) {
            stationID_t nodeInAreaID = *areaNodesIt;
            if (nodeInAreaID == vehicle->m_icsId) {
                // If the IDs match, include the pointer in the collection to return
#ifdef LOG_ON
                log << "Log " << nodeInAreaID << ", ";
#endif
                nodesInZone->push_back(vehicle);
            } else {
#ifdef LOG_ON
                log << nodeInAreaID << ", ";
#endif
            }
        }
#ifdef LOG_ON
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    }
    return nodesInZone;
}


//this function increases the performance when the number of vehicles in the area is less than n/logn
//it also avoids an additional iterator to find vector Vehicles (with type = staType_CAR)
int
SubsReturnsCarInZone::GetCarsInZone(vector<VehicleNode*>*  carsInZone, NodeMap* nodes) {
    int i = 0;
#ifdef LOG_ON
    stringstream log;
#endif

    // Get the IDs of the nodes in area from the facilities
    Point2D point(m_baseX, m_baseY);
    Circle area(point, m_radius);
    vector<stationID_t>* nodesInArea = SyncManager::m_facilitiesManager->getMobileStationsIDsInArea(area);

    if (nodesInArea->size() == 0) {
#ifdef LOG_ON
        log << "[INFO] GetCarsInZone() There are 0 nodes of total "
            << " vehicles in the area according to the Facilities";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        return 0;
    } else {
#ifdef LOG_ON
        log << "[INFO] GetCarsInZone() There are " << nodesInArea->size() << " nodes in the area";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    }
#ifdef LOG_ON
    log << "[INFO] GetCarsInZone() Starting the Lookup ";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    carsInZone->reserve(nodesInArea->size());

    for (vector<stationID_t>::iterator areaNodesIt = nodesInArea->begin(); areaNodesIt < nodesInArea->end();
            areaNodesIt++) {
        stationID_t nodeInAreaID = *areaNodesIt;
        NodeMap::iterator nodeItInArea = nodes->find(nodeInAreaID); //regarding performance, if the number of nodes (in zone) is big enough
        //for iterator should be used instead of Lookup (find)
        if (nodeItInArea != nodes->end()) {
            if ((static_cast<VehicleNode*>(nodeItInArea->second))->m_type == staType_CAR) {
                i++;
                carsInZone->push_back(static_cast<VehicleNode*>(nodeItInArea->second));
            }
        }
    }

    return i;
}



int SubsReturnsCarInZone::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message,
        SyncManager* syncManager) {
    IcsLog::LogLevel("[WARNING] ProcessReceivedGeobroadcastMessage() SubsReturnsCarInZone not based on Geobroadcasting",
                     kLogLevelWarning);

    return EXIT_SUCCESS;
}

int SubsReturnsCarInZone::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message) {
    stringstream log;
    log << "[WARNING] ProcessReceivedUnicastMessage() SubsReturnsCarInZone not based on Unicasting";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

    return EXIT_SUCCESS;
}

}
