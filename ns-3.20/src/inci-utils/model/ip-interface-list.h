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
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

#ifndef IP_INTERFACE_LIST_H
#define IP_INTERFACE_LIST_H

#include "ns3/object.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/ipv6-interface-address.h"
#include <map>

namespace ns3 {

/**
 * @class IpInterfaceList
 * @brief The class IpInterfaceList should be attached to every node that has IP stack. It maintains a mapping between the name of the NetDevices and their assigned IP addresses. Thus, it allows retrieving the IP address of a given NetDevice from other modules in ns-3, e.g. GetIpAddress("WiFi")
 */
class IpInterfaceList : public Object {
public:
    static TypeId GetTypeId(void);
    IpInterfaceList(void);
    bool AddIpInterface(std::string InterfaceName, Ipv4InterfaceAddress interface);
    bool AddIpInterface(std::string InterfaceName, Ipv6InterfaceAddress interface);
    Ipv4InterfaceAddress GetIpInterfaceAddress(std::string InterfaceName);
    //NEW
    Ipv6InterfaceAddress GetIpv6InterfaceAddress(std::string InterfaceName);
    Ipv4Address GetIpAddress(std::string interfaceName);
    //NEW
    Ipv6Address GetIpv6Address(std::string interfaceName);

private:
    typedef std::map<std::string, Ipv4InterfaceAddress> InterfaceList;
    typedef std::map<std::string, Ipv6InterfaceAddress> InterfaceListv6;
    InterfaceList m_ipInterfaceList;
    InterfaceListv6 m_ipv6InterfaceList;
};

}

#endif
