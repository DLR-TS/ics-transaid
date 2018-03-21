/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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

#ifndef WIMAX_VEHICLE_SCAN_MNGR_H
#define WIMAX_VEHICLE_SCAN_MNGR_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/umts-net-device.h"

namespace ns3
{

class SsCommandManager;

/**
 * The base class WimaxScanMngr implements the BaseStationScanMngr for the technology Wimax
 */
class WimaxVehicleScanMngr : public VehicleScanMngr
{
  public:
    static TypeId GetTypeId (void);
    virtual ~WimaxVehicleScanMngr();
    Ptr<IpBaseStation> GetBestServingBs (void);
    Ptr<IpBaseStation> GetBestServingBs (STACK stack);
    void SetSsCommandManager (void);
    void ActivateNode (void);
    void DeactivateNode (void);

  private:
    Ptr<SsCommandManager> m_ssCommandManager;
    
};

}

#endif
