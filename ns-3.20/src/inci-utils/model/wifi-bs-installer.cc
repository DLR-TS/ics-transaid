/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "wifi-bs-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"

// Specific objects for Wifi BSs
#include "ns3/wifi-bs-mgnt.h"

NS_LOG_COMPONENT_DEFINE ("WifiBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WifiBsInstaller);

TypeId WifiBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<WifiBsInstaller> ()             
    ;
  return tid;
}
    
void
WifiBsInstaller::Install (NodeContainer container) 
{
  NetDeviceContainer netDevices = wifi.Install (wifiPhy, wifiMac, container);
  m_ipAddressHelper.Assign (netDevices);
  AddInterfacesToIpInterfaceList (container);
  ConfigureDerivedWifiStation (container, netDevices);
}

void
WifiBsInstaller::ConfigureDerivedWifiStation (NodeContainer container, NetDeviceContainer devices)
{
  NS_LOG_INFO ("WifiBsInstaller");
  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the base station has the object WifiBsMgnt already installed
      Ptr<IpBaseStaMgnt> wifiBsMgnt = (*i)->GetObject <WifiBsMgnt> ();
      if (wifiBsMgnt == NULL)
	{
          Ptr<NetDevice> device = devices.Get(index);
          wifiBsMgnt = CreateObject <WifiBsMgnt> ();
          wifiBsMgnt->SetNode (*i);
          wifiBsMgnt->SetNetDevice (device);
          (*i)->AggregateObject (wifiBsMgnt);
          NS_LOG_INFO ("The object WifiBsMgnt has been installed in the base station");
        }
      index ++;
    }
}

} // namespace ns3
