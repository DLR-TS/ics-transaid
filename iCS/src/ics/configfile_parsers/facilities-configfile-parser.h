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
    xercesc::XercesDOMParser *m_ConfigFileParser;


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
