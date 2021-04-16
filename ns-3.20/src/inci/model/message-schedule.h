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
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#ifndef MESSAGESCHEDULE_H_
#define MESSAGESCHEDULE_H_

#include "ns3/itetris-types.h"

namespace ns3 {

class MessageSchedule {
public:
    virtual ~MessageSchedule();
    virtual void DoInvoke() = 0;

    std::vector<std::string> senderIdCollection;
    std::string serviceId;
    int commProfile;
    std::vector<std::string> technologies;
    float frequency;
    int payloadLength;
    float msgRegenerationTime;
    int msgLifetime;
    std::vector<unsigned char> genericContainer;
    double time;
    int messageId;
protected:
    MessageSchedule();
};

class MessageScheduleUnicast: public MessageSchedule {
public:
    MessageScheduleUnicast();
    virtual ~MessageScheduleUnicast();
    virtual void DoInvoke();

    int destination;
};

class MessageScheduleGeoBroadcast: public MessageSchedule {
public:
    MessageScheduleGeoBroadcast();
    virtual ~MessageScheduleGeoBroadcast();
    virtual void DoInvoke();

    CircularGeoAddress destination;
};

} /* namespace ns3 */

#endif /* MESSAGESCHEDULE_H_ */
