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

#ifndef IP_BASE_STA_MGNT_H
#define IP_BASE_STA_MGNT_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/node.h"
#include <map>
#include "ns3/itetris-types.h"


namespace ns3
{
/**
 * @class IpBaseStaMgnt
 * @brief The class IpBaseStaMgnt is attached to every iTETRIS IP Base Station and allows retrieving the IP address of a vehicle registered with the Base Station.
 */
class IpBaseStaMgnt : public Object
{
  public:
    static TypeId GetTypeId (void);
    IpBaseStaMgnt ();
    virtual ~IpBaseStaMgnt ();
    void SetNetDevice (Ptr<NetDevice> netDevice);
    Ptr<NetDevice> GetNetDevice (void);
    void SetNode (Ptr<Node> node);
    Ipv4Address* GetIpBroadcastAddress (void) const;
    Ipv6Address* GetIpv6MulticastAddress (void) const;

     /**
      * Function called from the facility Addressing Support to retrieve
      * the Ipv4Address of a vehicle. 
      */
    virtual Ipv4Address* GetIpAddress (uint32_t nodeId) const = 0;
  
    virtual Ipv6Address* GetIpv6Address (uint32_t nodeId) const = 0;

    virtual uint32_t GetNumberOfActiveConnections (void) const = 0;
    virtual uint32_t GetNumberOfRegisteredUsers (void) const = 0;
    virtual double GetCoverageRange (void) const = 0;

    void ActivateNode (void);
    void DeactivateNode (void);
    bool IsNodeActive (void);

  protected:
    virtual void TriggerVehiclesScanning (void) const = 0;
    Ptr<NetDevice> m_netDevice;
    Ptr<Node> m_node;
    bool m_nodeActive;
};

}

#endif
