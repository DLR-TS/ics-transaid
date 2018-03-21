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

#ifndef VEHICLE_SCAN_MNGR_H
#define VEHICLE_SCAN_MNGR_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/ip-base-station.h"
#include "ns3/itetris-types.h"
#include "ns3/node.h"

namespace ns3
{
/**
 * @class VehicleScanMngr
 * @brief The base class VehicleScanMngr can be derived to implement a technology-specific StationScanManager which allows retrieving the best serving base station for a given IP-based technology
 */

class VehicleScanMngr : public Object
{
  public:
    static TypeId GetTypeId (void);
    virtual ~VehicleScanMngr();
    void SetNetDevice (Ptr<NetDevice> device);
    virtual Ptr<IpBaseStation> GetBestServingBs (void) = 0;
    virtual Ptr<IpBaseStation> GetBestServingBs (STACK stack) = 0;
    virtual void ActivateNode (void) {};
    virtual void DeactivateNode (void) {};
    void SetNode (Ptr<Node> node);
    
  protected:
    Ptr<NetDevice> m_netDevice; 
    Ptr<Node> m_node;

};

}

#endif
