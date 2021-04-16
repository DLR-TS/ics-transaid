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

#include "payload.h"
#include "current-time.h"
#include "log/log.h"

namespace protocol {
namespace server {

int Payload::PAYLOAD_ID = 0;

Payload::Payload(int size) :
    m_size(size) {
    m_id = ++PAYLOAD_ID;
    m_timeStep = CurrentTime::Now();
}

Payload::Payload(int id, int size) :
    m_id(id), m_size(size) {
    m_timeStep = CurrentTime::Now();
}

Payload::Payload(int id, int size, int timeStep) :
    m_id(id), m_timeStep(timeStep), m_size(size)
{}

Payload::~Payload() {
    for (std::list<application::Header*>::const_iterator it = m_headerList.begin(); it != m_headerList.end(); ++it) {
        delete *it;
    }
    m_headerList.clear();
}

int Payload::size() const {
    int size = m_size;
    for (std::list<application::Header*>::const_iterator it = m_headerList.begin(); it != m_headerList.end(); ++it) {
        size += (*it)->GetSerializedSize();
    }
    return size;
}

void Payload::addHeader(application::Header* header) {
    m_headerList.push_front(header);
}

void Payload::getHeader(application::Header*& header, Position position) const {
    if (position == PAYLOAD_FRONT) {
        header = m_headerList.front();
    } else {
        header = m_headerList.back();
    }
}

application::Header* Payload::getHeader(Position position) const {
    if (position == PAYLOAD_FRONT) {
        return m_headerList.front();
    } else {
        return m_headerList.back();
    }
}

} /* namespace server */
} /* namespace protocol */
