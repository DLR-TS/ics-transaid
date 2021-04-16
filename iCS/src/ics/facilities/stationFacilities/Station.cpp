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
/// @file    Station.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Station.h"

namespace ics_facilities {

Station::Station() {
    position.set(-1, -1);
    isActive = true;
}

Station::~Station() {
    if (!RATs.empty()) {
        RATs.clear();
    }
}

stationID_t Station::getID() const {
    return ID;
}

icsstationtype_t Station::getType() const {
    return type;
}

const Point2D& Station::getPosition() const {
    return position;
}

vector<RATID>* Station::getRATs() {
    vector<RATID>* vrats = new vector<RATID>;
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++) {
        vrats->push_back(it->first);
    }
    return vrats;
}

vector<RATID>* Station::getActiveRATs() {
    vector<RATID>* vrats = new vector<RATID>;
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->second) {
            vrats->push_back(it->first);
        }
    return vrats;
}

void Station::setPosition(Point2D position) {
    this->position = position;
}

void Station::setRATs(vector< pair<RATID, bool> > RATs) {
    if (!this->RATs.empty()) {
        this->RATs.clear();
    }
    this->RATs = RATs;
    return;
}

bool Station::enableRAT(RATID toEnable) {
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->first == toEnable) {
            it->second = true;
            return true;
        }
    return false;
}

bool Station::disableRAT(RATID toDisable) {
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->first == toDisable) {
            it->second = false;
            return true;
        }
    return false;
}

}
