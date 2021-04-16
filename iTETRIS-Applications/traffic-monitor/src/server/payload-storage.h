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

#ifndef MESSAGESTORAGE_H_
#define MESSAGESTORAGE_H_

#include <map>
#include <string>
#include "payload.h"

namespace protocol {
namespace server {

enum StoragePolicy {
    kDeleteOnRead = (int) 'd', kMultipleRead = (int) 'm'
};

class PayloadStorage {
public:
    PayloadStorage();
    virtual ~PayloadStorage();

    std::string insert(const Payload* payload, const StoragePolicy policy = kDeleteOnRead);
    bool find(const std::string& key, Payload*& payload);
    bool erase(const std::string& key);
    bool eraseAndDelete(const std::string& key);
    bool hasKey(const std::string& key) const;
    int expiredPayloadCleanUp(const int oldTimeStep);
    StoragePolicy asPolicy(const std::string& key) const;
    int size() const {
        return m_storage.size();
    }

private:
    std::string createKey(const StoragePolicy policy) const;
    char asChar(const StoragePolicy policy) const;
    unsigned getIndex(const std::string& key) const;

private:
    std::map<unsigned, const Payload*> m_storage;
    unsigned m_counter;
};

} /* namespace server */
} /* namespace protocol */

#endif /* MESSAGESTORAGE_H_ */
