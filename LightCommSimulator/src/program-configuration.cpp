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

#include "program-configuration.h"


#include "utils/log/console.h"
#include "utils/xml/tinyxml2.h"
#include "fatal-error.h"


using namespace std;

namespace lightcomm {

using namespace tinyxml2;

int ProgramConfiguration::m_socket;

int ProgramConfiguration::LoadConfiguration(const char* fileName, int port) {
    std::cout << "Lightcomm: Loading Configuration" << endl;


    if (port == -1) {
        XMLDocument* doc = new XMLDocument();
        XMLError result = doc->LoadFile(fileName);
        if (result != XML_NO_ERROR) {
            std::cout << "Lightcomm: XML ERROR loading file '" << string(fileName) << endl;
            return EXIT_FAILURE;
        }

        XMLElement* xmlElem =  doc->RootElement()->FirstChildElement("port");
        if (!xmlElem) {
            return EXIT_FAILURE;
        }
        delete doc;
        m_socket = xmlElem->IntAttribute("value");
    } else {
        // a port was given via command line -> overrides config file
        if (fileName != nullptr) {
            std::cout << "LightComm -> Note: port given via command line overrides port specified in config file!" << std::endl;
        }
        m_socket = port;
    }
    std::cout << "LightComm listening on port " << m_socket << endl;
    return EXIT_SUCCESS;
}


int ProgramConfiguration::ParseGeneral(XMLElement* general) {

    return EXIT_SUCCESS;
}



} /* namespace lightcomm */
