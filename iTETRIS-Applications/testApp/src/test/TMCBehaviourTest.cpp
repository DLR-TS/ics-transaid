/*
 * TMCBehaviourTest.cpp
 *
 *  Created on: Mar 26, 2019
 *      Author: Leonhard Luecken
 */


#include "TMCBehaviourTest.h"
#include "current-time.h"
#include "ics-interface.h"

namespace testapp {
namespace application {

TMCBehaviourTest::TMCBehaviourTest() {

}

TMCBehaviourTest::~TMCBehaviourTest() {
}


void
TMCBehaviourTest::addRSU(iCSInterface * rsu) {
    TMCBehaviour::addRSU(rsu);
    std::cout << CurrentTime::Now() << " TMCBehaviourTest::addRSU(), rsuID: " << rsu->GetId() << std::endl;
}

void TMCBehaviourTest::ReceiveMessage(int rsuID, server::Payload * payload, double snr) {
    if (m_RSUController.find(rsuID) == m_RSUController.end()) {
        std::cerr << CurrentTime::Now() << "TMCBehaviourTest::ReceiveMessage(): ERROR: RSU " << rsuID << " unknown to TMC Behaviour!" << std::endl;
    } else {
        std::cout << CurrentTime::Now() << " TMCBehaviourTest::ReceiveMessage(): received message from RSU " << rsuID << ", snr=" << snr << std::endl;
    }
}

void TMCBehaviourTest::Execute() {
    std::cout << CurrentTime::Now() << " TMCBehaviourTest::Execute() called."
            << " hostController: " << iface->GetId() << std::endl;
}

void TMCBehaviourTest::OnAddSubscriptions() {
    std::cout << CurrentTime::Now() << " TMCBehaviourTest::onAddSubscriptions() called."
            << " hostController: " << iface->GetId() << std::endl;
}

} /* namespace application */
} /* namespace testapp */
