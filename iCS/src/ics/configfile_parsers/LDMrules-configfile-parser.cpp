/****************************************************************************/
/// @file    LDMrules-configfile-parser.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 28, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "LDMrules-configfile-parser.h"
#include "../../utils/common/TplConvert.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <cstdio>
#include <cmath>
#include <limits>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>

#ifdef WIN32
#include <limits>
#define NAN std::numeric_limits<float>::quiet_NaN()
#endif


using namespace xercesc;

namespace ics_parsing {

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

LDMrulesGetConfig::LDMrulesGetConfig() {
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
    TAG_LDMrules 					= XMLString::transcode("LDMrules");

    TAG_defaultMessageLifeInterval	= XMLString::transcode("defaultMessageLifeInterval");
    ATTR_value						= XMLString::transcode("value");

    TAG_relevantArea				= XMLString::transcode("relevantArea");

    TAG_lane 						= XMLString::transcode("lane");
    TAG_edge 						= XMLString::transcode("edge");
    TAG_junction 					= XMLString::transcode("junction");
    ATTR_ID 						= XMLString::transcode("ID");

    TAG_circle 						= XMLString::transcode("circle");
    ATTR_centerX 					= XMLString::transcode("centerX");
    ATTR_centerY 					= XMLString::transcode("centerY");
    ATTR_centerLat 					= XMLString::transcode("centerLat");
    ATTR_centerLon 					= XMLString::transcode("centerLon");
    ATTR_radius 					= XMLString::transcode("radius");

    TAG_ellipse 					= XMLString::transcode("ellipse");
    ATTR_focus1X 					= XMLString::transcode("focus1X");
    ATTR_focus1Y 					= XMLString::transcode("focus1Y");
    ATTR_focus1Lat 					= XMLString::transcode("focus1Lat");
    ATTR_focus1Lon 					= XMLString::transcode("focus1Lon");
    ATTR_focus2X 					= XMLString::transcode("focus2X");
    ATTR_focus2Y 					= XMLString::transcode("focus2Y");
    ATTR_focus2Lat 					= XMLString::transcode("focus2Lat");
    ATTR_focus2Lon 					= XMLString::transcode("focus2Lon");
    ATTR_eccentricity				= XMLString::transcode("eccentricity");
    ATTR_majorAxis 					= XMLString::transcode("majorAxis");
    ATTR_minorAxis 					= XMLString::transcode("minorAxis");
    ATTR_rotationAngleRadians 		= XMLString::transcode("rotationAngleRadians");

    TAG_rectangle 					= XMLString::transcode("rectangle");
    ATTR_vertexAX 					= XMLString::transcode("vertexAX");
    ATTR_vertexAY 					= XMLString::transcode("vertexAY");
    ATTR_vertexALat 				= XMLString::transcode("vertexALat");
    ATTR_vertexALon 				= XMLString::transcode("vertexALon");
    ATTR_vertexBX 					= XMLString::transcode("vertexBX");
    ATTR_vertexBY 					= XMLString::transcode("vertexBY");
    ATTR_vertexBLat 				= XMLString::transcode("vertexBLat");
    ATTR_vertexBLon 				= XMLString::transcode("vertexBLon");
    ATTR_vertexCX 					= XMLString::transcode("vertexCX");
    ATTR_vertexCY 					= XMLString::transcode("vertexCY");
    ATTR_vertexCLat 				= XMLString::transcode("vertexCLat");
    ATTR_vertexCLon 				= XMLString::transcode("vertexCLon");
    ATTR_vertexDX 					= XMLString::transcode("vertexDX");
    ATTR_vertexDY 					= XMLString::transcode("vertexDY");
    ATTR_vertexDLat 				= XMLString::transcode("vertexDLat");
    ATTR_vertexDLon 				= XMLString::transcode("vertexDLon");
    ATTR_pointAX 					= XMLString::transcode("pointAX");
    ATTR_pointAY 					= XMLString::transcode("pointAY");
    ATTR_pointALat 					= XMLString::transcode("pointALat");
    ATTR_pointALon 					= XMLString::transcode("pointALon");
    ATTR_pointBX 					= XMLString::transcode("pointBX");
    ATTR_pointBY 					= XMLString::transcode("pointBY");
    ATTR_pointBLat 					= XMLString::transcode("pointBLat");
    ATTR_pointBLon 					= XMLString::transcode("pointBLon");
    ATTR_height 					= XMLString::transcode("height");

    TAG_convexPolygon 				= XMLString::transcode("convexPolygon");
    TAG_vertex 						= XMLString::transcode("vertex");
    ATTR_X 							= XMLString::transcode("X");
    ATTR_Y 							= XMLString::transcode("Y");
    ATTR_Lat 						= XMLString::transcode("Lat");
    ATTR_Lon 						= XMLString::transcode("Lon");

    TAG_relevantDirection 			= XMLString::transcode("relevantDirection");
    ATTR_radiansFromNorth 			= XMLString::transcode("radiansFromNorth");
    ATTR_accuracy 					= XMLString::transcode("accuracy");

    TAG_relevantStationTypes 		= XMLString::transcode("relevantStationTypes");
    ATTR_fixed 						= XMLString::transcode("fixed");
    ATTR_mobile 					= XMLString::transcode("mobile");

    TAG_relevantMessages 			= XMLString::transcode("relevantMessages");
    ATTR_cam 						= XMLString::transcode("cam");
    ATTR_denm 						= XMLString::transcode("denm");
    ATTR_topobroadcast              = XMLString::transcode("topobroadcast");
    ATTR_geobroadcast               = XMLString::transcode("geobroadcast");
    ATTR_geoanycast                 = XMLString::transcode("geoanycast");
    ATTR_unicast                    = XMLString::transcode("unicast");
    ATTR_multicast                  = XMLString::transcode("multicast");

    m_ConfigFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

LDMrulesGetConfig::~LDMrulesGetConfig() {
    delete[] TAG_LDMrules;

    delete[] TAG_defaultMessageLifeInterval;
    delete[] ATTR_value;

    delete[] TAG_relevantArea;

    delete[] TAG_lane;
    delete[] TAG_edge;
    delete[] TAG_junction;
    delete[] ATTR_ID;

    delete[] TAG_circle;
    delete[] ATTR_centerX;
    delete[] ATTR_centerY;
    delete[] ATTR_centerLat;
    delete[] ATTR_centerLon;
    delete[] ATTR_radius;

    delete[] TAG_ellipse;
    delete[] ATTR_focus1X;
    delete[] ATTR_focus1Y;
    delete[] ATTR_focus1Lat;
    delete[] ATTR_focus1Lon;
    delete[] ATTR_focus2X;
    delete[] ATTR_focus2Y;
    delete[] ATTR_focus2Lat;
    delete[] ATTR_focus2Lon;
    delete[] ATTR_eccentricity;
    delete[] ATTR_majorAxis;
    delete[] ATTR_minorAxis;
    delete[] ATTR_rotationAngleRadians;

    delete[] TAG_rectangle;
    delete[] ATTR_vertexAX;
    delete[] ATTR_vertexAY;
    delete[] ATTR_vertexALat;
    delete[] ATTR_vertexALon;
    delete[] ATTR_vertexBX;
    delete[] ATTR_vertexBY;
    delete[] ATTR_vertexBLat;
    delete[] ATTR_vertexBLon;
    delete[] ATTR_vertexCX;
    delete[] ATTR_vertexCY;
    delete[] ATTR_vertexCLat;
    delete[] ATTR_vertexCLon;
    delete[] ATTR_vertexDX;
    delete[] ATTR_vertexDY;
    delete[] ATTR_vertexDLat;
    delete[] ATTR_vertexDLon;
    delete[] ATTR_pointAX;
    delete[] ATTR_pointAY;
    delete[] ATTR_pointALat;
    delete[] ATTR_pointALon;
    delete[] ATTR_pointBX;
    delete[] ATTR_pointBY;
    delete[] ATTR_pointBLat;
    delete[] ATTR_pointBLon;
    delete[] ATTR_height;

    delete[] TAG_convexPolygon;
    delete[] TAG_vertex;
    delete[] ATTR_X;
    delete[] ATTR_Y;
    delete[] ATTR_Lat;
    delete[] ATTR_Lon;

    delete[] TAG_relevantDirection;
    delete[] ATTR_radiansFromNorth;
    delete[] ATTR_accuracy;

    delete[] TAG_relevantStationTypes;
    delete[] ATTR_fixed;
    delete[] ATTR_mobile;

    delete[] TAG_relevantMessages;
    delete[] ATTR_cam;
    delete[] ATTR_denm;
    delete[] ATTR_topobroadcast;
    delete[] ATTR_geobroadcast;
    delete[] ATTR_geoanycast;
    delete[] ATTR_unicast;
    delete[] ATTR_multicast;
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
LDMrulesGetConfig::readConfigFile(string& configFile)
throw(std::runtime_error) {
    // Test to see if the file is ok.

    struct stat fileStatus;

    if (stat(configFile.c_str(), &fileStatus)<0) {
        if (errno == ENOENT)
            throw(std::runtime_error("Could not find ldm configuration file '" + configFile + "'."));
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

        // Initialize useful variables
        DOMNodeList* def;
        DOMNode* node;
        DOMElement* element;

        // read defaultMessageLifeInterval
        def = elementRoot->getElementsByTagName(TAG_defaultMessageLifeInterval);
        if (def->getLength() == 1) {
            node = def->item(0);
            if (node->getNodeType() &&  // true is not NULL
                    node->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                element = static_cast< xercesc::DOMElement* >(node);
                const XMLCh* xmlch_string = element->getAttribute(ATTR_value);
                char* tmp_string_ch = XMLString::transcode(xmlch_string);
                sscanf(tmp_string_ch, "%u", &m_defaultMessageLifeInterval);
                delete[] tmp_string_ch;
            }
        }

        // read relevantArea, if defined
        def = elementRoot->getElementsByTagName(TAG_relevantArea);
        if (def->getLength() == 1) {
            node = def->item(0);
            if (node->getNodeType() &&  // true is not NULL
                    node->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >(node);
                DOMNodeList* children = currentElement->getChildNodes();

                for (XMLSize_t xx = 0; xx < children->getLength(); ++xx) {
                    DOMNode* _node = children->item(xx);
                    DOMElement* _element = static_cast< xercesc::DOMElement* >(_node);

                    // parse "circle"
                    if (XMLString::equals(_element->getTagName(), TAG_circle)) {
                        circle_str newCircle;

                        if (_element->hasAttribute(ATTR_centerX) &&
                                _element->hasAttribute(ATTR_centerY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerX);
                            char* tmp_stringX_ch = XMLString::transcode(xmlch_string);
                            newCircle.m_center.m_X = TplConvert::_2SUMOReal(tmp_stringX_ch);
                            delete[] tmp_stringX_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerY);
                            char* tmp_stringY_ch = XMLString::transcode(xmlch_string);
                            newCircle.m_center.m_Y = TplConvert::_2SUMOReal(tmp_stringY_ch);
                            delete[] tmp_stringY_ch;
                        } else {
                            newCircle.m_center.m_X = NAN;
                            newCircle.m_center.m_Y = NAN;

                        }

                        if (_element->hasAttribute(ATTR_centerLat) &&
                                _element->hasAttribute(ATTR_centerLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerLat);
                            char * tmp_stringLat_ch = XMLString::transcode(xmlch_string);
                            newCircle.m_center.m_Lat = TplConvert::_2SUMOReal(tmp_stringLat_ch);
                            delete[] tmp_stringLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerLon);
                            char* tmp_stringLon_ch = XMLString::transcode(xmlch_string);
                            newCircle.m_center.m_Lon = TplConvert::_2SUMOReal(tmp_stringLon_ch);
                            delete[] tmp_stringLon_ch;
                        } else {
                            newCircle.m_center.m_Lat = NAN;
                            newCircle.m_center.m_Lon = NAN;

                        }

                        if (_element->hasAttribute(ATTR_radius)) {
                            const XMLCh* xmlch_stringRadius = _element->getAttribute(ATTR_radius);
                            char* tmp_stringRadius_ch = XMLString::transcode(xmlch_stringRadius);
                            newCircle.m_radius = TplConvert::_2SUMOReal(tmp_stringRadius_ch);
                            delete[] tmp_stringRadius_ch;
                        } else newCircle.m_radius = NAN;

                        m_relevantCircles.push_back(newCircle);
                    }

                    // parse "ellipse"
                    if (XMLString::equals(_element->getTagName(), TAG_ellipse)) {
                        ellipse_str newEllipse;

                        if (_element->hasAttribute(ATTR_centerX) &&
                                _element->hasAttribute(ATTR_centerY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerX);
                            char* tmp_stringCenterX_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_center.m_X = TplConvert::_2SUMOReal(tmp_stringCenterX_ch);
                            delete[] tmp_stringCenterX_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerY);
                            char* tmp_stringCenterY_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_center.m_Y = TplConvert::_2SUMOReal(tmp_stringCenterY_ch);
                            delete[] tmp_stringCenterY_ch;
                        } else {
                            newEllipse.m_center.m_Y = NAN;
                            newEllipse.m_center.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_centerLat) &&
                                _element->hasAttribute(ATTR_centerLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerLat);
                            char* tmp_stringCenterLat_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_center.m_Lat = TplConvert::_2SUMOReal(tmp_stringCenterLat_ch);
                            delete[] tmp_stringCenterLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerLon);
                            char* tmp_stringCenterLon_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_center.m_Lon = TplConvert::_2SUMOReal(tmp_stringCenterLon_ch);
                            delete[] tmp_stringCenterLon_ch;
                        } else {
                            newEllipse.m_center.m_Lat = NAN;
                            newEllipse.m_center.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_focus1X) &&
                                _element->hasAttribute(ATTR_focus1Y)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_focus1X);
                            char* tmp_stringFocus1X_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus1.m_X = TplConvert::_2SUMOReal(tmp_stringFocus1X_ch);
                            delete[] tmp_stringFocus1X_ch;
                            xmlch_string = _element->getAttribute(ATTR_focus1Y);
                            char* tmp_stringFocus1Y_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus1.m_Y = TplConvert::_2SUMOReal(tmp_stringFocus1Y_ch);
                            delete[] tmp_stringFocus1Y_ch;
                        } else {
                            newEllipse.m_focus1.m_X = NAN;
                            newEllipse.m_focus1.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_centerLat) &&
                                _element->hasAttribute(ATTR_centerLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_focus1Lat);
                            char* tmp_stringFocus1Lat_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus1.m_Lat = TplConvert::_2SUMOReal(tmp_stringFocus1Lat_ch);
                            delete[] tmp_stringFocus1Lat_ch;
                            xmlch_string = _element->getAttribute(ATTR_focus1Lon);
                            char* tmp_stringFocus1Lon_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus1.m_Lon = TplConvert::_2SUMOReal(tmp_stringFocus1Lon_ch);
                            delete[] tmp_stringFocus1Lon_ch;
                        } else {
                            newEllipse.m_focus1.m_Lat = NAN;
                            newEllipse.m_focus1.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_focus2X) &&
                                _element->hasAttribute(ATTR_focus2Y)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_focus2X);
                            char* tmp_stringFocus2X_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus2.m_X = TplConvert::_2SUMOReal(tmp_stringFocus2X_ch);
                            delete[] tmp_stringFocus2X_ch;
                            xmlch_string = _element->getAttribute(ATTR_focus2Y);
                            char* tmp_stringFocus2Y_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus2.m_Y = TplConvert::_2SUMOReal(tmp_stringFocus2Y_ch);
                            delete[] tmp_stringFocus2Y_ch;
                        } else {
                            newEllipse.m_focus2.m_X = NAN;
                            newEllipse.m_focus2.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_centerLat) &&
                                _element->hasAttribute(ATTR_centerLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_focus2Lat);
                            char* tmp_stringFocus2Lat_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus2.m_Lat = TplConvert::_2SUMOReal(tmp_stringFocus2Lat_ch);
                            delete[] tmp_stringFocus2Lat_ch;
                            xmlch_string = _element->getAttribute(ATTR_focus2Lon);
                            char* tmp_stringFocus2Lon_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_focus2.m_Lon = TplConvert::_2SUMOReal(tmp_stringFocus2Lon_ch);
                            delete[] tmp_stringFocus2Lon_ch;
                        } else {
                            newEllipse.m_focus2.m_Lat = NAN;
                            newEllipse.m_focus2.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_eccentricity)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_eccentricity);
                            char* tmp_stringEcc_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_eccentricity = TplConvert::_2SUMOReal(tmp_stringEcc_ch);
                            delete[] tmp_stringEcc_ch;
                        } else newEllipse.m_eccentricity = NAN;

                        if (_element->hasAttribute(ATTR_minorAxis)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_minorAxis);
                            char* tmp_stringMinor_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_minorAxis = TplConvert::_2SUMOReal(tmp_stringMinor_ch);
                            delete[] tmp_stringMinor_ch;
                        } else newEllipse.m_minorAxis = NAN;

                        if (_element->hasAttribute(ATTR_majorAxis)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_majorAxis);
                            char* tmp_stringMajor_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_majorAxis = TplConvert::_2SUMOReal(tmp_stringMajor_ch);
                            delete[] tmp_stringMajor_ch;
                        } else newEllipse.m_majorAxis = NAN;

                        if (_element->hasAttribute(ATTR_rotationAngleRadians)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_rotationAngleRadians);
                            char* tmp_stringAngle_ch = XMLString::transcode(xmlch_string);
                            newEllipse.m_rotationAngleRadians = TplConvert::_2SUMOReal(tmp_stringAngle_ch);
                            delete[] tmp_stringAngle_ch;
                        } else newEllipse.m_rotationAngleRadians = NAN;

                        m_relevantEllipses.push_back(newEllipse);
                    }


                    // parse "rectangle"
                    if (XMLString::equals(_element->getTagName(), TAG_rectangle)) {
                        rectangle_str newRectangle;

                        if (_element->hasAttribute(ATTR_vertexAX) &&
                                _element->hasAttribute(ATTR_vertexAY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexAX);
                            char* tmp_stringVertexAX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexA.m_X = TplConvert::_2SUMOReal(tmp_stringVertexAX_ch);
                            delete[] tmp_stringVertexAX_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexAY);
                            char* tmp_stringVertexAY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexA.m_Y = TplConvert::_2SUMOReal(tmp_stringVertexAY_ch);
                            delete[] tmp_stringVertexAY_ch;
                        } else {
                            newRectangle.m_vertexA.m_X = NAN;
                            newRectangle.m_vertexA.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexALat) &&
                                _element->hasAttribute(ATTR_vertexALon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexALat);
                            char* tmp_stringVertexALat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexA.m_Lat = TplConvert::_2SUMOReal(tmp_stringVertexALat_ch);
                            delete[] tmp_stringVertexALat_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexALon);
                            char* tmp_stringVertexALon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexA.m_Lon = TplConvert::_2SUMOReal(tmp_stringVertexALon_ch);
                            delete[] tmp_stringVertexALon_ch;
                        } else {
                            newRectangle.m_vertexA.m_Lat = NAN;
                            newRectangle.m_vertexA.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexBX) &&
                                _element->hasAttribute(ATTR_vertexBY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexBX);
                            char* tmp_stringVertexBX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexB.m_X = TplConvert::_2SUMOReal(tmp_stringVertexBX_ch);
                            delete[] tmp_stringVertexBX_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexBY);
                            char* tmp_stringVertexBY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexB.m_Y = TplConvert::_2SUMOReal(tmp_stringVertexBY_ch);
                            delete[] tmp_stringVertexBY_ch;
                        } else {
                            newRectangle.m_vertexB.m_X = NAN;
                            newRectangle.m_vertexB.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexBLat) &&
                                _element->hasAttribute(ATTR_vertexBLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexBLat);
                            char* tmp_stringVertexBLat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexB.m_Lat = TplConvert::_2SUMOReal(tmp_stringVertexBLat_ch);
                            delete[] tmp_stringVertexBLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexBLon);
                            char* tmp_stringVertexBLon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexB.m_Lon = TplConvert::_2SUMOReal(tmp_stringVertexBLon_ch);
                            delete[] tmp_stringVertexBLon_ch;
                        } else {
                            newRectangle.m_vertexB.m_Lat = NAN;
                            newRectangle.m_vertexB.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexCX) &&
                                _element->hasAttribute(ATTR_vertexCY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexCX);
                            char* tmp_stringVertexCX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexC.m_X = TplConvert::_2SUMOReal(tmp_stringVertexCX_ch);
                            delete[] tmp_stringVertexCX_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexCY);
                            char* tmp_stringVertexCY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexC.m_Y = TplConvert::_2SUMOReal(tmp_stringVertexCY_ch);
                            delete[] tmp_stringVertexCY_ch;
                        } else {
                            newRectangle.m_vertexC.m_X = NAN;
                            newRectangle.m_vertexC.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexCLat) &&
                                _element->hasAttribute(ATTR_vertexCLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexCLat);
                            char* tmp_stringVertexCLat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexC.m_Lat = TplConvert::_2SUMOReal(tmp_stringVertexCLat_ch);
                            delete[] tmp_stringVertexCLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexCLon);
                            char* tmp_stringVertexCLon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexC.m_Lon = TplConvert::_2SUMOReal(tmp_stringVertexCLon_ch);
                            delete[] tmp_stringVertexCLon_ch;
                        } else {
                            newRectangle.m_vertexC.m_Lat = NAN;
                            newRectangle.m_vertexC.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexDX) &&
                                _element->hasAttribute(ATTR_vertexDY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexDX);
                            char* tmp_stringVertexDX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexD.m_X = TplConvert::_2SUMOReal(tmp_stringVertexDX_ch);
                            delete[] tmp_stringVertexDX_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexDY);
                            char* tmp_stringVertexDY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexD.m_Y = TplConvert::_2SUMOReal(tmp_stringVertexDY_ch);
                            delete[] tmp_stringVertexDY_ch;
                        } else {
                            newRectangle.m_vertexD.m_X = NAN;
                            newRectangle.m_vertexD.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_vertexDLat) &&
                                _element->hasAttribute(ATTR_vertexDLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_vertexDLat);
                            char* tmp_stringVertexDLat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexD.m_Lat = TplConvert::_2SUMOReal(tmp_stringVertexDLat_ch);
                            delete[] tmp_stringVertexDLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_vertexDLon);
                            char* tmp_stringVertexDLon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_vertexD.m_Lon = TplConvert::_2SUMOReal(tmp_stringVertexDLon_ch);
                            delete[] tmp_stringVertexDLon_ch;
                        } else {
                            newRectangle.m_vertexD.m_Lat = NAN;
                            newRectangle.m_vertexD.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_centerX) &&
                                _element->hasAttribute(ATTR_centerY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerX);
                            char* tmp_stringCenterX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_center.m_X = TplConvert::_2SUMOReal(tmp_stringCenterX_ch);
                            delete[] tmp_stringCenterX_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerY);
                            char* tmp_stringCenterY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_center.m_Y = TplConvert::_2SUMOReal(tmp_stringCenterY_ch);
                            delete[] tmp_stringCenterY_ch;
                        } else {
                            newRectangle.m_center.m_X = NAN;
                            newRectangle.m_center.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_centerLat) &&
                                _element->hasAttribute(ATTR_centerLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_centerLat);
                            char* tmp_stringCenterLat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_center.m_Lat = TplConvert::_2SUMOReal(tmp_stringCenterLat_ch);
                            delete[] tmp_stringCenterLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_centerLon);
                            char* tmp_stringCenterLon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_center.m_Lon = TplConvert::_2SUMOReal(tmp_stringCenterLon_ch);
                            delete[] tmp_stringCenterLon_ch;
                        } else {
                            newRectangle.m_center.m_Lat = NAN;
                            newRectangle.m_center.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_pointAX) &&
                                _element->hasAttribute(ATTR_pointAY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_pointAX);
                            char* tmp_stringPointAX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointA.m_X = TplConvert::_2SUMOReal(tmp_stringPointAX_ch);
                            delete[] tmp_stringPointAX_ch;
                            xmlch_string = _element->getAttribute(ATTR_pointAY);
                            char* tmp_stringPointAY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointA.m_Y = TplConvert::_2SUMOReal(tmp_stringPointAY_ch);
                            delete[] tmp_stringPointAY_ch;
                        } else {
                            newRectangle.m_pointA.m_X = NAN;
                            newRectangle.m_pointA.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_pointALat) &&
                                _element->hasAttribute(ATTR_pointALon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_pointALat);
                            char* tmp_stringPointALat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointA.m_Lat = TplConvert::_2SUMOReal(tmp_stringPointALat_ch);
                            delete[] tmp_stringPointALat_ch;
                            xmlch_string = _element->getAttribute(ATTR_pointALon);
                            char* tmp_stringPointALon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointA.m_Lon = TplConvert::_2SUMOReal(tmp_stringPointALon_ch);
                            delete[] tmp_stringPointALon_ch;
                        } else {
                            newRectangle.m_pointA.m_Lat = NAN;
                            newRectangle.m_pointA.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_pointBX) &&
                                _element->hasAttribute(ATTR_pointBY)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_pointBX);
                            char* tmp_stringPointBX_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointB.m_X = TplConvert::_2SUMOReal(tmp_stringPointBX_ch);
                            delete[] tmp_stringPointBX_ch;
                            xmlch_string = _element->getAttribute(ATTR_pointBY);
                            char* tmp_stringPointBY_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointB.m_Y = TplConvert::_2SUMOReal(tmp_stringPointBY_ch);
                            delete[] tmp_stringPointBY_ch;
                        } else {
                            newRectangle.m_pointB.m_X = NAN;
                            newRectangle.m_pointB.m_Y = NAN;
                        }

                        if (_element->hasAttribute(ATTR_pointBLat) &&
                                _element->hasAttribute(ATTR_pointBLon)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_pointBLat);
                            char* tmp_stringPointBLat_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointB.m_Lat = TplConvert::_2SUMOReal(tmp_stringPointBLat_ch);
                            delete[] tmp_stringPointBLat_ch;
                            xmlch_string = _element->getAttribute(ATTR_pointBLon);
                            char* tmp_stringPointBLon_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_pointB.m_Lon = TplConvert::_2SUMOReal(tmp_stringPointBLon_ch);
                            delete[] tmp_stringPointBLon_ch;
                        } else {
                            newRectangle.m_pointB.m_Lat = NAN;
                            newRectangle.m_pointB.m_Lon = NAN;
                        }

                        if (_element->hasAttribute(ATTR_height)) {
                            const XMLCh* xmlch_string = _element->getAttribute(ATTR_height);
                            char* tmp_stringHeight_ch = XMLString::transcode(xmlch_string);
                            newRectangle.m_height = TplConvert::_2SUMOReal(tmp_stringHeight_ch);
                            delete[] tmp_stringHeight_ch;
                        } else newRectangle.m_height = NAN;

                        m_relevantRectangles.push_back(newRectangle);
                    }

                    if (XMLString::equals(_element->getTagName(), TAG_convexPolygon)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        convexPolygon_str newConvexPolygon;

                        for (XMLSize_t yy = 0; yy < children2->getLength(); ++yy) {
                            DOMNode* __node = children2->item(yy);
                            DOMElement* __element = static_cast< xercesc::DOMElement* >(__node);
                            // parse "vertex"
                            if (XMLString::equals(__element->getTagName(), TAG_vertex)) {
                                point_str vertex;

                                if (__element->hasAttribute(ATTR_X) &&
                                        __element->hasAttribute(ATTR_Y)) {
                                    const XMLCh* xmlch_stringX = __element->getAttribute(ATTR_X);
                                    char* tmp_stringX_ch = XMLString::transcode(xmlch_stringX);
                                    vertex.m_X = TplConvert::_2SUMOReal(tmp_stringX_ch);
                                    delete[] tmp_stringX_ch;
                                    const XMLCh* xmlch_stringY = __element->getAttribute(ATTR_Y);
                                    char* tmp_stringY_ch = XMLString::transcode(xmlch_stringY);
                                    vertex.m_Y = TplConvert::_2SUMOReal(tmp_stringY_ch);
                                    delete[] tmp_stringY_ch;
                                } else {
                                    vertex.m_X = NAN;
                                    vertex.m_Y = NAN;
                                }

                                if (__element->hasAttribute(ATTR_X) &&
                                        __element->hasAttribute(ATTR_Y)) {
                                    const XMLCh* xmlch_stringLat = __element->getAttribute(ATTR_Lat);
                                    char* tmp_stringLat_ch = XMLString::transcode(xmlch_stringLat);
                                    vertex.m_Lat = TplConvert::_2SUMOReal(tmp_stringLat_ch);
                                    delete[] tmp_stringLat_ch;
                                    const XMLCh* xmlch_stringLon = __element->getAttribute(ATTR_Lon);
                                    char* tmp_stringLon_ch = XMLString::transcode(xmlch_stringLon);
                                    vertex.m_Lon = TplConvert::_2SUMOReal(tmp_stringLon_ch);
                                    delete[] tmp_stringLon_ch;
                                } else {
                                    vertex.m_Lat = NAN;
                                    vertex.m_Lon = NAN;
                                }
                                newConvexPolygon.m_verteces.push_back(vertex);
                            }
                        }
                        m_relevantConvexPolygons.push_back(newConvexPolygon);
                    }

                    if (XMLString::equals(_element->getTagName(), TAG_lane)) {
                        lane_str newLane;
                        const XMLCh* xmlch_string = _element->getAttribute(ATTR_ID);
                        char* tmp_stringID_ch = XMLString::transcode(xmlch_string);
                        newLane.ID = (char*) tmp_stringID_ch;
                        delete[] tmp_stringID_ch;

                        m_relevantLanes.push_back(newLane);
                    }

                    if (XMLString::equals(_element->getTagName(), TAG_edge)) {
                        edge_str newEdge;
                        const XMLCh* xmlch_string = _element->getAttribute(ATTR_ID);
                        char* tmp_stringID_ch = XMLString::transcode(xmlch_string);
                        newEdge.ID = (char*) tmp_stringID_ch;
                        delete[] tmp_stringID_ch;

                        m_relevantEdges.push_back(newEdge);
                    }

                    if (XMLString::equals(_element->getTagName(), TAG_junction)) {
                        junction_str newJunction;
                        const XMLCh* xmlch_string = _element->getAttribute(ATTR_ID);
                        char* tmp_stringID_ch = XMLString::transcode(xmlch_string);
                        newJunction.ID = (char*) tmp_stringID_ch;
                        delete[] tmp_stringID_ch;

                        m_relevantJunctions.push_back(newJunction);
                    }

                }
            } // end relevant area
        }

        // read relevantDirection, if defined
        def = elementRoot->getElementsByTagName(TAG_relevantDirection);
        if (def->getLength() == 1) {
            m_relevantDirection.m_defined = true;
            node = def->item(0);
            if (node->getNodeType() &&  // true is not NULL
                    node->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                element = static_cast< xercesc::DOMElement* >(node);
                const XMLCh* xmlch_stringAng = element->getAttribute(ATTR_radiansFromNorth);
                char* tmp_stringAngle_ch = XMLString::transcode(xmlch_stringAng);
                m_relevantDirection.m_radiansFromNorth = TplConvert::_2SUMOReal(tmp_stringAngle_ch);
                delete[] tmp_stringAngle_ch;
                const XMLCh* xmlch_stringAcc = element->getAttribute(ATTR_accuracy);
                char* tmp_stringAccuracy_ch = XMLString::transcode(xmlch_stringAcc);
                m_relevantDirection.m_accuracy = TplConvert::_2SUMOReal(tmp_stringAccuracy_ch);
                delete[] tmp_stringAccuracy_ch;
            }
        } else
            m_relevantDirection.m_defined = false;

        // read relevantStationTypes, if defined
        def = elementRoot->getElementsByTagName(TAG_relevantStationTypes);
        if (def->getLength() == 1) {
            node = def->item(0);
            if (node->getNodeType() &&  // true is not NULL
                    node->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                element = static_cast< xercesc::DOMElement* >(node);
                const XMLCh* xmlch_stringFixed = element->getAttribute(ATTR_fixed);
                char* tmp_stringFixed_ch = XMLString::transcode(xmlch_stringFixed);
                m_relevantStationTypes.m_fixed = (bool)TplConvert::_2int(tmp_stringFixed_ch);
                delete[] tmp_stringFixed_ch;
                const XMLCh* xmlch_stringMobile = element->getAttribute(ATTR_mobile);
                char* tmp_stringMobile_ch = XMLString::transcode(xmlch_stringMobile);
                m_relevantStationTypes.m_mobile = (bool)TplConvert::_2int(tmp_stringMobile_ch);
                delete[] tmp_stringMobile_ch;
            }
        }

        // read relevantMessages, if defined
        def = elementRoot->getElementsByTagName(TAG_relevantMessages);
        if (def->getLength() == 1) {
            node = def->item(0);
            if (node->getNodeType() &&  // true is not NULL
                    node->getNodeType() == DOMNode::ELEMENT_NODE) { // is element
                element = static_cast< xercesc::DOMElement* >(node);
                const XMLCh* xmlch_stringCAM = element->getAttribute(ATTR_cam);
                char* tmp_stringCAM_ch = XMLString::transcode(xmlch_stringCAM);
                m_relevantMessages.m_cam = (bool)TplConvert::_2int(tmp_stringCAM_ch);
                delete[] tmp_stringCAM_ch;
                const XMLCh* xmlch_stringDENM = element->getAttribute(ATTR_denm);
                char* tmp_stringDENM_ch = XMLString::transcode(xmlch_stringDENM);
                m_relevantMessages.m_denm = (bool)TplConvert::_2int(tmp_stringDENM_ch);
                delete[] tmp_stringDENM_ch;
                const XMLCh* xmlch_stringBROADCAST = element->getAttribute(ATTR_topobroadcast);
                char* tmp_stringTOPOBROADCAST_ch = XMLString::transcode(xmlch_stringBROADCAST);
                m_relevantMessages.m_topobroadcast = (bool)TplConvert::_2int(tmp_stringTOPOBROADCAST_ch);
                delete[] tmp_stringTOPOBROADCAST_ch;
                const XMLCh* xmlch_stringGEOBROADCAST = element->getAttribute(ATTR_geobroadcast);
                char* tmp_stringGEOBROADCAST_ch = XMLString::transcode(xmlch_stringGEOBROADCAST);
                m_relevantMessages.m_geobroadcast = (bool)TplConvert::_2int(tmp_stringGEOBROADCAST_ch);
                delete[] tmp_stringGEOBROADCAST_ch;
                const XMLCh* xmlch_stringGEOANYCAST = element->getAttribute(ATTR_geoanycast);
                char* tmp_stringGEOANYCAST_ch = XMLString::transcode(xmlch_stringGEOANYCAST);
                m_relevantMessages.m_geoanycast = (bool)TplConvert::_2int(tmp_stringGEOANYCAST_ch);
                delete[] tmp_stringGEOANYCAST_ch;
                const XMLCh* xmlch_stringUNICAST = element->getAttribute(ATTR_unicast);
                char* tmp_stringUNICAST_ch = XMLString::transcode(xmlch_stringUNICAST);
                m_relevantMessages.m_unicast = (bool)TplConvert::_2int(tmp_stringUNICAST_ch);
                delete[] tmp_stringUNICAST_ch;
                const XMLCh* xmlch_stringMULTICAST = element->getAttribute(ATTR_multicast);
                char* tmp_stringMULTICAST_ch = XMLString::transcode(xmlch_stringMULTICAST);
                m_relevantMessages.m_multicast = (bool)TplConvert::_2int(tmp_stringMULTICAST_ch);
                delete[] tmp_stringMULTICAST_ch;
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

icstime_t                       LDMrulesGetConfig::getDefaultMessageLifeInterval() {
    return m_defaultMessageLifeInterval;
}

vector <lane_str>              LDMrulesGetConfig::getRelevantLanes() {
    return m_relevantLanes;
}

vector <edge_str>              LDMrulesGetConfig::getRelevantEdges() {
    return m_relevantEdges;
}

vector <junction_str>          LDMrulesGetConfig::getRelevantJunctions() {
    return m_relevantJunctions;
}

vector <circle_str>            LDMrulesGetConfig::getRelevantCircles() {
    return m_relevantCircles;
}

vector <ellipse_str>           LDMrulesGetConfig::getRelevantEllipses() {
    return m_relevantEllipses;
}

vector <rectangle_str>         LDMrulesGetConfig::getRelevantRectangles() {
    return m_relevantRectangles;
}

vector <convexPolygon_str>     LDMrulesGetConfig::getRelevantConvexPolygons() {
    return m_relevantConvexPolygons;
}

relevantDirection_str          LDMrulesGetConfig::getRelevantDirection() {
    return m_relevantDirection;
}

relevantStationTypes_str       LDMrulesGetConfig::getRelevantStationTypes() {
    return m_relevantStationTypes;
}

relevantMessages_str           LDMrulesGetConfig::getRelevantMessages() {
    return m_relevantMessages;
}



}
