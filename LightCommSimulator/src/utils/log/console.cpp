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
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#include "console.h"
#include <iostream>
#include <sstream>

namespace protocol {
using namespace std;

string Console::m_appName = "Protocol Speed -> ";

void Console::Log(const string& msg, const int& msg2) {
    std::ostringstream oss;
    oss << msg << msg2;
    Log(oss.str());
}

void Console::Log(const string& msg, const char* msg2) {
    Log(msg + string(msg2));
}

void Console::Log(const string& msg) {
    cout << m_appName << msg << endl;
}

void Console::Warning(const string& msg) {
    cout << m_appName << "[WARNING]" << msg << endl;
}

void Console::Error(const string& msg, const int& msg2) {
    std::ostringstream oss;
    oss << msg << msg2;
    Error(oss.str());
}

void Console::Error(const string& msg, const char* msg2) {
    Error(msg + string(msg2));
}

void Console::Error(const string& msg) {
    cerr << m_appName << "[ERROR]" << msg << endl;
}

void Console::SetAppName(const string& appName) {
    m_appName = string(appName + " -> ");
}

} /* namespace protocol */
