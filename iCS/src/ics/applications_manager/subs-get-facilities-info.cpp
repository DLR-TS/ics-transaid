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
/// @file    subs-get-facilities-info.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    December 3rd, 2010
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

#include "subs-get-facilities-info.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "subscriptions-helper.h"

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsGetFacilitiesInfo::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it = subscriptions->begin() ; it < subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsGetFacilitiesInfo*)) {
            SubsGetFacilitiesInfo* subsGetFacilitiesInfo = static_cast<SubsGetFacilitiesInfo*>(sub);
            if (subsGetFacilitiesInfo->m_nodeId == stationID) {
                delete subsGetFacilitiesInfo;
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
SubsGetFacilitiesInfo::SubsGetFacilitiesInfo(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize) : Subscription(stationId),  m_subscribedInformation(msg, msgSize) {
    m_id = ++m_subscriptionCounter;

    m_name = "RETURN INFORMATION ABOUT THE POSITION OF A NODE";

    m_appId = appId;

}

SubsGetFacilitiesInfo::~SubsGetFacilitiesInfo() { }

void
SubsGetFacilitiesInfo::getFacilitiesInformation(tcpip::Storage* info) {

    if (m_subscribedInformation.size() <= 0) {
        IcsLog::LogLevel("[ICS][INFO] getFacilitiesInformation() m_subscribedInformaiton is empty-nothing to send back", kLogLevelInfo);
        return;
    }

    unsigned int numBlocks = (int)m_subscribedInformation.readChar();

    info->writeUnsignedByte(numBlocks); // indicates to the application how many blocks it will have to read
    for (unsigned int i = 0; i < numBlocks; i++) {
        unsigned char type = (unsigned char)m_subscribedInformation.readUnsignedByte();
        int numFields = (int) m_subscribedInformation.readShort();

        switch (type) {
            case TYPE_TOPO: {
                getTopologicalInformation(numFields, info);
                break;
            }
            case TYPE_RECEIVED_CAM: {
                getReceivedCamInformation(numFields, info);
                break;
            }
            default: {
#ifdef LOG_ON
                stringstream log;
                log << "[ICS][ERROR]SubsGetFacilitiesInfo::getFacilitiesInformation() - unknown type: " << type;
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
            }
        }
    }
}


void SubsGetFacilitiesInfo::getTopologicalInformation(int numFields, tcpip::Storage* info) {
    float x, y;
    speed_t speed;
    direction_t direction;
    acceleration_t acceleration;
    roadElementID_t laneID;
    roadElementID_t edgeID;
    roadElementID_t junctionID;

    tcpip::Storage info_tmp;


    // inserting the number of entries for decoding at the Applicaiton module
    unsigned char field;
    for (int i = 0; i < numFields; i++) {
        field = m_subscribedInformation.readUnsignedByte();

        switch (field) {
            case VALUE__POS_X: {
                info_tmp.writeUnsignedByte(VALUE__POS_X);
                x = SyncManager::m_facilitiesManager->getStationPositionX(m_nodeId);
#ifdef LOG_ON
                stringstream log;
                log << "[ICS][INFO] getFacilitiesInformation() Position Transmitted - POS-X is: " << x;
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                info_tmp.writeFloat(x);
                break;
            }
            case VALUE__POS_Y: {
                info_tmp.writeUnsignedByte(VALUE__POS_Y);
                y = SyncManager::m_facilitiesManager->getStationPositionY(m_nodeId);
#ifdef LOG_ON
                stringstream log;
                log << "[ICS][INFO] getFacilitiesInformation() Position Transmitted - POS-Y is: " << y;
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                info_tmp.writeFloat(y);
                break;
            }
            case VALUE__SPEED: {
                info_tmp.writeUnsignedByte(VALUE__SPEED);
                speed = 0.0;
                if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE) {
                    speed = SyncManager::m_facilitiesManager->getMobileStationSpeed(m_nodeId);
                }
                info_tmp.writeFloat(speed);
                break;
            }
            case VALUE__DIRECTION: {
                if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE) {
                    info_tmp.writeUnsignedByte(VALUE__DIRECTION);
                    direction = SyncManager::m_facilitiesManager->getMobileStationDirection(m_nodeId);
                    info_tmp.writeFloat(direction);
                }
                break;
            }
            case VALUE__ACCELERATION: {
                if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE) {
                    info_tmp.writeUnsignedByte(VALUE__ACCELERATION);
                    acceleration = SyncManager::m_facilitiesManager->getMobileStationAcceleration(m_nodeId);
                    info_tmp.writeFloat(acceleration);
                }
                break;
            }
            case VALUE__LANE_ID:
            case VALUE__EDGE_ID:
            case VALUE__JUNCTION_ID: {
                Point2D pos = SyncManager::m_facilitiesManager->getStationPosition(m_nodeId);
                laneID = SyncManager::m_facilitiesManager->convertPoint2LaneID(pos);
                edgeID = SyncManager::m_facilitiesManager->getEdgeIDFromLane(laneID);
                junctionID = SyncManager::m_facilitiesManager->getJunctionIDFromLane(laneID);
                if (field == VALUE__LANE_ID) {
                    info_tmp.writeUnsignedByte(VALUE__LANE_ID);
                    info_tmp.writeString(laneID);
                }
                if (field == VALUE__EDGE_ID) {
                    info_tmp.writeUnsignedByte(VALUE__EDGE_ID);
                    info_tmp.writeString(edgeID);
                }
                if (field == VALUE__JUNCTION_ID) {
                    info_tmp.writeUnsignedByte(VALUE__JUNCTION_ID);
                    info_tmp.writeString(junctionID);
                }
                break;
            }
            default: {
#ifdef LOG_ON
                stringstream log;
                log << "[ICS][getTopologicalInformation] - Impossible to retrieve topological information with code: " << field;
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
                break;
            }
        }
    }
    info->writeShort(1 + 2 + (unsigned short)info_tmp.size());
    info->writeUnsignedByte(TYPE_TOPO);
    info->writeShort(numFields);
    info->writeStorage(info_tmp);

    //info_tmp.clear(); // TODO reclaim memory

}

void SubsGetFacilitiesInfo::getReceivedCamInformation(int numFields, tcpip::Storage* info) {
    //TODO: to be implemented
    IcsLog::LogLevel("[ICS] Subscription SubsGetFacilitiesInfo cannot return information about the received CAM messages yet.", kLogLevelError);
}


short int SubsGetFacilitiesInfo::getNumberOfSubscribedFields() {
    return (short int) m_subscribedInformation.size();
}

}
