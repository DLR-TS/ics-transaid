/****************************************************************************/
/// @file    app-result-open-buslanes.cpp
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2010 iTetris Project Consortium - All rights reserved
/****************************************************************************/

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
#include "app-result-open-buslanes.h"
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

std::map<ics_types::stationID_t, bool> ResultOpenBuslanes::m_knownInfo;

// ===========================================================================
// member method definitions
// ===========================================================================
ResultOpenBuslanes::ResultOpenBuslanes(stationID_t owner, int appHandlerId)
        : m_runningMessageID(0)
{
    m_applicationHandlerId = appHandlerId;
}

bool
ResultOpenBuslanes::ProcessResult(Storage& storage)
{
    if (storage.size() == 0) {
        cout << "[ERROR] ProcessResult() There is no data from the application to be processed." << endl;
        return false;
    }

    try {
        // read the command length
        storage.readUnsignedByte();
        // read the command id
        storage.readUnsignedByte();
        bool amMobile = storage.readUnsignedByte()!=0;
        // reads if the application execution status
        if (!amMobile) {
            m_areOpen = storage.readUnsignedByte()!=0;
        }
    } catch (std::invalid_argument e) {
        cout << "[ERROR] ProcessResult() an exception was thrown while reading result state message" << endl;
        return false;
    }

    return true;
}

int
ResultOpenBuslanes::ApplyResult(SyncManager* syncManager)
{
    ITetrisNode *node = syncManager->GetNodeByIcsId(m_ownerStation);
    if ((int) node->m_type == (int) STATION_MOBILE) {
        if (m_knownInfo.find(m_ownerStation)!=m_knownInfo.end()) {
            if (m_knownInfo.find(m_ownerStation)->second) {
                syncManager->m_trafficSimCommunicator->ReRoute(*node);
            }
        }
    } else {
        ics_types::icstime_t sendingTime = syncManager->m_simStep;
        int frequency = 500;                    // TODO: message frequency hardcoded.
        int payloadLength = 300;                // TODO: message payload length hardcoded.
        unsigned char appMsgType = 0x01;        // TODO: Application message type hardcoded.
        short numHops = 1;                      // TODO: Number of hops hardcoded.
        unsigned int msgLifetime = 2;           // TODO: Application message life time hardcoded.
        if (syncManager->ScheduleV2xTopobroadcastMessages(m_ownerStation, m_applicationHandlerId, m_runningMessageID, frequency, payloadLength, 1, appMsgType, 0xFF, 0xFF, numHops, msgLifetime) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        OpenBuslanesMessage msg;
        msg.messageID = m_runningMessageID;
        msg.sendingTime = sendingTime;
        msg.opened = m_areOpen;
        m_messages.push_back(msg);
        ++m_runningMessageID;
        return EXIT_SUCCESS;
    }
}

int
ResultOpenBuslanes::CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager)
{
    // Loop the msgs for the different vehicles
    int result = EXIT_FAILURE;
    for (vector<OpenBuslanesMessage>::iterator i=m_messages.begin() ; i<m_messages.end();) {
        OpenBuslanesMessage &msg = *i;
        if (msg.sendingTime + 2 <  syncManager->m_simStep) {
            i = m_messages.erase(i);
            continue;
        }
        if (msg.messageID == appMessageId) {
            msg.receiver.push_back(receiverId);
            result = EXIT_SUCCESS;
        }
        ++i;
    }

    return result;
}

vector<pair<int,stationID_t> >
ResultOpenBuslanes::GetReceivedMessages()
{
    std::set<ics_types::stationID_t> doubleCheck;
    vector<pair<int,stationID_t> > messages;
    for (std::vector<OpenBuslanesMessage>::const_iterator i=m_messages.begin(); i!=m_messages.end(); ++i) {
        const OpenBuslanesMessage &msg = *i;
        for (std::vector<ics_types::stationID_t>::const_reverse_iterator j=(*i).receiver.rbegin(); j!=(*i).receiver.rend(); ++j) {
            if (doubleCheck.find(*j)!=doubleCheck.end()) {
                continue;
            }
            doubleCheck.insert(*j);
            pair<int,stationID_t> message = make_pair(msg.messageID, (*j));
            messages.push_back(message);
        }
    }
/*#ifdef LOG_ON
    stringstream log;
    log << "[INFO] GetReceivedMessage() Message Id: " << m_travelTime.m_messageId << " checked as arrived.";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif*/
    return messages;
}

}
