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

#include "ics-interface.h"
#include "node.h"
#include <app-commands-subscriptions-constants.h>
#include "behaviour-speed-rsu.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {

///BehaviourNodeWithSink implementation

BehaviourSpeedRSU::BehaviourSpeedRSU(iCSInterface* controller) :
    BehaviourRsu(controller)
{}

BehaviourSpeedRSU::~BehaviourSpeedRSU()
{}

void BehaviourSpeedRSU::Start() {

    if (m_directions.size() == 0) {
        NS_FATAL_ERROR(Log() << "Can't start the RSU with 0 directions");
    }
    int totTime = 0;
    for (std::vector<VehicleDirection>::iterator it = m_directions.begin(); it != m_directions.end(); ++it) {
        if (it->time == 0) {
            it->time = m_timeBeaconMin;
            NS_LOG_INFO(Log() << "Direction " << *it << " beacon time was Zero. Set to " << m_timeBeaconMin);
        } else {
            NS_LOG_INFO(Log() << "Direction " << *it << " beacon time is " << it->time);
        }
        totTime += it->time;
    }
    NS_LOG_INFO(Log() << "Total beacon time will be " << totTime);
    m_eventBeacon = Scheduler::Schedule(0, &BehaviourSpeedRSU::EventBeacon, this, 0);
    m_eventCheck = Scheduler::Schedule(m_timeCheck, &BehaviourSpeedRSU::EventCheck, this);

    BehaviourRsu::Start();
    GetController()->startReceivingUnicast();
    GetController()->startReceivingGeobroadcast(PROTOCOL_MESSAGE);
    GetController()->requestMobilityInfo();
    GetController()->requestTrafficLightInfo();
}


} /* namespace application */
} /* namespace protocol */
