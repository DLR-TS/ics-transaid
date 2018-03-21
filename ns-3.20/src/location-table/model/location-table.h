/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

/*
 *
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 *
 * EURECOM 2015
 *
 * Added IPv6 support
 *
 */


#ifndef LOCATION_TABLE_H
#define LOCATION_TABLE_H

#include <vector>


#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/c2c-common-header.h"
#include "ns3/ipv6-address.h"

namespace ns3 {
class c2cCommonHeader;

class LocationTable : public Object
{

public:

  static const float LOCATION_ENTRY_LIFETIME;

  static TypeId GetTypeId (void);
  LocationTable ();
  ~LocationTable ();
  void DoDispose (void);

  struct LocTableEntry
  {
    uint64_t gnAddr;
    uint32_t Ts;
    uint32_t Lat;
    uint32_t Long;
    uint16_t Speed;
    uint16_t Heading;
    uint16_t Alt;
    uint8_t TimeAcc;
    uint8_t PosAcc;
    uint8_t SpeedAcc;
    uint8_t HeadingAcc;
    uint8_t AltAcc;
    std::vector<Ipv6Address> ipAddr;
    bool is_neigh;
  };

  typedef std::vector<struct LocTableEntry> Table;
  Table m_Table;

/**
* Add an entry to the location table when receiving a new beacon
*/
  void AddPosEntry (c2cCommonHeader commonheader);
  void AddPosEntry (c2cCommonHeader::LongPositionVector vector);

/**
* Add an entry to the location table with the own data of the node
*/
  void AddOwnEntry();

/**
* Clean the location table
*/
  void CleanTable();

  Table GetTable();
  int GetNbNeighs();
  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  EventId m_posEvent;
  EventId m_updateEvent;
  Ptr<Node> m_node;

  void ScheduleCleanTable();
  void ScheduleUpdatePos();
};

}; //namespace ns3

#endif /* LOCATION_TABLE_H */
