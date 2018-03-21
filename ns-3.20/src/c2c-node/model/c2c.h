/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#ifndef C2C_H
#define C2C_H

#include <stdint.h>
#include "ns3/object.h"
#include "ns3/callback.h"

#include "ns3/socketc2c.h"
#include "ns3/itetris-technologies.h"

namespace ns3 {

class Node;
class NetDevice;
class Packet;
class c2cRoutingProtocol;

/**
 * \ingroup node
 * \defgroup c2c c2c
 */
/**
 * \ingroup c2c
 * \brief  iTETRIS [WP600] - Access to the c2c interfaces and configuration
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class c2c : public Object
{
public:
  static TypeId GetTypeId (void);
  
  /**
   * \brief Constructor.
   */
  c2c ();

  /**
   * \brief Destructor.
   */
virtual ~c2c ();

  /**
   * \brief Register a new routing protocol to be used by this c2c stack
   *
   * This call will replace any routing protocol that has been previously 
   * registered.  If you want to add multiple routing protocols, you must
   * add them to a c2cListRoutingProtocol directly.
   * 
   * \param routingProtocol smart pointer to c2cRoutingHelper object
   */

  virtual void SetRoutingProtocol (Ptr<c2cRoutingProtocol> routingProtocol) = 0;
  /**
   * \brief Get the routing protocol to be used by this Ipv4 stack
   * 
   * \returns smart pointer to c2cRoutingHelper object, or null pointer if none
   */
  virtual Ptr<c2cRoutingProtocol> GetRoutingProtocol (void) const = 0;

  /**
   * \brief Add a NetDevice interface.
   *
   * Once a device has been added, it can never be removed: if you want
   * to disable it, you can invoke c2c::SetDown (To Do) which will
   * make sure that it is never used during packet forwarding.
   * \param deviceType Type of iTETRIS NetDevice (ITS_CCH / ITS_SCH / UMTS / WIMAX / DVB ) to add to the list of c2c interfaces
   *        which can be used as output interfaces during packet forwarding.
   * \param device device to add to the list of c2c interfaces
   *        which can be used as output interfaces during packet forwarding.
   * \returns the index of the c2c interface added.
   */
  virtual uint32_t AddInterface (NetDeviceType deviceType, Ptr<NetDevice> device) = 0;

  virtual uint32_t AddInterface (Ptr<NetDevice> device) = 0;

  virtual int32_t GetInterfaceForDevice (Ptr<const NetDevice> device) const = 0;

  /**
   * \brief Get number of interfaces.
   * \returns the number of interfaces added by the user.
   */
  virtual uint32_t GetNInterfaces (void) const = 0;
};

} // namespace ns3 

#endif /* C2C_H */

