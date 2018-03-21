/****************************************************************************/
/// @file    utilities.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef CONVERSION
#define CONVERSION


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sstream>
#include <stdlib.h>
#include <limits>
#include <iostream>

namespace utils
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Conversion
 * @brief Series of functions to convert between types and other useful functions.
*/
class Conversion
{
public:

    /// @brief Convert a value from Integer to String.
    static std::string int2String(int intValue);

    /// @brief Convert a value from String to Integer.
    static int string2Int(std::string stringValue);

    /// @brief Show the input message and stop the execution until user inputs <ENTER> key
    static void Wait(std::string);

    /// @brief Show the input message and stop the execution until user inputs <ENTER> key if the timestep parameter matches the current simulation timestep
    static bool Wait(std::string, int timeStep);

    /// @brief Returns the duration of the simulation in seconds.
    static double GetElapsedTime();

    /// @brief Start time of the simulation.
    static time_t m_startTime;

    /// @brief End time of the simulation.
    static time_t m_endTime;
};

}

#endif
