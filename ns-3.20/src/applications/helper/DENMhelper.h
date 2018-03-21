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
 * Author: Michele Rondinone <mrondinone@umh.es>
 */

/**
 * \ingroup applicationshelper
 * \defgroup denm-helper
 *
 * iTETRIS helper to install a DEN  service on an iTETRIS node
 *
*/

/**
 * \ingroup denm-helper
 *
 * \brief a helper to install and configure a DEN service on an iTETRIS node
 *
 * @author Michele Rondinone <mrondinone@umh.es>
 *
  */


#ifndef DENM_HELPER_H
#define DENM_HELPER_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"
#include "ns3/application-helper.h"
#include "ns3/net-device.h"

namespace ns3 {

/**

 * This class creates one or multiple instances of ns3::DENMApp and associates
 * it/them to one/multiple node(s).
 *
 */
class DENMHelper: public ApplicationHelper
{
public:
  /**
   * Create a DENMHelper which is used to make life easier for people wanting
   * to use DENMAp.
   */
  DENMHelper (std::string SocketFactorytype1);

  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Install DENMApp on each Node in the provided NodeContainer.
   *
   * \param nodes The NodeContainer containing all of the nodes on which the C2C-IP-App has to be installed.
   *
   * \returns A list of DENM-Apps, one for each input node
   */
 ApplicationContainer Install (NodeContainer nodes) const;

  /**
   * Install DENM-App on the provided Node.  The Node is specified
   * directly by a Ptr<Node>
   *
   * \param node The node to install the application on.
   *
   * \returns An ApplicationContainer holding the application created.
   */
 ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install DENM-App on the provided Node.  The Node is specified
   * by a string that must have previously been associated with a Node using the
   * Object Name Service.
   *
   * \param nodeName The node to install the application on.
   *
   * \returns An ApplicationContainer holding the application created.
   */
  ApplicationContainer Install (std::string nodeName) const;

private:
  /**
   * \internal
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  ObjectFactory m_factory;
  std::string m_protocol;
};

} // namespace ns3

#endif /* DENM_HELPER */
