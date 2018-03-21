/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
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
 * Author: Sendoa Vaz
 */

#ifndef UMTS_APP_HELPER_H
#define UMTS_APP_HELPER_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h" 
#include "ns3/application-helper.h"
#include "ns3/net-device.h"

namespace ns3 {

/**

 * This class creates one or multiple instances of ns3::UMTS-App and associates
 * it/them to one/multiple node(s).
 *
 * @author 
 *
 */
class UMTSAppHelper :public ApplicationHelper
{
public:
  /**
   * Create a UMTSAppHelper which is used to make life easier for people wanting
   * to use UMTS.
   */
  UMTSAppHelper ();
  
  virtual ~UMTSAppHelper(); // added by Andrea

  void SetAttribute (std::string name, const AttributeValue &value);
  
  void SetApplicationNodeType(std::string umtsNodeType);

  /**
   * Install UMTS-App on each Node in the provided NodeContainer.
   *
   * \param nodes The NodeContainer containing all of the nodes on which the UMTS-App has to be installed.
   *
   * \returns A list of UMTS-App, one for each input node
   */
 ApplicationContainer Install (NodeContainer nodes) const;

  /**
   * Install UMTS-App on the provided Node.  The Node is specified
   * directly by a Ptr<Node>
   *
   * \param node The node to install UMTS on.
   *
   * \returns An ApplicationContainer holding the UMTS manager created.
   */
 ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install UMTS-App on the provided Node.  The Node is specified
   * by a string that must have previously been associated with a Node using the
   * Object Name Service.
   *
   * \param nodeName The node to install UMTS on.
   *
   * \returns An ApplicationContainer holding the UMTS created.
   */
  ApplicationContainer Install (std::string nodeName) const;

private:
  /**
   * \internal
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  ObjectFactory m_factory;
  std::string m_protocol;
  std::string m_nodeType;
};

} // namespace ns3

#endif /* UMTS_APP_HELPER */
