/****************************************************************************/
/// @file    apps-configfile-parser.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
/// @todo Comments pending.
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef APPCONFIGFILE_PARSER_H
#define APPCONFIGFILE_PARSER_H

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

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

namespace ics {

// ===========================================================================
// struct defintions
// ===========================================================================

/**
 * @struct Application
 * @brief Struct to store information from application config file.
 * @todo To be commented.
 */
struct Application {
    char* m_name;
    char* m_executable;
    char* m_ip;
    char* m_port;
    char* m_seed;
    char* m_rate;
    char* m_resultContainer;
    char* m_unicastServiceId;
    char* m_multicastServiceId;
    char* m_broadcastServiceId;
    char* m_geoBroadcastServiceId;
    char* m_topoBroadcastServiceId;
    vector<char*> m_stations;
};

// ===========================================================================
// class definiton
// ===========================================================================
class AppConfigFileParse {

public:
    /**
     * @brief  Constructor initializes xerces-C libraries.
     *  The XML tags and attributes which we seek are defined.
     *  The xerces-C DOM parser infrastructure is initialized.
     */
    AppConfigFileParse();

    /// @brief Destructor·
    ~AppConfigFileParse();

    /**
    * @brief Reads the information from the config file.
    * @param [in] . The path of the config file.
    */
    void readConfigFile(std::string&) throw(std::runtime_error);

    /**
    * @brief Fill the struct with the information read by the application config file.
    * @return A collection of applications configuration.
    */
    vector<Application*>* GetApplicationsConfig();

private:

    /// @brief Xerces DOM parser.
    xercesc::XercesDOMParser* m_ConfigFileParser;

    /// @brief Pointer to the information of the application config file.
    vector<Application*>* m_ApplicationCollection;

    /// @brief Root tag.
    XMLCh* TAG_Applications;

    /// @brief Application tag.
    XMLCh* TAG_Application;

    /// @brief Name tag.
    XMLCh* TAG_Name;

    /// @brief Executable tag.
    XMLCh* TAG_Executable;

    /// @brief Ip tag.
    XMLCh* TAG_Ip;

    /// @brief Port tag.
    XMLCh* TAG_Port;

    /// @brief Random seed tag.
    XMLCh* TAG_Seed;

    /// @brief Rate tag.
    XMLCh* TAG_Rate;

    /// @brief ResultContainer tag.
    XMLCh* TAG_ResultContainer;

    /// @brief Service Id tag.
    XMLCh* TAG_ServiceId;

    /// @brief Unicast service id tag.
    XMLCh* ATTR_Unicast;

    /// @brief Multicast service id tag.
    XMLCh* ATTR_Multicast;

    /// @brief Broadcast service id tag.
    XMLCh* ATTR_Broadcast;

    /// @brief Topobroadcast service id tag.
    XMLCh* ATTR_TopoBroacast;

    /// @brief Geobroadcast service id tag.
    XMLCh* ATTR_GeoBroacast;

    /// @brief Station tag.
    XMLCh* TAG_Stations;

    /// @brief Station id tag.
    XMLCh* TAG_Id;
};

}

#endif
