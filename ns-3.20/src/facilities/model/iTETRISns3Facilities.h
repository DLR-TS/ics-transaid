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


/**
 * \ingroup itetrisns3facilities
 * \defgroup itetrisns3facilities
 *
 * iTETRIS Facilities Object. Integrates communication facilities only,
 * the applications facilities are at the iCS.
 *
*/

/**
 * \ingroup itetrisns3facilities
 *
 * \brief the iTETRIS ns3 Facilities object to provide the service management and technology selector functionalities. It connects to iTETRISapplications.
 *
 *
 * @author Michele Rondinone <mrondinone@umh.es>
 * @author Fatma Hrizi <fatma.hrizi@eurecom.fr>
 *
  */


#ifndef ITETRIS_NS3_FACILITIES_H
#define ITETRIS_NS3_FACILITIES_H

#include <vector>

#include "ns3/service-management.h"
#include "ns3/local-comm-ch-selector.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/ipv4-address.h"
#include "ns3/itetris-types.h"
#include "ns3/addressing-support.h"
#include "ns3/c2c-address.h"
#include "ns3/channel-tag.h"

namespace ns3
{

	class iTETRISns3Facilities: public Object
	{

		public:

			static TypeId GetTypeId(void);
			iTETRISns3Facilities();
			~iTETRISns3Facilities();
			void DoDispose(void);

			void SetServiceManagement(Ptr<ServiceManagement> ServMng);
			void SetAddressingSupport(Ptr<AddressingSupport> AddrSupp);
			void SetLocalCommCHSelector(Ptr<LocalCOMMchSelector> localCHsel);

			/**
             * \brief initiate a transmission at facilities level specifying the destination by the identifier of a physical station
			 */

			void InitiateIdBasedTxon(std::string ServiceID, uint32_t commProfile, TechnologyList technologies,
					uint32_t destination, double frequency, uint32_t packetSize, double MessRegenerationTime, uint8_t msgLifetime,
					uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
             * \brief initiate a geobroadcast transmission at facilities level specifying the destination by a geographical zone
			 */

			void InitiateGeoBroadcastTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
             * \brief initiate a geoanycast transmission at facilities level specifying the destination by a geographical zone
			 */

			void InitiateGeoAnycastTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
             * \brief initiate a topobroadcast transmission at facilities level specifying the number of hops
			 */
			void InitiateTopoBroadcastTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime, uint32_t numHops,
					uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
             * \brief initiate the transmission of CAM messages at facilities level
			 */

			void ActivateCamTxon(double frequency, uint32_t packetSize, uint32_t messageId/*, ChannelTag ch_tag*/);

			/**
             * \brief initiate the transmission of DENM messages at facilities level
			 */
			void ActivateDenmTxon(CircularGeoAddress destination, double frequency, uint32_t packetSize,
					double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId/*, ChannelTag ch_tag*/);

			/**
             * \brief stop the transmission of CAM messages at facilities level
			 */
			void DeactivateCamTxon();

			/**
             * \brief stop the transmission of DENM messages at facilities level
			 */
			void DeactivateDenmTxon();

			/**
             * \brief stop the transmission of the messages belonging to a given service at facilities level
			 */
			void DeactivateServiceTxon(std::string serviceId);

			void SetNode(Ptr<Node> node);
			void NotifyNewAggregate();

		private:

			Ptr<Node> m_node;
			Ptr<ServiceManagement> m_ServiceManagement;
			Ptr<AddressingSupport> m_AddressingSupport;
			Ptr<LocalCOMMchSelector> m_LocalCOMMchSelector;

	};

}
;
//namespace ns3

#endif /* ITETRIS_NS3_FACILITIES_H */
