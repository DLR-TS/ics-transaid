/****************************************************************************/
/// @file    FixedStation.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "FixedStation.h"

namespace ics_facilities {

FixedStation::FixedStation(stationID_t ID) {
    this->type = STATION_FIXED;
    this->ID = ID;
}

FixedStation::~FixedStation() { }

void FixedStation::setCommunicationProfile(string commProfile) {
    this->communicationProfile = commProfile;
}

string FixedStation::getCommunicationProfile() {
    return communicationProfile;
}

}

