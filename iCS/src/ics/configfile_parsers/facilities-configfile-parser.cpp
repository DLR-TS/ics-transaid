/****************************************************************************/
/// @file    facilities-configfile-parser.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 28, 2010
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

#include "./facilities-configfile-parser.h"
#include "../../utils/common/TplConvert.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

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

FacilitiesGetConfig::FacilitiesGetConfig() {
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

    TAG_Facilties               = XMLString::transcode("facilities");

    TAG_LocalCoordinates        = XMLString::transcode("localCoordinates");

    TAG_MapConfig               = XMLString::transcode("mapConfig");
    TAG_StationsConfig          = XMLString::transcode("stationsConfig");
    TAG_LDMrulesConfig          = XMLString::transcode("LDMrulesConfig");

    ATTR_latitude               = XMLString::transcode("latitude");
    ATTR_longitude              = XMLString::transcode("longitude");
    ATTR_altitude               = XMLString::transcode("altitude");

    ATTR_MapConFilename         = XMLString::transcode("mapConFilename");
    ATTR_StationsConFilename    = XMLString::transcode("stationsConFilename");
    ATTR_LDMrulesConFilename    = XMLString::transcode("LDMrulesConFilename");

    m_ConfigFileParser = new XercesDOMParser;

    referenceLatitude = -1;
    referenceLongitude = -1;
    referenceAltitude = -1;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

FacilitiesGetConfig::~FacilitiesGetConfig() {
    XMLString::release(&TAG_Facilties);
    XMLString::release(&TAG_LocalCoordinates);
    XMLString::release(&TAG_MapConfig);
    XMLString::release(&TAG_StationsConfig);
    XMLString::release(&TAG_LDMrulesConfig);
    XMLString::release(&ATTR_latitude);
    XMLString::release(&ATTR_longitude);
    XMLString::release(&ATTR_altitude);
    XMLString::release(&ATTR_MapConFilename);
    XMLString::release(&ATTR_StationsConFilename);
    XMLString::release(&ATTR_LDMrulesConFilename);
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
FacilitiesGetConfig::readConfigFile(string& configFile)
throw(std::runtime_error) {
    // Test to see if the file is ok.

    struct stat fileStatus;

    if (stat(configFile.c_str(), &fileStatus)<0) {
        if (errno == ENOENT)
            throw(std::runtime_error("Could not find facilities configuration file '" + configFile + "'."));
        else if (errno == ENOTDIR)
            throw(std::runtime_error("A component of the path is not a directory."));
#ifndef _MSC_VER
        else if (errno == ELOOP)
            throw(std::runtime_error("Too many symbolic links encountered while traversing the path."));
#endif
        else if (errno == EACCES)
            throw(std::runtime_error("Permission denied."));
        else if (errno == ENAMETOOLONG)
            throw(std::runtime_error("File can not be read\n"));
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

        // Get the top-level element: Name is "root". No attributes for "root"

        DOMElement* elementRoot = xmlDoc->getDocumentElement();
        if (!elementRoot) throw(std::runtime_error("empty XML document"));

        DOMNodeList* item = elementRoot->getElementsByTagName(TAG_MapConfig);
        XMLSize_t nodeCount = item->getLength();

        for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {
            DOMNode* currentNode = item->item(xx);
            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);

                const XMLCh* xmlch_mapConfFileName = currentElement->getAttribute(ATTR_MapConFilename);
                char* tmpString = XMLString::transcode(xmlch_mapConfFileName);
                mapConfigurationFilename = tmpString;
                XMLString::release(&tmpString);
            }
        }

        item = elementRoot->getElementsByTagName(TAG_LocalCoordinates);
        nodeCount = item->getLength();
        for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {
            DOMNode* currentNode = item->item(xx);
            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);
                if (currentElement->hasAttribute(ATTR_latitude)) {
                    const XMLCh* xmlch_latitude = currentElement->getAttribute(ATTR_latitude);
                    char* tmpString = XMLString::transcode(xmlch_latitude);
                    referenceLatitude = TplConvert::_2SUMOReal(tmpString);
                    XMLString::release(&tmpString);
                }
                if (currentElement->hasAttribute(ATTR_longitude)) {
                    const XMLCh* xmlch_longitude = currentElement->getAttribute(ATTR_longitude);
                    char* tmpString = XMLString::transcode(xmlch_longitude);
                    referenceLongitude = TplConvert::_2SUMOReal(tmpString);
                    XMLString::release(&tmpString);
                }
                if (currentElement->hasAttribute(ATTR_altitude)) {
                    const XMLCh* xmlch_altitude = currentElement->getAttribute(ATTR_altitude);
                    char* tmpString = XMLString::transcode(xmlch_altitude);
                    referenceAltitude = TplConvert::_2SUMOReal(tmpString);
                    XMLString::release(&tmpString);
                } else
                    referenceAltitude = -10000;     // Clearly you cannot be -10000 meters below the see level!
            }
        }

        item = elementRoot->getElementsByTagName(TAG_StationsConfig);
        nodeCount = item->getLength();

        for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {
            DOMNode* currentNode = item->item(xx);
            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);

                const XMLCh* xmlch_mapConfFileName = currentElement->getAttribute(ATTR_StationsConFilename);
                char* tmpString = XMLString::transcode(xmlch_mapConfFileName);
                stationsConfigurationFilename = tmpString;
                XMLString::release(&tmpString);
            }
        }


        item = elementRoot->getElementsByTagName(TAG_LDMrulesConfig);
        nodeCount = item->getLength();

        for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {
            DOMNode* currentNode = item->item(xx);
            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(currentNode);

                const XMLCh* xmlch_mapConfFileName = currentElement->getAttribute(ATTR_LDMrulesConFilename);
                char* tmpString = XMLString::transcode(xmlch_mapConfFileName);
                LDMrulesConfigurationFilename = tmpString;
                XMLString::release(&tmpString);
            }
        }

        delete m_ConfigFileParser;

        XMLPlatformUtils::Terminate();

    } catch (xercesc::XMLException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        ostringstream errBuf;
        errBuf << "Error parsing file: " << message << flush;
        XMLString::release(&message);
    }
}

string FacilitiesGetConfig::getMapConfigFilename() {
    return mapConfigurationFilename;
}

string FacilitiesGetConfig::getStationsConfigFilename() {
    return stationsConfigurationFilename;
}

string FacilitiesGetConfig::getLDMrulesConfigFilename() {
    return LDMrulesConfigurationFilename;
}

float FacilitiesGetConfig::getLocalLatitude() {
    return referenceLatitude;
}

float FacilitiesGetConfig::getLocalLongitude() {
    return referenceLongitude;
}

float FacilitiesGetConfig::getLocalAltitude() {
    return referenceAltitude;
}

}
