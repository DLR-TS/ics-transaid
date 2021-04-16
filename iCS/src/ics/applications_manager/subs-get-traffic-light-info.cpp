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

#include "subs-get-traffic-light-info.h"
#include "subs-get-mobility-info.h"
#include "app-message-manager.h"
#include "foreign/tcpip/storage.h"
#include "../../utils/ics/log/ics-log.h"
#include "../sync-manager.h"
#include <vector>

namespace ics {

SubsGetTrafficLightInfo::SubsGetTrafficLightInfo(int appId, ics_types::stationID_t stationId, unsigned char* msg,
        int msgSize) :
    Subscription(stationId) {
    m_id = ++m_subscriptionCounter;
    m_name = "RETURN INFORMATION ABOUT A TRAFFIC LIGHT";
    m_appId = appId;
    m_error = false;
    m_sentAllInfo = true;
    tcpip::Storage message(msg, msgSize);
    int type = message.readUnsignedByte();
    if (type == VALUE__LIST_IDS) {
        std::vector<std::string> tmp = message.readStringList();
        for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            m_trafficLightsIds.insert(*it);
        }
        if (m_trafficLightsIds.size() == 0) {
            m_error = true;
            m_message = "Error no traffic light found";
            IcsLog::LogLevel("SubsGetTrafficLightInfo::SelectTrafficLight() Error no traffic light found", kLogLevelError);
        }
    } else {
        Point2D position;
        switch (type) {
            case VALUE__SELECT_POS:
                position.setx(message.readFloat());
                position.sety(message.readFloat());
                break;
            case VALUE__NODE_POS:
                position = SyncManager::m_facilitiesManager->getStationPosition(stationId);
                break;
        }
        SelectTrafficLight(position);
    }
}

SubsGetTrafficLightInfo::~SubsGetTrafficLightInfo() {
}

void SubsGetTrafficLightInfo::SelectTrafficLight(Point2D position) {
    string rsuLane = SyncManager::m_facilitiesManager->convertPoint2LaneID(position);
    std::vector<ics_types::trafficLightID_t> trafficLigthIds;
    if (SyncManager::m_trafficSimCommunicator->GetTrafficLights(trafficLigthIds) == EXIT_FAILURE) {
        m_error = true;
        m_message = "Error GetTrafficLights";
        IcsLog::LogLevel("SubsGetTrafficLightInfo::SelectTrafficLight() Error GetTrafficLights", kLogLevelError);
        return;
    }
    vector<pair<string, string> > laneLinks;
    if (SyncManager::m_trafficSimCommunicator->GetLaneLinksConsecutiveLane(rsuLane, laneLinks) == EXIT_FAILURE) {
        m_error = true;
        m_message = "Error GetLaneLinksConsecutiveLane";
        IcsLog::LogLevel("SubsGetTrafficLightInfo::SelectTrafficLight() Error GetLaneLinksConsecutiveLane",
                         kLogLevelError);
        return;
    }
    for (std::vector<ics_types::trafficLightID_t>::iterator trafficLigthIdIt = trafficLigthIds.begin();
            trafficLigthIdIt != trafficLigthIds.end(); ++trafficLigthIdIt) {
        vector<vector<string> > controlledLinks;
        if (SyncManager::m_trafficSimCommunicator->GetTrafficLightControlledLinks(*trafficLigthIdIt,
                controlledLinks) == EXIT_FAILURE) {
            m_error = true;
            m_message = "Error GetTrafficLightControlledLinks";
            IcsLog::LogLevel("SubsGetTrafficLightInfo::SelectTrafficLight() Error GetTrafficLightControlledLinks",
                             kLogLevelError);
            return;
        }
        for (vector<pair<string, string> >::iterator laneLinksIt = laneLinks.begin(); laneLinksIt != laneLinks.end();
                ++laneLinksIt)
            for (vector<vector<string> >::iterator controlledLinksIt = controlledLinks.begin();
                    controlledLinksIt != controlledLinks.end(); ++controlledLinksIt)
                for (vector<string>::iterator internalIt = controlledLinksIt->begin(); internalIt != controlledLinksIt->end();
                        ++internalIt) {
                    if (laneLinksIt->first == *internalIt || laneLinksIt->second == *internalIt) {
                        if (m_trafficLightsIds.insert(*trafficLigthIdIt).second) {
#ifdef LOG_ON
                            std::ostringstream oss;
                            oss << "SubsGetTrafficLightInfo::SelectTrafficLight() New traffic light found " << *trafficLigthIdIt;
                            IcsLog::LogLevel(oss.str().c_str(), kLogLevelInfo);
#endif
                        }
                    }
                }
    }
    if (m_trafficLightsIds.size() == 0) {
        m_error = true;
        m_message = "Error no traffic light found";
        IcsLog::LogLevel("SubsGetTrafficLightInfo::SelectTrafficLight() Error no traffic light found", kLogLevelError);
        return;
    }
}

int SubsGetTrafficLightInfo::InformApp(AppMessageManager* messageManager) {
    if (SyncManager::m_simStep < 0) {
        return EXIT_SUCCESS;
    }
    vector<std::string> result;
    if (m_error) {
        ostringstream oss;
        oss << m_id << ":" << m_message;
        result.push_back(oss.str());
    } else {
        for (std::set<std::string>::const_iterator id = m_trafficLightsIds.begin(); id != m_trafficLightsIds.end(); ++id) {
            const TrafficLight* tl = SyncManager::m_facilitiesManager->getTrafficLight(*id);
            if (tl == NULL) {
                m_error = true;
                ostringstream oss;
                oss << "Id " << *id << " does not exist";
                m_message = oss.str();
                oss.str("");
                oss << "SubsGetTrafficLightInfo::InformApp() " << m_message;
                IcsLog::LogLevel(oss.str().c_str(), kLogLevelError);
                oss.str("");
                oss << m_id << ":" << m_message;
                result.push_back(oss.str());
                break;
            }
            if (m_sentAllInfo) {
                ostringstream oss;
                oss << tl->getId() << " " << tl->getState() << " " << tl->getLanes().size();
                result.push_back(oss.str());
                int index = 0;
                for (std::vector<TrafficLightLane>::const_iterator it = tl->getLanes().begin(); it != tl->getLanes().end();
                        ++it) {
                    ostringstream oss;
                    oss << index++ << " " << it->getControlledLaneID() << " " << it->getFollowingLaneID();
                    result.push_back(oss.str());
                }
            } else {
                result.push_back((tl->getId() + " " + tl->getState()));
            }

        }
    }
    m_sentAllInfo = false;
    if (messageManager->CommandSendSubscriptionTrafficLightInfo(result, m_nodeId, m_error) == EXIT_FAILURE) {
        IcsLog::LogLevel("SubsGetTrafficLightInfo::InformApp() Could not send the result of the subscription",
                         kLogLevelError);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
} /* namespace ics */
