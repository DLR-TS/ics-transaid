/****************************************************************************/
/// @file    itetris-simulation-config.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef ITETRISSIMULATIONCONFIG_H
#define ITETRISSIMULATIONCONFIG_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#define CONFIG_11P	0
#define CONFIG_UMTS	1
#define CONFIG_WIMAX	2
#define CONFIG_DVBH	3

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class ITetrisNode;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ITetrisSimulationConfig
 * @brief A class to store and return configuration parameters of the simulation
 */
class ITetrisSimulationConfig
{
public:

    /**
     * @brief Stores the penetration rate of vehicles to simulate.
     */
    static float m_simulatedVehiclesPenetrationRate;

    /**
     * @brief Check if has Radio Acces tecnhlogy isntalled.
     * Based on the penetration rate of RAT defined by the user this fucntion
     * calculates if a given vehicle will have a RAT installed or not.
     * This will determine if the node will be transfered to ns-3 or not.
     * The id in the parameters is the id given by SUMO that will be added to
     * collection of discarded id if the result of the calculation is negative.
     */
    static bool HasRat();

    /**
    * @brief Decide wether the application will be installed in the node.
    * @param[in] rate The penetration rate of the application.
    * @param[in] seed The seed value to generate random numbers.
    */
    static bool AssignApplication(double rate, long seed);


    /// @brief Time in seconds to assume the message will not be received.
    static int m_scheduleMessageCleanUp;
};

}

#endif