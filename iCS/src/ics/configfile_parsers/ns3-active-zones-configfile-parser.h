/****************************************************************************************
 * ns3-active-zones-configfile-parser.cpp
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/
/****************************************************************************************
 * Copyright (c) 2016 EURECOM
 * This code has been developed in the context of the
 * SINETIC project
 * ....
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Additional permission under GNU GPL version 3 section 7
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgment: 'This product includes software developed by
 * EURECOM and its contributors'.
 * 4. Neither the name of EURECOM nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 ***************************************************************************************/

#ifndef NS3ACTIVEZONESCONFIGFILEPARSER_H_
#define NS3ACTIVEZONESCONFIGFILEPARSER_H_
/**
 *  @file
 *  Class "GetConfig" provides the functions to read the XML data.
 *  @version 1.0
 */
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

namespace ics {

struct ActiveZone {
    float m_xpos;
    float m_ypos;
    float m_radius;
};

/**
 * @class Ns3ActiveZonesConfigFileParse
 * @brief this class represents the active zones, with a higher granularity. Used to diffentiate between component and system-level simulations
 *
 */
class Ns3ActiveZonesConfigFileParse {

public:
    /**
     * @brief  Constructor initializes xerces-C libraries.
     *  The XML tags and attributes which we seek are defined.
     *  The xerces-C DOM parser infrastructure is initialized.
     */
	Ns3ActiveZonesConfigFileParse();

    /// @brief Destructor·
    ~Ns3ActiveZonesConfigFileParse();

    /**
    * @brief Reads the information from the config file.
    * @param [in] . The path of the config file.
    */
    void readConfigFile(std::string&) throw(std::runtime_error);

    /**
    * @brief Fill the struct with the information read by the application config file.
    * @return A collection of applications configuration.
    */
    vector<ActiveZone*>* GetNs3ActiveZonesConfig();

private:

    /// @brief Xerces DOM parser.
    xercesc::XercesDOMParser* m_ConfigFileParser;

    /// @brief Pointer to the information of the application config file.
    vector<ActiveZone*>* m_ActiveZoneCollection;

    /// @brief Root tag.
    XMLCh* TAG_ActiveZones;

    /// @brief ActiveZone tag.
    XMLCh* TAG_ActiveZone;

    /// @brief Xpos tag.
    XMLCh* TAG_Xpos;
    /// @brief Ypos tag.
    XMLCh* TAG_Ypos;

    /// @brief Radius tag.
    XMLCh* TAG_Radius;
};

}//namespace


#endif /* NS3ACTIVEZONESCONFIGFILEPARSER_H_ */
