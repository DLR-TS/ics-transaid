/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 */

#include "c2c-end-point-demux.h"
#include "c2c-end-point.h"
#include "ns3/log.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("c2cEndPointDemux");

c2cEndPointDemux::c2cEndPointDemux ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

c2cEndPointDemux::~c2cEndPointDemux ()
{
  NS_LOG_FUNCTION_NOARGS ();
  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++) 
    {
      c2cEndPoint *endPoint = *i;
      delete endPoint;
    }
  m_endPoints.clear ();
}

bool
c2cEndPointDemux::LookupPortLocal (uint16_t port)
{
  NS_LOG_FUNCTION_NOARGS ();
  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++) 
    {
      if ((*i)->GetLocalPort  () == port) 
        {
          return true;
        }
    }
  return false;
}

bool
c2cEndPointDemux::LookupLocal (uint64_t nodeId, uint16_t port)
{
  NS_LOG_FUNCTION_NOARGS ();
  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++) 
    {
      if ((*i)->GetLocalPort () == port &&
          (*i)->GetLocalNodeId () == nodeId)
        {
          return true;
        }
    }
  return false;
}

c2cEndPoint *
c2cEndPointDemux::Allocate (uint16_t port)
{
  NS_LOG_FUNCTION (this <<  port);
  c2cEndPoint *endPoint = new c2cEndPoint (port);
  m_endPoints.push_back (endPoint);

  //----------------  check iTETRIS --------------------------------------------
  NS_LOG_INFO ("     * c2cEndPointDemux - End Point Allocated | Total Number of EndPoints = "<<m_endPoints.size ());
  //----------------  check iTETRIS --------------------------------------------

  NS_LOG_DEBUG ("Now have >>" << m_endPoints.size () << "<< endpoints.");
  return endPoint;

}

void 
c2cEndPointDemux::DeAllocate (c2cEndPoint *endPoint)
{
  NS_LOG_FUNCTION_NOARGS ();
  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++) 
    {
      if (*i == endPoint)
        {
          delete endPoint;
          m_endPoints.erase (i);
          break;
        }
    }
}

/*
 * return list of all available Endpoints
 */
c2cEndPointDemux::EndPoints
c2cEndPointDemux::GetAllEndPoints (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  EndPoints ret;

  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++)
   {
      c2cEndPoint* endP = *i;
      ret.push_back(endP);
    }
  return ret;
}

c2cEndPointDemux::EndPoints
c2cEndPointDemux::Lookupc2c (Ptr<const c2cAddress> daddr, uint16_t dport,
                            Ptr<const c2cAddress> saddr, uint16_t sport)
{
  NS_LOG_FUNCTION_NOARGS ();
  EndPoints retval1;   // Lookup criteria 1: Matches exact on local port, wildcards on others
 // EndPoints retval2; // Lookup criteria 2
 // EndPoints retval3; // Lookup criteria 3
 // EndPoints retval4; // Lookup criteria 4

  NS_LOG_FUNCTION (this << /*daddr <<*/ dport << /*saddr <<*/ sport);

  //----------------  check iTETRIS --------------------------------------------
  NS_LOG_INFO ("     **************** c2cEndPointDemux - matching 'Dest Port number' with existing EndPoints | Total number of Endpoints = "<<m_endPoints.size ());
  //----------------  check iTETRIS --------------------------------------------

  if(m_endPoints.begin () == m_endPoints.end ())
	{

	  //----------------  check iTETRIS --------------------------------------------
	  NS_LOG_INFO ("     * c2cEndPointDemux - !! NO APP BIND FOUND !!");
	  NS_LOG_INFO ("=================================  END : NO APP BIND =======================================");
	  //sleep(1);
	  //----------------  check iTETRIS --------------------------------------------

	}

//   NS_LOG_DEBUG ("Looking up endpoint for destination nodeId " << daddr);
  for (EndPointsI i = m_endPoints.begin (); i != m_endPoints.end (); i++)
    {
      c2cEndPoint* endP = *i;
      NS_LOG_DEBUG ("Looking at endpoint dport=" << endP->GetLocalPort ()
                    << " dnodeId=" << endP->GetLocalNodeId ()
                    << " sport=" << endP->GetPeerPort ()
                    << " snodeId=" << endP->GetPeerNodeId ());
     // dport = 8080; // Modified by Ramon Bauza
      if (endP->GetLocalPort () != dport)
        {
          NS_LOG_LOGIC ("Skipping endpoint " << &endP
                        << " because endpoint dport "
                        << endP->GetLocalPort ()
                        << " does not match packet dport " << dport);

    	  //----------------  check iTETRIS --------------------------------------------
    	  NS_LOG_INFO ("     * c2cEndPointDemux - EndPoint Criteria Check - NO MATCH : Local Port = "<<(*i)->GetLocalPort ());
    	  NS_LOG_INFO ("============  END : NO MATCH - DESTINATION PORT Does not match LOCAL PORT!! =============");
    	  //----------------  check iTETRIS --------------------------------------------

          continue;
        }

      if ((*i)->GetLocalPort () == dport )
        { // Lookup criteria 1 : Only local port matches exactly

    	  //----------------  check iTETRIS --------------------------------------------
    	  NS_LOG_INFO ("     * c2cEndPointDemux - EndPoint Criteria Check - MATCH (Dest Port ("<<dport<<") = Local Port ("<<(*i)->GetLocalPort ()<<") ");
    	  //----------------  check iTETRIS --------------------------------------------

    	  retval1.push_back(endP);
        }
      /*
      if ( -- matching look up criteria 2 --)
        { // Lookup criteria 2 description
          retval2.push_back(endP);
        }
      if ( -- matching look up criteria 3 --)
        { // Lookup criteria 3 description
          retval3.push_back(endP);
        }
      if ( -- matching look up criteria 4 --)
        { // Lookup criteria 4 description
          retval4.push_back(endP);
        }*/
    }

  // Here we find the most exact match
 // if (!retval4.empty()) return retval4;
 // if (!retval3.empty()) return retval3;
 // if (!retval2.empty()) return retval2;
  return retval1;  // might be empty if no matches
}

} //namespace ns3

