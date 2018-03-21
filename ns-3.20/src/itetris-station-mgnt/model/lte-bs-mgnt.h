/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
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
 */

#ifndef LTE_BS_MGNT_H
#define LTE_BS_MGNT_H

#include "ns3/simulator.h"
#include "ns3/lte-net-device.h"
#include "ns3/object.h"
#include "ns3/ip-base-sta-mgnt.h"
#include <map>

namespace ns3
{
/**
 * The base class LteBsMgnt implements the IpBaseStaMgnfor the technology LTE
 */
class LteBsMgnt : public IpBaseStaMgnt
{
  public:
    static TypeId GetTypeId (void);
    virtual ~LteBsMgnt();
    Ipv4Address* GetIpAddress (uint32_t nodeId) const;
    /**Jin: added ipv6, as pure virtual function need to be define here**/
    Ipv6Address* GetIpv6Address (uint32_t nodeId) const;

    uint32_t GetNumberOfActiveConnections (void) const;
    uint32_t GetNumberOfRegisteredUsers (void) const;
    double GetCoverageRange (void) const;

    void AddVehicle(Ptr<LteNetDevice> device);
    void TriggerVehiclesScanning (void) const;
    
    std::map<uint32_t, Ipv4Address> vehicleMap;

};

}

#endif

