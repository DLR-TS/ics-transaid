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
#include "vehicle-sta-mgnt.h"
#include "ns3/c2c-address.h"
#include "ns3/location-table.h"
#include "ns3/itetris-types.h"
#include "ns3/node-list.h"
//#include "ns3/ptr.h"
#include "rsu-sta-mgnt.h"
#include <stdlib.h>

NS_LOG_COMPONENT_DEFINE ("VehicleStaMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (VehicleStaMgnt);

TypeId VehicleStaMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VehicleStaMgnt")
    .SetParent<Object> ()           
    .AddConstructor<VehicleStaMgnt> ()        
    ;
  return tid;
}

VehicleStaMgnt::VehicleStaMgnt ()
{
  m_ipBaseStationList = new IpBaseStationList ();
  m_roadSideUnitList = new RoadSideUnitList ();
  m_nodeActive = false;
}

VehicleStaMgnt::~VehicleStaMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
  delete m_ipBaseStationList;
  delete m_roadSideUnitList;
  m_ipBaseStationList = NULL;
  m_roadSideUnitList = NULL;
}

bool 
VehicleStaMgnt::AddIpTechnology (std::string technology, Ptr<NetDevice> netDevice, Ptr<VehicleScanMngr> scanMngr)
{
  IpNetDeviceList::iterator iter = m_ipNetDeviceList.find (technology);
  if( iter != m_ipNetDeviceList.end() ) 
    {
      NS_LOG_INFO ("The IP NetDevice " << technology << " is already being used");
      return false;
    }
  IpStationTuple ipStation;
  ipStation.device = netDevice;
  ipStation.scanMngr = scanMngr;
  m_ipNetDeviceList.insert (std::make_pair(technology, ipStation));    
  NS_LOG_INFO ("The IP NetDevice " << technology << " has been successfully added to VehicleStaMgnt");
  return true;
}

bool 
VehicleStaMgnt::AddC2cTechnology (std::string technology, Ptr<NetDevice> netDevice)
{
  C2cNetDeviceList::iterator iter = m_c2cNetDeviceList.find (technology);
  if( iter != m_c2cNetDeviceList.end() ) 
    {
      NS_LOG_INFO ("The C2C NetDevice " << technology << " is already being used");
      return false;
    }
  m_c2cNetDeviceList.insert (std::make_pair(technology, netDevice));    
  NS_LOG_INFO ("The C2C NetDevice " << technology << " has been successfully added to VehicleStaMgnt");
  return true;
}


IpBaseStationList*
VehicleStaMgnt::GetRegisteredIpBaseStations (STACK stack) const
{
  ScanIpBaseStations (stack);
  return (m_ipBaseStationList);
}

IpBaseStationList*
VehicleStaMgnt::GetRegisteredIpBaseStations (void) const
{
  ScanIpBaseStations ();
  return (m_ipBaseStationList);
}

RoadSideUnitList*
VehicleStaMgnt::GetRsusInCoverage (void) const
{
  ScanRoadSideUnits ();
  return (m_roadSideUnitList);
}

void 
VehicleStaMgnt::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<c2cAddress> 
VehicleStaMgnt::GetC2cAddress (uint32_t nodeId) const
{
  Ptr<c2cAddress> resAddress = NULL; 
  if (nodeId == ID_BROADCAST)
    { 
      resAddress = CreateObject<c2cAddress> (); 
      NS_LOG_INFO ("[VehicleStaMgnt::GetC2cAddress] Broadcast C2C address");
      resAddress->Set(c2cAddress::BROAD, 1); // One-hop topo-broadcast
      return (resAddress);
    }
  else
    {
      Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
      if (locationTable != NULL)
	{
	  LocationTable::Table table = locationTable->GetTable ();
	  NS_LOG_INFO ("[VehicleStaMgnt::GetC2cAddress] Looking up c2cAdress of node "<< nodeId << " in neighbor table of node " << m_node->GetId ());
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
    }    void ActivateNode (void);
    void DeactivateNode (void);
  return (resAddress);
}

Ipv4Address* 
VehicleStaMgnt::GetIpAddress (uint32_t nodeId) const
{
  ScanIpBaseStations ();
  Ipv4Address* resAddress = NULL; 
  for (IpBaseStationList::iterator iter = m_ipBaseStationList->begin(); iter != m_ipBaseStationList->end(); iter++)
    {
      Ptr<const IpBaseStation> station = (*iter).second;
      if (station->GetNodeId () == nodeId)
        {
          resAddress = new Ipv4Address ();
          resAddress->Set(station->GetIpAddress ().Get ());
          return (resAddress);
	}
      }
  return (resAddress);
}

Ipv6Address* 
VehicleStaMgnt::GetIpv6Address (uint32_t nodeId) const
{
  ScanIpBaseStations (IPv6);
  Ipv6Address* resAddress = NULL;
  for (IpBaseStationList::iterator iter = m_ipBaseStationList->begin(); iter != m_ipBaseStationList->end(); iter++)
    {
      Ptr<const IpBaseStation> station = (*iter).second;
      if (station->GetNodeId () == nodeId)
        {
          resAddress = new Ipv6Address ();
          uint8_t* addr_buf = (uint8_t*) malloc (16*sizeof (uint8_t));
          resAddress->Set(station->GetIpv6Address ().GetBytes2 (addr_buf));
          return (resAddress);
	}
      }
   if(resAddress == NULL)
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
		  resAddress = new Ipv6Address ();
                  uint8_t* addr_buf = (uint8_t*) malloc (16*sizeof (uint8_t));
                  resAddress->Set((*iter).ipAddr[0].GetBytes2 (addr_buf));
                  //std::cout<<"Inside VehicleStaMgnt::GetIpv6Address!!!"<<resAddress<<std::endl; 
		  NS_LOG_INFO ("Node found with Id "<< nodeId << " and IPv6 address: "<< *resAddress <<"\n");
		  return (resAddress);
		}
	    }
	}
    }
  return (resAddress);
}


void 
VehicleStaMgnt::ScanIpBaseStations (void) const
{
   // Clean previous entries in the m_ipBaseStationList
   for (IpBaseStationList::iterator iter = m_ipBaseStationList->begin(); iter != m_ipBaseStationList->end(); iter++)
      {
	(*iter).second = NULL;
      }
    m_ipBaseStationList->clear ();

    // Find best serving base station for each technology
    for (IpNetDeviceList::const_iterator iter = m_ipNetDeviceList.begin(); iter != m_ipNetDeviceList.end(); iter++)
      {
	Ptr<VehicleScanMngr> scanMngr = (*iter).second.scanMngr;
        if (scanMngr != NULL)
          {
	    NS_LOG_INFO ("VehicleScanMgnr found in Node "<< m_node->GetId () << " for NetDevice " << (*iter).first );
	    Ptr<IpBaseStation> station = scanMngr->GetBestServingBs (); 
            if (station != NULL)
	      {
                m_ipBaseStationList->insert (std::make_pair((*iter).first, station));
	        NS_LOG_INFO ("Best serving station. Id=" << station->GetNodeId () << " IpAddress=" << station->GetIpAddress () << " Lat=" << station->GetLat () << " Lon=" << station->GetLon () ); 
	      }
	  }
         else
          {
	    NS_LOG_INFO ("VehicleScanMgnr not found in Node "<< m_node->GetId () << " for NetDevice " << (*iter).first );
	  }
      }
}


void 
VehicleStaMgnt::ScanIpBaseStations (STACK stack) const
{
   // Clean previous entries in the m_ipBaseStationList
   for (IpBaseStationList::iterator iter = m_ipBaseStationList->begin(); iter != m_ipBaseStationList->end(); iter++)
      {
	(*iter).second = NULL;
      }
    m_ipBaseStationList->clear ();

    // Find best serving base station for each technology
    for (IpNetDeviceList::const_iterator iter = m_ipNetDeviceList.begin(); iter != m_ipNetDeviceList.end(); iter++)
      {
	Ptr<VehicleScanMngr> scanMngr = (*iter).second.scanMngr;
        if (scanMngr != NULL)
          {
	    NS_LOG_INFO ("VehicleScanMgnr found in Node "<< m_node->GetId () << " for NetDevice " << (*iter).first );
            Ptr<IpBaseStation> station = scanMngr->GetBestServingBs (stack); 
            if (station != NULL)
	      {
                if(stack==IPv4)
                {
                        m_ipBaseStationList->insert (std::make_pair((*iter).first, station));
	                NS_LOG_INFO ("Best serving station. Id=" << station->GetNodeId () << " Ipv4Address=" << station->GetIpAddress () << " Lat=" << station->GetLat () << " Lon=" << station->GetLon () );
                }
                else
                {
                        m_ipBaseStationList->insert (std::make_pair((*iter).first, station));
	                NS_LOG_INFO ("Best serving station. Id=" << station->GetNodeId () << " Ipv6Address=" << station->GetIpv6Address () << " Lat=" << station->GetLat () << " Lon=" << station->GetLon () );
                }
	      }
	  }
         else
          {
	    NS_LOG_INFO ("VehicleScanMgnr not found in Node "<< m_node->GetId () << " for NetDevice " << (*iter).first );
	  }
      }
}


bool 
VehicleStaMgnt::IsNodeActive (void)
{
  return (m_nodeActive);
}

void VehicleStaMgnt::ActivateNode (void)
{
  C2cNetDeviceList::iterator iter = m_c2cNetDeviceList.begin ();
  for (iter = m_c2cNetDeviceList.begin (); iter != m_c2cNetDeviceList.end (); ++iter)
      iter->second->ActivateNetDevice ();
  m_nodeActive = true;
}
    
void VehicleStaMgnt::DeactivateNode (void)
{
  C2cNetDeviceList::iterator iter = m_c2cNetDeviceList.begin ();
  for (iter = m_c2cNetDeviceList.begin (); iter != m_c2cNetDeviceList.end (); ++iter)
      iter->second->DeactivateNetDevice ();
  m_nodeActive = false;
}

void VehicleStaMgnt::ScanRoadSideUnits (void) const
{
   // Clean previous entries in the m_roadSideUnitList
  for (RoadSideUnitList::iterator iter = m_roadSideUnitList->begin(); iter != m_roadSideUnitList->end(); iter++)
  {
    (*iter) = NULL;
  }
  m_roadSideUnitList->clear ();

  Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
  if (locationTable != NULL)
  {
  LocationTable::Table table = locationTable->GetTable ();
  for (LocationTable::Table::const_iterator iter = table.begin(); iter < table.end(); iter++)
  {
   Ptr<Node> node = NodeList::GetNode ((*iter).gnAddr);
   if (!node->IsMobileNode() )
   {
    Ptr<RsuStaMgnt> staMgnt = node->GetObject<RsuStaMgnt> ();
    if (staMgnt != NULL)
    {
     Ptr<RoadSideUnit> station = Create <RoadSideUnit> ();
     station->SetLat ((*iter).Lat);
     station->SetLon ((*iter).Long);
     station->SetNodeId ((*iter).gnAddr); 
     m_roadSideUnitList->push_back (station);
    }
   }
  }
  }

}

C2cNetDeviceList 
VehicleStaMgnt::GetC2CDeviceList ()
{
return m_c2cNetDeviceList;
}

IpNetDeviceList 
VehicleStaMgnt::GetIPDeviceList ()
{
return m_ipNetDeviceList;
}

Ptr<NetDevice> 
VehicleStaMgnt::GetIpNetDevice (std::string technology)
{
  IpNetDeviceList::iterator iter = m_ipNetDeviceList.begin();
  iter = m_ipNetDeviceList.find(technology);
    if (iter != m_ipNetDeviceList.end()) 
      {
        return ((*iter).second.device);
      }
    else
      {
        return NULL;
      }
}

} // namespace ns3
