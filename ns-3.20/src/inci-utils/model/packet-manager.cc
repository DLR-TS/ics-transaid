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

#include <list>
#include "ns3/log.h"

#include "iTETRISNodeManager.h"
#include "ns3/CAMmanagement.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node-container.h"
#include "ns3/inci-packet-list.h"
#include "ns3/itetris-types.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/IPCIUFacilities.h"
#include "ns3/umts-vehicle-scan-mngr.h"
#include "ns3/umts-bs-mgnt.h"
#include "ns3/channel-tag.h"
#include "ns3/ns3-server.h"

#include "packet-manager.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE("PacketManager");

namespace ns3 {

PacketManager::PacketManager() :
    m_nodeManager(0) {
}

PacketManager::~PacketManager() {
}

void PacketManager::SetNodeManager(iTETRISNodeManager* nodeManager) {
    m_nodeManager = nodeManager;
}

void PacketManager::ActivateCamTxon(uint32_t nodeId, float frequency, uint32_t packetSize, uint32_t messageId) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->ActivateCamTxon(frequency, packetSize, messageId);
    }
}

void PacketManager::DeactivateCamTxon(uint32_t nodeId) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL) {
        facilities->DeactivateCamTxon();
    }
}

void PacketManager::ActivateDenmTxon(uint32_t nodeId, CircularGeoAddress destination, double frequency,
                                     uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId) {

    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->ActivateDenmTxon(destination, frequency, packetSize, msgRegenerationTime, msgLifetime, messageId);
    }
}

void PacketManager::DeactivateDenmTxon(uint32_t nodeId) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL) {
        facilities->DeactivateDenmTxon();
    }
}

bool PacketManager::GetReceivedPacket(uint32_t nodeId, struct InciPacket::ReceivedInciPacket& inciPacketData) {
    bool morePackets = false;
    Ptr<Node> node = m_nodeManager->GetItetrisNode(nodeId);
    if (node != NULL) {
        Ptr<InciPacketList> packetList = node->GetObject<InciPacketList>();
        if (packetList != NULL) {
            InciPacket packet;
            morePackets = packetList->GetReceivedPacket(packet);
            inciPacketData = packet.GetData();
        } else {
            NS_LOG_DEBUG("Node with ID " << nodeId << " does not have a PacketList attached");
        }
    } else {
        NS_LOG_DEBUG("Node with ID " << nodeId << " has not been found in the ItetrisNodeManager");
    }

    return morePackets;
}

int PacketManager::GetNumberOfReceivedPackets(uint32_t nodeId) {
    Ptr<Node> node = m_nodeManager->GetItetrisNode(nodeId);
    if (node != NULL) {
        Ptr<InciPacketList> packetList = node->GetObject<InciPacketList>();
        if (packetList != NULL) {
            return packetList->Size();
        } else {
            NS_LOG_DEBUG("Node with ID " << nodeId << " does not have a PacketList attached");
        }
    } else {
        NS_LOG_DEBUG("Node with ID " << nodeId << " has not been found in the ItetrisNodeManager");
    }
    return 0;
}

Ptr<iTETRISns3Facilities> PacketManager::GetFacilities(uint32_t nodeId) {
    Ptr<Node> node = m_nodeManager->GetItetrisNode(nodeId);
    if (node != NULL) {
        return node->GetObject<iTETRISns3Facilities>();
    } else {
        return NULL;
    }
}

Ptr<IPCIUFacilities> PacketManager::GetIPCIUFacilities(uint32_t nodeId) {
    Ptr<Node> node = m_nodeManager->GetItetrisNode(nodeId);
    if (node != NULL) {
        return node->GetObject<IPCIUFacilities>();
    } else {
        return NULL;
    }
}

Ptr<MWFacilities> PacketManager::GetMWFacilities(uint32_t nodeId) {
    Ptr<Node> node = m_nodeManager->GetItetrisNode(nodeId);
    if (node != NULL) {
        return node->GetObject<MWFacilities>();
    } else {
        return NULL;
    }
}

void PacketManager::InitiateIdBasedTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                        TechnologyList technologies, float frequency, uint32_t packetSize, uint32_t destination,
                                        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateIdBasedTxon(nodeId, serviceId, commProfile, technologies, frequency, packetSize, destination,
                        msgRegenerationTime, msgLifetime, messageId, container);
}

void PacketManager::InitiateIdBasedTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                        TechnologyList technologies, float frequency, uint32_t packetSize, uint32_t destination,
                                        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer) {

#ifdef _DEBUG
    stringstream log;
    log << "[ns-3][InitiateIdBasedTxon ] Source ID: " << nodeId << " Receiver ID: " << destination;
    Ns3Server::Log(log.str().c_str());
#endif

    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateIdBasedTxon(serviceId, commProfile, technologies, destination, frequency, packetSize,
                                        msgRegenerationTime, msgLifetime, messageId, genericContainer);
    } else {
        stringstream log;
        log << "GetFacilities (nodeId) == NULL or !IsNodeActive (nodeId)";
        Ns3Server::Log(log.str().c_str());
    }
}

void PacketManager::InitiateIPCIUTxon(uint32_t nodeId, std::string serviceId, float frequency, uint32_t packetSize,
                                      uint32_t destination, double msgRegenerationTime, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateIPCIUTxon(nodeId, serviceId, frequency, packetSize, destination, msgRegenerationTime, messageId, container);
}

void PacketManager::InitiateIPCIUTxon(uint32_t nodeId, std::string serviceId, float frequency, uint32_t packetSize,
                                      uint32_t destination, double msgRegenerationTime, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<IPCIUFacilities> facilities = GetIPCIUFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateIPBasedTxon(serviceId, destination, frequency, packetSize, msgRegenerationTime, messageId,
                                        genericContainer);
    }
}

void PacketManager::InitiateMWTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                   TechnologyList technologies, CircularGeoAddress destination, float frequency, uint32_t packetSize,
                                   double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateMWTxon(nodeId, serviceId, commProfile, technologies, destination, frequency, packetSize,
                   msgRegenerationTime, msgLifetime, messageId, container);

}

void PacketManager::InitiateMWTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                   TechnologyList technologies, CircularGeoAddress destination, float frequency, uint32_t packetSize,
                                   double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<MWFacilities> facilities = GetMWFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateMWGeoBasedTxon(serviceId, commProfile, technologies, destination, frequency, packetSize,
                                           msgRegenerationTime, msgLifetime, messageId, genericContainer);
    }
}

void
PacketManager::InitiateMWIdTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                TechnologyList technologies, float frequency, uint32_t packetSize, double msgRegenerationTime,
                                uint8_t msgLifetime, uint32_t destid, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateMWIdTxon(nodeId, serviceId, commProfile, technologies, frequency, packetSize, msgRegenerationTime,
                     msgLifetime, destid, messageId, container);

}
void PacketManager::InitiateMWIdTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
                                     TechnologyList technologies, float frequency, uint32_t packetSize, double msgRegenerationTime,
                                     uint8_t msgLifetime, uint32_t destid, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<MWFacilities> facilities = GetMWFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateMWIdBasedTxon(serviceId, commProfile, technologies, frequency, packetSize,
                                          msgRegenerationTime, msgLifetime, destid, messageId, genericContainer);
    }
}

void PacketManager::InitiateGeoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateGeoBroadcastTxon(nodeId, serviceId, commProfile, technologies, destination, frequency, packetSize,
                             msgRegenerationTime, msgLifetime, messageId, container);

}
void PacketManager::InitiateGeoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateGeoBroadcastTxon(serviceId, commProfile, technologies, destination, frequency, packetSize,
                                             msgRegenerationTime, msgLifetime, messageId, genericContainer);
    }
}

void PacketManager::InitiateGeoAnycastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId) {
    std::vector<unsigned char> container;
    InitiateGeoAnycastTxon(nodeId, serviceId, commProfile, technologies, destination, frequency, packetSize,
                           msgRegenerationTime, msgLifetime, messageId, container);

}

void PacketManager::InitiateGeoAnycastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize,
        double msgRegenerationTime, uint8_t msgLifetime, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateGeoAnycastTxon(serviceId, commProfile, technologies, destination, frequency, packetSize,
                                           msgRegenerationTime, msgLifetime, messageId, genericContainer);
    }
}

void PacketManager::ActivateTopoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime,
        uint8_t msgLifetime, uint32_t numHops, uint32_t messageId) {
    std::vector<unsigned char> container;
    ActivateTopoBroadcastTxon(nodeId, serviceId, commProfile, technologies, frequency, packetSize, msgRegenerationTime,
                              msgLifetime, numHops, messageId, container);

}

void PacketManager::ActivateTopoBroadcastTxon(uint32_t nodeId, std::string serviceId, uint32_t commProfile,
        TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime,
        uint8_t msgLifetime, uint32_t numHops, uint32_t messageId, std::vector<unsigned char> genericContainer) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL && IsNodeActive(nodeId)) {
        facilities->InitiateTopoBroadcastTxon(serviceId, commProfile, technologies, frequency, packetSize,
                                              msgRegenerationTime, msgLifetime, numHops, messageId, genericContainer);
    }
}

void PacketManager::DeactivateServiceTxon(uint32_t nodeId, std::string serviceId) {
    Ptr<iTETRISns3Facilities> facilities = GetFacilities(nodeId);
    if (facilities != NULL) {
        facilities->DeactivateServiceTxon(serviceId);
    }
}

void PacketManager::DeactivateIPCIUServiceTxon(uint32_t nodeId, std::string serviceId) {
    Ptr<IPCIUFacilities> facilities = GetIPCIUFacilities(nodeId);
    if (facilities != NULL) {
        facilities->DeactivateServiceTxon(serviceId);
    }
}

void PacketManager::DeactivateMWServiceTxon(uint32_t nodeId, std::string serviceId) {
    Ptr<MWFacilities> facilities = GetMWFacilities(nodeId);
    if (facilities != NULL) {
        facilities->DeactivateServiceTxon(serviceId);
    }
}

bool PacketManager::IsNodeActive(uint32_t nodeId) {
    return m_nodeManager->IsNodeActive(nodeId);
}

}
