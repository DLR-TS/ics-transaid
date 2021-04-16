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
/// @file    app-result-generic.cpp
/// @author  Jerome Haerri
/// @date 17 juin 2012
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
#include <cstdlib>
#include <sstream>

#include "foreign/tcpip/storage.h"
#include "app-result-generic.h"
#include "../traffic_sim_communicator/traffic-simulator-communicator.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/log/ics-log.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace tcpip;
using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// member method definitions
// ===========================================================================
ResultGeneric::ResultGeneric(stationID_t owner, int appHandlerId) {
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool ResultGeneric::ProcessResult(Storage& storage) {

    if (storage.size() == 0) {
#ifdef LOG_ON
        IcsLog::LogLevel("iCS --> [ResultGeneric] ProcessResult() There is no data from the application to be processed.",
                         kLogLevelInfo);
#endif
        return false;
    }

    try {

        // read the command length
        storage.readInt();
        // read the command id
        storage.readUnsignedByte();
        // reads if the application execution status
        int hasResult = storage.readUnsignedByte();
        if (hasResult == APP_RESULT_ON) {
            // read the result
            m_containerIsEmpty = false;
            int numOfResults = storage.readInt();
#ifdef LOG_ON
            stringstream log;
            log << "ResultGeneric::ProcessResult() for TAGs [";
#endif
            for (int i = 0; i < numOfResults; i++) {
                int cmd = storage.readUnsignedByte();
                std::string tag = storage.readString();
                int local_length = storage.readInt();
                vector<unsigned char> packet; //(local_length);
                packet.reserve(local_length);
                for (unsigned int i = 0; i < local_length; i++) {
                    packet.push_back(storage.readChar());
                }
#ifdef LOG_ON
                log << tag << ", ";
#endif
                push(cmd, tag, packet);
            }

#ifdef LOG_ON
            log << "]";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
            log.str("");
            log << "ResultGeneric::ProcessResult() The application executed in the station: " << m_ownerStation
                << " appHandlerId " << m_applicationHandlerId;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        } else if (hasResult == APP_RESULT_OFF) {
            m_containerIsEmpty = true;

#ifdef LOG_ON
            stringstream log;
            log << "ResultGeneric::ProcessResult() The application did NOT execute in the station: " << m_ownerStation;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        } else {
            IcsLog::LogLevel("ResultGeneric::ProcessResult() The status of the result is unknown", kLogLevelWarning);
        }
    } catch (std::invalid_argument e) {
        cout << "[ERROR] ResultGeneric::ProcessResult() an exception was thrown while reading result state message" << endl;
        return false;
    }
    return true;
}

int ResultGeneric::ApplyResult(SyncManager* syncManager) {
#ifdef LOG_ON
    IcsLog::LogLevel("ApplyResult() The application does not return any result and no data is applied.", kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

vector<pair<int, stationID_t> > ResultGeneric::GetReceivedMessages() {
    vector<pair<int, stationID_t> > auxiliar;
#ifdef LOG_ON
    IcsLog::LogLevel("GetReceivedMessages() The application does not return any result and no messages are not received.",
                     kLogLevelInfo);
#endif
    return auxiliar;
}

bool ResultGeneric::push(int CMD_TYPE, std::string TAG, vector<unsigned char> result) {
    containerMap::iterator it = m_resultMap.find(CMD_TYPE);
    if (it != m_resultMap.end()) {
        inMap* resultsMap = &(it->second);
        inMap::iterator it2 = resultsMap->find(TAG);
        if (it2 != resultsMap->end()) {
            resultsMap->erase(TAG);
            resultsMap->insert(make_pair(TAG, result));
        } else {
            resultsMap->insert(make_pair(TAG, result));
        }
    } else {
        inMap resultsMap;
        resultsMap.insert(make_pair(TAG, result));
        m_resultMap.insert(make_pair(CMD_TYPE, resultsMap));
    }
    return true;
}

std::vector<unsigned char> ResultGeneric::pull(int CMD_TYPE, std::string TAG) {
#ifdef LOG_PULL
    stringstream log;
    log << "ResultGeneric::pull ";
    for (containerMap::iterator it = m_resultMap.begin(); it != m_resultMap.end(); it++) {
        log << "Command " << it->first << " {";
        for (inMap::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            log << it2->first << " " << it2->second.size() << " [";
            for (int i = 0; i <= it2->second.size(); i++) {
                log << (int) it2->second[i];
            }
            log << "] ";
        }
        log << "}\n";
    }
    IcsLog::LogLevel(log.str().c_str(), kLogLevelInfo);
#endif
    containerMap::iterator it = m_resultMap.find(CMD_TYPE);
    if (it != m_resultMap.end()) {
        inMap resultsMap = it->second;

        inMap::iterator it2 = resultsMap.find(TAG);
        if (it2 != resultsMap.end()) {
            return it2->second;
        } else {
            std::vector<unsigned char> empty;
            return empty;
        }
    } else {
        std::vector<unsigned char> empty;
        return empty;
    }
}

}

