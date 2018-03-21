/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Michele Rondinone <mrondinone@umh.es>, Ramon Bauza <rbauza@umh.es>
 */

/**
 * \ingroup applicationshelper
 * \defgroup cam-manage-helper
 *
 * iTETRIS helper to install a CA service on an iTETRIS node
 *
*/

/**
 * \ingroup cam-manage-helper
 *
 * \brief a helper to install and configure a CA service on an iTETRIS node
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * @author Michele Rondinone <mrondinone@umh.es>
 * @author Ramon Bauza <rbauza@umh.es>
 *
  */


#ifndef CAM_MANAGE_HELPER_H
#define CAM_MANAGE_HELPER_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"
#include "ns3/application-helper.h"
#include "ns3/net-device.h"


namespace ns3 {

/**
 * \brief  iTETRIS - create a CAM Manager and associate it to a node
 *
 * This class creates one or multiple instances of ns3::CAMmanage and associates
 * it/them to one/multiple node(s).
 *
 *
 */
class CAMmanageHelper:  public ApplicationHelper           
{
public:
  /**
   * Create a CAMmanageHelper which is used to make life easier for people wanting
   * to use CAM Manager.
   */
  CAMmanageHelper (std::string protocol);
  CAMmanageHelper ();

  virtual ~CAMmanageHelper();

  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Install CAM on each Node in the provided NodeContainer.
   *
   * \param nodes The NodeContainer containing all of the nodes to get CAM.
   *
   * \returns A list of CAM managers, one for each input node
   */
  ApplicationContainer Install (NodeContainer nodes) const;

  /**
   * Install CAM on the provided Node.  The Node is specified
   * directly by a Ptr<Node>
   *
   * \param node The node to install CAM on.
   *
   * \returns An ApplicationContainer holding the CAM manager created.
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install CAM manager on the provided Node.  The Node is specified
   * by a string that must have previously been associated with a Node using the
   * Object Name Service.
   *
   * \param nodeName The node to install CAM on.
   *
   * \returns An ApplicationContainer holding the CAM created.
   */
  ApplicationContainer Install (std::string nodeName) const;

  void SetProtocol (std::string protocol);

private:
  /**
   * \internal
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  ObjectFactory m_factory;
  std::string m_protocol;
};

} // namespace ns3

#endif /* CAM_MANAGE_HELPER_H */
