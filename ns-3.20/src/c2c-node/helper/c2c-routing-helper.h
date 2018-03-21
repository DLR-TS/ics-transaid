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

#ifndef C2C_ROUTING_HELPER_H
#define C2C_ROUTING_HELPER_H

#include "ns3/ptr.h"

namespace ns3 {

class c2cRoutingProtocol;
class Node;

/**
 * \brief a factory to create ns3::c2cRoutingProtocol objects
 *
 * For each new routing protocol created as a subclass of 
 * ns3::c2cRoutingProtocol, you need to create a subclass of 
 * ns3::c2cRoutingHelper which can be used by 
 * ns3::InternetStackHelper::SetRoutingHelper and 
 * ns3::InternetStackHelper::Install.
 */
class c2cRoutingHelper
{
public:
  /*
   * Destroy an instance of an c2cRoutingHelper
   */
  virtual ~c2cRoutingHelper ();

  /**
   * \brief virtual constructor
   * \returns pointer to clone of this c2cRoutingHelper 
   * 
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  virtual c2cRoutingHelper* Copy (void) const = 0;

  /**
   * \param node the node within which the new routing protocol will run
   * \returns a newly-created c2c routing protocol
   */
  virtual Ptr<c2cRoutingProtocol> Create (Ptr<Node> node) const = 0;
};

} // namespace ns3


#endif /* C2C_ROUTING_HELPER_H */
