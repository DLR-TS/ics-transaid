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

#ifndef CAM_RECV_HELPER_H
#define CAM_RECV_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"

namespace ns3 {

/**
 * \brief iTETRIS [WP600] - A helper to make it easier to instantiate an ns3::CAMrecvApplication
 * on a set of nodes.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */
class CAMrecvHelper
{
public:
  /**
   * Create a CAMrecvHelper to make it easier to work with CAMrecvApplications
   *
   * \param protocol the name of the protocol to use to receive traffic
   *        This string identifies the socket factory type used to create
   *        sockets for the applications.  A typical value would be 
   *        ns3::c2cl4TSocketFactory.
   */
  CAMrecvHelper (std::string protocol);

  /**
   * Helper function used to set the underlying application attributes.
   *
   * \param name the name of the application attribute to set
   * \param value the value of the application attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Install an ns3::CAMrecvApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which a CAMrecvApplication
   * will be installed.
   */
  ApplicationContainer Install (NodeContainer c) const;

  /**
   * Install an ns3::CAMrecvApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param node The node on which a CAMrecvApplication will be installed.
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install an ns3::CAMrecvApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param nodeName The name of the node on which a CAMrecvApplication will be installed.
   */
  ApplicationContainer Install (std::string nodeName) const;

private:
  /**
   * \internal
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  ObjectFactory m_factory;
};

} // namespace ns3

#endif /* CAM_RECV_HELPER_H */
