/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
 * Author:  Sendoa Vaz
 */

#ifndef LTE_APP_HELPER_H
#define LTE_APP_HELPER_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/object-factory.h"
#include "ns3/application-helper.h"
#include "ns3/net-device.h"

namespace ns3 {

/**

 * This class creates one or multiple instances of ns3::LTE-App and associates
 * it/them to one/multiple node(s).
 *
 */
class LTEAppHelper : public ApplicationHelper {
public:
    /**
     * Create a LTEAppHelper which is used to make life easier for people wanting to use LTE.
     */
    LTEAppHelper();

    virtual ~LTEAppHelper();

    void SetAttribute(std::string name, const AttributeValue& value);

    void SetApplicationNodeType(std::string lteNodeType);

    /**
     * Install LTE-App on each Node in the provided NodeContainer.
     *
     * \param nodes The NodeContainer containing all of the nodes on which the LTE-App has to be installed.
     *
     * \returns A list of LTE-App, one for each input node
     */
    ApplicationContainer Install(NodeContainer nodes) const;

    /**
     * Install LTE-App on the provided Node.  The Node is specified
     * directly by a Ptr<Node>
     *
     * \param node The node to install LTE on.
     *
     * \returns An ApplicationContainer holding the LTE manager created.
     */
    ApplicationContainer Install(Ptr<Node> node) const;

    /**
     * Install LTE-App on the provided Node.  The Node is specified
     * by a string that must have previously been associated with a Node using the
     * Object Name Service.
     *
     * \param nodeName The node to install LTE on.
     *
     * \returns An ApplicationContainer holding the LTE created.
     */

    ApplicationContainer Install(std::string nodeName) const;

private:

    Ptr<Application> InstallPriv(Ptr<Node> node) const;
    ObjectFactory m_factory;
    std::string m_protocol;

    std::string m_nodeType;
};

}

#endif

