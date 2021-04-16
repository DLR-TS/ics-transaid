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

#ifndef PROGRAM_CONFIGURATION_H_
#define PROGRAM_CONFIGURATION_H_

#include <map>
#include <string>
#include "vector.h"

namespace tinyxml2 {
class XMLElement;
}

namespace lightcomm {

class ProgramConfiguration {
public:
    static int LoadConfiguration(const char* fileName, int port);


    static int GetSocketPort() {
        return m_socket;
    }

private:
    ProgramConfiguration();
    ~ProgramConfiguration();

    static int ParseGeneral(tinyxml2::XMLElement* general);


private:
    static int m_socket;
};

} /* namespace lightcomm */

#endif /* PROGRAM_CONFIGURATION_H_ */
