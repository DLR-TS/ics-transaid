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
/*
 * TMCBehaviourTest.cpp
 *
 *  Created on: Mar 26, 2019
 *      Author: Leonhard Luecken
 */


#include <iostream>
#include <map>
#include "TMCBehaviourTest.h"
#include "current-time.h"
#include "application/model/ics-interface.h"

namespace testapp {
namespace application {

TMCBehaviourTest::TMCBehaviourTest() {

}

TMCBehaviourTest::~TMCBehaviourTest() {
}

void TMCBehaviourTest::ReceiveMessage(int rsuID, server::Payload* payload, double snr, bool mobileNode) {
    if (mobileNode || !isActive()) {
        return;
    }
    if (m_RSUController.find(rsuID) == m_RSUController.end()) {
        std::cerr << CurrentTime::Now() << "TMCBehaviourTest::ReceiveMessage(): ERROR: RSU " << rsuID << " unknown to TMC Behaviour!" << std::endl;
    } else {
        std::cout << CurrentTime::Now() << " TMCBehaviourTest::ReceiveMessage(): received message from RSU " << rsuID << ", snr=" << snr << std::endl;
    }
}

void TMCBehaviourTest::Execute() {
    if (!isActive()) {
        return;
    }
    std::cout << CurrentTime::Now() << " TMCBehaviourTest::Execute() called."
              << " hostController: " << iface->GetId() << std::endl;
}

void TMCBehaviourTest::OnAddSubscriptions() {
    if (!isActive()) {
        return;
    }
    std::cout << CurrentTime::Now() << " TMCBehaviourTest::onAddSubscriptions() called."
              << " hostController: " << iface->GetId() << std::endl;
}

bool TMCBehaviourTest::isActive() {
    return CurrentTime::Now() >= 0;
}


} /* namespace application */
} /* namespace testapp */
