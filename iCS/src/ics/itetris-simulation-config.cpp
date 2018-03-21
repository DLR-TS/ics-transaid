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

#ifndef _WIN32
#include <unistd.h>	// need for getpid() function
#endif

#include "itetris-simulation-config.h"
#include "itetris-node.h"
#include "../utils/ics/iCSRandom.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

namespace ics
{

// ===========================================================================
// static members definitions
// ===========================================================================
float ITetrisSimulationConfig::m_simulatedVehiclesPenetrationRate;
int ITetrisSimulationConfig::m_scheduleMessageCleanUp = -1;

// ===========================================================================
// member method definitions
// ===========================================================================
bool
ITetrisSimulationConfig::HasRat()
{
    //initialize random seed
#ifndef _WIN32
    srand(time(NULL) + getpid());
#endif

    //Generates random number
    int randomNumber = rand() % 100;

    if (randomNumber < m_simulatedVehiclesPenetrationRate) {
        return true;
    }

    return false;
}

bool
ITetrisSimulationConfig::AssignApplication(double rate, long seed)
{
    //initialize random seed
#ifndef _WIN32
    srand(time(NULL) + getpid());
#endif

    //Generates random number
    iCSRandom randomGenerator;
    randomGenerator.seed(seed);
    double randomNumber = randomGenerator.rand(100); // to get a number between 0 and 100

    if (randomNumber < rate) {
        return true;
    }

    return false;
}

}
