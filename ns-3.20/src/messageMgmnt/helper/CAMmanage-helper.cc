/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Michele Rondinone <mrondinone@umh.es>, Ramon Bauza <rbauza@umh.es>
 */

#include "CAMmanage-helper.h"
#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/inci-packet-list.h"
#include "ns3/CAMmanagement.h"
#include "ns3/iTETRIS-Application.h"

NS_LOG_COMPONENT_DEFINE ("CAMmanageHelper");

namespace ns3 {


CAMmanageHelper::CAMmanageHelper (std::string protocol)
{
  m_factory.SetTypeId ("ns3::CAMmanage");
  m_factory.Set ("Protocol", StringValue (protocol));
}


CAMmanageHelper::CAMmanageHelper ()
{
}

CAMmanageHelper::~CAMmanageHelper()
{
}

void 
CAMmanageHelper::SetProtocol (std::string protocol)
{
  m_factory.SetTypeId ("ns3::CAMmanage");
  m_factory.Set ("Protocol", StringValue (protocol));
}

void
CAMmanageHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
CAMmanageHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
CAMmanageHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
CAMmanageHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
CAMmanageHelper::InstallPriv (Ptr<Node> node) const
{
  NS_LOG_INFO("[ns3][CAMmanageHelper]  install CAM application on node " << node->GetId() <<" --------\n");
  NS_LOG_INFO("\n");

  Ptr<iTETRISApplication> app = m_factory.Create<iTETRISApplication> ();   
  uint32_t app_index = node->AddApplication (app);
  app->SetServiceIndex(app_index);
  app->SetServiceType (m_apptype);
  Ptr<InciPacketList> packetList = node->GetObject <InciPacketList> ();
  app->SetReceiveCallback (MakeCallback (&InciPacketList::ReceiveFromApplication, packetList));

  return app;
}

} // namespace ns3
