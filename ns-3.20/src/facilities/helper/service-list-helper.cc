/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project

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

#include "ns3/service-list.h"  
#include "ns3/log.h" 
#include "ns3/node.h"
#include "ns3/application-container.h"
#include "ns3/application-helper.h"
#include "service-list-helper.h"   
      
NS_LOG_COMPONENT_DEFINE ("ServiceListHelper");

namespace ns3 {

ServiceListHelper::ServiceListHelper()
{
}

ServiceListHelper::~ServiceListHelper()
{ 
}


ServiceListHelper* 
ServiceListHelper::Copy (void) const 
{
  return new ServiceListHelper (*this); 
}

void 
ServiceListHelper::Add (ApplicationHelper *servicehelper, std::string service_type)
{ 
  m_app_helper_list.insert (std::make_pair(service_type, servicehelper));  
}

ServiceList* 
ServiceListHelper::Install (Ptr<Node> node)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][ServiceListHelper] install the list of services contained in the service list helper on node " << node->GetId() <<"--------"<<"\n"); 

  ServiceList *slist = new ServiceList();
  ApplicationContainer AppCont;  
  AppStringList appstringlist;
  for(ApplicationHelperList::const_iterator it = m_app_helper_list.begin(); it != m_app_helper_list.end(); it++)
    {
      ApplicationHelper *apphelper = it->second;
      std::string apptype = it->first;
      apphelper->SetApplicationType (apptype);
      ApplicationContainer thisAppCont = apphelper->Install (node);
      AppCont.Add(thisAppCont);  
      appstringlist.push_back (apptype); 
    }

  slist->AddServices(AppCont, appstringlist); 

  NS_LOG_INFO("[ns3][ServiceListHelper]the service list of this node contains " << slist->GetNServices() <<" services--------"<<"\n");
      
  return slist;
}

void
ServiceListHelper::AddServices (Ptr<Node> node, ServiceList *slist)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][ServiceListHelper] add the list of services contained in the service list helper on node " << node->GetId() <<"--------"<<"\n"); 
  ApplicationContainer AppCont;  
  AppStringList appstringlist;
  for(ApplicationHelperList::const_iterator it = m_app_helper_list.begin(); it != m_app_helper_list.end(); it++)
    {
      ApplicationHelper *apphelper = it->second;
      std::string apptype = it->first;
      apphelper->SetApplicationType (apptype);
      ApplicationContainer thisAppCont = apphelper->Install (node);
      AppCont.Add(thisAppCont);  // Install returns an applicationcontainer holding the pointer to the newly installed app.This nodecontainer is everytime appended to the applicationcontainer of the previously created applications.
      appstringlist.push_back (apptype); 
    }
  slist->AddServices(AppCont, appstringlist); 
  NS_LOG_INFO("[ns3][ServiceListHelper]the service list of this node contains " << slist->GetNServices() <<" services--------"<<"\n");   
}

} // namespace ns3
