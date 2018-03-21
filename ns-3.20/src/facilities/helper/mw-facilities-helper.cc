/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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

#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/node.h"
#include "ns3/mw-comm-ch-selector.h"
#include "ns3/MWFacilities.h"

#include "mw-facilities-helper.h"

NS_LOG_COMPONENT_DEFINE ("MWFacilitiesHelper");

namespace ns3 {

MWFacilitiesHelper::MWFacilitiesHelper ()
{
}

MWFacilitiesHelper::~MWFacilitiesHelper ()
{
}

void
MWFacilitiesHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> obj = factory.Create <Object> ();
  node->AggregateObject (obj);
}

void 
MWFacilitiesHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
MWFacilitiesHelper::Install (Ptr<Node> node)
{  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][MWFacilitiesHelper] creating and installing MW facilities and MW comm CH selector on MW node " << node->GetId() <<"--------\n");
  NS_LOG_INFO("\n");

  if ((node->GetObject<MWFacilities> () != 0) || (node->GetObject<MWCOMMchSelector> () != 0))
    {
      NS_FATAL_ERROR ("creation attempt of a Facilities object to a node with an existing Facilities layer");
      return;
    }

  CreateAndAggregateObjectFromTypeId (node, "ns3::MWFacilities");
  CreateAndAggregateObjectFromTypeId (node, "ns3::MWCOMMchSelector");

  Ptr<MWFacilities> facilities = node->GetObject<MWFacilities> ();
  Ptr<MWCOMMchSelector> MWCHsel = node->GetObject<MWCOMMchSelector> ();

  facilities->SetMWCommCHSelector(MWCHsel);
}

void
MWFacilitiesHelper::Install (std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

} // namespace ns3
