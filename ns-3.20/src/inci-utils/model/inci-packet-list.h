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
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ramon Bauza <rbauza@umh.es>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef INCI_PACKET_LIST_H
#define INCI_PACKET_LIST_H

#include "ns3/object.h"
#include "ns3/node.h"
#include <stdint.h>
#include <string>
#include <vector>
#include "ns3/storage.h"

namespace ns3 {

class InciPacket;

/**
 * @class InciPacketList
 * @brief The class InciPacketList stores all the packets that have been received by a node. The InciPacketList is attached to every node.
 */
class InciPacketList: public Object {
public:

    static TypeId GetTypeId(void);
    InciPacketList(void);
    bool ReceiveFromApplication(uint32_t senderId, std::string msgType, uint32_t ts, uint32_t tsSeqNo,
                                uint32_t messageId, tcpip::Storage* genericTagContainer);
    bool GetReceivedPacket(InciPacket& packet);

    void DoDispose(void);

    void SetNode(Ptr<Node> node);
    void NotifyNewAggregate();

    int Size() const;

private:

    std::vector<InciPacket*> m_packetList;
    Ptr<Node> m_node;
    unsigned m_position;

};

}

#endif
