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

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <list>
#include "headers.h"

namespace protocol {
namespace server {
//I always only add 2 headers
enum Position {
    PAYLOAD_FRONT, PAYLOAD_END
};
class Payload {
public:
    Payload(int size);
    Payload(int id, int size);
    Payload(int id, int size, int timeStep);
    virtual ~Payload();
    int getId() const {
        return m_id;
    }
    int getTimeStep() const {
        return m_timeStep;
    }
    virtual int size() const;
    virtual void addHeader(application::Header* header);
    virtual void getHeader(application::Header*& header, Position position) const;
    virtual application::Header* getHeader(Position position) const;

    double snr;
private:
    int m_timeStep;
    int m_id;
    int m_size;
    std::list<application::Header*> m_headerList;
    static int PAYLOAD_ID;
};

} /* namespace server */
} /* namespace protocol */
#endif /* PAYLOAD_H_ */
