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
/// @file    application-handler.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
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
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
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

#include <typeinfo>

#include <utils/common/RandHelper.h>
#include "application-handler.h"
#include "subscription.h"
#include "subs-return-cars-zone.h"
#include "subs-set-cam-area.h"
#include "subs-start-travel-time-calculation.h"
#include "subs-stop-travel-time-calculation.h"
#include "subs-calculate-travel-time.h"
#include "subs-get-received-cam-info.h"
#include "subs-get-facilities-info.h"
#include "subs-app-message-send.h"
#include "subs-app-message-receive.h"
#include "subs-app-result-traff-sim.h"
#include "subs-app-cmd-traff-sim.h"
#include "subs-x-application-data.h"
#include "subs-sumo-traci-command.h"
#include "app-message-manager.h"
#include "app-result-maximum-speed.h"
#include "app-result-travel-time.h"
#include "app-result-void.h"
#include "app-result-generic.h"
#include "app-result-traffic-jam-detection.h"
#include "app-result-travel-time.h"
#include "subs-x-application-data.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "../wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"
#include "subs-get-mobility-info.h"
#include "subs-get-traffic-light-info.h"
#include "subs-app-control-traci.h"

using namespace std;

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
int ApplicationHandler::m_idCounter = 0;

// ===========================================================================
// member method definitions
// ===========================================================================
ApplicationHandler::ApplicationHandler(SyncManager* syncManager, string name, string host, string executable, int port,
                                       long seed, double rate, int resultType, ServiceId serviceIds) {
    m_id = ++m_idCounter;
    m_syncManager = syncManager;
    m_name = name;
    m_executable = executable;
    m_host = host;
    m_port = port;
    RandHelper::initRand(&m_rng, false, seed);
    m_rate = rate;
    m_resultType = resultType;
    m_serviceId = serviceIds;

    m_appMessageManager = new AppMessageManager(m_syncManager);
}

ApplicationHandler::ApplicationHandler(const ApplicationHandler& appHandler) {
    m_id = appHandler.m_id;
    m_name = appHandler.m_name;
    m_host = appHandler.m_host;
    m_port = appHandler.m_port;
    m_rate = appHandler.m_rate;

    m_appMessageManager = new AppMessageManager(appHandler.m_syncManager);
}

ApplicationHandler::~ApplicationHandler() {
    delete m_appMessageManager;
}

bool ApplicationHandler::RemoveVehicleNode(VehicleNode* node) {
    if (node == NULL) {
        return false;
    }
    return m_appMessageManager->CommandRemoveVehicleNode(node);
}

bool ApplicationHandler::CreateVehicleNodeApplication(VehicleNode* node) {
    if (node == NULL) {
        return false;
    }
    return m_appMessageManager->CommandUpdateVehicleNodeExistence(node);
}

bool ApplicationHandler::AskForNewSubscriptions(int nodeId, vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return false;
    }

    bool noMoreSubscriptions = false;

    // Ask for subscriptions until the applications requests to stop
    while (!noMoreSubscriptions) {
        if (!m_appMessageManager->CommandGetNewSubscriptions(nodeId, m_id, subscriptions, noMoreSubscriptions)) {
            return false;
        }
    }

    return true;
}

bool ApplicationHandler::AskForUnsubscriptions(int nodeId, vector<Subscription*>* subscriptions) {
    if (subscriptions == NULL) {
        return false;
    }

    for (vector<Subscription*>::iterator it = subscriptions->begin(); it != subscriptions->end(); it++) {
        if (m_id != (*it)->m_appId) {
            continue;
        }
        int status = m_appMessageManager->CommandUnsubscribe(nodeId, (*it));

        switch (status) {
            case 0: {
#ifdef LOG_ON
                stringstream log;
                log << "AskForUnsubscriptions() subscription " << (*it)->m_id << " in node [iCS-ID] [" << nodeId << "] is alive.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                break;
            }
            case 1: {
                int id = (*it)->m_id;
                delete *it;
                subscriptions->erase(it); // Performs the removal of the subscription from the collection
                it--;
#ifdef LOG_ON
                stringstream log;
                log << "AskForUnsubscriptions() unsubscribing " << id << " in node [iCS-ID] [" << nodeId << "]";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                break;
            }
            case -1: {
#ifdef LOG_ON
                stringstream log;
                log << "AskForUnsubscriptions() error unsubscribing " << (*it)->m_id << " in node [iCS-ID] [" << nodeId << "]";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
                cerr << "iCS --> [ERROR] AskForUnsubscriptions() unsubscribing node [iCS-ID] [" << nodeId << "]" << endl;
                return false;
            }
            default:
                IcsLog::LogLevel("AskForUnsubscriptions() Unknown error code.", kLogLevelWarning);
                break;
        }
    }

    return true;
}

int ApplicationHandler::SendSubscribedData(int nodeId, Subscription* subscription, NodeMap* nodes) {
    if (subscription == NULL || nodes == NULL) {
        IcsLog::LogLevel("SendSubscribedData() Subscription or nodes are NULL.", kLogLevelError);
        return EXIT_FAILURE;
    }
//  test(subscription);
    Subscription& sub = *subscription;
    const std::type_info& typeinfo = typeid(sub);

    if (typeinfo == typeid(SubsReturnsCarInZone)) {
        // Subscription to return car IDs in certain zone
#ifdef LOG_ON
        stringstream log;
        log << "iCS --> [AppHanlder] - SendSubscribedData() - forward subscribed cars in zone to node " << nodeId << " ";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        // Check out from the vehicles which ones are in the zone
        SubsReturnsCarInZone* subCarsInZone = static_cast<SubsReturnsCarInZone*>(subscription);

        vector<VehicleNode*>* carsInZone = new vector<VehicleNode*>();
        if (subCarsInZone->GetCarsInZone(carsInZone, nodes) > 0) {

            if (!m_appMessageManager->CommandSendSubscriptionCarsInZone(carsInZone, nodeId, subscription->m_id)) {
#ifdef LOG_ON
                IcsLog::LogLevel("SendSubscribedData() SendSubscribedData() Error sending data of cars in zone.",
                                 kLogLevelError);
#endif
                return EXIT_FAILURE;
            } else {
#ifdef LOG_ON
                stringstream log;
                log << "iCS --> AppHandler SendSubscribedData() Cars in subscribed zones are " << carsInZone->size();
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                return EXIT_SUCCESS;
            }
        } else {
            //carsInZone->size() == 0

#ifdef LOG_ON
            IcsLog::LogLevel("SendSubscribedData() Cars in zone are 0.", kLogLevelInfo);
#endif
            return EXIT_SUCCESS;
        }
    }

    if (typeinfo == typeid(SubsSetCamArea)) {
        // Subscription of setting a CAM beaconing area
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() SubsSetCamArea does not inform about anything.", kLogLevelInfo);
#endif
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(SubsStartTravelTimeCalculation)) {
        // Subscription of starting the calculation
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsStartTravelTimeCalculation does not inform about anything.",
                         kLogLevelInfo);
#endif
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(SubsStopTravelTimeCalculation)) {
        // Subscription of stopping the calculation
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsStopTravelTimeCalculation does not inform about anything.",
                         kLogLevelInfo);
#endif
        return EXIT_SUCCESS;
    }

    // Subscription to calculate travel time
    if (typeinfo == typeid(SubsCalculateTravelTime)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsCalculateTravelTime processing.", kLogLevelInfo);
#endif
        SubsCalculateTravelTime* subCalculateTT = static_cast<SubsCalculateTravelTime*>(subscription);
        if (subCalculateTT->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    // Subscription to return information about the received CAMs
    if (typeinfo == typeid(SubsGetReceivedCamInfo)) {

        SubsGetReceivedCamInfo* subsGetReceivedCamInfo = static_cast<SubsGetReceivedCamInfo*>(subscription);
        vector<TCamInformation>* camInfo = NULL;
        camInfo = subsGetReceivedCamInfo->getInformationFromLastReceivedCAMs();
        if (camInfo == NULL) {
            IcsLog::LogLevel("SendSubscribedData() received CAMs are NULL.", kLogLevelError);
            return EXIT_FAILURE;
        }
        if (camInfo->size() > 0) {

#ifdef LOG_ON
            stringstream log;
            log << "SendSubscribedData() Node " << nodeId << " received " << camInfo->size()
                << " CAM messages in the last time step.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            if (!m_appMessageManager->CommandSendSubscriptionReceivedCamInfo(camInfo, nodeId)) {
                IcsLog::LogLevel("SendSubscribedData() Error sending information about received CAMs.", kLogLevelError);
                delete camInfo;
                return EXIT_FAILURE;
            }
            delete camInfo;
            return EXIT_SUCCESS;
        } else {
            if (camInfo->size() == 0) {
#ifdef LOG_ON
                stringstream log;
                log << "[INFO] SendSubscribedData() Node " << nodeId << " did not receive CAM messages in the last time step.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                delete camInfo;
                return EXIT_SUCCESS;
            } else {
#ifdef LOG_ON
                stringstream log;
                log << "[ERROR] SendSubscribedData() Number of received CAMs is negative.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
                delete camInfo;
                return EXIT_FAILURE;
            }
        }
    }

    // Subscription to return information about facilities information about a node
    if (typeinfo == typeid(SubsGetFacilitiesInfo)) {

#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsGetFacilitiesInfo processing.", kLogLevelInfo);
#endif

        SubsGetFacilitiesInfo* subsGetFacilitiesInfo = static_cast<SubsGetFacilitiesInfo*>(subscription);

        //vector<unsigned char> facInfo = subsGetFacilitiesInfo->getFacilitiesInformation();
        tcpip::Storage* facilities_message = new tcpip::Storage();
        subsGetFacilitiesInfo->getFacilitiesInformation(facilities_message);

#ifdef LOG_ON
        stringstream log;
        log << "SendSubscribedData() Node " << nodeId << " will be updated about "
            << subsGetFacilitiesInfo->getNumberOfSubscribedFields() << " location related fields.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        if (!m_appMessageManager->CommandSendSubscriptionFacilitiesInfo(facilities_message, nodeId)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending facilities information.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    // Subscription to send an Application-level message
    if (typeinfo == typeid(SubsAppMessageSend)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsAppMessageSend processing.", kLogLevelInfo);
#endif
        SubsAppMessageSend* subsAppMessageSend = static_cast<SubsAppMessageSend*>(subscription);
        bool schedulingStatus = subsAppMessageSend->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppMessageSend() Node " << nodeId
            << " will be updated about the scheduling of the message of subscription " << subsAppMessageSend->m_id;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppMessageSend(schedulingStatus, nodeId, subsAppMessageSend->m_id)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppMessageReceive)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsAppMessageReceive processing.", kLogLevelInfo);
#endif
        SubsAppMessageReceive* subsAppMessageReceive = static_cast<SubsAppMessageReceive*>(subscription);

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppMessageReceive() Node " << nodeId << " will Pull the Communication Simulator for received data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (subsAppMessageReceive->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppCmdTraffSim)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsAppCmdTraffSim processing.", kLogLevelInfo);
#endif
        SubsAppCmdTraffSim* subsAppCmdTraffSim = static_cast<SubsAppCmdTraffSim*>(subscription);
        bool resultStatus = subsAppCmdTraffSim->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppCmdTraffSim() Node " << nodeId
            << " will be updated about the status of the command to Traffic Simulator.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppCmdTraffSim(resultStatus, nodeId, subsAppCmdTraffSim->m_id)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppResultTraffSim)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsAppResultTraffSim processing.", kLogLevelInfo);
#endif
        SubsAppResultTraffSim* subsAppResultTraffSim = static_cast<SubsAppResultTraffSim*>(subscription);
        vector<unsigned char> tsInfo = subsAppResultTraffSim->pull(m_syncManager);

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppResultTraffSim() Node " << nodeId << " will pull the Traffic Simulator for data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppResultTraffSim(tsInfo, nodeId, subsAppResultTraffSim->m_id)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsXApplicationData)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsXApplicationData processing.", kLogLevelInfo);
#endif
        SubsXApplicationData* subsXApplicationData = static_cast<SubsXApplicationData*>(subscription);
        vector<unsigned char> xAppData = subsXApplicationData->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsXApplicationData() Node " << nodeId << " will pull cross-application data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionXApplicationData(xAppData, nodeId, subsXApplicationData->m_id)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsGetMobilityInfo)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsGetMobilityInfo processing.", kLogLevelInfo);
#endif
        SubsGetMobilityInfo* subsMobility = static_cast<SubsGetMobilityInfo*>(subscription);
        if (subsMobility->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsGetTrafficLightInfo)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsGetTrafficLightInfo processing.", kLogLevelInfo);
#endif
        SubsGetTrafficLightInfo* subsTrafficLight = static_cast<SubsGetTrafficLightInfo*>(subscription);
        if (subsTrafficLight->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsSumoTraciCommand)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsSumoTraciCommand processing.", kLogLevelInfo);
#endif
        SubsSumoTraciCommand* subsMobility = static_cast<SubsSumoTraciCommand*>(subscription);
        if (subsMobility->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppControlTraci)) {
#ifdef LOG_ON
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsAppControlTraci processing.", kLogLevelInfo);
#endif
        SubsAppControlTraci* subsAppControlTraci = static_cast<SubsAppControlTraci*>(subscription);
        if (subsAppControlTraci->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }


    return EXIT_FAILURE;
}

bool ApplicationHandler::ExecuteApplication(int nodeId, ResultContainer* resultContainer) {
    if (resultContainer == NULL) {
        return false;
    }

    if (!m_appMessageManager->CommandApplicationToExecute(nodeId, resultContainer)) {
        return false;
    }

    return true;
}

int ApplicationHandler::SendMessageStatus(int nodeId, ResultContainer* result) {
    if (result == NULL) {
        cerr << "iCS --> [ERROR] SendMessageStatus() The result is NULL for node " << nodeId << endl;
        return EXIT_FAILURE;
    }

    if (result->AskSendMessageStatus()) {
        vector<pair<int, stationID_t> > messages;
        result->GetReceivedMessages(messages);
        if (m_appMessageManager->NotifyMessageStatus(nodeId, messages) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } else {
#ifdef LOG_ON
        stringstream log;
        log << "SendMessageStatus() The application does not send message status.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        return EXIT_SUCCESS;
    }

}

}
