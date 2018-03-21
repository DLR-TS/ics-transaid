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
#include "ns3/CAMmanage-helper.h"
#include "ns3/DENMhelper.h"
#include "ns3/service-list-helper.h"
#include "ns3/service-list.h"
#include "ns3/service-management.h"
#include "ns3/local-comm-ch-selector.h"
#include "ns3/inci-packet-list.h"
#include "ns3/local-comm-ch-selector.h"
#include "ns3/addressing-support.h"
#include "ns3/iTETRISns3Facilities.h"

#include "c2c-facilities-helper.h"

NS_LOG_COMPONENT_DEFINE ("C2CFacilitiesHelper");

namespace ns3 {


C2CFacilitiesHelper::C2CFacilitiesHelper ()
{
}

C2CFacilitiesHelper::~C2CFacilitiesHelper ()
{
}

void 
C2CFacilitiesHelper::SetServiceListHelper (ServiceListHelper* servicelisthelper)
{
  m_ServiceListHelper = servicelisthelper;
}

void 
C2CFacilitiesHelper::AddDefaultServices (ServiceListHelper* servicelisthelper)
{
  SetServiceListHelper (servicelisthelper);

  // CAM and DENM applications are always installed by the default

  CAMmanageHelper* cam = new CAMmanageHelper("ns3::c2cl4TSocketFactory");
  uint16_t PortCAM = 8080;  // APP binding port for c2c
  cam->SetAttribute("Port",UintegerValue (PortCAM)); // Port to bind
  cam->SetAttribute("Frequency",DoubleValue (2)); // In Hz
  cam->SetAttribute("PacketSize",UintegerValue (300)); // In bytes
  m_ServiceListHelper->Add (cam, "CAM");

  DENMHelper* denm = new DENMHelper("ns3::c2cl4TSocketFactory");
  uint16_t PortDENM = 8090;  // APP binding port for c2c
  denm->SetAttribute("Port",UintegerValue (PortDENM)); // Port to bind
  denm->SetAttribute("MsgLifeTime",UintegerValue (60)); // In seconds 
  denm->SetAttribute("PacketSize",UintegerValue (400)); // In bytes
  m_ServiceListHelper->Add (denm, "DENM");
}

void
C2CFacilitiesHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> obj = factory.Create <Object> ();
  node->AggregateObject (obj);
}

void 
C2CFacilitiesHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
C2CFacilitiesHelper::Install (Ptr<Node> node)
{  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][c2cFacilitiesHelper] creating and installing facilities on node " << node->GetId() <<"--------\n");
  NS_LOG_INFO("\n");

  if ((node->GetObject<iTETRISns3Facilities> () != 0) || (node->GetObject<MessageManagement> () != 0) || (node->GetObject<ServiceManagement> () != 0) || (node->GetObject<AddressingSupport> () != 0) || (node->GetObject<LocalCOMMchSelector> () != 0) || (node->GetObject<InciPacketList> () != 0))
    {
      NS_FATAL_ERROR ("creation attempt of a Facilities object to a node with an existing Facilities layer");
      return;
    }
  
  // install the Facilities
  CreateAndAggregateObjectFromTypeId (node, "ns3::iTETRISns3Facilities");
  CreateAndAggregateObjectFromTypeId (node, "ns3::AddressingSupport");
  CreateAndAggregateObjectFromTypeId (node, "ns3::MessageManagement");
  CreateAndAggregateObjectFromTypeId (node, "ns3::ServiceManagement");
  CreateAndAggregateObjectFromTypeId (node, "ns3::LocalCOMMchSelector");
  CreateAndAggregateObjectFromTypeId (node, "ns3::InciPacketList");

  Ptr<iTETRISns3Facilities> facilities = node->GetObject<iTETRISns3Facilities> ();
  Ptr<ServiceManagement> ServMng = node->GetObject<ServiceManagement> ();
  Ptr<AddressingSupport> AddrSupp= node->GetObject<AddressingSupport> ();
  Ptr<MessageManagement> messMgmt = node->GetObject<MessageManagement> ();
  Ptr<LocalCOMMchSelector> localCHsel = node->GetObject<LocalCOMMchSelector> ();

  // install the list of services contained in the service list helper

  ServiceList* services = m_ServiceListHelper->Install (node);

  ServMng->SetServiceList(services);

  facilities->SetServiceManagement(ServMng);
  facilities->SetAddressingSupport(AddrSupp);
  facilities->SetLocalCommCHSelector(localCHsel);
  ServMng->SetMessageManagement(messMgmt);
 ////std::cout<<"C2CFacilitiesHelper::Install finished..."<<std::endl;
}


void
C2CFacilitiesHelper::AddServices (Ptr<iTETRISns3Facilities> facilities, Ptr<Node> node)
{
  Ptr<ServiceManagement> ServMng = node->GetObject<ServiceManagement> ();
  ServiceList* services = ServMng->GetServiceList ();
  m_ServiceListHelper->AddServices (node, services); 
}

void
C2CFacilitiesHelper::Install (std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

} // namespace ns3
