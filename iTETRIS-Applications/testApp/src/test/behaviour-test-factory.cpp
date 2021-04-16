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
#include "node.h"
#include "server.h"
#include "ics-interface.h"
#include "behaviour-test-node.h"
#include "behaviour-test-rsu.h"
#include "behaviour-test-factory.h"
#include "TMCBehaviourTest.h"

using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {

void BehaviourTestFactory::createRSUBehaviour(iCSInterface* interface, Node* node) {
    interface->SubscribeBehaviour(new BehaviourTestRSU(interface));

    if (ProgramConfiguration::GetTestCase() == "TMCBehaviour" || ProgramConfiguration::GetTestCase() == "TMCBehaviour_multiRSU") {
        // Manually create a TMCBehaviour (in other applications, this should be done in BehaviourFactory::createTMCBehaviour())
        if (!createdTMCBehaviour) {
            std::cout << "BehaviourTestFactory::createRSUBehaviour(): Creating TMCBehaviour." << std::endl;
            TMCBehaviour* b = new TMCBehaviourTest();
            server::Server::GetNodeHandler()->setTMCBehaviour(b);
            createdTMCBehaviour = true;
        }
    }
}

void BehaviourTestFactory::createNodeBehaviour(iCSInterface* interface, Node* node) {
    interface->SubscribeBehaviour(new BehaviourTestNode(interface));
}

} /* namespace application */
} /* namespace protocol */
