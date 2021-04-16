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

#include "behaviour.h"

#include "ics-interface.h"
#include "log/log.h"

namespace protocol {
namespace application {

Behaviour::Behaviour(iCSInterface* controller) :
    m_running(false) {
    m_controller = controller;
    RegisterTrace("StartToggle", m_traceStartToggle);
}

Behaviour::~Behaviour() {
    //do not delete m_controller here
}

bool Behaviour::IsActiveOnStart() const {
    return true;
}

bool Behaviour::IsRunning() const {
    return m_running;
}

iCSInterface* Behaviour::GetController() const {
    return m_controller;
}

void Behaviour::Start() {
    NS_LOG_FUNCTION(Log());
    if (m_running) {
        NS_LOG_ERROR(Log() << "Was already on");
    }
    m_running = true;
    m_traceStartToggle(true);
}

void Behaviour::Stop() {
    NS_LOG_FUNCTION(Log());
    m_running = false;
    m_traceStartToggle(false);
}

std::string Behaviour::Log() const {
    std::ostringstream outstr;
    outstr << m_controller->NodeName() << ": " << ToString(this->GetType()) << ": ";
    return outstr.str();
}

} /* namespace application */
} /* namespace protocol */
