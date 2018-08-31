/****************************************************************************/
/// @file    app-result-traffic-jam-detection.cpp
/// @author  Ramon Bauza
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>

#include "foreign/tcpip/storage.h"
#include "app-result-traffic-jam-detection.h"
#include "../traffic_sim_communicator/traffic-simulator-communicator.h"
#include "app-commands-subscriptions-constants.h"
#include "../itetris-node.h"
#include "../sync-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"
#include "../utilities.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace tcpip;
using namespace std;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
ResultTrafficJamDetection::ResultTrafficJamDetection(stationID_t owner, int appHandlerId)
{
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool
ResultTrafficJamDetection::ProcessResult(Storage& storage)
{
    if (storage.size() == 0) {
        cout << "iCS --> #Error: There is no data from the application to be processed." << endl;
        return false;
    }

    try {
        // read the command length
        storage.readUnsignedByte();
        // read the command id
        storage.readUnsignedByte();
        // reads if the application execution status
        int hasResult = storage.readUnsignedByte();

        if (hasResult == APP_RESULT_ON) {
            // read the result
            m_containerIsEmpty = false;
            int numOfResults = storage.readInt();
            for (int i = 0; i < numOfResults ; i++) {
                TCteMessage result;
                result.m_nodeId = m_ownerStation;
                result.m_messageId = storage.readInt();
                int status = storage.readInt();
                switch (status) {

                case APP_RESULT_TO_SCHEDULE: {
                    result.m_frequency = storage.readInt();
                    result.m_payloadLength = storage.readInt();
                    result.m_msgRegenerationTime = storage.readFloat();
                    result.m_messageStatus = kToBeScheduled;
#ifdef LOG_ON
                    stringstream log;
                    log << "[INFO] ProcessResult() Node Id: " <<result.m_nodeId << " Message Id: " <<result.m_messageId << " received from App to be SCHEDULED";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                    m_cteMessages.push_back(result); // The message is new so it has to be included in the collection
                    break;
                }

                default:
                    cout << "iCS --> [ERROR] ProcessResult() Application returned result status does not exist: " << status << endl;
                    return false;
                }
            }
        } else {
            m_containerIsEmpty = true;
        }

    } catch (std::invalid_argument e) {
        cout << "iCS --> #Error: an exception was thrown while reading result state message" << endl;
        return false;
    }

    return true;
}

int
ResultTrafficJamDetection::ApplyResult(SyncManager* syncManager)
{
    vector<TCteMessage>::iterator it=m_cteMessages.begin();
    while (it != m_cteMessages.end()) {
        TCteMessage msg = *it;
        ITetrisNode* node = syncManager->GetNodeByIcsId(msg.m_nodeId);
        if (node == NULL) {
#ifdef LOG_ON
            stringstream log;
            log << "[WARNING] ApplyResult() Node was not retrieved with the iCS ID: " << msg.m_nodeId << " Left the simulation?";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
            ++it;
        } else { // Check the status of the message
            switch (msg.m_messageStatus) {
            case kToBeScheduled: { // Message has to be scheduled in ns-3
                unsigned char appMessageType = 0x01;
                unsigned char commProfile = 0xFF;
                unsigned char preferredRATs = 0xFF;
                short numHops = 1;
                unsigned int msgLifetime = 2;
                if (syncManager->ScheduleV2xTopobroadcastMessages(
                            m_ownerStation,
                            m_applicationHandlerId,
                            msg.m_messageId,
                            msg.m_frequency,
                            msg.m_payloadLength,
                            msg.m_msgRegenerationTime,
                            appMessageType,
                            commProfile,
                            preferredRATs,
                            numHops,
                            msgLifetime) == EXIT_FAILURE) {
                    return EXIT_FAILURE;
                }
                msg.m_messageStatus = kScheduled;
                (*it).m_messageStatus = kScheduled;
                (*it).m_schedulingTime = syncManager->m_simStep;
                ++it;
                break;
            }
            case kScheduled: {
                ++it;
                break;
            }
            case kArrived: {
                ++it;
                break;
            }
            case kToBeDiscarded: {
                it = m_cteMessages.erase(it);
                break;
            }
            default : {
                cout << "iCS --> [ApplyResult] The message result status is undefined. Node iCS ID: " << msg.m_nodeId << " Status: " << msg.m_messageStatus << endl;
                return EXIT_FAILURE;
            }
            }
        }
    }

    return EXIT_SUCCESS;
}

int
ResultTrafficJamDetection::CheckMessage(int appMessageId, ics_types::stationID_t receiverId)
{
    // Loop the msgs for the different vehicles
    for (vector<TCteMessage>::iterator it=m_cteMessages.begin() ; it<m_cteMessages.end() ; it++) {
        TCteMessage msg = *it;
        if (msg.m_messageId == appMessageId) {
            (*it).m_messageStatus = kArrived;
            (*it).m_listOfReceiverIds.push_back(receiverId);
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

int
ResultTrafficJamDetection::CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager)
{
    // Loop the msgs for the different vehicles
    int result = EXIT_FAILURE;
    for (vector<TCteMessage>::iterator it=m_cteMessages.begin() ; it<m_cteMessages.end() ; it++) {
        TCteMessage msg = *it;
        if (msg.m_messageId == appMessageId) {
            (*it).m_messageStatus = kArrived;
            (*it).m_listOfReceiverIds.push_back(receiverId);
            result = EXIT_SUCCESS;
        }
        if ((*it).m_schedulingTime + 2 <  syncManager->m_simStep) {
            (*it).m_messageStatus = kToBeDiscarded;
        }
    }

    return result;
}

void ResultTrafficJamDetection::GetReceivedMessages(vector<pair<int,stationID_t> > & messages)
{
    for (vector<TCteMessage>::iterator it=m_cteMessages.begin() ; it<m_cteMessages.end() ; it++) {
        TCteMessage msg = *it;
        if (msg.m_messageStatus == kArrived) {

            for (vector<int>::iterator iterReceivers = msg.m_listOfReceiverIds.begin(); iterReceivers!= msg.m_listOfReceiverIds.end(); iterReceivers++) {
                pair<int,stationID_t> message = make_pair(msg.m_messageId, (*iterReceivers)); // Store receiverId instead of senderId
                messages.push_back(message);

                stringstream log;
                log << "[INFO] GetReceivedMessage() Message Id: " << msg.m_messageId << " arrived. SenderId="<<msg.m_nodeId<<" ReceiverId="<<(*iterReceivers);
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            }
            (*it).m_listOfReceiverIds.clear();
            (*it).m_messageStatus = kScheduled;
        } else {

            stringstream log;
            log << "[INFO] GetReceivedMessage() Message Id: " << msg.m_messageId << " not arrived.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        }
    }
}

bool ResultTrafficJamDetection::AskSendMessageStatus()
{
	return true;
}

}

