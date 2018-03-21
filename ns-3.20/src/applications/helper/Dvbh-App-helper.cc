/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
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
 * Author: Sendoa Vaz
 */

#include "Dvbh-App-helper.h"
#include "ns3/names.h"
#include "ns3/string.h"
#include "ns3/Dvbh-app.h"
#include "ns3/log.h"

#include "ns3/callback.h" 
#include "ns3/inci-packet-list.h"
#include "ns3/application-helper.h"
#include "ns3/iTETRIS-Application.h"

NS_LOG_COMPONENT_DEFINE ("DvbhAppHelper");

namespace ns3 {

DvbhAppHelper::DvbhAppHelper ()
{
  m_factory.SetTypeId("ns3::DvbhApp");
}

// added by Andrea
DvbhAppHelper::~DvbhAppHelper()
{
}

void
DvbhAppHelper::SetApplicationNodeType(std::string dvbhNodeType)
{
  m_nodeType=dvbhNodeType;
}

void 
DvbhAppHelper::AddStreamToService(uint32_t streamId,std::string name)
{  
 streamList.push_back(std::make_pair(name,streamId)); 
}

void
DvbhAppHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
DvbhAppHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DvbhAppHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DvbhAppHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DvbhAppHelper::InstallPriv (Ptr<Node> node) const
{
  
  NS_LOG_INFO("[ns3][DVBH-App Helper] install DVBH-APP application on node " << node->GetId() <<" --------\n");
  NS_LOG_INFO(m_factory.GetTypeId()<<" \n ");
  
  Ptr<iTETRISApplication> app = m_factory.Create<iTETRISApplication> ();
  uint32_t app_index =node->AddApplication (app);
  app->SetServiceIndex(app_index);
  app->SetNode(node);
  DynamicCast<DvbhApp>(app)->AddStreams(streamList);
  app->SetSockets();
  app->SetServiceType (m_apptype);    
  Ptr<InciPacketList> packetList = node->GetObject <InciPacketList> ();
  // set the callback for the application to communicate to the inci when a packet is received at facilities level
  app->SetReceiveCallback (MakeCallback (&InciPacketList::ReceiveFromApplication, packetList));
  
  
  return app;
}

} // namespace ns3
