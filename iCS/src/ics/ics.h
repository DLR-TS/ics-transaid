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
/****************************************************************************/
/// @file    ics.h
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
#ifndef ICS_H
#define ICS_H

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
#include <fstream>
#include <time.h>
#include <pthread.h>

/**
 * @namespace ics
 * @brief iCS main namespace.
 * Namespace fo the iCS core source code.
 */
namespace ics {

// ===========================================================================
// class declarations
// ===========================================================================
class SyncManager;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ICS
 * @brief Configures, executes and closes the iCS
 */
class ICS {
public:

    /**
    * @brief Constructor.
    * @param[in] ns3Port Port in which the connection with the ns-3 socket takes place.
    * @param[in] sumoPort Port in which the connection with the SUMO socket takes place.
    * @param[in] sumoHost Host where Sumo is running.
    * @param[in] ns3Host Host where ns-3 is running.
    * @param[in] beginTime Timestep in which the simulation starts.
    * @param[in] endTime Timestep in which the simulation ends.
    * @param[in] ratPenetrationRate Value of the penetration rate.
    * @param[in] ratIdentifiers List of strings which are used to test whether a vehicle is equipped with RAT.
    * @param[in] interactive Whether the simulation run in interactive mode.
    */
    ICS(int ns3Port, int sumoPort, std::string sumoHost, std::string ns3Host, int beginTime, int endTime, int resolution, int ratPenetrationRate, const std::vector<std::string>& ratIdentifiers, bool interactive);

    /// @brief Destructor
    ~ICS();

    /**
    * @brief- Connects to SUMO and ns-3 via socket, reads the iCS configuration file
    * and sends data to ns-3 to build up CIUs.
    * @param[in] facilitiesConfigFile Path to the facilities configuration file
    * @param[in] appConfigFile Path to the Applications configuration file
    * @return EXIT_FAILURE: If an error occurs.
    * @return EXIT_SUCCESS: If the parsing is successful.
    */
    int Setup(std::string facilitiesConfigFile, std::string appConfigFile);

    /**
    * @brief Starts iTETRIS run time-phase.
    * @return 0: If an error occurs.
    * @return 1: If the simulation runtime is successful.
    */
    int Run();

    /**
    * @brief Closes the simulation.
    * @return 0: If an error occurs.
    * @return 1: If the closing of the simulation is successful.
    */
    int Close();

private:

    /**
    * @brief Gets information from the facilities configuration file.
    * @param[in] &filePath Path to the facilities configuration file.
    * @return EXIT_FAILURE: If an error occurs.
    * @return EXIT_SUCCESS: If the parsing is successful.
    */
    int ReadFacilitiesConfigFile(std::string& filePath);

    /**
    * @brief Gets information about the applications configuration and launches the applications.
    * @param[in] filePath Path to the applications configuration file.
    * @return EXIT_FAILURE: If an error occurs.
    * @return EXIT_SUCCESS: If the parsing is successful.
    */
    int SetupApplications(std::string filePath);

    /// @brief Member of the iCS related to synchronizing issues.
    SyncManager* m_syncManager;

    /// @brief Whether the loop shall run in interactive mode.
    bool m_Interactive;

    /// @brief Collection of threads for the execution of the applications.
    static std::vector<pthread_t> m_applicationsThreads;
};

}


#endif
