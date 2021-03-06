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

#include "payload-storage.h"
#include "../utils/log/log.h"
#include <sstream>

namespace protocol {
namespace server {

using namespace std;

PayloadStorage::PayloadStorage() {
    m_counter = 0;
}

PayloadStorage::~PayloadStorage() {
    if (m_storage.size() > 0) {
        for (map<unsigned, const Payload*>::const_iterator it = m_storage.begin(); it != m_storage.end(); ++it) {
            delete (it->second);
        }
        m_storage.clear();
    }
}

string PayloadStorage::insert(const Payload* payload, const StoragePolicy policy) {
    m_storage.insert(make_pair(++m_counter, payload));
    return createKey(policy);
}

bool PayloadStorage::find(const string& key, Payload*& payload) {
    map<unsigned, const Payload*>::iterator it = m_storage.find(getIndex(key));
    if (it == m_storage.end()) {
        return false;
    }
    payload = (Payload*) it->second;
    if (asPolicy(key) == kDeleteOnRead) {
        m_storage.erase(it);
    }
    return true;
}

bool PayloadStorage::erase(const std::string& key) {
    return m_storage.erase(getIndex(key));
}

bool PayloadStorage::eraseAndDelete(const string& key) {
    map<unsigned, const Payload*>::iterator it = m_storage.find(getIndex(key));
    if (it == m_storage.end()) {
        return false;
    }
    delete (it->second);
    m_storage.erase(it);
    return true;
}

bool PayloadStorage::hasKey(const string& key) const {
    return m_storage.count(getIndex(key));
}

int PayloadStorage::expiredPayloadCleanUp(const int oldTimeStep) {
    int number = 0;
    ostringstream log;
    log << "ExpiredPayloadCleanUp for time: " << oldTimeStep << ". Removing keys: ";
    for (map<unsigned, const Payload*>::iterator it = m_storage.begin(); it != m_storage.end();) {
        if (it->second->getTimeStep() <= oldTimeStep) {
            log << it->first << " ";
            ++number;
            delete (it->second);
            m_storage.erase(it++);
        } else {
            ++it;
        }
    }
    if (number == 0) {
        log << "noting to remove ";
    }
    log << "Current size: " << m_storage.size();
    Log::WriteLog(log);
    return number;
}

unsigned PayloadStorage::getIndex(const string& key) const {
    unsigned index;
    istringstream(key.substr(1, key.size())) >> index;
    return index;
}

string PayloadStorage::createKey(const StoragePolicy policy) const {
    ostringstream key;
    key << asChar(policy) << m_counter;
    return key.str();
}

char PayloadStorage::asChar(const StoragePolicy policy) const {
    return (char) policy;
}

StoragePolicy PayloadStorage::asPolicy(const string& key) const {
    switch (key[0]) {
        case 'd':
            return kDeleteOnRead;
        case 'm':
            return kMultipleRead;
        default:
            return kDeleteOnRead;
    }
}

} /* namespace server */
} /* namespace protocol */
