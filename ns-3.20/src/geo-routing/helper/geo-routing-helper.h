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

#ifndef GEO_ROUTING_HELPER_H
#define GEO_ROUTING_HELPER_H

#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/c2c.h"
#include "ns3/geo-routing.h"
#include "ns3/geo-broadcast.h"
#include "ns3/geo-unicast.h"
#include "ns3/geo-anycast.h"
#include "ns3/topo-broadcast.h"

#include "ns3/c2c-routing-helper.h"

namespace ns3 {

/**
 * \brief Helper class that adds ns3::GeoRouting objects
 *
 * This class is expected to be used in conjunction with 
 * ns3::InternetStackHelper::SetRoutingHelper
 */
class GeoRoutingHelper : public c2cRoutingHelper
{
public:
  /*
   * Construct a geoRoutingHelper object, used to make configuration
   * of geo routing easier.
   */
  GeoRoutingHelper ();

  /**
   * \brief Construct an GeoRoutingHelper from another previously 
   * initialized instance (Copy Constructor).
   */
  GeoRoutingHelper (const GeoRoutingHelper &);

  /**
   * \internal
   * \returns pointer to clone of this geoRoutingHelper
   *
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  GeoRoutingHelper* Copy (void) const;

  /**
   * \param node the node on which the routing protocol will run
   * \returns a newly-created routing protocol
   *
   * This method will be called by ns3::InternetStackHelper::Install
   */
  virtual Ptr<c2cRoutingProtocol> Create (Ptr<Node> node) const;

  /**
   * Try and find the geo routing protocol as either the main routing
   * protocol or in the list of routing protocols associated with the 
   * c2c provided.
   *
   * \param c2c the Ptr<c2c> to search for the geo routing protocol
   */
  Ptr<GeoRoutingProtocol> GetGeoRouting (Ptr<c2c> c2c) const;


  void Set (geoUnicast uprotocol, geoBroadcast bprotocol, geoAnycast aprotocol, topoBroadcast tprotocol);

private:
  /**
   * \internal
   * \brief Assignment operator declared private and not implemented to disallow
   * assignment and prevent the compiler from happily inserting its own.
   */
  GeoRoutingHelper &operator = (const GeoRoutingHelper &o);
};

} // namespace ns3

#endif /* GEO_ROUTING_HELPER_H */
