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
/// @file    itetris-simulation-config.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <stdlib.h>	// need for srand() function	
#include <time.h>	// need for time() function	
#include <iostream>
#include <vector>

#ifndef _WIN32
#include <unistd.h>	// need for getpid() function
#endif

#include "itetris-simulation-config.h"
#include "itetris-node.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

namespace ics {

// ===========================================================================
// static members definitions
// ===========================================================================
float ITetrisSimulationConfig::m_simulatedVehiclesPenetrationRate;
int ITetrisSimulationConfig::m_scheduleMessageCleanUp = -1;
std::vector<std::string> ITetrisSimulationConfig::RATIdentifiersList;

// ===========================================================================
// member method definitions
// ===========================================================================
bool
ITetrisSimulationConfig::HasRat(const std::string& vehID) {
    //initialize random seed
#ifndef _WIN32
    srand(time(NULL) + getpid());
#endif

    bool hasRAT = false;
    if (RATIdentifiersList.empty()) {
        hasRAT = true;
    } else {
        for (std::string& s : RATIdentifiersList) {
            if (vehID.find(s) != std::string::npos) {
                hasRAT = true;
                break;
            }
        }
    }

    // Superimpose penetration rate on ID-wise selected RAT vehicles
    hasRAT = hasRAT && (rand() % 100 < m_simulatedVehiclesPenetrationRate);

    return hasRAT;
}

}
