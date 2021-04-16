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

#include "message-id-tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

MessageIdTag::MessageIdTag() {
}

MessageIdTag::~MessageIdTag() {
}

TypeId MessageIdTag::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::MessageIdTag").SetParent<Tag>().AddConstructor<MessageIdTag>().AddAttribute(
                            "messageId", "Id of the message", UintegerValue(0), MakeUintegerAccessor(&MessageIdTag::Get),
                            MakeUintegerChecker<uint32_t>());
    return tid;
}

TypeId MessageIdTag::GetInstanceTypeId(void) const {
    return GetTypeId();
}

uint32_t MessageIdTag::GetSerializedSize(void) const {
    return 4;
}

void MessageIdTag::Serialize(TagBuffer i) const {
    i.WriteU32(m_messageId);
}

void MessageIdTag::Deserialize(TagBuffer i) {
    m_messageId = i.ReadU32();
}

void MessageIdTag::Set(uint32_t messageId) {
    m_messageId = messageId;
}

uint32_t MessageIdTag::Get() const {
    return m_messageId;
}

void MessageIdTag::Print(std::ostream& os) const {
    os << "MessageId=" << m_messageId;
}

} /* namespace ns3 */
