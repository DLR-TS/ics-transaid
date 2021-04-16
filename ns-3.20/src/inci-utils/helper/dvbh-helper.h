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
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */


#ifndef DVBH_HELPER_H
#define DVBH_HELPER_H

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"

namespace ns3 {

class DVBHLinkLayerBaseStation;
class DVBHControlPacket;
class DVBHManagerBaseStation;
class DVBHPhyLayerBaseStation;
class DVBHNetDeviceBaseStation;
class DVBHOfdmLayer;
class DVBHChannel;

class Node;

class DvbhPhyHelper {
public:
    ~DvbhPhyHelper();
    Ptr<Object> Create(std::string type) const;
    static DvbhPhyHelper Default(void);
    void SetNodeType(std::string type);
    void SetChannel(Ptr<DVBHChannel> channel);
    void Set(std::string name, const AttributeValue& v);
private:

    ObjectFactory m_phy;
    Ptr<DVBHChannel> m_channel;
};

class DvbhOfdmLayerHelper {
public:
    DvbhOfdmLayerHelper();
    ~DvbhOfdmLayerHelper();
    Ptr<DVBHOfdmLayer> Create(void) const;
    static DvbhOfdmLayerHelper Default(void);
    void Set(std::string name, const AttributeValue& v);
private:

    ObjectFactory m_ofdm;

};


class DvbhLinkLayerHelper {
public:
    ~DvbhLinkLayerHelper();
    Ptr<Object> Create(std::string type) const;
    static DvbhLinkLayerHelper Default(void);
private:

    ObjectFactory m_linkLayer;
};



class DvbhHelper {
public:

    DvbhHelper();

    static DvbhHelper Default(void);
    NetDeviceContainer Install(const DvbhOfdmLayerHelper& ofdmHelper, NodeContainer c, std::string nodeType/*,Ptr<DVBHChannel> broadcastChannel*/) const;

};

} // namespace ns3

#endif /* DVBH_BaseStation_Helper */
