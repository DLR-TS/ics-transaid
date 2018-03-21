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

/*
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 *
 * IPv6 integration
 *
 */

#ifndef RSU_STA_MGNT_H
#define RSU_STA_MGNT_H

#include "ns3/simulator.h"
#include "ns3/object.h"

#include "ns3/simulator.h"
#include "vehicle-scan-mngr.h"
#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/road-side-unit.h"
#include "ns3/node.h"
#include "ns3/itetris-types.h"
#include "ns3/channel-load-monitor-mngr.h"
#include <map>

namespace ns3
{

class c2cAddress;
class Ipv6Address;
class Node;

typedef struct {
    Ptr<NetDevice> device;
    Ptr<ChannelLoadMonitorMngr> channelLoadMngr;
} rsuStationTuple;

typedef std::map<std::string, Ptr<const rsuStationTuple>  > rsuStationList;
typedef std::map<std::string, Ptr<NetDevice> > rsuNetDeviceList; // RSUs may have multiple channels



class RsuStaMgnt : public Object
{
  public:
    static TypeId GetTypeId (void);
    virtual ~RsuStaMgnt();

    /**
     * Function called from the facility Addressing Support to retrieve
     * the c2cAddress (nodeId+position) of a given node. The function looks up 
     * the nodeId in the rsu's Neighbors table.
     */
    Ptr<c2cAddress> GetC2cAddress (uint32_t nodeId) const;
    Ipv6Address* GetIpv6Address (uint32_t nodeId) const;

    bool AddC2cTechnology (std::string technology, Ptr<NetDevice>); // RSUs may have multiple channels


    uint32_t GetNumberOfNodesInCoverage (void) const;
    double 	 GetCoverageRange (void) const;
    bool CompareIpv6Prefix (Ipv6Address a, Ipv6Address b, uint32_t pref_length) const;

    void SetNode (Ptr<Node> node);

    uint32_t GetNodeId ();
  private:
    Ptr<Node> m_node;
    rsuNetDeviceList m_rsuNetDeviceList;

};

}

#endif
