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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>

namespace baseapp {

class Console {
public:
    static void SetAppName(const std::string& appName);

    static void Log(const std::string& msg);
    static void Log(const std::string& msg, const int& msg2);
    static void Log(const std::string& msg, const char* msg2);

    static void Warning(const std::string& msg);

    static void Error(const std::string& msg);
    static void Error(const std::string& msg, const int& msg2);
    static void Error(const std::string& msg, const char* msg2);
private:
    static std::string m_appName;
    Console();
    virtual ~Console();
};

} /* namespace protocol */

#endif /* CONSOLE_H_ */
