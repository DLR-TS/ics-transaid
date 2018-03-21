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

#ifndef C2C_L4T_SOCKET_FACTORY_IMPL_H
#define C2C_L4T_SOCKET_FACTORY_IMPL_H

#include "ns3/c2cl4T-socket-factory.h"
#include "ns3/ptr.h"

namespace ns3 {

class c2cTransport;

/**
 * \ingroup c2cStack
 * \defgroup c2cTransport c2cTransport
 *
 */

/**
 * \ingroup c2cTransport
 * \brief  iTETRIS [WP600] - Object to create c2c socket instances
 * \internal
 *
 * This class implements the API for creating c2c Transport sockets.
 * It is a socket factory (deriving from class SocketFactoryc2c).
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class c2cl4TSocketFactoryImpl : public c2cl4TSocketFactory
{
public:
  c2cl4TSocketFactoryImpl ();
  virtual ~c2cl4TSocketFactoryImpl ();

  void Setc2cTransport (Ptr<c2cTransport> c2cTransport);

  /**
   * \brief Implements a method to create a c2c-based socket and return
   * a base class smart pointer to the socket.
   * \internal
   *
   * \return smart pointer to Socket
   */
  virtual Ptr<Socketc2c> CreateSocket (void);

protected:
  virtual void DoDispose (void);
private:
  Ptr<c2cTransport> m_c2cTransport;
};

} // namespace ns3

#endif /* C2C_SOCKET_FACTORY_IMPL_H */
