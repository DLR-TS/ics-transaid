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

#include <time.h>
#include "utilities.h"
#include "sync-manager.h"
#include <utils/common/StringUtils.h>

namespace utils {

time_t Conversion::m_startTime;
time_t Conversion::m_endTime;

std::string
Conversion::int2String(int intValue) {
    std::stringstream out;
    out << intValue;
    return out.str();
}

int
Conversion::string2Int(std::string stringValue) {
    int rValue;

    rValue = StringUtils::toInt(stringValue.c_str());

    return rValue;
}

bool
Conversion::Wait(std::string message, int timeStep) {
    if (ics::SyncManager::m_simStep == timeStep) {
        std::cout << message;
        std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
        return true;
    }

    return false;
}

void
Conversion::Wait(std::string message) {
    std::cout << message;
    std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
}

double
Conversion::GetElapsedTime() {
    return difftime(m_endTime, m_startTime);
}

}
