/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
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
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/channel-tag.h"
#include "ns3/icmpv6-l4-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-interface.h"
#include "ns3/mobility-model.h"
#include <stdlib.h>
#include "ns3/storage.h"
#include "ns3/itetris-types.h"
#include "iTETRISns3Facilities.h"
#include <iostream>
#include <fstream>

NS_LOG_COMPONENT_DEFINE ("iTETRISns3Facilities");

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED (iTETRISns3Facilities);

	TypeId iTETRISns3Facilities::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::iTETRISns3Facilities").SetParent<Object>().AddConstructor<iTETRISns3Facilities>();
		return tid;
	}

	iTETRISns3Facilities::iTETRISns3Facilities()
	{
	}

	iTETRISns3Facilities::~iTETRISns3Facilities()
	{
	}

	void iTETRISns3Facilities::DoDispose(void)
	{
		NS_LOG_FUNCTION(this);
		m_node = 0;
		Object::DoDispose();
	}

	void iTETRISns3Facilities::SetNode(Ptr<Node> node)
	{
		NS_LOG_FUNCTION(this);
		m_node = node;
	}

	void iTETRISns3Facilities::NotifyNewAggregate()
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

	void iTETRISns3Facilities::InitiateIdBasedTxon(std::string ServiceID, uint32_t commProfile,
			TechnologyList technologies, uint32_t destination, double frequency, uint32_t packetSize,
			double MessRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
		NS_LOG_LOGIC(
				"[ns-3][iTETRISns-3Facilities] at " << Simulator::Now() << " on node " << m_node->GetId()
						<< " InitiateIDBasedTxon to node " << destination);

		NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] decoding Generic Container");

		tcpip::Storage tmpStore;
		tmpStore.writePacket(genericContainer.data(), genericContainer.size());

		NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] container_l is: " << genericContainer.size());

		if (genericContainer.size() > 0)
		{
			unsigned char type = tmpStore.readUnsignedByte();
			NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] type : " << (int) type);
			if (type == TYPE_TOPO)
			{
				NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] is TOPO: ");
				short num_fields = tmpStore.readShort();
				NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] number of fields: " << num_fields);
				for (int i = 0; i < num_fields; i++)
				{
					unsigned char field = tmpStore.readUnsignedByte();
					switch (field)
					{
					case VALUE__POS_X:
					{
						float pos_X = tmpStore.readFloat();
						NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] X_pos is: " << pos_X);
						break;
					}
					case VALUE__POS_Y:
					{
						float pos_Y = tmpStore.readFloat();
						NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] Y_pos is: " << pos_Y);
						break;
					}
					default:
					{
						NS_LOG_LOGIC("[ns-3][iTETRISns3Facilities] unsupported field " << field);
						break;
					}
					}
				}
			}
		}


		stacktodestination* stacktodest;
		stacktodest = (stacktodestination*) malloc(sizeof(stacktodestination));

		// Only OBU may have a techno-selector - RSU can only use IEEE 802.11p
		if (!m_node->IsMobileNode())
		{
			Ptr<c2cAddress> C2Caddress = m_AddressingSupport->getC2Caddress(destination);
			if (C2Caddress != NULL)
				m_ServiceManagement->ActivateC2CService(ServiceID, C2Caddress, frequency, MessRegenerationTime, msgLifetime,
						packetSize, messageId);
			return;
		}

		if (commProfile != 0)
		{
			if (destination == TMC_CONSTANT) // the TMC has to be reached through one of the communication technologies included in technologylist
			{
				stacktodest = m_LocalCOMMchSelector->GetCommunicationChTMC(commProfile, technologies);
			} else
			{
				stacktodest = m_LocalCOMMchSelector->GetCommunicationCh(commProfile, technologies);
				stacktodest->destination = destination;
			}
		}


		//Here we will probably have to add an additional communication profile. Right now
		//what we have is: For communication profile = 0 and Umts technology only IPv6 is
		//provided. For CP=0 and Wave technology only C2C is provided and for the rest
		//of technologies only IPv4
		else
		{
			if (destination == TMC_CONSTANT) // the TMC has to be reached through one of the communication technologies included in technologylist
			{
				stacktodest->tech = (char*) malloc(1 * sizeof(char));
				stacktodest->tech = (char*) "";
				stacktodest->stack = C2C;
				stacktodest->destination = ID_BROADCAST;
			} else
			{
				if (technologies.size() == 1)
				{
					if ((technologies.front() == "WaveVehicle") || (technologies.front() == "WaveRsu"))
					{
						stacktodest->tech = (char*) malloc(1 * sizeof(char));
						stacktodest->tech = (char*) "";
						stacktodest->stack = C2C;
						stacktodest->destination = destination;
					}

					else
					{
						stacktodest->stack = IPv4;
						stacktodest->destination = destination;
						if ((technologies.front() == "UmtsVehicle"))
						{
							stacktodest->tech = (char*) malloc(5 * sizeof(char));
							stacktodest->tech = (char*) "UMTS-";
						}
						if ((technologies.front() == "WimaxVehicle"))
						{
							stacktodest->tech = (char*) malloc(6 * sizeof(char));
							stacktodest->tech = (char*) "Wimax-";
						}
						if ((technologies.front() == "LteVehicle"))
						{
							stacktodest->tech = (char*) malloc(4 * sizeof(char));
							stacktodest->tech = (char*) "Lte-";
						}

						if ((technologies.front() == "UmtsBs"))
						{
							NS_FATAL_ERROR(
									"wrong use of the function InitiateIdBasedTxon for node " << m_node->GetId()
											<< ": a txon on a UMTS BS has to be called through the IPCIUFacilities class");
							return;
						}
						if ((technologies.front() == "DvbhVehicle"))
						{
							NS_FATAL_ERROR(
									"wrong use of the function InitiateIdBasedTxon for node " << m_node->GetId()
											<< ": DVB-H uplink txon on a vehicle is not supported");
							return;
						}
						if ((technologies.front() == "DvbhBs"))
						{
							NS_FATAL_ERROR(
									"wrong use of the function InitiateIdBasedTxon for node " << m_node->GetId()
											<< ": a txon on a DVB-H BS has to be called through the IPCIUFacilities class");
							return;
						}

                        if ((technologies.front() == "LteBs"))
						{
							NS_FATAL_ERROR(
									"wrong use of the function InitiateIdBasedTxon for node " << m_node->GetId()
											<< ": a txon on a LTE BS has to be called through the IPCIUFacilities class");
							return;
						}


						if ((technologies.front() == "WimaxBs"))
						{
							NS_FATAL_ERROR(
									"wrong use of the function InitiateIdBasedTxon for node " << m_node->GetId()
											<< ": a txon on a WiMAX BS has to be called through the IPCIUFacilities class");
							return;
						}
					}
				} else
				{
					NS_FATAL_ERROR(
							"ambiguous call to function InitiateIdBasedTxon for node " << m_node->GetId()
									<< ": too many RATs as arguments");
					return;
				}
			}
		}

		// now that  the addressed node is unambiguous, the AddressingSupport is called to retrieve its address (C2C or IP) according to the destination stack. after that, the transmission is triggered
		if (stacktodest != NULL)
		{
			if (stacktodest->stack == IPv4)
			{
				Ipv4Address* IPaddress = m_AddressingSupport->getIPaddress(stacktodest->destination);
				if (IPaddress != NULL)
				{
					NS_LOG_LOGIC(
							"[ns-3][iTETRISns3Facilities] on node " << m_node->GetId() << " Ip address found for destination node "
									<< destination);
					m_ServiceManagement->ActivateIPService(stacktodest->tech + ServiceID, *IPaddress, frequency,
							MessRegenerationTime, packetSize, messageId);
				} else
				{
					NS_LOG_LOGIC(
							"[ns-3][iTETRISns3Facilities] on node " << m_node->GetId()
									<< " Ip address not found for destination node " << destination
									<< ". The node is probably out of coverage.");
				}
			} else if (stacktodest->stack == IPv6)
			{
				Ipv6Address* IPaddress = m_AddressingSupport->getIPv6address(stacktodest->destination);
   			    Ptr<Ipv6Interface> interface = m_node->GetObject<Ipv6Interface> ();
				/*Simulator::Schedule (Seconds (0.), &Icmpv6L4Protocol::DoDAD, icmpv6, interface->GetLinkLocalAddress().GetAddress(), interface);*/
					if (IPaddress != NULL)
				{
					NS_LOG_LOGIC(
							"[ns-3][iTETRISns3Facilities] on node " << m_node->GetId() << " Ipv6 address found for destination node "
									<< destination);
					m_ServiceManagement->ActivateIPv6Service(stacktodest->tech + ServiceID, *IPaddress, frequency,
							MessRegenerationTime, packetSize, messageId);
				} else
				{
					NS_LOG_LOGIC(
							"[ns-3][iTETRISns3Facilities] on node " << m_node->GetId()
									<< " Ip address not found for destination node " << destination
									<< ". The node is probably out of coverage.");
				}
			} else
			{

				Ptr<c2cAddress> C2Caddress = m_AddressingSupport->getC2Caddress(stacktodest->destination);
				if (C2Caddress != NULL)
					m_ServiceManagement->ActivateC2CService(stacktodest->tech + ServiceID, C2Caddress, frequency,
                            MessRegenerationTime, msgLifetime, packetSize/*, ch_tag*/, messageId);
			}
		}
	}

	void iTETRISns3Facilities::InitiateGeoBroadcastTxon(std::string serviceId, uint32_t commProfile,
			TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
			double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
	    if (commProfile == 0)
		{
			NS_LOG_LOGIC(
					"iTETRISns3Facilities::InitiateGeoBroadcastTxon Lat=" << destination.lat << " Lon=" << destination.lon
							<< " Area=" << destination.areaSize);
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CGeoBroadcastAddress(destination);
			m_ServiceManagement->ActivateC2CService(serviceId, c2caddress, frequency, msgRegenerationTime, msgLifetime,
					packetSize, messageId);
		} else
		{
			NS_LOG_LOGIC(
					"iTETRISns3Facilities::InitiateGeoBroadcastTxon Using TechnoSelector: Communication Profile " << commProfile);
			stacktodestination* stacktodest;
			stacktodest = m_LocalCOMMchSelector->GetCommunicationCh(/*serviceId, commProfile*/4, technologies); //GeoBroadcast => GP_WAVE => C2C + WAVE
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CGeoBroadcastAddress(destination);
			m_ServiceManagement->ActivateC2CService((stacktodest->tech) + serviceId, c2caddress, frequency,
					msgRegenerationTime, msgLifetime, packetSize, messageId);
		}
	}

	void iTETRISns3Facilities::InitiateGeoAnycastTxon(std::string serviceId, uint32_t commProfile,
			TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
			double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{
		if (commProfile == 0)
		{
			NS_LOG_LOGIC(
					"iTETRISns3Facilities::InitiateGeoAnycastTxon Lat=" << destination.lat << " Lon=" << destination.lon
							<< " Area=" << destination.areaSize);
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CGeoAnycastAddress(destination);
			m_ServiceManagement->ActivateC2CService(serviceId, c2caddress, frequency, msgRegenerationTime, msgLifetime,
					packetSize, messageId);
		} else
		{
			NS_LOG_LOGIC(
					"iTETRISns3Facilities::InitiateGeoAnycastTxon Using TechnoSelector: Communication Profile " << commProfile);
			stacktodestination* stacktodest;
			stacktodest = m_LocalCOMMchSelector->GetCommunicationCh(/*serviceId, commProfile*/4, technologies);
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CGeoAnycastAddress(destination);
			m_ServiceManagement->ActivateC2CService((stacktodest->tech) + serviceId, c2caddress, frequency,
					msgRegenerationTime, msgLifetime, packetSize, messageId);
		}

	}

	void iTETRISns3Facilities::InitiateTopoBroadcastTxon(std::string serviceId, uint32_t commProfile,
			TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime,
			uint8_t msgLifetime, uint32_t numHops, uint32_t messageId, std::vector<unsigned char> genericContainer)
	{

		if (commProfile == 0)
		{
			NS_LOG_LOGIC("iTETRISns3Facilities::InitiateTopoBroadcastTxon Number of Hops =" << numHops);
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CTopoBroadcastAddress(numHops);
			m_ServiceManagement->ActivateC2CService(serviceId, c2caddress, frequency, msgRegenerationTime, msgLifetime,
					packetSize, messageId);
		} else
		{
			NS_LOG_LOGIC(
					"iTETRISns3Facilities::InitiateTopoBroadcastTxon Using TechnoSelector: Communication Profile "
							<< commProfile);
			stacktodestination* stacktodest;
			stacktodest = m_LocalCOMMchSelector->GetCommunicationCh(/*serviceId, commProfile*/4, technologies);
			Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CTopoBroadcastAddress(numHops);
			m_ServiceManagement->ActivateC2CService((stacktodest->tech) + serviceId, c2caddress, frequency,
					msgRegenerationTime, msgLifetime, packetSize, messageId);
		}
	}

	void iTETRISns3Facilities::ActivateCamTxon(double frequency, uint32_t packetSize,
			uint32_t messageId/*, ChannelTag ch_tag*/)
	{
		Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CTopoBroadcastAddress(1);
		m_ServiceManagement->ActivateC2CService("CAM", c2caddress, frequency, packetSize, messageId/*, ch_tag*/);
	}

	void iTETRISns3Facilities::DeactivateCamTxon()
	{
		m_ServiceManagement->DeactivateService("CAM");
	}

	void iTETRISns3Facilities::ActivateDenmTxon(CircularGeoAddress destination, double frequency, uint32_t packetSize,
			double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId/*, ChannelTag ch_tag*/)
	{
		Ptr<c2cAddress> c2caddress = m_AddressingSupport->getC2CGeoBroadcastAddress(destination);
		m_ServiceManagement->ActivateC2CService("DENM", c2caddress, frequency, msgRegenerationTime, msgLifetime, packetSize,
				messageId/*, ch_tag*/);
	}

	void iTETRISns3Facilities::DeactivateDenmTxon()
	{
		m_ServiceManagement->DeactivateService("DENM");
	}

	void iTETRISns3Facilities::DeactivateServiceTxon(std::string serviceId)
	{
		m_ServiceManagement->DeactivateService(serviceId);
	}

	void iTETRISns3Facilities::SetAddressingSupport(Ptr<AddressingSupport> AddrSupp)
	{
		m_AddressingSupport = AddrSupp;
	}

	void iTETRISns3Facilities::SetServiceManagement(Ptr<ServiceManagement> ServMng)
	{
		m_ServiceManagement = ServMng;
	}

	void iTETRISns3Facilities::SetLocalCommCHSelector(Ptr<LocalCOMMchSelector> localCHsel)
	{
		m_LocalCOMMchSelector = localCHsel;
	}

} //namespace ns3
