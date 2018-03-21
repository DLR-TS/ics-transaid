/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "lte-bs-installer.h"

NS_LOG_COMPONENT_DEFINE ("LteBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (LteBsInstaller);

TypeId LteBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<LteBsInstaller>()
    ;
  return tid;
}

LteBsInstaller::LteBsInstaller ()
{

}

void
LteBsInstaller::DoInstall (NodeContainer container, NetDeviceContainer createdDevices)
{
  NS_LOG_INFO ("*** LteBsInstaller ***");

  uint32_t index = 0;

  for (NodeContainer::Iterator it = container.Begin (); it != container.End (); it++)
    {
      
      // Check if the base station has the object LteBsMgnt already installed  //TODO implement
      Ptr<LteBsMgnt> lteBsMg = (*it)->GetObject <LteBsMgnt> ();
      if (lteBsMg  == NULL)
      {
    	  Ptr<NetDevice> device = (createdDevices).Get(index);
    	  lteBsMg = CreateObject <LteBsMgnt> ();
    	  lteBsMg->SetNode(*it);
    	  lteBsMg->SetNetDevice(device);
    	  (*it)->AggregateObject(lteBsMg);
    	  NS_LOG_INFO ("The object LteBsMgnt has been installed in the base station");
      }

      Ptr<IPCIUFacilities> facilities = (*it)->GetObject <IPCIUFacilities> ();
      if (facilities == NULL)
      {
    	  IPCIUFacilitiesHelper facilitiesHelper;
    	  facilitiesHelper.SetServiceListHelper(m_servListHelper);
    	  facilitiesHelper.Install(*it);
    	  NS_LOG_INFO ("The object IPCIUFacilities has been installed in the vehicle");
      }

     index++ ;
    }
    
}

} // namespace ns3

