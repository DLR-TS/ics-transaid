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
 * Author Michael Behrisch
 ***************************************************************************************/

#include "program-configuration.h"
#include "application/node.h"
#include "application/model/ics-interface.h"
#include "behaviour-uc-node.h"
#include "behaviour-uc-rsu.h"
#include "behaviour-uc-tmc.h"
#include "behaviour-uc-factory.h"
#include "vehicleManager.h"

using namespace baseapp;
using namespace baseapp::application;

#define USE_NS3

namespace ucapp {
namespace application {

//-------------------------------------------------------------------------------------------------------
//start ns3 simulation via application_config_file-scenario.xml (add '--ns3' in executable params)
//-------------------------------------------------------------------------------------------------------
BehaviourUCFactory::BehaviourUCFactory(const bool _use_ns3) :
    use_ns3(_use_ns3) {

//or start ns3 simulation via compile
#ifdef USE_NS3
    use_ns3 = true;
#endif

    if (use_ns3) {
        std::cout << "Running with ns3 interface" << std::endl;
    }

    VehicleManager::getInstance().create(use_ns3);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
BehaviourUCFactory::~BehaviourUCFactory() {

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCFactory::createRSUBehaviour(iCSInterface* interface, Node* node) {
    interface->SubscribeBehaviour(new BehaviourUCRSU(interface, use_ns3));
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCFactory::createNodeBehaviour(iCSInterface* interface, Node* node) {
    interface->SubscribeBehaviour(new BehaviourUCNode(interface, use_ns3));
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
TMCBehaviour* BehaviourUCFactory::createTMCBehaviour() {
    return new BehaviourUCTMC(use_ns3);
}

} /* namespace application */
} /* namespace ucapp */
