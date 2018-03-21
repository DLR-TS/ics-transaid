/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez,
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
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
 * Edited by Jian Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * Added LTE support
***************************************************************************************/

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/service-management.h"
#include "ns3/service-list.h"
#include "ns3/itetris-types.h"
#include "ns3/ipv4-address.h"
#include "ns3/addressing-support.h"
#include "ns3/c2c-address.h"
#include "ns3/mw-comm-ch-selector.h"
#include "ns3/node-container.h"
#include "ns3/simulator.h"
#include "ns3/storage.h"
//#include "applications/src/foreign/tcpip/storage.h"

#include "MWFacilities.h"

NS_LOG_COMPONENT_DEFINE ("MWFacilities");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED (MWFacilities);

	TypeId MWFacilities::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::MWFacilities").SetParent<Object>().AddConstructor<MWFacilities>();
		return tid;
	}

	MWFacilities::MWFacilities()
	{
	}

	MWFacilities::~MWFacilities()
	{
	}

	void MWFacilities::DoDispose(void)
	{
		NS_LOG_FUNCTION(this);
		m_node = 0;
		Object::DoDispose();
	}

	void MWFacilities::SetNode(Ptr<Node> node)
	{
		NS_LOG_FUNCTION(this);
		m_node = node;
	}

	void MWFacilities::NotifyNewAggregate()
	{
		if (m_node == 0)
		{
			Ptr<Node> node = this->GetObject<Node>();
			// verify that it's a valid node and that
			// the node has not been set before
			if (node != 0)
			{
				this->SetNode(node);
			}
		}
		Object::NotifyNewAggregate();
	}

//Used for MW-Based Unicast Txons.
	void MWFacilities::InitiateMWIdBasedTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
			double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msglifetime, uint32_t destid,
			uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
		std::cout << "MWFacilities::InitiateMWIdBasedTxon 1, size: " << genericContainer.size() << std::endl;
		if (genericContainer.size() != 0)
		{
			//first convert the vector to storage
			tcpip::Storage ts(genericContainer.data(), genericContainer.size());

			CircularGeoAddress destination;

			//Extract number of fields
			unsigned short numFields = (unsigned short) ts.readShort();
			for (unsigned short i = 0; i < numFields; i++)
			{
				//Extract type of fields
				unsigned char field = ts.readChar();
				//i++;
				switch (field)
				{
				case VALUE__POS_X:
				{
					uint32_t x = (uint32_t) ts.readInt();
					destination.lat = x;
					break;
				}
				case VALUE__POS_Y:
				{
					uint32_t y = (uint32_t) ts.readInt();
					destination.lon = y;
					break;
				}
				}
			}
			destination.areaSize = 0;
			m_disseminationProfile = m_MWCOMMchSelector->GetDisseminationProfile(commProfile, destination, technologies);
			std::cout << "MWFacilities::InitiateMWIdBasedTxon destination's position, x:" << destination.lat << ", y:"
					<< destination.lon << std::endl;
		}
		else
		{
			m_disseminationProfile = m_MWCOMMchSelector->GetUnicastDisseminationProfile(commProfile, destid,
					technologies/*, stack*/);
		}

		Ptr<Node> disseminator = m_disseminationProfile.disseminator;

		NS_LOG_LOGIC(
				"[ns-3][MWFacilities] Disseminator node: " << m_node->GetId() << " at time: " << Simulator::Now().GetSeconds()
						<< "seconds");
		if (disseminator != NULL)
		{
			Ptr<ServiceManagement> servMngmt = disseminator->GetObject<ServiceManagement>();
			Ptr<AddressingSupport> addrsupp = disseminator->GetObject<AddressingSupport>();
			// if the selected disseminator belongs to the IP-based base stations, then retrieve the adequate IP address for disseminating (broadcast)
			if (m_disseminationProfile.stack == IPv4)
			{
				Ipv4Address* IPaddress = addrsupp->getIPaddress(destid);
				// finally activate the service on the selected IP basestation
				NS_ASSERT_MSG(IPaddress, "MWFacilities::InitiateMWGeoBasedTxon -> IP address not found.");
				servMngmt->ActivateIPService(/*m_disseminationProfile.tech+"-"+*/serviceId, *IPaddress, frequency,
						msgRegenerationTime, packetSize, messageId);
			}
			if (m_disseminationProfile.stack == IPv6)
			{
				Ipv6Address* IPaddress = addrsupp->getIPv6address(destid);
				// finally activate the service on the selected IP basestation
				NS_ASSERT_MSG(IPaddress, "MWFacilities::InitiateMWGeoBasedTxon -> IPv6 address not found.");
				servMngmt->ActivateIPv6Service(/*m_disseminationProfile.tech+"-"+*/serviceId, *IPaddress, frequency,
						msgRegenerationTime, packetSize, messageId);
			}
			// if the selected disseminator belongs to the C2C-based base stations (an RSU), then retrieve the adequate c2c address
			if (m_disseminationProfile.stack == C2C)
			{
				Ptr<c2cAddress> c2caddress = addrsupp->getC2Caddress(destid);
				// finally activate the service on the selected c2c base station
				NS_ASSERT_MSG(c2caddress, "MWFacilities::InitiateMWGeoBasedTxon -> C2C address not found.");
				servMngmt->ActivateC2CService(/*m_disseminationProfile.tech+"-"+*/serviceId, c2caddress, frequency,
						msgRegenerationTime, msglifetime, packetSize, messageId);
			}
		}
	}

	void MWFacilities::InitiateMWGeoBasedTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
			CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime,
			uint8_t msglifetime, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
		// retrieve the dissemination profile through the MW communication channel selector
		m_disseminationProfile = m_MWCOMMchSelector->GetDisseminationProfile(commProfile, destination,
				technologies/*, stack*/);
		Ptr<Node> disseminator = m_disseminationProfile.disseminator;

		NS_LOG_LOGIC(
				"[ns-3][MWFacilities] Disseminator node: " << m_node->GetId() << " at time: " << Simulator::Now().GetSeconds()
						<< "seconds");
		//std::cout<<"MWFacilities::InitiateMWGeoBasedTxon Dissemination Profile:"<<m_disseminationProfile.stack<<std::endl;
		if (disseminator != NULL)
		{
			Ptr<ServiceManagement> servMngmt = disseminator->GetObject<ServiceManagement>();
			Ptr<AddressingSupport> addrsupp = disseminator->GetObject<AddressingSupport>();
			// if the selected disseminator belongs to the IP-based base stations, then retrieve the adequate IP address for disseminating (broadcast)
			if (m_disseminationProfile.stack == IPv4)
			{
				Ipv4Address* IPaddress = addrsupp->getIPaddress(ID_BROADCAST);
				// finally activate the service on the selected IP basestation
				NS_ASSERT_MSG(IPaddress, "MWFacilities::InitiateMWGeoBasedTxon -> IP address not found.");
				servMngmt->ActivateIPService(/*m_disseminationProfile.tech+"-"+*/serviceId, *IPaddress, frequency,
						msgRegenerationTime, packetSize, messageId);
			}
			if (m_disseminationProfile.stack == IPv6)
			{
				Ipv6Address* IPaddress = addrsupp->getIPv6address(ID_MULTICAST);
				// finally activate the service on the selected IP basestation
				NS_ASSERT_MSG(IPaddress, "MWFacilities::InitiateMWGeoBasedTxon -> IP address not found.");
				servMngmt->ActivateIPv6Service(/*m_disseminationProfile.tech+"-"+*/serviceId, *IPaddress, frequency,
						msgRegenerationTime, packetSize, messageId);
			}
			// if the selected disseminator belongs to the C2C-based base stations (an RSU), then retrieve the adequate c2c address
			if (m_disseminationProfile.stack == C2C)
			{
				Ptr<c2cAddress> c2caddress = addrsupp->getC2CGeoBroadcastAddress(destination);
				// finally activate the service on the selected c2c base station
				NS_ASSERT_MSG(c2caddress, "MWFacilities::InitiateMWGeoBasedTxon -> C2C address not found.");
				servMngmt->ActivateC2CService(/*m_disseminationProfile.tech+"-"+*/serviceId, c2caddress, frequency,
						msgRegenerationTime, msglifetime, packetSize, messageId);
			}
		}
	}

	void MWFacilities::DeactivateServiceTxon(std::string serviceId)
	{
		Ptr<Node> disseminator = m_disseminationProfile.disseminator;
		Ptr<ServiceManagement> servMngmt = disseminator->GetObject<ServiceManagement>();
		servMngmt->DeactivateService(serviceId);
	}

	void MWFacilities::SetMWCommCHSelector(Ptr<MWCOMMchSelector> MWCHsel)
	{
		m_MWCOMMchSelector = MWCHsel;
	}

	void MWFacilities::AddInfrastructureTechNode(Ptr<Node> node, std::string typeOfModule)
	{
		NodeContainerList::iterator iterCommModule = m_infranodes.find(typeOfModule);
		if (iterCommModule != m_infranodes.end())
		{
			NodeContainer* container = iterCommModule->second;
			container->Add(node);
		} else
		{
			NodeContainer* newContainer = new NodeContainer(node);
			m_infranodes.insert(std::make_pair(typeOfModule, newContainer));
		}
	}

	NodeContainer*
	MWFacilities::getInfrastructureTechNodes(std::string typeOfModule)
	{
		NodeContainerList::iterator iterCommModule = m_infranodes.find(typeOfModule);
		if (iterCommModule != m_infranodes.end())
		{
			return (iterCommModule->second);
		} else
		{
			return (NULL);
		}
	}

} //namespace ns3
