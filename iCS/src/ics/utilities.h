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

namespace utils {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Conversion
 * @brief Series of functions to convert between types and other useful functions.
*/
class Conversion {
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
