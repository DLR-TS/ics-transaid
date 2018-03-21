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

#include "itetris-network-transport-installer.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("ItetrisNetworkTransportInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (ItetrisNetworkTransportInstaller);

TypeId ItetrisNetworkTransportInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ItetrisNetworkTransportInstaller")
    .SetParent<Object> ()     
    .AddConstructor<ItetrisNetworkTransportInstaller> ()             
    ;
  return tid;
}
    
ItetrisNetworkTransportInstaller::ItetrisNetworkTransportInstaller () 
{
  stack.Setc2cStackInstall (true);  // -- false = Disable
  stack.SetIpv6StackInstall(true);
  stack.SetIpv4StackInstall(false);
} 

void
ItetrisNetworkTransportInstaller::Install (NodeContainer container) {
  
//std::cout<<"ItetrisNetworkTransportInstaller::Install!!!!!!!!!!!!!!!!!"<<std::endl;
  stack.Install (container);

}

}
