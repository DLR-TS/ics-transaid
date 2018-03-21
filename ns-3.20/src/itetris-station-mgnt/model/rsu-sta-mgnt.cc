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


/*
 * Edited by Jérôme Härri <haerri@eurecom.fr>
 * EURECOM 2013
 *
 * Implementation of a ETSI DCC module for STF447
 *
 */

/*
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 *
 * IPv6 integration
 *
 */

#include "ns3/log.h"
#include "rsu-sta-mgnt.h"
#include "ns3/node.h"
#include "ns3/c2c-address.h"
#include "ns3/location-table.h"
#include "ns3/itetris-types.h"
#include "ns3/ipv6-l3-protocol.h"
//#include "ns3/ptr.h"
#include <stdlib.h>

NS_LOG_COMPONENT_DEFINE ("RsuStaMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (RsuStaMgnt);

TypeId RsuStaMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RsuStaMgnt")
    .SetParent<Object> ()                  
    ;
  return tid;
}

RsuStaMgnt::~RsuStaMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<c2cAddress> 
RsuStaMgnt::GetC2cAddress (uint32_t nodeId) const
{
  Ptr<c2cAddress> resAddress = NULL; 
  if (nodeId == ID_BROADCAST)
    { 
      resAddress = CreateObject<c2cAddress> (); 
      NS_LOG_INFO ("[RsuStaMgnt::GetC2cAddress] Broadcast C2C address");
      resAddress->Set(c2cAddress::BROAD, 1); // One-hop topo-broadcast
      return (resAddress);
    }
  else
    {
      Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
      if (locationTable != NULL)
	{
	  LocationTable::Table table = locationTable->GetTable ();
	  NS_LOG_INFO ("[RsuStaMgnt::GetC2cAddress] Looking up c2cAdress of node "<< nodeId << " in neighbor table of node " << m_node->GetId ());
	  for (LocationTable::Table::const_iterator iter = table.begin(); iter < table.end(); iter++)
	    {
	      if ((*iter).gnAddr == nodeId)
		{
		  resAddress = CreateObject<c2cAddress> (); 
		  NS_LOG_INFO ("Node found with Id "<< nodeId );
		  resAddress->Set(nodeId, (*iter).Lat, (*iter).Long);
		  return (resAddress);
		}
	    }
	}
    }
  return (resAddress);
}

bool
RsuStaMgnt::CompareIpv6Prefix (Ipv6Address a, Ipv6Address b, uint32_t pref_length) const
{
	uint8_t* a_pre = (uint8_t*) malloc (pref_length*sizeof (uint8_t));
	a.getPrefixBytes(a_pre, pref_length);
	uint8_t* b_pre = (uint8_t*) malloc (pref_length*sizeof (uint8_t));
	b.getPrefixBytes(b_pre, pref_length);
	for (unsigned int i=0; i<pref_length/8; i++)
		if(&a_pre[i] != &b_pre[i])
		{
			delete[] a_pre;
			delete[] b_pre;
			return false;
		}
	delete[] a_pre;
	delete[] b_pre;
	return true;
}


Ipv6Address*
RsuStaMgnt::GetIpv6Address (uint32_t nodeId) const
{
	Ipv6Address* resAddress = NULL;
	if (nodeId == ID_MULTICAST || nodeId == ID_BROADCAST)
	{
		resAddress = new Ipv6Address ();
		resAddress->Set("ff02::1");
		return resAddress;

	}
	else
	{
	Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
	if (locationTable != NULL)
	{
	  LocationTable::Table table = locationTable->GetTable ();
	  NS_LOG_INFO ("[VehicleStaMgnt::GetIPv6Address] Looking up IPv6Adress of node "<< nodeId << " in neighbor table of node " << m_node->GetId ()<<"\n");
	  for (LocationTable::Table::const_iterator iter = table.begin(); iter < table.end(); iter++)
	  {
		  if ((*iter).gnAddr == nodeId)
		  {
			  for (unsigned int i = 0; i< (*iter).ipAddr.size(); i++)
			  {
				  //Compare Ipv6 prefixes of location table entries and RSU Ipv6 address.
				  if(CompareIpv6Prefix((*iter).ipAddr[i], m_node->GetObject<Ipv6L3Protocol> ()->GetAddress(1,1).GetAddress(), 64))
				  {
					  resAddress = new Ipv6Address ();
					  uint8_t* addr_buf = (uint8_t*) malloc (16*sizeof (uint8_t));
					  resAddress->Set((*iter).ipAddr[i].GetBytes2 (addr_buf));
					  NS_LOG_INFO ("Node found with Id "<< nodeId << " and IPv6 address: "<< *resAddress <<"\n");
					  return (resAddress);
				  }
			  }
		  }
	    }
	}
	return resAddress;
	}
}

void 
RsuStaMgnt::SetNode (Ptr<Node> node)
{
  m_node = node;
}

uint32_t 
RsuStaMgnt::GetNodeId ()
{
  return m_node->GetId();
}

bool
RsuStaMgnt::AddC2cTechnology (std::string technology, Ptr<NetDevice> netDevice)
{
  rsuNetDeviceList::iterator iter = m_rsuNetDeviceList.find (technology);
  if( iter != m_rsuNetDeviceList.end() )
    {
      NS_LOG_INFO ("The C2C NetDevice " << technology << " is already being used");
      return false;
    }
  m_rsuNetDeviceList.insert (std::make_pair(technology, netDevice));
  NS_LOG_INFO ("The C2C NetDevice " << technology << " has been successfully added to VehicleStaMgnt");
  return true;
}

uint32_t 
RsuStaMgnt::GetNumberOfNodesInCoverage (void) const
{
  Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
  if (locationTable != NULL)
    {
      return (locationTable->GetNbNeighs ());
    }
  return (0);
}

double
RsuStaMgnt::GetCoverageRange (void) const
{
    // TODO (JHNOTE) complete the CoverageRange of a RSU STA-MANAGER, for example...
    //return DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
    return (500.0); // hardcoded - the method is not available in the netDevice and does not make much sense
}

} // namespace ns3
