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

#ifndef MW_FACILITIES_H
#define MW_FACILITIES_H

#include <vector>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"

#include "ns3/service-list.h"
#include "ns3/ipv4-address.h"
#include "ns3/mw-comm-ch-selector.h"
#include "ns3/itetris-types.h"
#include "ns3/iTETRISNodeManager.h"
#include "ns3/node-container.h"

#define TYPE_TOPO            0x00
#define TYPE_RECEIVED_CAM    0x01
#define TYPE_COMMUNICATION   0x02

#define VALUE__POS_X         0x01
#define VALUE__POS_Y         0x02
#define VALUE__SPEED         0x03
#define VALUE__DIRECTION     0x04
#define VALUE__ACCELERATION  0x05
#define VALUE__LANE_ID       0x06
#define VALUE__EDGE_ID       0x07
#define VALUE__JUNCTION_ID   0x08

namespace ns3
{

	class MWFacilities: public Object
	{

		public:

			static TypeId GetTypeId(void);
			MWFacilities();
			~MWFacilities();
			void DoDispose(void);

			void SetMWCommCHSelector(Ptr<MWCOMMchSelector> MWCHsel);

			/**
			 * initiate a geo based transmission after the selection of the disseminating CIU among those present in the simulation scenario
			 */
			void InitiateMWGeoBasedTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime,
					uint8_t msglifetime, uint32_t messageId, std::vector<unsigned char> genericContainer);

			/**
			 * initiate an Id based (unicast) transmission after the selection of the disseminating CIU among those present in the simulation scenario.
			 */
//<<<<<<< .mine
			void InitiateMWIdBasedTxon(std::string serviceId, uint32_t commProfile, TechnologyList technologies,
					double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msglifetime, uint32_t destid,
					uint32_t messageId, std::vector<unsigned char> genericContainer);
//=======
//  void InitiateMWIdBasedTxon (std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msglifetime, uint32_t destid);
//>>>>>>> .r208

			/**
			 * deactivate a previously initiated geo based transmission on the CIU that was chosen for this purpose
			 */
			void DeactivateServiceTxon(std::string serviceId);

			void SetNode(Ptr<Node> node);
			void NotifyNewAggregate();
			void AddInfrastructureTechNode(Ptr<Node> node, std::string typeOfModule);
			NodeContainer* getInfrastructureTechNodes(std::string typeOfModule);

		private:

			Ptr<Node> m_node;
			disseminationProfile m_disseminationProfile;
			Ptr<MWCOMMchSelector> m_MWCOMMchSelector;

			typedef std::map<std::string, NodeContainer*> NodeContainerList;

			/**
			 * \brief List of NodeContainers with a NodeContainer per communication module (e.g. WAVE, DVB-H, etc.)
			 */
			NodeContainerList m_infranodes;

	};

}
;
//namespace ns3

#endif /* MW_FACILITIES_H */
