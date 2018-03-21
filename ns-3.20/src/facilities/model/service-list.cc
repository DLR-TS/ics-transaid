/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>
 */
#include <list>
#include <map>
#include "ns3/object.h"
#include "ns3/application-container.h"
#include "ns3/assert.h"
#include "ns3/vector.h"
#include "ns3/log.h"

#include "service-list.h"

NS_LOG_COMPONENT_DEFINE ("ServiceList");

namespace ns3 {

ServiceList::ServiceList()
{
  application_index=0;
}

ServiceList::~ServiceList(){}

void
ServiceList::AddServices (ApplicationContainer appcont, AppStringList appstringlist)
{
   m_service_container.Add(appcont); 
   for(AppStringList::const_iterator it = appstringlist.begin (); it != appstringlist.end (); it++)
     {
        m_service_corr_list.insert(std::make_pair((*it), application_index));
        NS_LOG_INFO("[ns3][ServiceList] service " << application_index<<" = " << *it <<" --------\n");
        application_index++;
     }
}

uint32_t
ServiceList::GetServiceIndex (std::string servicetype)
{
  ServiceCorrespondaceList::iterator it = m_service_corr_list.find(servicetype);
  if( it != m_service_corr_list.end() ) 
    {
      return it->second;
    }
  else
    {
      return 0;
    }
}



ApplicationContainer
ServiceList::GetAllServices (void)
{
  return m_service_container;
}

void
ServiceList::AddServices (ApplicationContainer appcont, std::string appstring)
{
  m_service_container.Add(appcont); 
  m_service_corr_list.insert(std::make_pair(appstring, application_index));
 //std::cout<<std::endl;
      
  application_index++;
}


uint32_t 
ServiceList::GetNServices (void) 
{
  return m_service_corr_list.size (); 
}

Ptr<Application> 
ServiceList::GetService (uint32_t index) 
{
  if (index > m_service_corr_list.size ())
    {
      NS_FATAL_ERROR ("ServiceList::GetService:  index " << index << " out of range");
      return (NULL);
    }
  else
    return m_service_container.Get (index);
}

Ptr<Application> 
ServiceList::GetService (std::string servicetype) 
{
  ServiceCorrespondaceList::iterator it = m_service_corr_list.find(servicetype);
  if( it != m_service_corr_list.end() ) 
    {
      return m_service_container.Get((it->second));
    }
  else
    {
      return (NULL);
    }
}

}//namespace ns3
