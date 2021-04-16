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
/// @file    stations-configfile-parser.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 3, 2010
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

#include "stations-configfile-parser.h"
#include <utils/common/StringUtils.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>

using namespace xercesc;

namespace ics_parsing {

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

StationsGetConfig::StationsGetConfig() {
    try {
        XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
    } catch (XMLException& e) {
        char* message = XMLString::transcode(e.getMessage());
        cerr << "XML toolkit initialization error: " << message << endl;
        XMLString::release(&message);
        // throw exception here to return ERROR_XERCES_INIT
    }

    // Tags and attributes used in XML file.
    // Can't call transcode till after Xerces Initialize()

    TAG_Default                     = XMLString::transcode("default");

    TAG_MobileStas                  = XMLString::transcode("mobileStas");

    TAG_RATseed                     = XMLString::transcode("RATseed");
    ATTR_RATseedValue               = XMLString::transcode("value");

    TAG_MobileSta                   = XMLString::transcode("mobileSta");
    ATTR_MobRATtype                 = XMLString::transcode("RAT-type");
    ATTR_PenetrationRate            = XMLString::transcode("penetration-rate");
    ATTR_MobileCommunicationProfile = XMLString::transcode("communication-profile");

    TAG_FixedStas                   = XMLString::transcode("fixedStas");
    TAG_FixedSta                    = XMLString::transcode("fixedSta");
    ATTR_id                         = XMLString::transcode("id");
    ATTR_x                          = XMLString::transcode("x");
    ATTR_y                          = XMLString::transcode("y");
    ATTR_FixRATtype                 = XMLString::transcode("RAT-type");
    ATTR_enabledRAT                 = XMLString::transcode("enabledRAT");
    ATTR_FixedCommunicationProfile  = XMLString::transcode("communication-profile");

    m_ConfigFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

StationsGetConfig::~StationsGetConfig() {
    XMLString::release(&TAG_Default);
    XMLString::release(&TAG_MobileStas);
    XMLString::release(&TAG_RATseed);
    XMLString::release(&ATTR_RATseedValue);
    XMLString::release(&TAG_MobileSta);
    XMLString::release(&ATTR_MobRATtype);
    XMLString::release(&ATTR_PenetrationRate);
    XMLString::release(&ATTR_MobileCommunicationProfile);
    XMLString::release(&TAG_FixedStas);
    XMLString::release(&TAG_FixedSta);
    XMLString::release(&ATTR_id);
    XMLString::release(&ATTR_x);
    XMLString::release(&ATTR_y);
    XMLString::release(&ATTR_FixRATtype);
    XMLString::release(&ATTR_enabledRAT);
    XMLString::release(&ATTR_FixedCommunicationProfile);
}

/**
 *  This function:
 *  - Tests the access and availability of the XML configuration file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param in configFile The text string name of the configuration file.
 */

void
StationsGetConfig::readConfigFile(string& configFile)
throw(std::runtime_error) {
    // Test to see if the file is ok.

    struct stat fileStatus;

    if (stat(configFile.c_str(), &fileStatus) < 0) {
        if (errno == ENOENT) {
            throw (std::runtime_error("Could not find stations configuration file '" + configFile + "'."));
        } else if (errno == ENOTDIR) {
            throw (std::runtime_error("A component of the path is not a directory."));
        }
#ifndef _MSC_VER
        else if (errno == ELOOP) {
            throw (std::runtime_error("Too many symbolic links encountered while traversing the path."));
        }
#endif
        else if (errno == EACCES) {
            throw (std::runtime_error("Permission denied."));
        } else if (errno == ENAMETOOLONG) {
            throw (std::runtime_error("File can not be read\n"));
        }
    }

    // Configure DOM parser.
    m_ConfigFileParser->setValidationScheme(XercesDOMParser::Val_Never);
    m_ConfigFileParser->setDoNamespaces(false);
    m_ConfigFileParser->setDoSchema(false);
    m_ConfigFileParser->setLoadExternalDTD(false);

    try {

        m_ConfigFileParser->parse(configFile.c_str());

        // no need to free this pointer - owned by the parent parser object
        DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();

        // Get the top-level element: NAme is "root". No attributes for "root"

        DOMElement* elementRoot = xmlDoc->getDocumentElement();
        if (!elementRoot) {
            throw (std::runtime_error("empty XML document"));
        }

        DOMNodeList* def = elementRoot->getElementsByTagName(TAG_Default);
        XMLSize_t nodeCountDef = def->getLength();

        for (XMLSize_t xx = 0; xx < nodeCountDef; ++xx) {

            DOMNode* currentNode = def->item(xx);

            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element

                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
                DOMNodeList* children = currentElement->getChildNodes();

                for (XMLSize_t yy = 0; yy < children->getLength(); ++yy) {
                    DOMNode* _node = children->item(yy);
                    DOMElement* _element = static_cast< xercesc::DOMElement* >(_node);


                    // parse "RATseed"
                    if (XMLString::equals(_element->getTagName(), TAG_RATseed)) {
                        const XMLCh* xmlch_stringRATseed = _element->getAttribute(ATTR_RATseedValue);
                        char* stringRATseed_ch = XMLString::transcode(xmlch_stringRATseed);
                        sscanf(stringRATseed_ch, "%u", &RATseed);
                        XMLString::release(&stringRATseed_ch);
                    }

                    // parse "mobileStas"
                    if (XMLString::equals(_element->getTagName(), TAG_MobileStas)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            if (XMLString::equals(children2->item(index)->getNodeName(), TAG_MobileSta)) {
                                DOMNode* currentNode = children2->item(index);
                                if (currentNode->getNodeType() &&  // true is not NULL
                                        currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                                    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);

                                    const XMLCh* xmlch_MobRATtype = currentElement->getAttribute(ATTR_MobRATtype);
                                    char* type_ch = XMLString::transcode(xmlch_MobRATtype);
                                    int type = (bool)StringUtils::toInt(type_ch);
                                    XMLString::release(&type_ch);

                                    const XMLCh* xmlch_PenetrationRate = currentElement->getAttribute(ATTR_PenetrationRate);
                                    char* penRate_ch = XMLString::transcode(xmlch_PenetrationRate);
                                    float penRate = (float)StringUtils::toDouble(penRate_ch);
                                    XMLString::release(&penRate_ch);
                                    defaultPenetrationRates.insert(pair<int, float>(type, penRate));

                                    const XMLCh* xmlch_commProfile = currentElement->getAttribute(ATTR_MobileCommunicationProfile);
                                    char* profile_ch = XMLString::transcode(xmlch_commProfile);
                                    string profile = profile_ch;
                                    XMLString::release(&profile_ch);

                                    mobileCommunicationProfiles.insert(pair<int, string>(type, profile));

#ifdef _DEB_STATIONS
                                    cout << "(MobRATtype, penRate)     = (" << type << ", " << penRate << ")" << endl;
                                    cout << "(MobRATtype, commProfile) = (" << type << ", " << profile << ")" << endl;
#endif
                                }
                            }
                        }
                    }

                    // parse "fixedStas"
                    if (XMLString::equals(_element->getTagName(), TAG_FixedStas)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            if (XMLString::equals(children2->item(index)->getNodeName(), TAG_FixedSta)) {
                                DOMNode* currentNode = children2->item(index);
                                if (currentNode->getNodeType() &&  // true is not NULL
                                        currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                                    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);

                                    FixedStationStr fs;

                                    const XMLCh* xmlch_id = currentElement->getAttribute(ATTR_id);
                                    char* m_ID_ch = XMLString::transcode(xmlch_id);
                                    sscanf(m_ID_ch, "%u", &(fs.m_ID));
                                    XMLString::release(&m_ID_ch);

                                    const XMLCh* xmlch_x = currentElement->getAttribute(ATTR_x);
                                    char* m_X_ch = XMLString::transcode(xmlch_x);
                                    sscanf(m_X_ch, "%f", &(fs.m_X));
                                    XMLString::release(&m_X_ch);

                                    const XMLCh* xmlch_y = currentElement->getAttribute(ATTR_y);
                                    char* m_Y_ch = XMLString::transcode(xmlch_y);
                                    sscanf(m_Y_ch, "%f", &(fs.m_Y));
                                    XMLString::release(&m_Y_ch);

                                    const XMLCh* xmlch_RATtype = currentElement->getAttribute(ATTR_FixRATtype);
                                    char* m_RATtype_ch = XMLString::transcode(xmlch_RATtype);
                                    sscanf(m_RATtype_ch, "%d", &(fs.m_RATtype));
                                    XMLString::release(&m_RATtype_ch);

                                    const XMLCh* xmlch_enabledRAT = currentElement->getAttribute(ATTR_enabledRAT);
                                    char* m_enabledRAT_ch = XMLString::transcode(xmlch_enabledRAT);
                                    sscanf(m_enabledRAT_ch, "%d", &(fs.m_enabledRAT));
                                    XMLString::release(&m_enabledRAT_ch);

                                    const XMLCh* xmlch_commProfile = currentElement->getAttribute(ATTR_FixedCommunicationProfile);
                                    char* profile_ch = XMLString::transcode(xmlch_commProfile);
                                    fs.m_communicationprofile = profile_ch;
                                    XMLString::release(&profile_ch);

                                    m_FixedStationCollection.push_back(fs);
                                }
                            }
                        }
                    }

                }
            }
        }

        delete (m_ConfigFileParser);

        XMLPlatformUtils::Terminate();


    } catch (xercesc::XMLException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        ostringstream errBuf;
        errBuf << "Error parsing file: " << message << flush;
        XMLString::release(&message);
    }
}

unsigned int                StationsGetConfig::getRATseed() {
    return RATseed;
}

map <int, float>           StationsGetConfig::getDefaultPenetrationRates() {
    return defaultPenetrationRates;
}

vector<FixedStationStr>    StationsGetConfig::getFixedStationCollection() {
    return m_FixedStationCollection;
}

map <int, string>          StationsGetConfig::getMobileCommunicationProfiles() {
    return mobileCommunicationProfiles;
}

}
