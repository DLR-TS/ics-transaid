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

#ifndef SERVICE_LIST_HELPER_H
#define SERVICE_LIST_HELPER_H

#include <stdint.h>
#include "ns3/service-list.h"   
#include "ns3/node.h"
#include "ns3/application-helper.h"
#include <list>
#include <map>


namespace ns3 {

/**
 * \brief Helper class that adds ns3::ServiceList objects and installs list of services  at nodes
 *
 */
class ServiceListHelper
{
public:
   /**
   * Construct a ServiceListHelper 
   */
  ServiceListHelper ();

   /**
   * \internal
   * Destroy a ServiceListHelper
   */
  virtual ~ServiceListHelper ();

  /**
   * \returns  a pointer to a copy of this ServiceListHelper 
   */
  ServiceListHelper* Copy (void) const;

  /**
   * \param servicehelper an application helper
   * \param service_type the type of the application that is going to be installed
   *
   * Store in the internal list a reference to the service helper. These helpers will be used later by
   * the ns3::ServiceListHelper::Install method to create
   * an ns3::ServiceList object and install every service of this list on the node.
   */
  void Add (ApplicationHelper *servicehelper, std::string service_type);

  /**
   * \param node the node to install an application or a list of applications (services) on
   * \returns a newly-created (and installed) application or list of applications
   */
  ServiceList* Install (Ptr<Node> node);

  void AddServices (Ptr<Node> node, ServiceList *slist); 

private:

  typedef std::map< std::string, ApplicationHelper* > ApplicationHelperList;
  ApplicationHelperList m_app_helper_list;

};

} // namespace ns3

#endif /* SERVICE_LIST_HELPER_H */
