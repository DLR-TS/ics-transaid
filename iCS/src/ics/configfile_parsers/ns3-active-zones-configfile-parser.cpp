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


#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>

#include "ns3-active-zones-configfile-parser.h"
#include "../../utils/common/TplConvert.h"

using namespace xercesc;

namespace ics {

Ns3ActiveZonesConfigFileParse::Ns3ActiveZonesConfigFileParse() {
	m_ActiveZoneCollection = new vector<ActiveZone*>();

	try {
		XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
	} catch (XMLException& e) {
		char* message = XMLString::transcode(e.getMessage());
		cerr << "XML toolkit initialization error: " << message << endl;
		XMLString::release(&message);
		// throw exception here to return ERROR_XERCES_INIT
	}

	TAG_ActiveZones = XMLString::transcode("ns3-active-zones");
	TAG_ActiveZone = XMLString::transcode("ns3-active-zone");
	TAG_Xpos = XMLString::transcode("xpos");
	TAG_Ypos = XMLString::transcode("ypos");
	TAG_Radius = XMLString::transcode("radius");

	m_ConfigFileParser = new XercesDOMParser;
}

Ns3ActiveZonesConfigFileParse::~Ns3ActiveZonesConfigFileParse() {
	delete m_ActiveZoneCollection;
	delete m_ConfigFileParser;
	delete[] TAG_ActiveZones;
	delete[] TAG_ActiveZone;
	delete[] TAG_Xpos;
	delete[] TAG_Ypos;
	delete[] TAG_Radius;
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
Ns3ActiveZonesConfigFileParse::readConfigFile(string& configFile)
throw(std::runtime_error) {
	// Test to see if the file is ok.

	struct stat fileStatus;

	if (stat(configFile.c_str(), &fileStatus)<0) {
		if (errno == ENOENT)
			throw(std::runtime_error("Could not find the configuration file '" + configFile + "'."));
		else if (errno == ENOTDIR)
			throw(std::runtime_error("A component of the path is not a directory."));
#ifndef _WIN32
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

		// Get the top-level element: NAme is "root". No attributes for "root"
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if (!elementRoot) throw(std::runtime_error("empty XML document"));

		DOMNodeList* activeZones = elementRoot->getElementsByTagName(TAG_ActiveZone);
		XMLSize_t nodeCount = activeZones->getLength();

		// Loop all <Application> tags
		for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {

			DOMNode* currentNode = activeZones->item(xx);

			if (currentNode->getNodeType() &&  // true is not NULL
					currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element

				ActiveZone* activeZone = new ActiveZone();
				DOMElement* currentElememt = dynamic_cast< xercesc::DOMElement* >(currentNode);
				DOMNodeList* children = currentElememt->getChildNodes();

				for (XMLSize_t yy = 0; yy < children->getLength(); ++yy) {
					DOMNode* _node = children->item(yy);
					DOMElement* _element = static_cast< xercesc::DOMElement* >(_node);

					if (XMLString::equals(_element->getTagName(), TAG_Xpos)) {

						DOMNodeList* children2 = _element->getChildNodes();
						for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
							char* tmp_stringX_ch =   XMLString::transcode(children2->item(index)->getNodeValue());
							activeZone->m_xpos = (float) atof(tmp_stringX_ch);
						}
					}
					if (XMLString::equals(_element->getTagName(), TAG_Ypos)) {

						DOMNodeList* children2 = _element->getChildNodes();
						for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
							char* tmp_stringY_ch =   XMLString::transcode(children2->item(index)->getNodeValue());
							activeZone->m_ypos = (float) atof(tmp_stringY_ch);
						}
					}
					if (XMLString::equals(_element->getTagName(), TAG_Radius)) {

						DOMNodeList* children2 = _element->getChildNodes();
						for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
							char* tmp_stringRadius_ch =   XMLString::transcode(children2->item(index)->getNodeValue());
							activeZone->m_radius = (float) atof(tmp_stringRadius_ch);
						}
					}
				}

				m_ActiveZoneCollection->push_back(activeZone);
			}
		}


	} catch (xercesc::XMLException& e) {
		char* message = xercesc::XMLString::transcode(e.getMessage());
		ostringstream errBuf;
		errBuf << "Error parsing file: " << message << flush;
		XMLString::release(&message);
	}
}

vector<ActiveZone*>*
Ns3ActiveZonesConfigFileParse::GetNs3ActiveZonesConfig()
{
	return m_ActiveZoneCollection;
}

}
