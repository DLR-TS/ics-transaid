/****************************************************************************/
/// @file    V2X-message-manager.cpp
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
/**************************************************************************************
 * Edited by Jérôme Härri <haerri@eurecom.fr>
 * EURECOM 2015
 *
 * Added a generic tap container to allow sending and returning
 * generic Tx/Rx specific data from iCS an ns-3
 *
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 *************************************************************************************/

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

#include <iostream>

#include "V2X-message-manager.h"
#include "V2X-cam-area.h"
#include "V2X-geobroadcast-area.h"

#ifndef _WIN32
#include <config.h>
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
V2xMessageManager::V2xMessageManager()
{
    m_v2xCamAreaCollection = new vector<V2xCamArea*>();
    m_v2xGeobroadcastAreaCollection = new vector<V2xGeobroadcastArea*>();
}

V2xMessageManager::~V2xMessageManager()
{
    delete m_v2xCamAreaCollection;
    delete m_v2xGeobroadcastAreaCollection;
}

bool
V2xMessageManager::CreateV2xCamArea(int subscriptionId, float frequency, unsigned int payloadLenght)
{
    m_v2xCamAreaCollection->push_back(new V2xCamArea(subscriptionId, frequency, payloadLenght));
    return true;
}

bool
V2xMessageManager::CreateV2xGeobroadcastArea(int subscriptionId, float frequency, unsigned int payloadLenght)
{
    m_v2xGeobroadcastAreaCollection->push_back(new V2xGeobroadcastArea(subscriptionId, frequency, payloadLenght));
    return true;
}

void
V2xMessageManager::InsertCamRow(vector<ScheduledCamMessageData> &table, ScheduledCamMessageData data)
{
    table.push_back(data);
}

void
V2xMessageManager::InsertUnicastRow(vector<ScheduledUnicastMessageData> &table, ScheduledUnicastMessageData data)
{
    table.push_back(data);
}

void
V2xMessageManager::InsertGeobroadcastRow(vector<ScheduledGeobroadcastMessageData> &table, ScheduledGeobroadcastMessageData data)
{
    table.push_back(data);
}

void
V2xMessageManager::InsertTopobroadcastRow(vector<ScheduledTopobroadcastMessageData> &table, ScheduledTopobroadcastMessageData data)
{
    table.push_back(data);
}

bool
V2xMessageManager::CompareCamRows(ScheduledCamMessageData rowReceived, ScheduledCamMessageData rowTable)
{
    bool coincidence;

    coincidence = false;
    if (rowReceived.senderNs3ID == rowTable.senderNs3ID) {
        if (rowReceived.messageType == rowTable.messageType) {
            if (rowReceived.timeStep == rowTable.timeStep) {
                if (rowReceived.sequenceNumber == rowTable.sequenceNumber) {
                    coincidence = true;
                }
            }
        }
    }

    return coincidence;
}

bool
V2xMessageManager::CompareUnicastRows(ScheduledUnicastMessageData rowReceived, ScheduledUnicastMessageData rowTable, int resolution)
{
    bool coincidence;

    coincidence = false;
    if (rowReceived.senderNs3ID == rowTable.senderNs3ID) {
        if (rowReceived.receiverNs3ID == rowTable.receiverNs3ID) {
            if (rowReceived.messageType == rowTable.messageType) {
              if (rowReceived.timeStep == rowTable.timeStep || rowReceived.timeStep == rowTable.timeStep + resolution || rowReceived.timeStep == rowTable.timeStep - resolution) {
                    if (rowReceived.sequenceNumber == rowTable.sequenceNumber) {
                        coincidence = true;
                    }
                }
            }
        }
    }

    return coincidence;
}

bool
V2xMessageManager::CompareGeobroadcastRows(ScheduledGeobroadcastMessageData rowReceived, ScheduledGeobroadcastMessageData rowTable, int resolution)
{
    bool coincidence;

    coincidence = false;
    if (rowReceived.senderNs3ID == rowTable.senderNs3ID) {
        if (rowReceived.messageType == rowTable.messageType) {
            if (rowReceived.timeStep == rowTable.timeStep || rowReceived.timeStep == rowTable.timeStep + resolution || rowReceived.timeStep == rowTable.timeStep - resolution) {
                if (rowReceived.sequenceNumber == rowTable.sequenceNumber) {
                    coincidence = true;
                }
            }
        }
    }

    return coincidence;
}

bool
V2xMessageManager::CompareTopobroadcastRows(ScheduledTopobroadcastMessageData rowReceived, ScheduledTopobroadcastMessageData rowTable)
{
    bool coincidence;

    coincidence = false;
    if (rowReceived.senderNs3ID == rowTable.senderNs3ID) {
        if (rowReceived.messageType == rowTable.messageType) {
            if (rowReceived.timeStep == rowTable.timeStep) {
                if (rowReceived.sequenceNumber == rowTable.sequenceNumber) {
                    coincidence = true;
                }
            }
        }
    }

    return coincidence;
}

void
V2xMessageManager::UpdateIdentifiersTable(vector<IdentifiersStorageStruct> &table, actionID_t actionID, stationID_t senderID, stationID_t receiverID)
{
    bool exists = true;

    if (table.size() > 0) {
        for (vector<IdentifiersStorageStruct>::iterator it = table.begin(); it != table.end(); it++) {
            if (actionID == (*it).actionID) {
                if (senderID == (*it).senderID) {
                    if (receiverID == (*it).receiverID) {
                        if ((*it).stored == false) {
                            exists = true;
                            (*it).stored = true;
                        } else {
                            exists = true;
                        }

                    } else {
                        exists = false;
                    }
                } else {
                    cout << "[UpdateIdentifiersStorageStruct]: Error-> The senderID doesn't correspond to the actionID" << endl;
                }
            } else {
                exists = false;
            }

        }
    } else {
        exists = false;
    }


    if (exists == false) {
        IdentifiersStorageStruct identifStruct;

        identifStruct.actionID = actionID;
        identifStruct.senderID = senderID;
        identifStruct.receiverID = receiverID;
        identifStruct.stored = false;
        table.push_back(identifStruct);
    }
}

vector<stationID_t>
V2xMessageManager::GroupReceivers(std::vector<IdentifiersStorageStruct> &table, actionID_t actionID, stationID_t senderID)
{
    vector<IdentifiersStorageStruct>::iterator i = table.begin();
    vector<stationID_t> vReceivers;

    while (i != table.end()) {
        if ((*i).actionID == actionID) {
            if ((*i).senderID == senderID) {
                vReceivers.push_back((*i).receiverID);
                (*i).stored = true;
            }
        }
        ++i;
    }
    return vReceivers;
}

}
