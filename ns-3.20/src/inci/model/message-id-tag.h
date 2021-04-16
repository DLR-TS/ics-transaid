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

#ifndef SRC_INCI_MODEL_MESSAGE_ID_TAG_H_
#define SRC_INCI_MODEL_MESSAGE_ID_TAG_H_

#include "ns3/tag.h"

namespace ns3 {
class MessageIdTag: public Tag {
public:
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;

    MessageIdTag();
    virtual ~MessageIdTag();

    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual uint32_t GetSerializedSize() const;
    virtual void Print(std::ostream& os) const;

    uint32_t Get(void) const;
    void Set(uint32_t MessageId);
private:
    uint32_t m_messageId;
};

} /* namespace ns3 */

#endif /* SRC_INCI_MODEL_MESSAGE_ID_TAG_H_ */
