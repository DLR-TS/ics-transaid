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
/// @file    facilities-configfile-parser.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 28, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef FACILITIESCONFIGFILEPARSER_H_
#define FACILITIESCONFIGFILEPARSER_H_
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
#include <string>
#include <stdexcept>

using namespace std;

namespace ics_parsing {

// Error codes
enum {
    ERROR_ARGS = 1,
    ERROR_XERCES_INIT,
    ERROR_PARSE,
    ERROR_EMPTY_DOCUMENT
};


class FacilitiesGetConfig {
public:
    FacilitiesGetConfig();
    ~FacilitiesGetConfig();
    void readConfigFile(std::string&) throw(std::runtime_error);

    float getLocalLatitude();
    float getLocalLongitude();
    float getLocalAltitude();

    string getMapConfigFilename();
    string getStationsConfigFilename();
    string getLDMrulesConfigFilename();

private:
    xercesc::XercesDOMParser* m_ConfigFileParser;


    float referenceLatitude;
    float referenceLongitude;
    float referenceAltitude;

    string mapConfigurationFilename;
    string stationsConfigurationFilename;
    string LDMrulesConfigurationFilename;

    //Root tag
    XMLCh* TAG_Facilties;

    XMLCh* TAG_LocalCoordinates;

    XMLCh* TAG_MapConfig;
    XMLCh* TAG_StationsConfig;
    XMLCh* TAG_LDMrulesConfig;

    XMLCh* ATTR_latitude;
    XMLCh* ATTR_longitude;
    XMLCh* ATTR_altitude;

    XMLCh* ATTR_MapConFilename;
    XMLCh* ATTR_StationsConFilename;
    XMLCh* ATTR_LDMrulesConFilename;
};

}//namespace


#endif /* FACILITIESCONFIGFILEPARSER_H_ */
