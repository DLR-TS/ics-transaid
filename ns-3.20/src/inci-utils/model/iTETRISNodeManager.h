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
#ifndef ITETRISNODEMANAGER_H
#define ITETRISNODEMANAGER_H

#include "ns3/mobility-model.h"
#include "comm-module-installer.h"
#include "ns3/itetris-types.h"
#include <map>

namespace ns3
{

class NodeContainer;
class Node;

/**
 * @class iTETRISNodeManager 
 * @brief The class iTETRISNodeManager manages the creation, the initial placement, and the position updates of ns3 nodes. The additions of communication modules (technologies, protocols stacks, etc.) is perfomed through installers.
 */
class iTETRISNodeManager
{
  public:
    iTETRISNodeManager (void);

    /**
     * @brief Create a single node
     */
    void CreateItetrisNode (void);
    void CreateItetrisNode(uint32_t numNodes); 
    uint32_t CreateItetrisNode (Vector position); 
    uint32_t CreateItetrisNode (const Vector &position, const float &speed, const float & heading, const std::string &laneId); 
    void CreateItetrisTMC (void);

    /**
     * @brief Get all the iTETRIS nodes
     */
    const NodeContainer & GetItetrisNodes () const;

    /**
     * @brief Get a NodeContainer pointer of the nodes with the corresponding communication module (typeOfModule)
     */
    NodeContainer* GetItetrisTechNodes (std::string typeOfModule);

    /**
     * @brief Attach a new communication module Installer to the NodeManager
     */
    void AttachInstaller (std::string typeOfModule, Ptr<CommModuleInstaller> installer); 

    /**
     * @brief Install a new communication module ('typeOfModule') in the last node that has been created
     */
    NodeContainer* InstallCommunicationModule (std::string typeOfModule);

    /**
     * @brief Install a new communication module ('typeOfModule') in the last node that has been created
     */
    //IP-case
    NodeContainer* InstallCommunicationModule (std::string typeOfModule, STACK stack);  

    /**
     * @brief Get the installer for the communication module ('typeOfModule') 
     */
    Ptr<CommModuleInstaller> GetInstaller (std::string typeOfModule);

    /**
     * @brief Set the default modules that will be installed in each new node that is created
     */
    void SetDefaultModule (std::string typeOfModule);

    /**
     * @brief Update node's position
     */
    void UpdateNodePosition(uint32_t nodeId, Vector position); 
    void UpdateNodePosition (uint32_t nodeId, const Vector &position, const float &speed, const float & heading, const std::string &laneId); 

    Ptr<Node> GetItetrisNode (uint32_t nodeId);

    bool ActivateNode (uint32_t nodeId);
    bool DeactivateNode (uint32_t nodeId);
    bool IsNodeActive (uint32_t nodeId);

    void SetKPIFilePrefix (const std::string& runID);
    const std::string& GetKPIFilePrefix (void) const;
    void SetKPILogging (bool on);
    inline bool KPILogOn() const {
    	return m_logKPIs;
    }
    void SetInitialX (int initial_x);
    void SetInitialY (int initial_y);
    void SetEndX (int end_x);
    void SetEndY (int end_y);

    int GetInitialX (void);
    int GetInitialY (void);
    int GetEndX (void);
    int GetEndY (void);

private:

    std::string GetEdgeId (std::string laneId);

    /**
     * @brief Node container with all the iTETRIS nodes
     */
    NodeContainer m_iTETRISNodes;

    typedef std::map<std::string, Ptr<CommModuleInstaller> > InstallerContainerList;
    typedef std::map<std::string, NodeContainer*> NodeContainerList;

    /**
     * @brief List of NodeContainers with a NodeContainer per communication module (e.g. WAVE, DVB-H, etc.)
     */
    NodeContainerList m_itetrisTechNodes;

    /**
     * @brief List of communication module installers.
     */
    InstallerContainerList m_itetrisInstallers;

    std::vector<Ptr<CommModuleInstaller> > m_defaultModules;

    /// @brief Whether to log communication related KPIs
    bool m_logKPIs;
    /// @brief defines an ID for the run (used for naming KPI output files), @see logKPIs
    std::string m_KPIFilePrefix;

    int m_InitialX;
    int m_InitialY;
    int m_EndX;
    int m_EndY;
};

}

#endif
