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

#ifndef IP_BASE_STATION_H
#define IP_BASE_STATION_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

namespace ns3
{

class IpBaseStation : public Object
{

  public:
    static TypeId GetTypeId (void);
    IpBaseStation();
    IpBaseStation(uint32_t nodeId, Ipv4Address ipAddress, uint32_t lat, uint32_t lon);
    //NEW FUNCTION
    IpBaseStation(uint32_t nodeId, Ipv6Address ipAddress, uint32_t lat, uint32_t lon);
    //NEW FUNCTION
    IpBaseStation(uint32_t nodeId, Ipv4Address ipAddress, Ipv6Address ipAddress2, uint32_t lat, uint32_t lon);
    virtual ~IpBaseStation();
    uint32_t GetNodeId (void) const;
    Ipv4Address GetIpAddress (void) const;
    //NEW FUNCTION
    Ipv6Address GetIpv6Address (void) const;
    uint32_t GetLat (void) const;
    uint32_t GetLon (void) const;
    void SetNodeId (uint32_t nodeId);
    void SetIpAddress (Ipv4Address ipAddress);
    void SetIpv6Address (Ipv6Address ipAddress);
    void SetLat (uint32_t lat);
    void SetLon (uint32_t lon);


  private:
    uint32_t m_nodeId;
    Ipv4Address m_ipAddress;
    Ipv6Address m_ipv6Address;
    uint32_t m_lat;
    uint32_t m_lon;

};

}

#endif
