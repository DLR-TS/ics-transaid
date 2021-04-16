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
/// @file    itetris-node.cpp
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "itetris-node.h"
#include "ics.h"
#include "../utils/ics/iCStypes.h"
#include "sync-manager.h"
#include "FacilitiesManager.h"
#include "../utils/ics/log/ics-log.h"

using namespace std;
using namespace ics_types;

namespace ics {

// ===========================================================================
// static member definitions
// ===========================================================================
int ITetrisNode::m_idCounter = -1;
set<stationID_t> ITetrisNode::m_preAssignedIds;


// ===========================================================================
// member method definitions
// ===========================================================================
ITetrisNode::ITetrisNode() {
    m_idCounter++;

    m_applicationHandlerInstalled = new vector<ApplicationHandler*>();
    m_subscriptionCollection = new vector<Subscription*>();
    m_resultContainerCollection = new vector<ResultContainer*>();
    m_lastTimeStepReceivedMessages = new vector<V2xMessage*>();
    m_newNode = false;
}

ITetrisNode::~ITetrisNode() {
    delete m_applicationHandlerInstalled;
    delete m_subscriptionCollection;
    delete m_resultContainerCollection;
    delete m_lastTimeStepReceivedMessages;

    stringstream log;
    log << "ITetrisNode() Removed node with iCS ID: " << m_icsId;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
}

float
ITetrisNode::GetPositionX() {
    return SyncManager::m_facilitiesManager->getStationPosition(m_icsId).x();
}

float
ITetrisNode::GetPositionY() {
    return SyncManager::m_facilitiesManager->getStationPosition(m_icsId).y();
}

string
ITetrisNode::GetLane() {
    return SyncManager::m_facilitiesManager->getMobileStationLaneID(m_icsId);
}

}
