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

#include <memory>
#include "utils/xml/tinyxml2.h"
#include "speed/data-manager.h"
#include "speed/protocols.h"
#include "speed-configuration.h"

namespace protocolspeedapp {

int SpeedConfiguration::LoadConfiguration(const char* fileName, int port) {
    if (m_instance == nullptr) {
        m_instance = std::unique_ptr<SpeedConfiguration>(new SpeedConfiguration());
    }
    return ProgramConfiguration::LoadConfiguration(fileName, port);
}

int SpeedConfiguration::ParseSetup(tinyxml2::XMLElement* setup) {
    ProgramConfiguration::ParseSetup(setup);
    int iVal;
    double dVal;
    bool bVal;
    tinyxml2::XMLElement* xmlElem = setup->FirstChildElement("data-manager");
    if (xmlElem) {
        if (xmlElem->QueryBoolAttribute("enabled", &bVal) == tinyxml2::XML_NO_ERROR) {
            application::DataManager::Enabled = bVal;
        }
        if (xmlElem->QueryIntAttribute("execute-time", &iVal) == tinyxml2::XML_NO_ERROR)
            if (iVal >= 0 && iVal <= 65535) {
                application::DataManager::ExecuteTime = iVal;
            }
        if (xmlElem->QueryBoolAttribute("enable-centralized-protocol", &bVal) == tinyxml2::XML_NO_ERROR) {
            application::DataManager::EnableCentralizedProtocol = bVal;
        }
    }
    xmlElem = setup->FirstChildElement("centralized-protocol");
    if (xmlElem) {
        if (xmlElem->QueryDoubleAttribute("space-threshold", &dVal) == tinyxml2::XML_NO_ERROR) {
            application::CentralizedProtocol::SpaceThreshold = dVal;
        }
        if (xmlElem->QueryBoolAttribute("return-data", &bVal) == tinyxml2::XML_NO_ERROR) {
            application::CentralizedProtocol::ReturnData = bVal;
        }
    }
    return EXIT_SUCCESS;
}
} /* namespace protocol */
