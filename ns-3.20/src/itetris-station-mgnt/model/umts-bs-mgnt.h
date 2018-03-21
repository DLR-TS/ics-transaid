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
 * Author: Sendoa Vaz <svaz@cbt.es> based on the code of Ramon Bauza <rbauza@umh.es>
 */

#ifndef UMTS_BS_MGNT_H
#define UMTS_BS_MGNT_H

#include "ns3/simulator.h"
#include "ns3/umts-net-device.h"
#include "ns3/object.h"
#include "ns3/ip-base-sta-mgnt.h"

namespace ns3
{
/**
 * The base class UmtsBsMgnt implements the IpBaseStaMgnfor the technology UMTS
 */
class UmtsBsMgnt : public IpBaseStaMgnt
{
  public:
    static TypeId GetTypeId (void);
    virtual ~UmtsBsMgnt();
    Ipv4Address* GetIpAddress (uint32_t nodeId) const;
    //NEW
    Ipv6Address* GetIpv6Address (uint32_t nodeId) const;
    uint32_t GetNumberOfActiveConnections (void) const;
    uint32_t GetNumberOfRegisteredUsers (void) const;
    double 	 GetCoverageRange (void) const;

    void AddVehicle(Ptr<UMTSNetDevice> netDevice);
    void TriggerVehiclesScanning (void) const;
  protected:
    
    typedef std::list<UMTSNetDevice > VehicleList;
    VehicleList vehicleList;


};

}

#endif
