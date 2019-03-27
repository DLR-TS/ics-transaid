/*
 * TMCBehaviour.cpp
 *
 *  Created on: Mar 25, 2019
 *      Author: Leonhard Luecken
 */

#include "ics-interface.h"
#include "current-time.h"
#include "TMCBehaviour.h"

namespace baseapp {
namespace application {

TMCBehaviour::TMCBehaviour() : iface(nullptr) {
}

TMCBehaviour::~TMCBehaviour() {
}

void
TMCBehaviour::addRSU(iCSInterface* rsu) {
    m_RSUController.insert(std::make_pair(rsu->GetId(), rsu));
    if (iface == nullptr) {
        iface = rsu;
    }
}

} /* namespace application */
} /* namespace baseapp */
