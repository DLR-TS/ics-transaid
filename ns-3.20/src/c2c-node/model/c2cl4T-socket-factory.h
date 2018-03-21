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

#ifndef C2CTL4T_SOCKET_FACTORY_H
#define C2CTL4T_SOCKET_FACTORY_H

#include "socket-factoryc2c.h"

namespace ns3 {

class Socketc2c;

/**
 * \ingroup socket
 *
 * \brief  iTETRIS [WP600] - API to create c2cl4T socket instances
 *
 * This abstract class defines the API for c2cl4T socket factory.
 * All c2cl4T implementations must provide an implementation of CreateSocket
 * below.
 * 
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 * \see c2cl4TSocketFactoryImpl
 */
class c2cl4TSocketFactory : public SocketFactoryc2c
{
public:
  static TypeId GetTypeId (void);

};

} // namespace ns3

#endif /* C2CL4T_SOCKET_FACTORY_H */
