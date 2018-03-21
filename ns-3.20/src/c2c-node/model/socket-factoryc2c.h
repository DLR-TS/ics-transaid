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

#ifndef SOCKET_FACTORY_C2C_H
#define SOCKET_FACTORY_C2C_H

#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3 {

class Socketc2c;

/**
 * \ingroup socketc2c
 *
 * \brief  iTETRIS [WP600: HITA] - Object to create transport layer instances that provide a
 * socket API to applications.
 *
 * This base class defines the API for creating sockets.  
 * The socket factory also can hold the global variables used to
 * initialize newly created sockets, such as values that are
 * set through the sysctl or proc interfaces in Linux.

 * If you want to write a new transport protocol accessible through
 * sockets, you need to subclass this factory class, implement CreateSocket, 
 * instantiate the object, and aggregate it to the node.  
 * 
 * \see c2cTransport
 * \see c2cl4TSocketImpl
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class SocketFactoryc2c : public Object
{
public:
  static TypeId GetTypeId (void);

  SocketFactoryc2c ();

  /**
   * \return smart pointer to Socketc2c
   * 
   * Base class method for creating socket instances.
   */
  virtual Ptr<Socketc2c> CreateSocket (void) = 0;
};

} // namespace ns3


#endif /* SOCKET_FACTORY_C2C_H */
