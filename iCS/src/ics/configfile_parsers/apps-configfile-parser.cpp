/****************************************************************************/
/// @file    apps-configfile-parser.cpp
/// @author  Julen Maneros
/// @date
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

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>

#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>

#include "apps-configfile-parser.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace xercesc;

namespace ics {

AppConfigFileParse::AppConfigFileParse() {
    m_ApplicationCollection = new vector<Application*>();

    try {
        XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
    } catch (XMLException& e) {
        char* message = XMLString::transcode(e.getMessage());
        cerr << "XML toolkit initialization error: " << message << endl;
        XMLString::release(&message);
        // throw exception here to return ERROR_XERCES_INIT
    }

    TAG_Applications = XMLString::transcode("Applications");
    TAG_Application = XMLString::transcode("Application");
    TAG_Name = XMLString::transcode("name");
    TAG_Executable = XMLString::transcode("executable");
    TAG_Ip = XMLString::transcode("ip");
    TAG_Port = XMLString::transcode("port");
    TAG_Seed = XMLString::transcode("seed");
    TAG_Rate = XMLString::transcode("rate");
    TAG_ResultContainer = XMLString::transcode("result-container");
    TAG_Stations = XMLString::transcode("stations");
    TAG_Id = XMLString::transcode("id");
    TAG_ServiceId = XMLString::transcode("serviceId");
    ATTR_Unicast= XMLString::transcode("unicast");
    ATTR_Multicast = XMLString::transcode("multicast");
    ATTR_Broadcast = XMLString::transcode("broadcast");
    ATTR_TopoBroacast = XMLString::transcode("topobroadcast");
    ATTR_GeoBroacast = XMLString::transcode("geobroadcast");

    m_ConfigFileParser = new XercesDOMParser;
}

AppConfigFileParse::~AppConfigFileParse() {
    delete m_ApplicationCollection;
    delete m_ConfigFileParser;
    delete[] TAG_Applications;
    delete[] TAG_Application;
    delete[] TAG_Name;
    delete[] TAG_Executable;
    delete[] TAG_Ip;
    delete[] TAG_Port;
    delete[] TAG_Seed;
    delete[] TAG_Rate;
    delete[] TAG_ResultContainer;
    delete[] TAG_Stations;
    delete[] TAG_Id;
    delete[] TAG_ServiceId;
    delete[] ATTR_Unicast;
    delete[] ATTR_Multicast;
    delete[] ATTR_Broadcast;
    delete[] ATTR_TopoBroacast;
    delete[] ATTR_GeoBroacast;
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
AppConfigFileParse::readConfigFile(string& configFile)
throw(std::runtime_error) {
    // Test to see if the file is ok.

    struct stat fileStatus;

    if (stat(configFile.c_str(), &fileStatus)<0) {
        if (errno == ENOENT)
            throw(std::runtime_error("Could not find application(s) configuration file '" + configFile + "'."));
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

        DOMNodeList* applications = elementRoot->getElementsByTagName(TAG_Application);
        XMLSize_t nodeCount = applications->getLength();

        // Loop all <Application> tags
        for (XMLSize_t xx = 0; xx < nodeCount; ++xx) {

            DOMNode* currentNode = applications->item(xx);

            if (currentNode->getNodeType() &&  // true is not NULL
                    currentNode->getNodeType() == DOMNode::ELEMENT_NODE) { // is element

                Application* app = new Application();

                DOMElement* currentElememt = dynamic_cast< xercesc::DOMElement* >(currentNode);

                DOMNodeList* children = currentElememt->getChildNodes();

                for (XMLSize_t yy = 0; yy < children->getLength(); ++yy) {

                    DOMNode* _node = children->item(yy);

                    DOMElement* _element = static_cast< xercesc::DOMElement* >(_node);

                    // Application name
                    if (XMLString::equals(_element->getTagName(), TAG_Name)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_name = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application executable
                    if (XMLString::equals(_element->getTagName(), TAG_Executable)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_executable = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application IP
                    if (XMLString::equals(_element->getTagName(), TAG_Ip)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_ip = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application Port
                    if (XMLString::equals(_element->getTagName(), TAG_Port)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_port = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application seed
                    if (XMLString::equals(_element->getTagName(), TAG_Seed)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_seed = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application Rate
                    if (XMLString::equals(_element->getTagName(), TAG_Rate)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_rate = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application Result Container
                    if (XMLString::equals(_element->getTagName(), TAG_ResultContainer)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            //std::cout << XMLString::transcode(children2->item(index)->getNodeValue()) << std::endl;
                            app->m_resultContainer = XMLString::transcode(children2->item(index)->getNodeValue());
                        }
                    }

                    // Application Service Id
                    if (XMLString::equals(_element->getTagName(), TAG_ServiceId)) {
                        const XMLCh* xmlch_Unicast  = _element->getAttribute(ATTR_Unicast);
                        app->m_unicastServiceId = XMLString::transcode(xmlch_Unicast);
                        const XMLCh* xmlch_Multicast  = _element->getAttribute(ATTR_Multicast);
                        app->m_multicastServiceId = XMLString::transcode(xmlch_Multicast);
                        const XMLCh* xmlch_Broadcast  = _element->getAttribute(ATTR_Broadcast);
                        app->m_broadcastServiceId = XMLString::transcode(xmlch_Broadcast);
                        const XMLCh* xmlch_TopoBroadcast  = _element->getAttribute(ATTR_TopoBroacast);
                        app->m_topoBroadcastServiceId = XMLString::transcode(xmlch_TopoBroadcast);
                        const XMLCh* xmlch_GeoBroadcast  = _element->getAttribute(ATTR_GeoBroacast);
                        app->m_geoBroadcastServiceId = XMLString::transcode(xmlch_GeoBroadcast);
                    }

                    // Applications nodes ID (nodes that will have installed the application regardless of the penetration rates)
                    if (XMLString::equals(_element->getTagName(), TAG_Stations)) {
                        DOMNodeList* children2 = _element->getChildNodes();
                        for (XMLSize_t index = 0; index < children2->getLength(); ++index) {
                            DOMNode* __node = children2->item(index);
                            DOMElement* __element = static_cast< xercesc::DOMElement* >(__node);
                            if (XMLString::equals(__element->getTagName(), TAG_Id)) {
                                DOMNodeList* children3 = __element->getChildNodes();
                                for (XMLSize_t index2 = 0; index2 < children3->getLength(); ++index2) {
                                    //std::cout << XMLString::transcode(children3->item(index2)->getNodeValue()) << std::endl;
                                    app->m_stations.push_back(XMLString::transcode(children3->item(index2)->getNodeValue()));
                                }
                            }
                        }
                    }
                }

                m_ApplicationCollection->push_back(app);
            }
        }
    } catch (xercesc::XMLException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        ostringstream errBuf;
        errBuf << "Error parsing file: " << message << flush;
        XMLString::release(&message);
    }
}

vector<Application*>*
AppConfigFileParse::GetApplicationsConfig() {
    return m_ApplicationCollection;
}



}
