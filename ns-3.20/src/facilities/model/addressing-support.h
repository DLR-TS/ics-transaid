/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
 */

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/


#ifndef ADDRESSING_SUPPORT_H
#define ADDRESSING_SUPPORT_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/itetris-types.h"
#include "ns3/ipv4-address.h"


namespace ns3 {

class c2cAddress;
class AddressingSupport : public Object
{

public:

  static TypeId GetTypeId (void);
  AddressingSupport ();
  ~AddressingSupport ();
  void DoDispose (void);

  Ipv4Address* getIPaddress(uint32_t destination);
  Ipv6Address* getIPv6address(uint32_t destination);
  Ptr<c2cAddress> getC2Caddress(uint32_t destination);
  Ptr<c2cAddress> getC2CGeoBroadcastAddress(CircularGeoAddress destination);
  Ptr<c2cAddress> getC2CGeoAnycastAddress(CircularGeoAddress destination);
  Ptr<c2cAddress> getC2CTopoBroadcastAddress(uint32_t NHops);
  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  Ptr<Node> m_node;

};

}; //namespace ns3

#endif /* ADDRESSING_SUPPORT_H */
