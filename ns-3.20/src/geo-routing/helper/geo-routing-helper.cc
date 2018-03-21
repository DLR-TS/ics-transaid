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

#include "ns3/c2c-routing-protocol.h"
#include "geo-routing-helper.h"

NS_LOG_COMPONENT_DEFINE("GeoRoutingHelper");

namespace ns3 {

GeoRoutingHelper::GeoRoutingHelper()
{}

GeoRoutingHelper::GeoRoutingHelper (const GeoRoutingHelper &o)
{
}

GeoRoutingHelper* 
GeoRoutingHelper::Copy (void) const 
{
  return new GeoRoutingHelper (*this);
}

Ptr<c2cRoutingProtocol> 
GeoRoutingHelper::Create (Ptr<Node> node) const
{
  return CreateObject<GeoRoutingProtocol> ();
}


Ptr<GeoRoutingProtocol>
GeoRoutingHelper::GetGeoRouting (Ptr<c2c> c2c) const
{
  NS_LOG_FUNCTION (this);
  Ptr<c2cRoutingProtocol> c2crp = c2c->GetRoutingProtocol ();
  NS_ASSERT_MSG (c2crp, "No routing protocol associated with c2c");
  if (DynamicCast<GeoRoutingProtocol> (c2crp))
    {
      NS_LOG_LOGIC ("Geo routing found as the main c2c routing protocol.");
      return DynamicCast<GeoRoutingProtocol> (c2crp);
    }
  NS_LOG_LOGIC ("Geo routing not found");
  return 0;
}

}; // namespace ns3
