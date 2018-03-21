/****************************************************************************/
/// @file    V2X-geobroadcast-area.cpp
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

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <sstream>

#include "V2X-geobroadcast-area.h"
#include "../../utils/ics/log/ics-log.h"

using namespace std;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
V2xGeobroadcastArea::V2xGeobroadcastArea(int subscriptionId, float frequency, unsigned int payloadLength)
{
    m_subscriptionId = subscriptionId;

    stringstream log;
    log << "V2xGeobroadcastArea() New geobroadcast area set by subscription ID " << subscriptionId;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    m_payloadLength = 60;
    m_frequency = frequency;
}

}
