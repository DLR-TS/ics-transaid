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
