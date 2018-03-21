/****************************************************************************/
/// @file    itetris-node.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef ITETRISNODE_H
#define ITETRISNODE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <set>

#include "utilities.h"
#include "../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class Subscription;
class ApplicationHandler;
class ResultContainer;
class V2xMessage;
class FacilitiesManager;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class ITetrisNode
 * @brief Manages information relative to a node involved in the simulation
*/
class ITetrisNode
{
public:

    /// @brief Constructor
    ITetrisNode();

    /// @brief Destructor
    virtual ~ITetrisNode();

    /// @brief Node identifier in iCS
    ics_types::stationID_t m_icsId;

    /// @brief Node identifier in traffic simulator.
    std::string m_tsId;

    /// @brief Node identifier in the network communication simulation.
    int m_nsId;

    /// @brief The kind of station
    ics_types::stationType_t m_type;

    /// @brief Technologies in the station.
    std::set<std::string> m_rats;

    /// @brief Data the applications are subscribed to
    std::vector<Subscription*>* m_subscriptionCollection;

    /// @brief Installed applications
    std::vector<ApplicationHandler*>* m_applicationHandlerInstalled;

    /// @brief Data from the application executions is placed here.
    std::vector<ResultContainer*>* m_resultContainerCollection;

    /// @brief Time since the last message reception
    std::vector<V2xMessage*>* m_lastTimeStepReceivedMessages;

    /// @brief The node has been created on this step
	bool m_newNode;

    /**
    * @brief Get the x position of the node from the facilities.
    * @return The x position of the node in the map
    */
    float GetPositionX();

    /**
    * @brief Get the y position of the node from the facilities.
    * @return The y position of the node in the map
    */
    float GetPositionY();

    /**
    * @brief Get the lane where the node is.
    * @return The lane of the node in the map.
    */
    std::string GetLane();

protected:
    /// @brief A counter to assign iCS node ID
    static int m_idCounter;

    /// @brief Container to hold the IDs already assigned by the user
    static std::set<ics_types::stationID_t> m_preAssignedIds;
};

}//ics

#endif
