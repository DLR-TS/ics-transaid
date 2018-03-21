/****************************************************************************/
/// @file    V2X-cam-area.cpp
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

#include "V2X-cam-area.h"

using namespace std;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
V2xCamArea::V2xCamArea(int subscriptionId, float frequency, unsigned int payloadLength)
{
    m_subscriptionId = subscriptionId;
    m_payloadLength = payloadLength;
    m_frequency = frequency;
}

}
