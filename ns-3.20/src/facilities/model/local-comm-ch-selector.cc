/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/****************************************************************************************
 * Edited by Jin Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * Added LTE support
***************************************************************************************/


#include "ns3/object.h"
#include <stdlib.h>
#include "ns3/node-list.h"
#include "ns3/mobility-model.h"
#include "local-comm-ch-selector.h"

NS_LOG_COMPONENT_DEFINE ("LocalCOMMchSelector");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LocalCOMMchSelector);

TypeId 
LocalCOMMchSelector::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LocalCOMMchSelector")
    .SetParent<Object> ()
    .AddConstructor<LocalCOMMchSelector> ()
    ;
  return tid;
}

LocalCOMMchSelector::LocalCOMMchSelector ()
{
}

LocalCOMMchSelector::~LocalCOMMchSelector ()
{
}

void
LocalCOMMchSelector::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
LocalCOMMchSelector::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
LocalCOMMchSelector::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}


stacktodestination*
LocalCOMMchSelector::GetCommunicationChTMC(uint32_t commProfile, TechnologyList technologies/*, STACK stack*/)
{  
  stacktodestination* stacktodest=NULL;
  Ptr<VehicleStaMgnt> vsta;
  vsta = m_node->GetObject <VehicleStaMgnt> ();
  Ptr<MobilityModel> mob = m_node->GetObject<MobilityModel> ();
  uint32_t lat1 = (uint32_t) mob->GetPosition().x;
  uint32_t lon1 = (uint32_t) mob->GetPosition().y;
  uint32_t lat2, lon2, distmin, dist, closetRsuId, closetIpBSId;

  if (vsta != NULL)
  {
  // Generic profile 1
  if (commProfile == 1)
  {

      C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
      if( iter != vsta->GetC2CDeviceList().end() ) 
      {// add test link state
        stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        stacktodest->stack= C2C;         
        stacktodest->destination= TOPO_BROADCAST;
        stacktodest->tech = (char*) malloc (1*sizeof (char));
        stacktodest->tech = (char*) "";
      }
      else
      {
        NS_FATAL_ERROR  ("There is no ITSG5 technology available for node "<< m_node->GetId());
      }

  }
  // Generic profile 2
  else if (commProfile == 2)
  {
      C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
      if( iter != vsta->GetC2CDeviceList().end() ) 
      {
        stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        stacktodest->stack= C2C;         
        stacktodest->destination= ID_BROADCAST;
        stacktodest->tech = (char*) malloc (1*sizeof (char));
        stacktodest->tech = (char*) ""; 
      }
      else
      {
        NS_FATAL_ERROR  ("There is no ITSG5 technology available for node "<< m_node->GetId());
      }
  }

  // Generic profile 3
  else if (commProfile == 3)
  {
    for (uint32_t i =0; i< technologies.size(); i++)
    {
        if (technologies[i] == "WaveVehicle")
        {
         C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle"); 
         if( iter != vsta->GetC2CDeviceList().end() )
         {
          if (vsta->GetRsusInCoverage()->size() > 0)
          {
            std::vector <Ptr<RoadSideUnit> > * vect = vsta->GetRsusInCoverage();
            lat2 = (uint32_t) (*vect)[0]->GetLat();
            lon2 = (uint32_t) (*vect)[0]->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetRsuId = (*vect)[0]->GetNodeId();

            for (uint32_t i = 1; i < vect->size(); i++)
            {
               lat2 = (uint32_t) (*vect)[i]->GetLat();
               lon2 = (uint32_t) (*vect)[i]->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetRsuId = (*vect)[i]->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= C2C;         
            stacktodest->destination= closetRsuId;
            stacktodest->tech = (char*) malloc (1*sizeof (char));
            stacktodest->tech = (char*) "";
            break;
          }
         }      
      }
      else if (technologies[i] == "UmtsVehicle")
      {
      IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle"); 
      if( iter != vsta->GetIPDeviceList().end() ) 
      {

          if (vsta->GetRegisteredIpBaseStations()->size() != 0)
          {
            /***This means here that each IpBaseStation is of different technology???****/
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations());
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetIpBSId = (*iter).second->GetNodeId();

            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;         
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "UMTS-";
            break;
          }
         }

      }


//support for LTE
 else if (technologies[i] == "LteVehicle")
      {
      IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle"); 
      if( iter != vsta->GetIPDeviceList().end() ) 
      {

          if (vsta->GetRegisteredIpBaseStations()->size() != 0)
          {
            /***This means here that each IpBaseStation is of different technology???****/
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations());
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetIpBSId = (*iter).second->GetNodeId();

            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;         
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "LTE-";
            break;
          }
         }

      }

 else if (technologies[i] == "DvbhVehicle")
      {
         IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle"); 
         if( iter != vsta->GetIPDeviceList().end() ) 
         {

          if (vsta->GetRegisteredIpBaseStations()->size() != 0)
          {
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations());
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;         
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "DVBH-";
            break;
          }
        }
      }
    }
  }

  // Generic profile 4
  else if (commProfile == 4)
  {
    for (uint32_t i =0; i< technologies.size(); i++)
    {
      if (technologies[i] == "WaveVehicle")
      {
         C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle"); 
         if( iter != vsta->GetC2CDeviceList().end() ) 
         {

         if (vsta->GetRsusInCoverage()->size() > 0)
          {
            std::vector <Ptr<RoadSideUnit> > * vect = vsta->GetRsusInCoverage();
            lat2 = (uint32_t) (*vect)[0]->GetLat();
            lon2 = (uint32_t) (*vect)[0]->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetRsuId = (*vect)[0]->GetNodeId();

            for (uint32_t i = 1; i < vect->size(); i++)
            {
               lat2 = (uint32_t) (*vect)[i]->GetLat();
               lon2 = (uint32_t) (*vect)[i]->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetRsuId = (*vect)[i]->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= C2C;         
            stacktodest->destination= closetRsuId;
            stacktodest->tech = (char*) malloc (1*sizeof (char));
            stacktodest->tech = (char*) "";
            break;
          }
         }
         else
         {
           NS_LOG_ERROR ("There is no ITSG5 technology available for node "<< m_node->GetId());
         }
      }
    }
  }


  // Generic profile 5 IPv6 support
  else if (commProfile == 5)
  {
    for (uint32_t i =0; i< technologies.size(); i++)
    {
        if (technologies[i] == "WaveVehicle")
        {
         C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
         if( iter != vsta->GetC2CDeviceList().end() )
         {
          if (vsta->GetRsusInCoverage()->size() > 0)
          {
            std::vector <Ptr<RoadSideUnit> > * vect = vsta->GetRsusInCoverage();
            lat2 = (uint32_t) (*vect)[0]->GetLat();
            lon2 = (uint32_t) (*vect)[0]->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetRsuId = (*vect)[0]->GetNodeId();

            for (uint32_t i = 1; i < vect->size(); i++)
            {
               lat2 = (uint32_t) (*vect)[i]->GetLat();
               lon2 = (uint32_t) (*vect)[i]->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetRsuId = (*vect)[i]->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= C2C;
            stacktodest->destination= closetRsuId;
            stacktodest->tech = (char*) malloc (1*sizeof (char));
            stacktodest->tech = (char*) "";
            break;
          }
         }
      }
      else if (technologies[i] == "UmtsVehicle")
      {
      IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle");
      if( iter != vsta->GetIPDeviceList().end() )
      {
          if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
          {
            /***This means here that each IpBaseStation is of different technology???****/
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetIpBSId = (*iter).second->GetNodeId();

            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv6;
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "UMTS-";
            break;
          }
         }


      }
        // support for LTE
      else if (technologies[i] == "LteVehicle")
      {
      IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
      if( iter != vsta->GetIPDeviceList().end() )
      {
          if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
          {
            /***This means here that each IpBaseStation is of different technology???****/
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            closetIpBSId = (*iter).second->GetNodeId();

            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv6;
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "LTE-";
            break;
          }
         }


      }
      else if (technologies[i] == "DvbhVehicle")
      {
         IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle");
         if( iter != vsta->GetIPDeviceList().end() )
         {

          if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
          {
            std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
            IpBaseStationList::iterator iter = mapp->begin ();
            lat2 = (uint32_t) iter->second->GetLat();
            lon2 = (uint32_t) iter->second->GetLon();
            distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
            for (iter = mapp->begin (); iter != mapp->end (); ++iter)
            {
               lat2 = (uint32_t) (*iter).second->GetLat();
               lon2 = (uint32_t) (*iter).second->GetLon();
               dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
               if (distmin > dist)
               {
                 distmin = dist;
                 closetIpBSId = (*iter).second->GetNodeId();
               }

            }
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv6;
            stacktodest->destination= closetIpBSId;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "DVBH-";
            break;
          }
        }
      }
    }
  }

  // Generic profile 6 IPv6 support
  else if (commProfile == 6)
  {
	  for (uint32_t i =0; i< technologies.size(); i++)
	  {
		  if (technologies[i] == "WaveVehicle")
		  {
			  C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
			  if( iter != vsta->GetC2CDeviceList().end() )
			  {
				  if (vsta->GetRsusInCoverage()->size() > 0)
				  {
					  std::vector <Ptr<RoadSideUnit> > * vect = vsta->GetRsusInCoverage();
					  lat2 = (uint32_t) (*vect)[0]->GetLat();
					  lon2 = (uint32_t) (*vect)[0]->GetLon();
					  distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
					  closetRsuId = (*vect)[0]->GetNodeId();
					  for (uint32_t i = 1; i < vect->size(); i++)
					  {
						  lat2 = (uint32_t) (*vect)[i]->GetLat();
						  lon2 = (uint32_t) (*vect)[i]->GetLon();
						  dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
						  if (distmin > dist)
						  {
							  distmin = dist;
							  closetRsuId = (*vect)[i]->GetNodeId();
						  }
					  }
					  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
					  stacktodest->stack= IPv6;
					  stacktodest->destination= closetRsuId;
					  stacktodest->tech = (char*) malloc (1*sizeof (char));
					  stacktodest->tech = (char*) "";
					  break;
				  }
			  }
		  }
          else if (technologies[i] == "UmtsVehicle")
          {
        	  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle");
        	  if( iter != vsta->GetIPDeviceList().end() )
        	  {
        		  if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
        		  {
        			  /***This means here that each IpBaseStation is of different technology???****/
        			  std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
        			  IpBaseStationList::iterator iter = mapp->begin ();
        			  lat2 = (uint32_t) iter->second->GetLat();
        			  lon2 = (uint32_t) iter->second->GetLon();
        			  distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        			  closetIpBSId = (*iter).second->GetNodeId();

        			  for (iter = mapp->begin (); iter != mapp->end (); ++iter)
        			  {
        				  lat2 = (uint32_t) (*iter).second->GetLat();
        				  lon2 = (uint32_t) (*iter).second->GetLon();
        				  dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        				  if (distmin > dist)
        				  {
        					  distmin = dist;
        					  closetIpBSId = (*iter).second->GetNodeId();
        				  }

        			  }
        			  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        			  stacktodest->stack= IPv6;
        			  stacktodest->destination= closetIpBSId;
        			  stacktodest->tech = (char*) malloc (10*sizeof (char));
        			  stacktodest->tech = (char*) "UMTS-";
        			  break;
        		  }
        	  }
          }
        // LTE support
          else if (technologies[i] == "LteVehicle")
          {
        	  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
        	  if( iter != vsta->GetIPDeviceList().end() )
        	  {
        		  if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
        		  {
        			  /***This means here that each IpBaseStation is of different technology???****/
        			  std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
        			  IpBaseStationList::iterator iter = mapp->begin ();
        			  lat2 = (uint32_t) iter->second->GetLat();
        			  lon2 = (uint32_t) iter->second->GetLon();
        			  distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        			  closetIpBSId = (*iter).second->GetNodeId();

        			  for (iter = mapp->begin (); iter != mapp->end (); ++iter)
        			  {
        				  lat2 = (uint32_t) (*iter).second->GetLat();
        				  lon2 = (uint32_t) (*iter).second->GetLon();
        				  dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        				  if (distmin > dist)
        				  {
        					  distmin = dist;
        					  closetIpBSId = (*iter).second->GetNodeId();
        				  }

        			  }
        			  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        			  stacktodest->stack= IPv6;
        			  stacktodest->destination= closetIpBSId;
        			  stacktodest->tech = (char*) malloc (10*sizeof (char));
        			  stacktodest->tech = (char*) "LTE-";
        			  break;
        		  }
        	  }
          }
          else if (technologies[i] == "DvbhVehicle")
          {
        	  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle");
        	  if( iter != vsta->GetIPDeviceList().end() )
        	  {

        		  if (vsta->GetRegisteredIpBaseStations(IPv6)->size() != 0)
        		  {
        			  std::map <std::string, Ptr<const IpBaseStation>  > * mapp = (vsta->GetRegisteredIpBaseStations(IPv6));
        			  IpBaseStationList::iterator iter = mapp->begin ();
        			  lat2 = (uint32_t) iter->second->GetLat();
        			  lon2 = (uint32_t) iter->second->GetLon();
        			  distmin = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        			  for (iter = mapp->begin (); iter != mapp->end (); ++iter)
        			  {
        				  lat2 = (uint32_t) (*iter).second->GetLat();
        				  lon2 = (uint32_t) (*iter).second->GetLon();
        				  dist = sqrt(pow(lat1 - lat2,2) + pow(lon1 - lon2,2));
        				  if (distmin > dist)
        				  {
        					  distmin = dist;
        					  closetIpBSId = (*iter).second->GetNodeId();
        				  }
        			  }
        			  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        			  stacktodest->stack= IPv6;
        			  stacktodest->destination= closetIpBSId;
        			  stacktodest->tech = (char*) malloc (10*sizeof (char));
        			  stacktodest->tech = (char*) "DVBH-";
        			  break;
        		  }
        	  }
          }
	  }
  }

  else
  {
    NS_LOG_ERROR ("Communication Profile Not found "<<commProfile);
  }

  }
  else
    NS_LOG_ERROR ("Vehicle Station Management is not installed in node"<<m_node);
  return stacktodest;
}


stacktodestination*
LocalCOMMchSelector::GetCommunicationCh(uint32_t commProfile, TechnologyList technologies/*, STACK stack*/)
{

  stacktodestination* stacktodest=NULL;
  Ptr<VehicleStaMgnt> vsta;
  vsta = m_node->GetObject <VehicleStaMgnt> ();
  Ptr<MobilityModel> mob = m_node->GetObject<MobilityModel> ();
  
  if (vsta != NULL)
  {
  // Generic profile 1
  if (commProfile == 1)
  {
      C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
      if( iter != vsta->GetC2CDeviceList().end() ) 
      {// add test link state
        stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        stacktodest->stack= C2C;         
        stacktodest->destination= ID_BROADCAST;
        stacktodest->tech = (char*) malloc (1*sizeof (char));
        stacktodest->tech = (char*) "";
        //stacktodest->tclass = TC1;
      }
      else
      {
        NS_FATAL_ERROR  ("There is no ITSG5 technology available for node "<< m_node->GetId());
      }
  }

  // Generic profile 2
  else if (commProfile == 2)
  {
      C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
      if( iter != vsta->GetC2CDeviceList().end() ) 
      {
        stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
        stacktodest->stack= C2C;         
        stacktodest->destination= ID_BROADCAST;
        stacktodest->tech = (char*) malloc (1*sizeof (char));
        stacktodest->tech = (char*) "";
        //stacktodest->tclass = TC2;
      }
      else
      {
        NS_FATAL_ERROR  ("There is no ITSG5 technology available for node "<< m_node->GetId());
      }
  }

  // Generic profile 3
  else if (commProfile == 3)
  {
    for (uint32_t i =0; i< technologies.size(); i++)
    {
        if (technologies[i] == "WaveVehicle")
        {
         C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle"); 
         if( iter != vsta->GetC2CDeviceList().end() )
         {
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= C2C;
            stacktodest->tech = (char*) malloc (1*sizeof (char));
            stacktodest->tech = (char*) "";
            //stacktodest->tclass = TC3;
            break;
         }
        }
      else if (technologies[i] == "UmtsVehicle")
      {
         IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle"); 
         if( iter != vsta->GetIPDeviceList().end() ) 
         {
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "UMTS-";
            //stacktodest->tclass = TC3;
            break;
          }
      }
        //JIN LTE ADDED
      else if (technologies[i] == "LteVehicle")
      {
          IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
         if( iter != vsta->GetIPDeviceList().end() ) 
         {
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "LTE-";
            //stacktodest->tclass = TC3;
            break;
          }
      }
        //jin end
      else if (technologies[i] == "DvbhVehicle")
      {
         IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle"); 
         if( iter != vsta->GetIPDeviceList().end() ) 
         {
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "DVBH-";
            //stacktodest->tclass = TC3;
            break;
          }
      }
      
    }

  }
  // Generic profile 4
  else if (commProfile == 4)
  {
    for (uint32_t i =0; i< technologies.size(); i++)
    {
      if (technologies[i] == "WaveVehicle")
      {
         C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle"); 
         if( iter != vsta->GetC2CDeviceList().end() ) 
         {

            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= C2C;
            stacktodest->tech = (char*) malloc (1*sizeof (char));
            stacktodest->tech = (char*) "";
            //stacktodest->tclass = TC3;
            break;
          }
         else
         {
           NS_LOG_ERROR ("There is no ITSG5 technology available for node "<< m_node->GetId());
         }
      }
      else if (technologies[i] == "LteVehicle")
      {
         IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
         if( iter != vsta->GetIPDeviceList().end() )
         {
            stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
            stacktodest->stack= IPv4;
            stacktodest->tech = (char*) malloc (10*sizeof (char));
            stacktodest->tech = (char*) "LTE-";
            //stacktodest->tclass = TC3;
            break;
          }
      }
    }
  }
  else if (commProfile == 5)
    {
      for (uint32_t i =0; i< technologies.size(); i++)
      {
          if (technologies[i] == "WaveVehicle")
          {
           C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
           if( iter != vsta->GetC2CDeviceList().end() )
           {
              stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
              stacktodest->stack= C2C;
              stacktodest->tech = (char*) malloc (1*sizeof (char));
              stacktodest->tech = (char*) "";
              //stacktodest->tclass = TC3;
              break;
           }
          }
        else if (technologies[i] == "UmtsVehicle")
        {
           IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle");
           if( iter != vsta->GetIPDeviceList().end() )
           {
              stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
              stacktodest->stack= IPv6;
              stacktodest->tech = (char*) malloc (10*sizeof (char));
              stacktodest->tech = (char*) "UMTS-";
              //stacktodest->tclass = TC3;
              break;
            }
        }
        else if (technologies[i] == "LteVehicle")
        {
           IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
           if( iter != vsta->GetIPDeviceList().end() )
           {
              stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
              stacktodest->stack= IPv6;
              stacktodest->tech = (char*) malloc (10*sizeof (char));
              stacktodest->tech = (char*) "LTE-";
              //stacktodest->tclass = TC3;
              break;
            }
        }
        //IPv6 has not been tested for Dvbh technology
        else if (technologies[i] == "DvbhVehicle")
        {
           IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle");
           if( iter != vsta->GetIPDeviceList().end() )
           {
              stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
              stacktodest->stack= IPv6;
              stacktodest->tech = (char*) malloc (10*sizeof (char));
              stacktodest->tech = (char*) "DVBH-";
              //stacktodest->tclass = TC3;
              break;
            }
        }

      }
    }
  else if (commProfile == 6)
  {
	  for (uint32_t i =0; i< technologies.size(); i++)
	  {
		  if (technologies[i] == "WaveVehicle")
		  {
			  C2cNetDeviceList::iterator iter = vsta->GetC2CDeviceList().find ("WaveVehicle");
	          if( iter != vsta->GetC2CDeviceList().end() )
	          {
	        	  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
	              stacktodest->stack= IPv6;
	              stacktodest->tech = (char*) malloc (1*sizeof (char));
	              stacktodest->tech = (char*) "";
	              //stacktodest->tclass = TC3;
	              break;
	          }
		  }
		  else if (technologies[i] == "UmtsVehicle")
		  {
			  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("UmtsVehicle");
	          if( iter != vsta->GetIPDeviceList().end() )
	          {
	        	  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
	              stacktodest->stack= IPv6;
	              stacktodest->tech = (char*) malloc (10*sizeof (char));
	              stacktodest->tech = (char*) "UMTS-";
	              //stacktodest->tclass = TC3;
	              break;
	          }
		  }
		  else if (technologies[i] == "LteVehicle")
		  {
			  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("LteVehicle");
	          if( iter != vsta->GetIPDeviceList().end() )
	          {
	        	  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
	              stacktodest->stack= IPv6;
	              stacktodest->tech = (char*) malloc (10*sizeof (char));
	              stacktodest->tech = (char*) "LTE-";
	              //stacktodest->tclass = TC3;
	              break;
	          }
		  }
          // IPv6 not having been tested for Dvbh technology
		  else if (technologies[i] == "DvbhVehicle")
		  {
			  IpNetDeviceList::iterator iter = vsta->GetIPDeviceList().find ("DvbhVehicle");
	          if( iter != vsta->GetIPDeviceList().end() )
	          {
	        	  stacktodest = (stacktodestination*) malloc (sizeof (stacktodestination));
	        	  stacktodest->stack= IPv6;
	              stacktodest->tech = (char*) malloc (10*sizeof (char));
	              stacktodest->tech = (char*) "DVBH-";
	              //stacktodest->tclass = TC3;
	              break;
	          }
		  }
	  }
  }
  else
  {
    NS_LOG_ERROR ("Communication Profile Not found "<<commProfile);
  }

  }
  else {
    NS_LOG_ERROR ("Vehicle Station Management is not installed in node"<<m_node);
  }
  return stacktodest;
}

TransmissionMode
LocalCOMMchSelector::GetC2CDisseminationMode(std::string serviceId, CircularGeoAddress destination)
{
  // hardcoded values
  TransmissionMode tmode = C2C_TOPOBROADCAST;
  return tmode;
}


uint32_t 
LocalCOMMchSelector::getNTopoHops(Ptr<Node> disseminator, CircularGeoAddress destination)
{
 // hardcoded values
  return 2; 
}

} //namespace ns3
