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

#include "message-schedule.h"
#include "ns3-server.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/iTETRISns3Facilities.h"

NS_LOG_COMPONENT_DEFINE("MessageSchedule");

namespace ns3 {

MessageSchedule::MessageSchedule() {
}

MessageSchedule::~MessageSchedule() {
}

MessageScheduleUnicast::MessageScheduleUnicast() {
}

MessageScheduleUnicast::~MessageScheduleUnicast() {
}

void MessageScheduleUnicast::DoInvoke() {
    for (std::vector<std::string>::iterator senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end();
            senderIt++) {
        std::string cadena = *senderIt;
        std::stringstream temp;
        uint32_t nodeId;
        temp << cadena;
        temp >> nodeId;
        NS_LOG_INFO(Simulator::Now().GetSeconds() << " MessageScheduleUnicast::DoInvoke node " << nodeId);
        Ns3Server::instance_->my_packetManagerPtr->InitiateIdBasedTxon(nodeId, serviceId, commProfile, technologies,
                frequency, payloadLength, destination, msgRegenerationTime, msgLifetime, messageId, genericContainer);
    }
    delete this;
}

MessageScheduleGeoBroadcast::MessageScheduleGeoBroadcast() {
}

MessageScheduleGeoBroadcast::~MessageScheduleGeoBroadcast() {
}

void MessageScheduleGeoBroadcast::DoInvoke() {
    for (std::vector<std::string>::iterator senderIt = senderIdCollection.begin(); senderIt < senderIdCollection.end();
            senderIt++) {
        std::string cadena = *senderIt;
        std::stringstream temp;
        uint32_t nodeId;
        temp << cadena;
        temp >> nodeId;
        NS_LOG_INFO(Simulator::Now().GetSeconds() << " MessageScheduleGeoBroadcast::DoInvoke node " << nodeId);
        Ns3Server::instance_->my_packetManagerPtr->InitiateGeoBroadcastTxon(nodeId, serviceId, commProfile, technologies,
                destination, frequency, payloadLength, msgRegenerationTime, msgLifetime, messageId, genericContainer);
    }
    delete this;
}

} /* namespace ns3 */
