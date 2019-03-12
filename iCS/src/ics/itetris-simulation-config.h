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

#include <vector>

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
     * calculates if a given vehicle will have a RAT installed or not
     * if it not filtered out by its ID, @see RATIdentifierList.
     * This will determine if the node will be transfered to ns-3 or not.
     * The id in the parameters is the id given by SUMO that will be added to
     * collection of discarded id if the result of the calculation is negative.
     */
    static bool HasRat(const std::string& vehID);

    /// @brief Time in seconds to assume the message will not be received.
    static int m_scheduleMessageCleanUp;

    /// @brief A list of identifiers for RAT equipped vehicles.
    ///        Has no effect, if empty, and is used to filter out unequipped vehicles
    ///        (those which do not have one of the given identifiers as a substring of their ID)
    static std::vector<std::string> RATIdentifiersList;
};

}

#endif
