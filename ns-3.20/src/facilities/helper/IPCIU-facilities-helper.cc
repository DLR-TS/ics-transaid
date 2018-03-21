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
#include "ns3/double.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/node.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/service-list.h"
#include "ns3/service-management.h"
#include "ns3/inci-packet-list.h"
#include "ns3/local-comm-ch-selector.h"
#include "ns3/addressing-support.h"
#include "ns3/IPCIUFacilities.h"

#include "IPCIU-facilities-helper.h"

NS_LOG_COMPONENT_DEFINE ("IPCIUFacilitiesHelper");

namespace ns3 {

IPCIUFacilitiesHelper::IPCIUFacilitiesHelper ()
{
  Initialize ();
}

void
IPCIUFacilitiesHelper::Initialize ()
{
  ServiceListHelper* servlisth = NULL;
  SetServiceListHelper (servlisth); 
}

IPCIUFacilitiesHelper::~IPCIUFacilitiesHelper()
{
}


void 
IPCIUFacilitiesHelper::SetServiceListHelper (ServiceListHelper* servicelisthelper)
{
  m_ServiceListHelper = servicelisthelper;
}

void
IPCIUFacilitiesHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> obj = factory.Create <Object> ();
  node->AggregateObject (obj);
}

void 
IPCIUFacilitiesHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
IPCIUFacilitiesHelper::Install (Ptr<Node> node)
{  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][IPCIUFacilitiesHelper] creating and installing IPCIUfacilities on IP based CIU node " << node->GetId() <<"--------\n");
  NS_LOG_INFO("\n");

  if ((node->GetObject<IPCIUFacilities> () != 0) || (node->GetObject<MessageManagement> () != 0) || (node->GetObject<ServiceManagement> () != 0) || (node->GetObject<AddressingSupport> () != 0) || (node->GetObject<InciPacketList> () != 0))
    {
      NS_FATAL_ERROR ("creation attempt of a Facilities object to a node with an existing Facilities layer");
      return;
    }

  // install the facilities
  CreateAndAggregateObjectFromTypeId (node, "ns3::IPCIUFacilities");
  CreateAndAggregateObjectFromTypeId (node, "ns3::AddressingSupport");
  CreateAndAggregateObjectFromTypeId (node, "ns3::MessageManagement");
  CreateAndAggregateObjectFromTypeId (node, "ns3::ServiceManagement");
  CreateAndAggregateObjectFromTypeId (node, "ns3::InciPacketList");

  Ptr<IPCIUFacilities> facilities = node->GetObject<IPCIUFacilities> ();
  Ptr<ServiceManagement> ServMng = node->GetObject<ServiceManagement> ();
  Ptr<AddressingSupport> AddrSupp= node->GetObject<AddressingSupport> ();
  Ptr<MessageManagement> messMgmt = node->GetObject<MessageManagement> ();

  // install the list of services contained in the service list helper
  ServiceList* services = m_ServiceListHelper->Install (node);
  ServMng->SetServiceList(services);

  facilities->SetServiceManagement(ServMng);
  facilities->SetAddressingSupport(AddrSupp);
  ServMng->SetMessageManagement(messMgmt);
}

void
IPCIUFacilitiesHelper::Install (std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

} // namespace ns3
