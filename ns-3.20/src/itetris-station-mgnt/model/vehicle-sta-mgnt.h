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

/*
 * Edited by Jérôme Härri <haerri@eurecom.fr>
 * EURECOM 2013
 *
 * Implementation of a ETSI DCC module for STF447
 *
 */


#ifndef VEHICLE_STA_MGNT_H
#define VEHICLE_STA_MGNT_H

#include "ns3/simulator.h"
#include "vehicle-scan-mngr.h"
#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/road-side-unit.h"
#include "ns3/ip-base-station.h"
#include "ns3/node.h"
#include "ns3/itetris-types.h"
#include "ns3/channel-load-monitor-mngr.h"
#include <map>


namespace ns3
{

class c2cAddress;
class Ipv4Address;

/**
 * @class VehicleStaMgnt
 * @brief The class VehicleStaMgnt is attached to every iTETRIS vehicle and mantains a list of the IP base stations (different technologies) the vehicle is connected to. Also, it provides a list of the RSUs located in the vehicle's neighborhood. The functions implemented by the class VehicleStaMgnt are called from the LocalCommChannelSelector and from the facilities AddressingSupport.
 */


typedef struct {
    Ptr<NetDevice> device;
    Ptr<VehicleScanMngr> scanMngr;
} IpStationTuple;

typedef struct {
    Ptr<NetDevice> device;
    Ptr<ChannelLoadMonitorMngr> channelLoadMngr;
} c2cStationTuple; // C2C station tuble, including a netdevice and a DCC channel load monitoring

typedef std::map<std::string, Ptr<NetDevice> > C2cNetDeviceList;
typedef std::map<std::string, IpStationTuple > IpNetDeviceList;
    typedef std::map <std::string, Ptr<const IpBaseStation>  > IpBaseStationList;
    typedef std::vector <Ptr<RoadSideUnit> > RoadSideUnitList;

class VehicleStaMgnt : public Object
{
  public:
    static TypeId GetTypeId (void);
    VehicleStaMgnt ();
    ~VehicleStaMgnt ();
    bool AddIpTechnology (std::string technology, Ptr<NetDevice>, Ptr<VehicleScanMngr>);
    bool AddC2cTechnology (std::string technology, Ptr<NetDevice>);

    /**
     * @brief Function called from the facility Addressing Support to retrieve the c2cAddress (nodeId+position) of a given node. The function looks up the nodeId in the vehicle's Neighbors table
     */
    Ptr<c2cAddress> GetC2cAddress (uint32_t nodeId) const;

    /**
     * @brief Function called from the faciliy Addressing Support to retrieve the Ipv4Address of a IpBaseStation. The function looks up the nodeId in the private memeber m_ipBaseStationList
     */
    Ipv4Address* GetIpAddress (uint32_t nodeId) const;

    /**
     * @brief Function called from the faciliy Addressing Support to retrieve the Ipv4Address of a IpBaseStation. The function looks up the nodeId in the private memeber m_ipBaseStationList
     */
    Ipv6Address* GetIpv6Address (uint32_t nodeId) const;

    /**
     * @brief Function called from the Local Communication Channel Selector to retrieve the set of IP based station the vehicles is registered with
     */
    IpBaseStationList*  GetRegisteredIpBaseStations (void) const;

    /**
     * @brief Function called from the Local Communication Channel Selector to retrieve the set of IP based station the vehicles is registered with
     */
    IpBaseStationList*  GetRegisteredIpBaseStations (STACK stack) const;

    /**
     * @brief Function called from the Local Communication Channel Selector to retrieve the set of RSUs located in the vehicle's neighborhood (e.g. from which the vehicle has received a CAM)
     */
    RoadSideUnitList* GetRsusInCoverage (void) const;


    C2cNetDeviceList GetC2CDeviceList ();
    IpNetDeviceList GetIPDeviceList ();
    Ptr<NetDevice> GetIpNetDevice (std::string technology);
  

    void SetNode (Ptr<Node> node);

    void ActivateNode (void);
    void DeactivateNode (void);
    bool IsNodeActive (void);

  private:
    void ScanIpBaseStations (void) const;
    void ScanIpBaseStations (STACK stack) const;
    void ScanRoadSideUnits (void) const;
    IpNetDeviceList m_ipNetDeviceList;
    C2cNetDeviceList m_c2cNetDeviceList;
    IpBaseStationList* m_ipBaseStationList;
    RoadSideUnitList* m_roadSideUnitList;
    Ptr<Node> m_node;
    bool m_nodeActive;
};

}

#endif
