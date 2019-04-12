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

void TMCBehaviourTest::ReceiveMessage(int rsuID, server::Payload * payload, double snr, bool mobileNode) {
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
