/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Eurecom, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <hrizi@eurecom.fr>, Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#ifndef C2C_ROUTE_H
#define C2C_ROUTE_H

#include "ns3/ptr.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class NetDevice;

/**
 * \ingroup c2cRouting
 *
 *\brief iTETRIS [WP600] - c2c route cache entry (similar to Linux struct rtable)
 *
 * This is a reference counted object.  In the future, we will add other 
 * entries from struct dst_entry, struct rtable, and struct dst_ops as needed.
 */
class c2cRoute : public Object {
public:


  static TypeId GetTypeId (void);
  c2cRoute ();

  /**
   * \param dest Destination c2cAddress
   */
  void SetDestination (Ptr<c2cAddress> dest);
  /**
   * \return Destination c2cDestAddress of the route
   */
  Ptr<c2cAddress> GetDestination (void) const;

  /**
   * \param src Source c2cAddress
   */
  void SetSource (Ptr<c2cAddress> src);
  /**
   * \return Source c2cAddress of the route
   */
  Ptr<c2cAddress> GetSource (void) const;

  /**
   * \param gw Gateway (next hop) c2cAddress
   */
  void SetGateway (Ptr<c2cAddress> gw);
  /**
   * \return c2cAddress of the gateway (next hop) 
   */
  Ptr<c2cAddress> GetGateway (void) const;

  /**
   * Equivalent in Linux to dst_entry.dev
   *
   * \param outputDevice pointer to NetDevice for outgoing packets
   */
  void SetOutputDevice (Ptr<NetDevice> outputDevice);
  /**
   * \return pointer to NetDevice for outgoing packets
   */
  Ptr<NetDevice> GetOutputDevice (void) const;

private:
  Ptr<c2cAddress> m_dest;
  Ptr<c2cAddress> m_source;
  Ptr<c2cAddress> m_gateway;
  Ptr<NetDevice> m_outputDevice;
};

}//namespace ns3

#endif /* C2C_ROUTE_H */
