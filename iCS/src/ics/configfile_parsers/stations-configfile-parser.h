/****************************************************************************/
/// @file    stations-configfile-parser.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    May 3, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef STATIONSCONFIGFILEPARSER_H_
#define STATIONSCONFIGFILEPARSER_H_
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

#include <vector>
#include <map>
using namespace std;

namespace ics_parsing {

struct FixedStationStr {
    unsigned int m_ID;
    float m_X;
    float m_Y;
    int m_RATtype;
    int m_enabledRAT;
    string m_communicationprofile;
};

// Error codes
enum {
    ERROR_ARGS = 1,
    ERROR_XERCES_INIT,
    ERROR_PARSE,
    ERROR_EMPTY_DOCUMENT
};


class StationsGetConfig {
public:
    StationsGetConfig();
    ~StationsGetConfig();
    void readConfigFile(std::string&) throw(std::runtime_error);

    unsigned int                getRATseed();
    map <int, float>            getDefaultPenetrationRates();
    vector<FixedStationStr>     getFixedStationCollection();
    map <int, string>           getMobileCommunicationProfiles();

private:
    xercesc::XercesDOMParser    *m_ConfigFileParser;

    // variables
    unsigned int                RATseed;
    map <int, float>            defaultPenetrationRates;
    map <int, string>           mobileCommunicationProfiles;
    vector<FixedStationStr>     m_FixedStationCollection;

    XMLCh* TAG_Default;

    XMLCh* TAG_RATseed;
    XMLCh* ATTR_RATseedValue;

    XMLCh* TAG_MobileStas;

    XMLCh* TAG_MobileSta;
    XMLCh* ATTR_MobRATtype;
    XMLCh* ATTR_PenetrationRate;
    XMLCh* ATTR_MobileCommunicationProfile;

    XMLCh* TAG_FixedStas;
    XMLCh* TAG_FixedSta;
    XMLCh* ATTR_id;
    XMLCh* ATTR_x;
    XMLCh* ATTR_y;
    XMLCh* ATTR_FixRATtype;
    XMLCh* ATTR_enabledRAT;
    XMLCh* ATTR_FixedCommunicationProfile;
};

}//namespace


#endif /* FACILITIESCONFIGFILEPARSER_H_ */
