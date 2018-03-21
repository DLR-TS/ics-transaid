/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */


#include "dvbh-helper.h"
#include "ns3/dvbh-link-layer-BaseStation.h"
#include "ns3/dvbh-link-layer-UserEquip.h"
#include "ns3/dvbh-phy-layer-BaseStation.h"
#include "ns3/dvbh-phy-layer-UserEquip.h"
#include "ns3/dvbh-ofdm-layer.h"
#include "ns3/dvbh-manager-BaseStation.h"
#include "ns3/dvbh-manager-UserEquip.h"
#include "ns3/dvbh-net-device.h"
#include "ns3/dvbh-channel.h"
#include "ns3/log.h"

#include "ns3/pcap-writer.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("DvbhHelper");

namespace ns3 {

DvbhPhyHelper::~DvbhPhyHelper()
{}

DvbhPhyHelper
DvbhPhyHelper::Default (void)
{
  DvbhPhyHelper helper;
 
  return helper;
}

Ptr<Object> 
DvbhPhyHelper::Create (std::string type) const
{
  if(type=="NodeUE")
  {      
      Ptr<DVBHPhyLayerUserEquip> phy = CreateObject<DVBHPhyLayerUserEquip> ();
      
      return phy;
  }
  else
  {
    Ptr<DVBHPhyLayerBaseStation> phy = CreateObject<DVBHPhyLayerBaseStation> ();

      return phy;
  }
       
}

void 
DvbhPhyHelper::Set (std::string name, const AttributeValue &v)
{  
  m_phy.Set (name, v);
}

void
DvbhPhyHelper::SetNodeType(std::string type)
{
  if(type=="NodeUE")
  {
    m_phy.SetTypeId ("ns3::PhyNodeUE"); 
  }
  else if(type=="NodeB")
  {
    m_phy.SetTypeId ("ns3::PhyNodeB"); 
  }
   
}

void 
DvbhPhyHelper::SetChannel (Ptr<DVBHChannel> channel)
{
  m_channel = channel;
}

DvbhOfdmLayerHelper::DvbhOfdmLayerHelper()
{
 m_ofdm.SetTypeId ("ns3::DVBHOfdmLayer"); 
}
DvbhOfdmLayerHelper::~DvbhOfdmLayerHelper ()
{}

DvbhOfdmLayerHelper
DvbhOfdmLayerHelper::Default (void)
{
  DvbhOfdmLayerHelper helper;
 
  return helper;
}

void 
DvbhOfdmLayerHelper::Set (std::string name, const AttributeValue &v)
{
  m_ofdm.Set (name, v);
}


Ptr<DVBHOfdmLayer> 
DvbhOfdmLayerHelper::Create () const
{
  Ptr<DVBHOfdmLayer> ofdm = m_ofdm.Create<DVBHOfdmLayer> ();

  return ofdm;
}


DvbhLinkLayerHelper::~DvbhLinkLayerHelper()
{}

DvbhLinkLayerHelper
DvbhLinkLayerHelper::Default (void)
{
  DvbhLinkLayerHelper helper;
 
  return helper;
}

Ptr<Object> 
DvbhLinkLayerHelper::Create (std::string type) const
{
  if(type=="NodeUE")
  {      
      Ptr<DVBHLinkLayerUserEquip> linkLayer = CreateObject<DVBHLinkLayerUserEquip> ();
      
      return linkLayer;
  }
  else
  {
    Ptr<DVBHLinkLayerBaseStation> linkLayer= CreateObject<DVBHLinkLayerBaseStation> ();

      return linkLayer;
  }

}

DvbhHelper::DvbhHelper()
{}

DvbhHelper
DvbhHelper::Default (void)
{
  DvbhHelper helper;
 
  return helper;
}

NetDeviceContainer 
DvbhHelper::Install (const DvbhOfdmLayerHelper &ofdmHelper,NodeContainer c,std::string nodeType/*,Ptr<DVBHChannel> broadcastChannel*/) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
   
      Ptr<Node> node = *i;
      Ptr<DvbhNetDevice> device = CreateObject<DvbhNetDevice> ();
      device->SetAddress(Mac48Address::Allocate ());
      device->SetNodeType(nodeType);
      
      Ptr<DVBHOfdmLayer> ofdm=ofdmHelper.Create();
      
      Ptr<DVBHChannel> broadcastChannel=Create<DVBHChannel>();
      
      ofdm->SetChannel(broadcastChannel);      
      DvbhPhyHelper phyHelper;
      Ptr<Object> phy = phyHelper.Create (nodeType);
	  
      DvbhLinkLayerHelper linklayerHelper;
      Ptr<Object> linkLayer=linklayerHelper.Create(nodeType);
	      
      device->SetOfdmLayer(ofdm); 
      
      device->SetPhyLayer(phy);
      device->SetLinkLayer(linkLayer);
             
      device->SetManager();
      
      node->AddDevice (device);
      devices.Add (device);
    }
  return devices;
}

} // namespace ns3
