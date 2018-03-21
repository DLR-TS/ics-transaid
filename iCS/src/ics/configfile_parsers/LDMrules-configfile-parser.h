/****************************************************************************/
/// @file    LDMrules-configfile-parser.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 28, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef LDMRULESCONFIGFILEPARSER_H_
#define LDMRULESCONFIGFILEPARSER_H_
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

#include "../../utils/ics/iCStypes.h"
using namespace ics_types;

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

struct lane_str {
    string ID;
};

struct edge_str {
    string ID;
};

struct junction_str {
    string ID;
};

struct point_str {
    float m_X;
    float m_Y;
    float m_Lat;
    float m_Lon;
};

struct circle_str {
    point_str m_center;
    float m_radius;
};

struct ellipse_str {
    point_str m_focus1;
    point_str m_focus2;
    point_str m_center;
    float m_eccentricity;
    float m_majorAxis;
    float m_minorAxis;
    float m_rotationAngleRadians;
};

struct rectangle_str {
    point_str m_vertexA;
    point_str m_vertexB;
    point_str m_vertexC;
    point_str m_vertexD;
    point_str m_center;
    point_str m_pointA;
    point_str m_pointB;
    float m_height;
};

struct convexPolygon_str {
    vector<point_str> m_verteces;
};

struct relevantDirection_str {
    float m_radiansFromNorth;
    float m_accuracy;
    bool  m_defined;
};

struct relevantStationTypes_str {
    bool m_fixed;
    bool m_mobile;
};

struct relevantMessages_str {
    bool m_cam;
    bool m_denm;
    bool m_topobroadcast;
    bool m_geobroadcast;
    bool m_geoanycast;
    bool m_unicast;
    bool m_multicast;
};

// Error codes
enum {
    ERROR_ARGS = 1,
    ERROR_XERCES_INIT,
    ERROR_PARSE,
    ERROR_EMPTY_DOCUMENT
};


class LDMrulesGetConfig {
public:
    LDMrulesGetConfig();
    ~LDMrulesGetConfig();
    void readConfigFile(std::string&) throw(std::runtime_error);

    icstime_t                       getDefaultMessageLifeInterval();
    vector <lane_str>               getRelevantLanes();
    vector <edge_str>               getRelevantEdges();
    vector <junction_str>           getRelevantJunctions();
    vector <circle_str>             getRelevantCircles();
    vector <ellipse_str> 		    getRelevantEllipses();
    vector <rectangle_str>          getRelevantRectangles();
    vector <convexPolygon_str>      getRelevantConvexPolygons();
    relevantDirection_str		    getRelevantDirection();
    relevantStationTypes_str        getRelevantStationTypes();
    relevantMessages_str            getRelevantMessages();

private:
    xercesc::XercesDOMParser        *m_ConfigFileParser;

    // variables
    icstime_t                       m_defaultMessageLifeInterval;

    vector <lane_str>               m_relevantLanes;
    vector <edge_str>               m_relevantEdges;
    vector <junction_str>           m_relevantJunctions;
    vector <circle_str>             m_relevantCircles;
    vector <ellipse_str>            m_relevantEllipses;
    vector <rectangle_str>          m_relevantRectangles;
    vector <convexPolygon_str>      m_relevantConvexPolygons;

    relevantDirection_str           m_relevantDirection;
    relevantStationTypes_str        m_relevantStationTypes;
    relevantMessages_str            m_relevantMessages;

    //Root tag
    XMLCh* TAG_LDMrules;

    XMLCh* TAG_defaultMessageLifeInterval;
    XMLCh* ATTR_value;

    XMLCh* TAG_relevantArea;

    XMLCh* TAG_lane;
    XMLCh* TAG_edge;
    XMLCh* TAG_junction;
    XMLCh* ATTR_ID;

    XMLCh* TAG_circle;
    XMLCh* ATTR_centerX;
    XMLCh* ATTR_centerY;
    XMLCh* ATTR_centerLat;
    XMLCh* ATTR_centerLon;
    XMLCh* ATTR_radius;

    XMLCh* TAG_ellipse;
    XMLCh* ATTR_focus1X;
    XMLCh* ATTR_focus1Y;
    XMLCh* ATTR_focus1Lat;
    XMLCh* ATTR_focus1Lon;
    XMLCh* ATTR_focus2X;
    XMLCh* ATTR_focus2Y;
    XMLCh* ATTR_focus2Lat;
    XMLCh* ATTR_focus2Lon;
    XMLCh* ATTR_eccentricity;
    XMLCh* ATTR_majorAxis;
    XMLCh* ATTR_minorAxis;
    XMLCh* ATTR_rotationAngleRadians;

    XMLCh* TAG_rectangle;
    XMLCh* ATTR_vertexAX;
    XMLCh* ATTR_vertexAY;
    XMLCh* ATTR_vertexALat;
    XMLCh* ATTR_vertexALon;
    XMLCh* ATTR_vertexBX;
    XMLCh* ATTR_vertexBY;
    XMLCh* ATTR_vertexBLat;
    XMLCh* ATTR_vertexBLon;
    XMLCh* ATTR_vertexCX;
    XMLCh* ATTR_vertexCY;
    XMLCh* ATTR_vertexCLat;
    XMLCh* ATTR_vertexCLon;
    XMLCh* ATTR_vertexDX;
    XMLCh* ATTR_vertexDY;
    XMLCh* ATTR_vertexDLat;
    XMLCh* ATTR_vertexDLon;
    XMLCh* ATTR_pointAX;
    XMLCh* ATTR_pointAY;
    XMLCh* ATTR_pointALat;
    XMLCh* ATTR_pointALon;
    XMLCh* ATTR_pointBX;
    XMLCh* ATTR_pointBY;
    XMLCh* ATTR_pointBLat;
    XMLCh* ATTR_pointBLon;
    XMLCh* ATTR_height;

    XMLCh* TAG_convexPolygon;
    XMLCh* TAG_vertex;
    XMLCh* ATTR_X;
    XMLCh* ATTR_Y;
    XMLCh* ATTR_Lat;
    XMLCh* ATTR_Lon;

    XMLCh* TAG_relevantDirection;
    XMLCh* ATTR_radiansFromNorth;
    XMLCh* ATTR_accuracy;

    XMLCh* TAG_relevantStationTypes;
    XMLCh* ATTR_fixed;
    XMLCh* ATTR_mobile;

    XMLCh* TAG_relevantMessages;
    XMLCh* ATTR_cam;
    XMLCh* ATTR_denm;
    XMLCh* ATTR_topobroadcast;
    XMLCh* ATTR_geobroadcast;
    XMLCh* ATTR_geoanycast;
    XMLCh* ATTR_unicast;
    XMLCh* ATTR_multicast;
};

}//namespace


#endif /* FACILITIESCONFIGFILEPARSER_H_ */
