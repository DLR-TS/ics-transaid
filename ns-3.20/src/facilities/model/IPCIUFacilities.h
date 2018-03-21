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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
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

#ifndef IP_CIU_FACILITIES_H
#define IP_CIU_FACILITIES_H

#include <vector>
#include "ns3/service-management.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/ipv4-address.h"
#include "ns3/itetris-types.h"
#include "ns3/addressing-support.h"

namespace ns3
{

	class IPCIUFacilities;

	class IPCIUFacilities: public Object
	{

		public:

			static TypeId GetTypeId(void);
			IPCIUFacilities();
			~IPCIUFacilities();
			void DoDispose(void);

			void SetServiceManagement(Ptr<ServiceManagement> ServMng);
			void SetAddressingSupport(Ptr<AddressingSupport> AddrSupp);

			/**
			 * initiate an IP transmission at facilities level specifying the destination by the identifier of a physical receiver station
			 */

			void InitiateIPBasedTxon(std::string ServiceID, uint32_t destination, double frequency, uint32_t packetSize,
					double MessRegenerationTime, uint32_t messageId, std::vector<unsigned char> genericContainer);
			void DeactivateServiceTxon(std::string serviceId);

			void SetNode(Ptr<Node> node);
			void NotifyNewAggregate();

		private:

			Ptr<Node> m_node;
			Ptr<ServiceManagement> m_ServiceManagement;
			Ptr<AddressingSupport> m_AddressingSupport;

	};

}
;
//namespace ns3

#endif /* IP_CIU_FACILITIES_H*/
