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
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

/****************************************************************************************
 * Edited by Jin Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * merge with MOTO ns-3 LTE support
***************************************************************************************/

#include "ns3/mobility-helper.h"
#include "iTETRISNodeManager.h"
#include "ns3/log.h"
#include <iostream>
#include "ns3/CAMmanagement.h"
#include "ns3/node-container.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/itetris-mobility-model.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE("iTETRISNodeManager");

namespace ns3 {

iTETRISNodeManager::iTETRISNodeManager() :
    m_logKPIs(false),
    m_KPIFilePrefix("")
{}

uint32_t iTETRISNodeManager::CreateItetrisNode(Vector position) {
    CreateItetrisNode();
    Ptr<Node> node = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
    Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
    Vector newPos = mobModel->GetPosition();
    mobModel->SetPosition(position);
    NS_LOG_DEBUG("ns-3 server --> node position= (" << newPos.x << ", " << newPos.y << ")");

    return node->GetId();
}

uint32_t iTETRISNodeManager::CreateItetrisNode(const Vector& position, const float& speed, const float& heading, const std::string& laneId) {
    CreateItetrisNode();
    Ptr<Node> node = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
    Ptr<ItetrisMobilityModel> itetrisMobModel = node->GetObject<ItetrisMobilityModel> ();
    if (itetrisMobModel == NULL) {
        Ptr<MobilityModel> basicMobModel = node->GetObject<MobilityModel> ();
        basicMobModel->SetPosition(position);
        return node->GetId();
    }
    itetrisMobModel->SetPositionAndSpeed(position, speed, heading, GetEdgeId(laneId), laneId);
    Vector newPos = itetrisMobModel->GetPosition();
    NS_LOG_DEBUG("ns-3 server --> node position= (" << newPos.x << ", " << newPos.y << ")");
    return node->GetId();
}

void iTETRISNodeManager::CreateItetrisNode(uint32_t numNodes) {
    uint32_t x = 0;
    for (x = 0; x < numNodes; x++) {
        m_iTETRISNodes.Create(1);
        Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
        NodeContainer singleNodeContainer;
        singleNodeContainer.Add(singleNode);

        vector<Ptr<CommModuleInstaller> >::iterator it;
        for (it = m_defaultModules.begin(); it < m_defaultModules.end(); it++) {
            (*it)->Install(singleNodeContainer);
        }
        InstallCommunicationModule("UmtsVehicle", IPv6);
        InstallCommunicationModule("WaveVehicle");
        InstallCommunicationModule("LteVehicle");
    }
}


void iTETRISNodeManager::CreateItetrisNode(void) {
    m_iTETRISNodes.Create(1);
    Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
    NodeContainer singleNodeContainer;
    singleNodeContainer.Add(singleNode);
    vector<Ptr<CommModuleInstaller> >::iterator it;
    for (it = m_defaultModules.begin(); it < m_defaultModules.end(); it++) {
        (*it)->Install(singleNodeContainer);
    }

}


void iTETRISNodeManager::CreateItetrisTMC(void) {
    m_iTETRISNodes.Create(1);
    Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
    NodeContainer singleNodeContainer;
    singleNodeContainer.Add(singleNode);
    Ptr<CommModuleInstaller> comInstaller = GetInstaller("TMC");
    comInstaller->Install(singleNodeContainer);
}


NodeContainer*
iTETRISNodeManager::InstallCommunicationModule(std::string typeOfModule, STACK stack) {
    NodeContainerList::iterator iterCommModule = m_itetrisTechNodes.find(typeOfModule);
    if (iterCommModule != m_itetrisTechNodes.end()) {
        NodeContainer* container = iterCommModule->second;
        InstallerContainerList::iterator iterInstaller = m_itetrisInstallers.find(typeOfModule);
        if (iterInstaller != m_itetrisInstallers.end()) {
            Ptr<CommModuleInstaller> installer = iterInstaller->second;
            Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
            //create NodeContainer with one node
            NodeContainer singleNodeContainer;
            singleNodeContainer.Add(singleNode);
            //we install the node container into the technology installer???
            if (typeOfModule == "UmtsVehicle" || typeOfModule == "UmtsBs" || typeOfModule == "UmtsBs2") {

                installer->Install(singleNodeContainer, stack);
            } else {

                installer->Install(singleNodeContainer);
            }
            container->Add(singleNode);
            return (container);
        }
        NS_FATAL_ERROR("Communication module installer not found in iTETRISNodeManager - " << typeOfModule);
        return (NULL);
    }
    NS_FATAL_ERROR("Communication module not found in iTETRISNodeManager - " << typeOfModule);
    return (NULL);
}


NodeContainer*
iTETRISNodeManager::InstallCommunicationModule(std::string typeOfModule) {
    NodeContainerList::iterator iterCommModule = m_itetrisTechNodes.find(typeOfModule);
    if (iterCommModule != m_itetrisTechNodes.end()) {
        NodeContainer* container = iterCommModule->second;
        InstallerContainerList::iterator iterInstaller = m_itetrisInstallers.find(typeOfModule);
        if (iterInstaller != m_itetrisInstallers.end()) {
            Ptr<CommModuleInstaller> installer = iterInstaller->second;
            Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
            //create NodeContainer with one node
            NodeContainer singleNodeContainer;
            singleNodeContainer.Add(singleNode);
            //we install the node container into the technology installer???
            NS_LOG_INFO("Before install. Type " << installer->GetInstanceTypeId().GetName());
            installer->Install(singleNodeContainer);
            NS_LOG_INFO("After install");
            container->Add(singleNode);
            return (container);
        }
        NS_FATAL_ERROR("Communication module installer not found in iTETRISNodeManager - " << typeOfModule);

        return (NULL);
    }
    NS_FATAL_ERROR("Communication module not found in iTETRISNodeManager - " << typeOfModule);
    return (NULL);
}

void
iTETRISNodeManager::AttachInstaller(std::string typeOfModule, Ptr<CommModuleInstaller> installer) {
    InstallerContainerList::iterator iter = m_itetrisInstallers.find(typeOfModule);
    if (iter != m_itetrisInstallers.end()) {
        return;
    }
    m_itetrisInstallers.insert(std::make_pair(typeOfModule, installer));
    //Create a new Node Container to hold the nodes for the introduced type of module???
    NodeContainer* newContainer = new NodeContainer();
    m_itetrisTechNodes.insert(std::make_pair(typeOfModule, newContainer));
}

const NodeContainer&
iTETRISNodeManager::GetItetrisNodes(void) const {
    return m_iTETRISNodes;
}

NodeContainer*
iTETRISNodeManager::GetItetrisTechNodes(std::string typeOfModule) {

    NodeContainerList::iterator iterCommModule = m_itetrisTechNodes.find(typeOfModule);
    if (iterCommModule != m_itetrisTechNodes.end()) {
        return (iterCommModule->second);
    } else {
        return (NULL);
    }
}

void
iTETRISNodeManager::SetDefaultModule(std::string typeOfModule) {
    InstallerContainerList::iterator iter = m_itetrisInstallers.find(typeOfModule);
    if (iter != m_itetrisInstallers.end()) {
        m_defaultModules.push_back(iter->second);
    }
}

void
iTETRISNodeManager::UpdateNodePosition(uint32_t nodeId, Vector position) {
    Ptr<Node> node = m_iTETRISNodes.GetById(nodeId);
    Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
    mobModel->SetPosition(position);
}

void
iTETRISNodeManager::UpdateNodePosition(uint32_t nodeId, const Vector& position, const float& speed, const float& heading, const std::string& laneId) {
    Ptr<Node> node = m_iTETRISNodes.GetById(nodeId);
    Ptr<ItetrisMobilityModel> itetrisMobModel = node->GetObject<ItetrisMobilityModel> ();
    if (itetrisMobModel == NULL) {
        Ptr<MobilityModel> basicMobModel = node->GetObject<MobilityModel> ();
        basicMobModel->SetPosition(position);
        return;
    }
    itetrisMobModel->SetPositionAndSpeed(position, speed, heading, GetEdgeId(laneId), laneId);
}

Ptr<Node>
iTETRISNodeManager::GetItetrisNode(uint32_t nodeId) {

    Ptr<Node> node = NULL;
    return m_iTETRISNodes.GetById(nodeId);
}

Ptr<CommModuleInstaller>
iTETRISNodeManager::GetInstaller(std::string typeOfModule) {
    Ptr<CommModuleInstaller> installer = NULL;
    InstallerContainerList::iterator iterInstaller = m_itetrisInstallers.find(typeOfModule);
    if (iterInstaller != m_itetrisInstallers.end()) {
        installer = iterInstaller->second;
    }
    return installer;
}

bool
iTETRISNodeManager::ActivateNode(uint32_t nodeId) {
    Ptr<Node> node = GetItetrisNode(nodeId);
    if (node) {
        if (node->IsMobileNode()) {
            Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
            NS_ASSERT_MSG(staMgnt, "VehicleStaMgnt object not found in the vehicle");
            staMgnt->ActivateNode();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool
iTETRISNodeManager::DeactivateNode(uint32_t nodeId) {
    Ptr<Node> node = GetItetrisNode(nodeId);
    if (node) {
        if (node->IsMobileNode()) {
            Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
            NS_ASSERT_MSG(staMgnt, "VehicleStaMgnt object not found in the vehicle");
            staMgnt->DeactivateNode();
            return true;
        }
    }
    return false;
}

bool iTETRISNodeManager::IsNodeActive(uint32_t nodeId) {
    Ptr<Node> node = GetItetrisNode(nodeId);
    if (node) {
        if (node->IsMobileNode()) {
            Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
            NS_ASSERT_MSG(staMgnt, "VehicleStaMgnt object not found in the vehicle");
            return staMgnt->IsNodeActive();
        }
    }

    return true; // Fixed nodes are considered to be always active
}

std::string
iTETRISNodeManager::GetEdgeId(std::string laneId) {
    string edgeId = "";
    size_t found = laneId.find("_");
    size_t pos = string::npos;

    while (found != string::npos) {
        pos = found;
        found = laneId.find("_", found + 1, 1);
    }

    if (pos != string::npos) {
        edgeId = laneId.substr(0, pos);
    }
    return edgeId;
}


const std::string&
iTETRISNodeManager::GetKPIFilePrefix(void) const {
    return m_KPIFilePrefix;
}

void
iTETRISNodeManager::SetKPIFilePrefix(const std::string& runID) {
    m_KPIFilePrefix = runID;
}


void
iTETRISNodeManager::SetKPILogging(bool on) {
    m_logKPIs = on;
}

void
iTETRISNodeManager::SetInitialX(int initial_x) {
    m_InitialX = initial_x;
    std::cout << " Initial X for logging " << m_InitialX << std::endl;

}

void
iTETRISNodeManager::SetInitialY(int initial_y) {
    m_InitialY = initial_y;
    std::cout << " Initial Y for logging " << m_InitialY << std::endl;
}

void
iTETRISNodeManager::SetEndX(int end_x) {
    m_EndX = end_x;
    std::cout << " End Y for logging " << m_EndX << std::endl;
}

void
iTETRISNodeManager::SetEndY(int end_y) {
    m_EndY = end_y;
    std::cout << " End Y for logging " << m_EndY << std::endl;
}

int
iTETRISNodeManager::GetInitialX(void) {
    return m_InitialX;
}

int
iTETRISNodeManager::GetInitialY(void) {
    return m_InitialY;
}

int
iTETRISNodeManager::GetEndX(void) {
    return m_EndX;
}

int
iTETRISNodeManager::GetEndY(void) {
    return m_EndY;
}

}
