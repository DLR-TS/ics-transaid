/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT EU FP7 iTETRIS project
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

#ifndef DVBH_VEHICLE_SCAN_MNGR_H
#define DVBH_VEHICLE_SCAN_MNGR_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/dvbh-net-device.h"

namespace ns3
{
/**
 * The base class WifiScanMngr implements the BaseStationScanMngr for the technology UMTS
 */
class DvbhVehicleScanMngr : public VehicleScanMngr
{
  public:
    static TypeId GetTypeId (void);
    virtual ~DvbhVehicleScanMngr();
    Ptr<IpBaseStation> GetBestServingBs (void);
    Ptr<IpBaseStation> GetBestServingBs (STACK stack);
    void AddBaseStation(Ptr<DvbhNetDevice> netDevice);
    void SetNode (Ptr<Node> node);
    Ptr<Node> m_node;
};

}

#endif
