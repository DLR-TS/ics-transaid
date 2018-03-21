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
 * Author: Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef PACKET_MANAGER_H
#define PACKET_MANAGER_H

#include "ns3/simulator.h"
#include "ns3/callback.h"
#include "inci-packet.h"

#include "ns3/itetris-types.h"
#include "ns3/MWFacilities.h"
#include "ns3/channel-tag.h"

#include <vector>

namespace ns3
{

	class iTETRISNodeManager;
	class CAMmanage;
	class Packet;
	class iTETRISns3Facilities;
	class IPCIUFacilities;
    // class MWFacilities;

	/**
	 * @class PacketManager
	 * @brief The class PacketManager manages the activation and deactivation of packet transmissions in ns-3. Besides, it allows iNCI to retrive the packets that have been received by a given node thorugh the wireless communication technologies
	 */

	class PacketManager
	{
		public:

			PacketManager(void);
			virtual ~PacketManager();
			void SetNodeManager(iTETRISNodeManager* nodeManager);

			/**
			 * @brief Funtion called from iNCI to activate CAM transmissions
			 */
			void ActivateCamTxon(uint32_t nodeId, float frequency, uint32_t packetSize, uint32_t messageId);

			/**
			 * @brief Funtion called from iNCI to deactivate CAM transmissions
			 */
			void DeactivateCamTxon(uint32_t nodeId);

			/**
			 * @brief Funtion called from iNCI to activate CAM transmissions
			 */
			void ActivateDenmTxon(uint32_t nodeId, CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId);

			/**
			 * @brief Funtion called from iNCI to deactivate CAM transmissions
			 */
			void DeactivateDenmTxon(uint32_t nodeId);

			/**
			 * @brief Get the packets that a given node has received thorugh wireless communications
			 * @param[in] recvNodeId Id of the node from which we want to retrive the received packets
			 * @param[in] inciPacket Reference struct (InciPacket::ReceivedInciPacket) with the data fields of the last packets received
			 * @return Bool indicating whether more packets have been received by the node
			 *
			 */
			bool GetReceivedPacket(uint32_t recvNodeId, struct InciPacket::ReceivedInciPacket &inciPacket);

			/**
			 * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
			 * @param[in] nodeId ID of the node that will transmit the ID-based packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] frequency Frequency of the id-based message transmission
			 * @param[in] packetSize Size of the id-based message payload
			 * @param[in] destination ID of the destination node
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void InitiateIdBasedTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, float frequency, uint32_t packetSize, uint32_t destination,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId,
					std::vector<unsigned char> genericContainer);
			/**
			 * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
			 * @param[in] nodeId ID of the node that will transmit the ID-based packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] frequency Frequency of the id-based message transmission
			 * @param[in] packetSize Size of the id-based message payload
			 * @param[in] destination ID of the destination node
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 */
			void InitiateIdBasedTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, float frequency, uint32_t packetSize, uint32_t destination,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId);

			/**
			 * @brief Activate an IP transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.).
			 * @param[in] nodeId ID of the CIU node that will transmit the IP packet.
			 * @param[in] serviceId ID of the service to be activated.
			 * @param[in] frequency Frequency of the IP message transmission.
			 * @param[in] packetSize Size of the IP message payload.
			 * @param[in] destination ID of the destination node.
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void InitiateIPCIUTxon(uint32_t nodeId, std::string serviceId, float frequency, uint32_t packetSize,
					uint32_t destination, double msgRegenerationTime, uint32_t messageId,
					std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate an IP transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.).
			 * @param[in] nodeId ID of the CIU node that will transmit the IP packet.
			 * @param[in] serviceId ID of the service to be activated.
			 * @param[in] frequency Frequency of the IP message transmission.
			 * @param[in] packetSize Size of the IP message payload.
			 * @param[in] destination ID of the destination node.
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3.
			 */
			void InitiateIPCIUTxon(uint32_t nodeId, std::string serviceId, float frequency, uint32_t packetSize,
					uint32_t destination, double msgRegenerationTime, uint32_t messageId);

			/**
			 * @brief Activate the transmission of a notification message in a geographical area. The selection of the CIU will be done by the MW node
			 * @param[in] nodeId ID of the node that will transmit the packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] destination Geographical area where the packet will be disseminated
			 * @param[in] frequency Frequency of the message transmission
			 * @param[in] packetSize Size of the message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void InitiateMWTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					CircularGeoAddress destination, float frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate the transmission of a notification message in a geographical area. The selection of the CIU will be done by the MW node
			 * @param[in] nodeId ID of the node that will transmit the packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] destination Geographical area where the packet will be disseminated
			 * @param[in] frequency Frequency of the message transmission
			 * @param[in] packetSize Size of the message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 */
			void InitiateMWTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					CircularGeoAddress destination, float frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t messageId);

			/**
			 * @brief Activate the transmission of a unicast message. The selection of the CIU will be done by the MW node based on geographic criteria.
			 * @param[in] nodeId ID of the node that will transmit the packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] destination Geographical area where the packet will be disseminated
			 * @param[in] frequency Frequency of the message transmission
			 * @param[in] packetSize Size of the message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] nodeId ID of the node that will receive the Unicast packet
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */

            void InitiateMWIdTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					float frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime, uint32_t destid,
					uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate the transmission of a unicast message. The selection of the CIU will be done by the MW node based on geographic criteria.
			 * @param[in] nodeId ID of the node that will transmit the packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] technologies List of technologies over which the service can operate
			 * @param[in] destination Geographical area where the packet will be disseminated
			 * @param[in] frequency Frequency of the message transmission
			 * @param[in] packetSize Size of the message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] nodeId ID of the node that will receive the Unicast packet
			 */
			void InitiateMWIdTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					float frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime, uint32_t destid,
					uint32_t messageId);

			//void InitiateMWTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, float frequency, uint32_t packetSize,  double msgRegenerationTime, uint8_t msgLifetime);

			/**
			 * @brief Activate a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
			 * @param[in] nodeId ID of the node that will transmit the geobroadcast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] destination Destination area defined as a circle
			 * @param[in] frequency Frequency of the geobraodcast message transmission
			 * @param[in] packetSize Size of the geobroadcast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void InitiateGeoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId,
					std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
			 * @param[in] nodeId ID of the node that will transmit the geobroadcast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] destination Destination area defined as a circle
			 * @param[in] frequency Frequency of the geobraodcast message transmission
			 * @param[in] packetSize Size of the geobroadcast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 */
			void InitiateGeoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId);

			/**
			 * @brief Activate a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
			 * @param[in] nodeId ID of the node that will transmit the geoanycast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] destination Destination area defined as a circle
			 * @param[in] frequency Frequency of the geoanycast message transmission
			 * @param[in] packetSize Size of the geoanycast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void InitiateGeoAnycastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId,
					std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
			 * @param[in] nodeId ID of the node that will transmit the geoanycast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] destination Destination area defined as a circle
			 * @param[in] frequency Frequency of the geoanycast message transmission
			 * @param[in] packetSize Size of the geoanycast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 */
			void InitiateGeoAnycastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId);

			/**
			 * @brief Activate a topobroadcast transmision using WAVE and the C2C stack
			 * @param[in] nodeId ID of the node that will transmit the topobroadcast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] frequency Frequency of the topobroadcast message transmission
			 * @param[in] packetSize Size of the topobroadcast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] numHops Number of hops or times that the topobroadcast packets can be retransmitted
			 * @param[in] genericContainer A container that may contain generic additional data required by ns-3 to handle this packet
			 */
			void ActivateTopoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t numHops, uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
			 * @brief Activate a topobroadcast transmision using WAVE and the C2C stack
			 * @param[in] nodeId ID of the node that will transmit the topobroadcast packet
			 * @param[in] serviceId ID of the service to be activated
			 * @param[in] frequency Frequency of the topobroadcast message transmission
			 * @param[in] packetSize Size of the topobroadcast message payload
			 * @param[in] msgRegenerationTime Time interval during which the packet will be generated in ns-3
			 * @param[in] msgLifetime Time interval during which the packet is considered as valid by the receivers
			 * @param[in] numHops Number of hops or times that the topobroadcast packets can be retransmitted
			 */
			void ActivateTopoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
					TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t numHops, uint32_t messageId);

			/**
			 * @brief Deactivate the txon of a service running in a vehicle or a RSU
			 * @param[in] nodeId ID of the node running the service to be deactivated
			 * @param[in] serviceId ID of the service to be deactivated
			 */
			void DeactivateServiceTxon(uint32_t nodeId, std::string serviceId);

			/**
			 * @brief Deactivate the txon of a service running in a IP-based base station or CIU
			 * @param[in] nodeId ID of the node running the service to be deactivated
			 * @param[in] serviceId ID of the service to be deactivated
			 */
			void DeactivateIPCIUServiceTxon(uint32_t nodeId, std::string serviceId);

			/**
			 * @brief Deactivate the txon of a service running in a MW node
			 * @param[in] nodeId ID of the node running the service to be deactivated
			 * @param[in] serviceId ID of the service to be deactivated
			 */
			void DeactivateMWServiceTxon(uint32_t nodeId, std::string serviceId);

			/**
			 * @brief Get the number of packets that a given node has received thorugh wireless communications
			 * @param[in] recvNodeId Id of the node from which we want to retrive the received packets
			 * @return Int indicating the number of received packets. 0 means no packed were received
			 *
			 */
			int GetNumberOfReceivedPackets(uint32_t recvNodeId);
		private:
			bool IsNodeActive(uint32_t nodeId);
			Ptr<iTETRISns3Facilities> GetFacilities(uint32_t nodeId);
			Ptr<IPCIUFacilities> GetIPCIUFacilities(uint32_t nodeId);
			Ptr<MWFacilities> GetMWFacilities(uint32_t nodeId);
			iTETRISNodeManager* m_nodeManager;

	};

}

#endif
