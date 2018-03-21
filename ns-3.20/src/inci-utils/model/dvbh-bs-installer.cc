/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT EU FP7 iTETRIS project
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

#include "dvbh-bs-installer.h"
#include "ns3/boolean.h"
#include "ns3/dvbh-bs-mgnt.h"
#include "ns3/ip-base-sta-mgnt.h"
#include "ns3/IPCIU-facilities-helper.h"
#include "ns3/IPCIUFacilities.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("DvbhBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (DvbhBsInstaller);

TypeId DvbhBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DvbhBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<DvbhBsInstaller>()
    ;
  return tid;
}

void 
DvbhBsInstaller::DoInstall (NodeContainer container, NetDeviceContainer devices)
{
  NS_LOG_INFO ("*** DvbhBsInstaller ***");
  
  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the base station has the object UmtsBsMgnt already installed
      Ptr<IpBaseStaMgnt> dvbhBsMgnt = (*i)->GetObject <DvbhBsMgnt> ();
      if (dvbhBsMgnt  == NULL)
	{
          Ptr<NetDevice> device = devices.Get(index);
          dvbhBsMgnt = CreateObject <DvbhBsMgnt> ();
          dvbhBsMgnt->SetNode (*i);
          dvbhBsMgnt->SetNetDevice (device);
          (*i)->AggregateObject (dvbhBsMgnt);
          NS_LOG_INFO ("The object DvbhBsMgnt has been installed in the base station");
        }
        
        DynamicCast<DvbhNetDevice>(devices.Get(index))->SetIpAddress();
        
    Ptr<IPCIUFacilities> facilities = (*i)->GetObject <IPCIUFacilities> ();
    if (facilities == NULL)
      {
	IPCIUFacilitiesHelper facilitiesHelper;
	facilitiesHelper.SetServiceListHelper (m_servListHelper);
	facilitiesHelper.Install (*i);
	NS_LOG_INFO ("The object IPCIUFacilities has been installed in the vehicle");
      }
      
      index ++;
    }
    
    
    
}


} // namespace ns3
