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


#ifndef SERVICE_LIST_H
#define SERVICE_LIST_H

#include <map>
#include <list>


#include "ns3/object.h"
#include "ns3/application-container.h"

namespace ns3 {

  typedef std::list<std::string> AppStringList;
  typedef std::map< std::string, uint32_t> ServiceCorrespondaceList;

/**
 * This class is  used to manage a list of services (iTETRISApplications) installed on a node
 */
class ServiceList
{
public:

  ServiceList ();
  virtual ~ServiceList ();

  /**
   * Add one or more newly created and installed applications to the service list and
   * update the list that matches the string representing a given service with the corresponding index in the application container
   *
   * \param appcont the application container holding the pointer(s) to the newly installed application(s)
   */
   void AddServices (ApplicationContainer appcont, AppStringList appstringlist);

  /**
   * Add a newly created and installed application to the service list and
   * update the list that matches the string representing a given service with the corresponding index in the application container
   *
   * \param appcont the application container holding the pointer to the newly installed application(s)
   */
   void AddServices (ApplicationContainer appcont, std::string appstring);

  /**
   * \return number of services in the list
   */
   uint32_t GetNServices (void);

  /**
   * Return a pointer to the service stored at index
   * 
   * \return a pointer to the service 
   * \param index the index of the service to return
   */
   Ptr<Application> GetService (uint32_t index);

  /**
   * Return a pointer to the service of type servicetype
   * 
   * \return a pointer to the service 
   * \param servicetype the type of the service to return
   */
   Ptr<Application> GetService (std::string servicetype);


  /**
   * Return the index of the service of type servicetype in the servicelist
   * 
   * \return the index of the service in the servicelist 
   * \param servicetype the type of the service 
   */
   uint32_t GetServiceIndex (std::string servicetype);

  /**
   * Return the number of services currently present in the servicelist
   * 
   * \return the number of services currently present in the servicelist 
   */
   ApplicationContainer GetAllServices (void);


private:
  uint32_t application_index; 
  ApplicationContainer m_service_container;
  ServiceCorrespondaceList m_service_corr_list;
};

} //namespace ns3

#endif /* SERVICE_LIST_H*/
