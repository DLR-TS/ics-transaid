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
 * Author Vasilios Karagounis
 ***************************************************************************************/
#include "behaviour-uc-tmc.h"
#include <app-commands-subscriptions-constants.h>
//#include "log/console.h"
#include "vehicleManager.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {

//-------------------------------------------------------------------------------------------------------
//BehaviourUCTMC implementation
//-------------------------------------------------------------------------------------------------------
BehaviourUCTMC::BehaviourUCTMC(const bool use_ns3):
    m_use_ns3(use_ns3),
    m_initialize(true),
    m_useMobilityInfo(false) {
    try {
        m_useMobilityInfo = JsonReader::get()["useMobilityInfo"].get<std::string>() == "True";
    } catch (json::exception& e) {
        std::cout << "ERROR : TMC json error : " << e.what() << " Exception id : " << e.id << std::endl;
        exit(0);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
BehaviourUCTMC::~BehaviourUCTMC() {

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCTMC::OnAddSubscriptions() {
    if (m_initialize) {
        if (m_useMobilityInfo) {
            iface->requestMobilityInfo();
        }

        VehicleManager::getInstance().initialize(iface);

        m_initialize = false;

        std::cout << "TMC OnAddSubscriptions done! " << std::endl;
    }

    if (CurrentTime::Now() % iface->getSUMOStepLength() == 0) {
        VehicleManager::getInstance().onAddSubscriptions();
    }
}

//-------------------------------------------------------------------------------------------------------
//CAM = Cooperative Awareness Message
//DENM = Decentralised Environmental Notification Message
//MCM = Manoeuvre Coordination Message
//RSU = Road-Side Unit
//-------------------------------------------------------------------------------------------------------
void BehaviourUCTMC::ReceiveMessage(int rsuID, server::Payload* payload, double snr, bool /* mobileNode */) {

}

//-------------------------------------------------------------------------------------------------------
// Execute application TM logic
//-------------------------------------------------------------------------------------------------------
void BehaviourUCTMC::Execute() {
    if (CurrentTime::Now() % iface->getSUMOStepLength() == 0) {
        VehicleManager::getInstance().execute();
    }
}
} /* namespace application */
} /* namespace ucapp */
