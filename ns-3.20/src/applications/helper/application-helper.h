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


/**
 * \ingroup applicationshelper
 * \defgroup applicationshelper
 *
 * iTETRIS helper to install applications a node
 *
*/

/**
 * \ingroup applicationshelper
 *
 * \brief a helper to install and configure an application service on a  node
 *
 * @author Michele Rondinone <mrondinone@umh.es>
 *
  */

#ifndef APPLICATION_HELPER_H
#define APPLICATION_HELPER_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"
#include "ns3/net-device.h"


namespace ns3 {

/**
 * Abstract class for the implementation of derived specific application helpers
 *
 */
class ApplicationHelper
{
public:
  /**
   * Create a basic application helper
   */
  ApplicationHelper ();

  void SetApplicationType (std::string apptype);


  /**
   * Install an application to all the nodes in the input NodeContainer.
   *
   * \param nodes The NodeContainer containing the nodes where the applications are going to be installed.
   *
   * \returns an application container containing the pointer to the applications installed on the nodes of the input node container
   */
  virtual ApplicationContainer Install (NodeContainer nodes) const=0;

  /**
   * Install an application on the input Node
   *
   * \param node The node where the application is going to be installed
   *
   * \returns An ApplicationContainer containing the pointer to the newly created application.
   */
  virtual ApplicationContainer Install (Ptr<Node> node) const =0;

protected:

  std::string m_apptype;

};

} // namespace ns3

#endif /* APPLICATION_HELPER_H */
