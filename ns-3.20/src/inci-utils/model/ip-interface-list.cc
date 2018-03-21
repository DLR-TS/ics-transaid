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

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/


#include "ip-interface-list.h"
#include "ns3/log.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("IpInterfaceList");

namespace ns3
{
NS_OBJECT_ENSURE_REGISTERED (IpInterfaceList);

TypeId IpInterfaceList::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::IpInterfaceList")
    .SetParent<Object> ()                  
    ;
  return tid;
}    

IpInterfaceList::IpInterfaceList( ){} 

bool 
IpInterfaceList::AddIpInterface (std::string interfaceName, Ipv4InterfaceAddress interface)
{
  InterfaceList::iterator iter = m_ipInterfaceList.find (interfaceName);
  if( iter != m_ipInterfaceList.end() ) 
    {
      NS_LOG_DEBUG ("The IP Interface name " << interfaceName << " is already being used");
      return false;
    }
  m_ipInterfaceList.insert (std::make_pair(interfaceName, interface));    
  return true;
}

bool 
IpInterfaceList::AddIpInterface (std::string interfaceName, Ipv6InterfaceAddress interface)
{
  InterfaceListv6::iterator iter = m_ipv6InterfaceList.find (interfaceName);
  if( iter != m_ipv6InterfaceList.end() ) 
    {
      NS_LOG_DEBUG ("The IPv6 Interface name " << interfaceName << " is already being used");
      return false;
    }
  m_ipv6InterfaceList.insert (std::make_pair(interfaceName, interface));    
  return true;
}

Ipv4InterfaceAddress
IpInterfaceList::GetIpInterfaceAddress (std::string interfaceName)
{
  InterfaceList::iterator iter = m_ipInterfaceList.find (interfaceName);
  if( iter != m_ipInterfaceList.end() ) 
    {
      return (iter->second);
    }
  else
    {
      NS_LOG_DEBUG ("The NetInterface " << interfaceName << " has not been found"); 
      return (Ipv4InterfaceAddress(Ipv4Address("0.0.0.0"),Ipv4Mask("0.0.0.0")));
    }
}


Ipv6InterfaceAddress
IpInterfaceList::GetIpv6InterfaceAddress (std::string interfaceName)
{
  InterfaceListv6::iterator iter = m_ipv6InterfaceList.find (interfaceName);
  if( iter != m_ipv6InterfaceList.end() ) 
    {
      return (iter->second);
    }
  else
    {
     //std::cout<<"IpInterfaceList::GetIpv6InterfaceAddress NET INTERFACE NOT FOUND!!!"<<std::endl;
      NS_LOG_DEBUG ("The NetInterface " << interfaceName << " has not been found"); 
      return (Ipv6InterfaceAddress(Ipv6Address("0::0")));
    }
}

Ipv4Address 
IpInterfaceList::GetIpAddress (std::string interfaceName)
{
  Ipv4InterfaceAddress interface = GetIpInterfaceAddress (interfaceName);
  return (interface.GetLocal ());
}

Ipv6Address 
IpInterfaceList::GetIpv6Address (std::string interfaceName)
{
  Ipv6InterfaceAddress interface = GetIpv6InterfaceAddress (interfaceName);
  //std::cout<<"INTERFACE ADDRESS:"<<interface<<std::endl;
  //Not sure about this
  return (interface.GetAddress ());
}

}

