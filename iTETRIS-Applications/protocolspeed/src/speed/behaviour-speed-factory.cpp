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
#include "ics-interface.h"
#include "behaviour-speed-node.h"
#include "behaviour-speed-rsu.h"
#include "data-manager.h"
#include "behaviour-speed-factory.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {

void BehaviourSpeedFactory::createRSUBehaviour(iCSInterface* interface, Node* node) {
    BehaviourSpeedRSU* rsu = new BehaviourSpeedRSU(interface);
    RsuData data = ProgramConfiguration::GetRsuData(node->getId());
    rsu->AddDirections(data.directions);
    interface->SubscribeBehaviour(rsu);
    interface->SubscribeBehaviour(new DataManager(interface));
}

void BehaviourSpeedFactory::createNodeBehaviour(iCSInterface* interface, Node* node) {
    if (iCSInterface::UseSink) {
        interface->SubscribeBehaviour(new BehaviourNodeWithSink(interface));
    } else {
        interface->SubscribeBehaviour(new BehaviourNodeWithoutSink(interface));
    }
}

} /* namespace application */
} /* namespace protocol */
