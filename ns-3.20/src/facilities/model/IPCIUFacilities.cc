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
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/****************************************************************************************
 * Edited by Jin Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * Added LTE support
***************************************************************************************/


#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/service-list.h"
#include "ns3/service-management.h"
#include "ns3/ipv4-address.h"
#include "ns3/itetris-types.h"
#include "ns3/addressing-support.h"
#include "ns3/simulator.h"

#include "IPCIUFacilities.h"

NS_LOG_COMPONENT_DEFINE ("IPCIUFacilities");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED (IPCIUFacilities);

	TypeId IPCIUFacilities::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::IPCIUFacilities").SetParent<Object>().AddConstructor<IPCIUFacilities>();
		return tid;
	}

	IPCIUFacilities::IPCIUFacilities()
	{
	}

	IPCIUFacilities::~IPCIUFacilities()
	{
	}

	void IPCIUFacilities::DoDispose(void)
	{
		NS_LOG_FUNCTION(this);
		m_node = 0;
		Object::DoDispose();
	}

	void IPCIUFacilities::SetNode(Ptr<Node> node)
	{
		NS_LOG_FUNCTION(this);
		m_node = node;
	}

	void IPCIUFacilities::NotifyNewAggregate()
	{
		if (m_node == 0)
		{
			Ptr<Node> node = this->GetObject<Node>();
			if (node != 0)
			{
				this->SetNode(node);
			}
		}
		Object::NotifyNewAggregate();
	}

	void IPCIUFacilities::InitiateIPBasedTxon(std::string ServiceID, uint32_t destination, double frequency,
			uint32_t packetSize, double MessRegenerationTime, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
		Ipv4Address* IPaddress = m_AddressingSupport->getIPaddress(destination);
		NS_LOG_LOGIC(
				"[ns-3][IPCIUFacilities] at " << Simulator::Now() << " on node " << m_node->GetId()
						<< " InitiateIPBasedTxon to node " << destination);
		if (IPaddress != NULL)
		{
			NS_LOG_LOGIC(
					"[ns-3][IPCIUFacilities] on node " << m_node->GetId() << " Ip address found for destination nodeId "
							<< destination << ". IP = " << *IPaddress);
		m_ServiceManagement->ActivateIPService(ServiceID, *IPaddress, frequency, MessRegenerationTime, packetSize, messageId);
	} else
	{
		NS_LOG_LOGIC(
				"[ns-3][IPCIUFacilities] on node " << m_node->GetId() << " Ip address not found for destination nodeId "
						<< destination << ". The node is probably out of coverage.");
	}
}

void IPCIUFacilities::DeactivateServiceTxon(std::string serviceId)
{
	m_ServiceManagement->DeactivateService(serviceId);
}

void IPCIUFacilities::SetAddressingSupport(Ptr<AddressingSupport> AddrSupp)
{
	m_AddressingSupport = AddrSupp;
}

void IPCIUFacilities::SetServiceManagement(Ptr<ServiceManagement> ServMng)
{
	m_ServiceManagement = ServMng;
}

} //namespace ns3
