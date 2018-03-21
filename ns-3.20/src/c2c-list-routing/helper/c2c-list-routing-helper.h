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

#ifndef C2C_LIST_ROUTING_HELPER_H
#define C2C_LIST_ROUTING_HELPER_H

#include <stdint.h>
#include <list>

#include "ns3/c2c-routing-helper.h"

namespace ns3 {

/**
 * \brief Helper class that adds ns3::c2cListRouting objects
 *
 * This class is expected to be used in conjunction with 
 * ns3::InternetStackHelper::SetRoutingHelper
 */
class c2cListRoutingHelper : public c2cRoutingHelper
{
public:
  /*
   * Construct an c2cListRoutingHelper used to make installing routing
   * protocols easier.
   */
  c2cListRoutingHelper ();

  /*
   * \internal
   * Destroy an c2cListRoutingHelper.
   */
  virtual ~c2cListRoutingHelper ();

  /**
   * \brief Construct an c2cListRoutingHelper from another previously 
   * initialized instance (Copy Constructor).
   */
  c2cListRoutingHelper (const c2cListRoutingHelper &);

  /**
   * \internal
   * \returns pointer to clone of this c2cListRoutingHelper 
   * 
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  c2cListRoutingHelper* Copy (void) const;

  /**
   * \param routing a routing helper
   * \param priority the priority of the associated helper
   *
   * Store in the internal list a reference to the input routing helper
   * and associated priority. These helpers will be used later by
   * the ns3::c2cListRoutingHelper::Create method to create
   * an ns3::c2cListRouting object and add in it routing protocols
   * created with the helpers.
   */

  void Add (const c2cRoutingHelper &routing, int16_t priority);
  /**
   * \param node the node on which the routing protocol will run
   * \returns a newly-created routing protocol
   *
   * This method will be called by ns3::InternetStackHelper::Install
   */

  virtual Ptr<c2cRoutingProtocol> Create (Ptr<Node> node) const;
private:
  /**
   * \internal
   * \brief Assignment operator declared private and not implemented to disallow
   * assignment and prevent the compiler from happily inserting its own.
   */
  c2cListRoutingHelper &operator = (const c2cListRoutingHelper &o);

  std::list<std::pair<const c2cRoutingHelper *,int16_t> > m_list;
};

} // namespace ns3

#endif /* C2C_LIST_ROUTING_HELPER_H */
