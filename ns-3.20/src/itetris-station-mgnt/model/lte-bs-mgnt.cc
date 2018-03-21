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
 */
#include "ns3/log.h"
#include "lte-bs-mgnt.h"
#include "ns3/lte-net-device.h"
#include "ns3/addressing-support.h"
#include "ns3/ipv4.h"

NS_LOG_COMPONENT_DEFINE ("LteBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LteBsMgnt);

TypeId LteBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteBsMgnt")
    .SetParent<IpBaseStaMgnt> ()        
    .AddConstructor<LteBsMgnt> ()
    ;
  return tid;
}

LteBsMgnt::~LteBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
LteBsMgnt::AddVehicle(Ptr<LteNetDevice> device)
{
        Ptr<Node> node=device->GetNode();
       
        uint32_t nodeId=node->GetId();
       
        Ptr<Ipv4> ipv4=node->GetObject<Ipv4>();
        Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1,0);  
        Ipv4Address ipv4addr=iaddr.GetLocal(); 

        vehicleMap[nodeId] = ipv4addr;  
}


Ipv4Address* 
LteBsMgnt::GetIpAddress (uint32_t nodeId) const
{
   
/**Jin: error occurs; comment this part for now **/
/*
        Ipv4Address* address = NULL;
        if (nodeId == ID_BROADCAST)
        {
              address = GetIpBroadcastAddress ();
        }
        else
        {
            return &(vehicleMap[nodeId]);
               
        }

  return (address); 
*/
}

/**Jin : build ipv6 part**/
Ipv6Address* 
LteBsMgnt::GetIpv6Address (uint32_t nodeId) const
{
   return 0;

}


uint32_t 
LteBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
LteBsMgnt::GetNumberOfRegisteredUsers (void) const
{
	return 0;
}

double
LteBsMgnt::GetCoverageRange (void) const
{
	return 0.0;
}

void 
LteBsMgnt::TriggerVehiclesScanning (void) const
{  
}

} // namespace ns3


