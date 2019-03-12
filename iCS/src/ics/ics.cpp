/****************************************************************************/
/// @file    ics.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 ***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author:  Jerome Haerri (jerome.haerri@eurecom.fr) and T.-T. Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM
 * version: 1.0
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
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <time.h>
#include <map>
#include <set>

#include "ics.h"
#include "configfile_parsers/apps-configfile-parser.h"
#include "sync-manager.h"
#include "itetris-node.h"
#include "fixed-node.h"
#include "itetris-simulation-config.h"
#include "applications_manager/application-handler.h"
#include "applications_manager/app-commands-subscriptions-constants.h"
#include "applications_manager/app-result-maximum-speed.h"
#include "applications_manager/app-result-void.h"
#include "applications_manager/app-result-generic.h"
#include "applications_manager/app-result-travel-time.h"
#include "applications_manager/app-result-traffic-jam-detection.h"
#include "FacilitiesManager.h"
#include "../utils/ics/log/ics-log.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace ics_types;

// ===========================================================================
// static method definitions
// ===========================================================================
void *launchApplication(void *ptr)
{
	char *message;
	message = (char *) ptr;
	system(message);
	return 0;
}

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
vector<pthread_t> ICS::m_applicationsThreads;

// ===========================================================================
// member method definitions
// ===========================================================================
ICS::ICS(int ns3Port, int sumoPort, std::string sumoHost, std::string ns3Host,
		int beginTime, int endTime, int resolution, int ratPenetrationRate,
		const std::vector<std::string>& ratIdentifiers, bool interactive)
{
	m_Interactive = interactive;
	m_syncManager = new SyncManager(ns3Port, sumoPort, sumoHost, ns3Host, beginTime, endTime, resolution);

    ITetrisSimulationConfig::m_simulatedVehiclesPenetrationRate = ratPenetrationRate;
    ITetrisSimulationConfig::RATIdentifiersList = ratIdentifiers;
}

ICS::~ICS()
{
	delete m_syncManager;
}

int
ICS::Setup(string facilitiesConfigFile, string appConfigFile)
{
	if (m_Interactive) {
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
		cout << "STEP 2 - SETUP PHASE" << endl;
		cout << "====================" << endl;
		cout << "[ns-3] LAUNCHED." << endl;
		cout << "[APP]  OFF. " << endl;
		cout << "[iCS]  (1) FACILITIES BLOCK IS INITIALIZED." << endl;
		cout << "       (2) READ APPLICATIONS CONFIGURATIONS FILE TO GET THE CONNECTION INFORMATION AND START THEM."  << endl;
		cout << "[SUMO] LAUNCHED." << endl << endl;
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
	} else {
		cout << endl << "STEP 2 - SETUP PHASE" << endl;
	}

	if (ReadFacilitiesConfigFile(facilitiesConfigFile) == EXIT_FAILURE) {
		cout << "iCS --> ERROR Initializing facilities." << endl;
		cout << endl;
		return EXIT_FAILURE;
	}

#ifdef APPLICATIONS_ON
	// Check the existence of the appConfigFile

	if (!FileHelpers::isReadable(appConfigFile)) {
		cout << "iCS --> ERROR application configuration file '" << appConfigFile << "' not found." << endl;
		return EXIT_FAILURE;
	}

	if (SetupApplications(appConfigFile) == EXIT_FAILURE) {
		cout << "iCS --> ERROR reading application configuration file." << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
#endif

	if (m_Interactive) {
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
		cout << "STEP 3 - SETUP PHASE" << endl;
		cout << "====================" << endl;
		cout << "[ns-3] WAITING FOR CONNECTION." << endl;
		cout << "[APP]  WAITING FOR CONNECTION. " << endl;
		cout << "[iCS]  ESTABLISH CONNECTION VIA SOCKET TO THE SIMULATORS & APPLICATIONS" << endl;
		cout << "[SUMO] WAITING FOR CONNECTION." << endl << endl;
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
	} else {
		cout << endl << "STEP 3 - SETUP PHASE" << endl;
	}

	int status = EXIT_SUCCESS;
#ifdef SUMO_ON
	status = m_syncManager->ConnectSumo();
	if (status == EXIT_FAILURE) {
		cout << "iCS -->  Could not connect to SUMO" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
	cout << "iCS --> Connected to SUMO" << endl;
	cout << endl;
#endif

#ifdef NS3_ON
	status = m_syncManager->ConnectNs3();
	if (status == EXIT_FAILURE) {
		cout << "iCS --> Could not connect to Ns-3" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
	cout << "iCS --> Connected to ns-3" << endl;
	cout << endl;

	Sleep(1000); //This sleep is just to be sure the next line is always showed after the ns3-server message
#endif

#ifdef APPLICATIONS_ON
	status = m_syncManager->ConnectToApps();
	if (status == EXIT_FAILURE) {
		cout << "iCS --> Could not connect to the Applications" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
	cout << endl;
#endif

	if (m_Interactive) {
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
		cout << "STEP 4 - SETUP PHASE" << endl;
		cout << "====================" << endl;
		cout << "[ns-3] SET FIXED UNITS." << endl;
		cout << "[APP]  WAITING." << endl;
		cout << "[iCS]	(1) SEND MESSAGES TO ns-3 TO SETUP FIXED UNITS." << endl;
		cout << " 	(2) COMMUNICATE WITH APPLICATIONS RUNNING ON FIXED UNITS" << endl;
		cout << "[SUMO] WAITING." << endl << endl;
		utils::Conversion::Wait("Press <Enter> to continue...");
		cout << endl;
	} else {
		cout << endl << "STEP 4 - SETUP PHASE" << endl;
	}

#ifdef NS3_ON
	status = m_syncManager->SetFixedStationInNs3();
	if (status == EXIT_FAILURE) {
		cout << "iCS --> ERROR setting fixed stations in ns-3" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
#endif

#ifdef APPLICATIONS_ON
	status = m_syncManager->RunApplicationLogic();
	if (status == EXIT_FAILURE) {
		cout << "iCS --> ERROR running application logic in setup phase" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
#endif

#ifdef NS3_ON
	cout << "iCS --> Starting scheduling V2X messages in setup phase" << endl;
	status = m_syncManager->ScheduleV2xMessages();
	if (status == EXIT_FAILURE) {
		cout << "iCS --> ERROR scheduling V2X messages in setup phase" << endl;
		cout << endl;
		return EXIT_FAILURE;
	}
#endif

	if (status == EXIT_SUCCESS) {
		cout << endl;
		cout << "**********************" << endl;
		cout << "SETUP PHASE ENDS HERE." << endl << endl;
		if (m_Interactive) {
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		}
	}

	return EXIT_SUCCESS;
}

int
ICS::Run()
{
	if (m_syncManager->Run(m_Interactive) == EXIT_FAILURE) {
		return m_syncManager->Stop();
	}

	return m_syncManager->Stop();
}

int
ICS::Close()
{
	if (m_syncManager->CloseSumo() == EXIT_SUCCESS) {
		return m_syncManager->CloseNs3();
	}
	return EXIT_FAILURE;
}

int
ICS::ReadFacilitiesConfigFile(string& filePath)
{
	// Initialized --read the files-- facilities
	if ((m_syncManager->InitializeFacilities(filePath)) == EXIT_FAILURE) {
		cout << "iCS --> Error setting up the facilities." << endl;
		return EXIT_FAILURE;
	}

	m_syncManager->m_facilitiesManager->updateClock(m_syncManager->m_simStep);

	// Get fixed units data
	map<stationID_t, Point2D>* positions =
			SyncManager::m_facilitiesManager->getAllFixedStationsPositions();
	for (map<stationID_t, Point2D>::iterator it = positions->begin(); it != positions->end(); it++) {
		string rat = SyncManager::m_facilitiesManager->getFixedStationCommunicationProfile((*it).first);

#ifdef LOG_ON
		stringstream log;
		log << "ReadFacilitiesConfigFile() fixedStationID: " << it->first << " position: " << it->second << " RAT: " << rat;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

		set<string> rats;
		rats.insert(rat);
		FixedNode* node = new FixedNode((*it).first, (*it).second.x(), (*it).second.y(), rats);
		m_syncManager->AddNode(node, false);

	}

	delete positions;
	return EXIT_SUCCESS;
}

int
ICS::SetupApplications(string filePath)
{
	AppConfigFileParse appConfig;
	try {
		appConfig.readConfigFile(filePath);
	} catch (std::runtime_error& e) {
		cout << "[ReadAppConfigFile] " << e.what() << endl;
		return EXIT_FAILURE;
	}

	int numOfApps = 0;

	for (int i = 0; i < appConfig.GetApplicationsConfig()->size(); i++) {
		char* name = appConfig.GetApplicationsConfig()->at(i)->m_name;
		char* executable = appConfig.GetApplicationsConfig()->at(i)->m_executable;
		char* ip = appConfig.GetApplicationsConfig()->at(i)->m_ip;
		char* port = appConfig.GetApplicationsConfig()->at(i)->m_port;
		char* seed = appConfig.GetApplicationsConfig()->at(i)->m_seed;
		char* rate = appConfig.GetApplicationsConfig()->at(i)->m_rate;
		char* result = appConfig.GetApplicationsConfig()->at(i)->m_resultContainer;
		char* m_resultContainer = appConfig.GetApplicationsConfig()->at(i)->m_resultContainer;
		char* m_unicastServiceId = appConfig.GetApplicationsConfig()->at(i)->m_unicastServiceId;
		char* m_multicastServiceId = appConfig.GetApplicationsConfig()->at(i)->m_multicastServiceId;
		char* m_broadcastServiceId = appConfig.GetApplicationsConfig()->at(i)->m_broadcastServiceId;
		char* m_geoBroadcastServiceId = appConfig.GetApplicationsConfig()->at(i)->m_geoBroadcastServiceId;
		char* m_topoBroadcastServiceId = appConfig.GetApplicationsConfig()->at(i)->m_topoBroadcastServiceId;
		vector<char*> stations = appConfig.GetApplicationsConfig()->at(i)->m_stations;

		stringstream _stream;
		_stream << result;
		string resultType;
		_stream >> resultType;
		int _result;
		if (resultType == "OUTPUT_SET_SPEED_ADVICE_DEMO") {
			_result = OUTPUT_SET_SPEED_ADVICE_DEMO;
		}

		if (resultType == "OUTPUT_SET_VEHICLE_MAX_SPEED") {
			_result = OUTPUT_SET_VEHICLE_MAX_SPEED;
		}

		if (resultType == "OUTPUT_TRAVEL_TIME_ESTIMATION") {
			_result = OUTPUT_TRAVEL_TIME_ESTIMATION;
		}

		if (resultType == "OUTPUT_TRAFFIC_JAM_DETECTION") {
			_result = OUTPUT_TRAFFIC_JAM_DETECTION;
		}

		if (resultType == "OUTPUT_VOID") {
			_result = OUTPUT_VOID;
		}

		if (resultType == "OUTPUT_GENERIC") {
			_result = OUTPUT_GENERIC;
		}

		int _port, _rate;
		_port = atoi(port);
		bool portEq0 = _port == 0;
		if (portEq0) {
		    // Port == 0 => Get a random free port
		    _port = tcpip::Socket::getFreeSocketPort();
		}
		_rate = atoi(rate);

		ServiceId serviceId;
		serviceId.unicastServiceId = (string) m_unicastServiceId;
		serviceId.multicastServiceId = (string) m_multicastServiceId;
		serviceId.broadcastServiceId = (string) m_broadcastServiceId;
		serviceId.geoBroadcastServiceId = (string) m_geoBroadcastServiceId;
		serviceId.topoBroadcastServiceId = (string) m_topoBroadcastServiceId;

		long _seed = StringUtils::toLong(seed);

		string execString(executable);

		// Create the instance of the application in the iCS
		ApplicationHandler* appHandler = new ApplicationHandler(m_syncManager, (string)name, (string)ip, execString, _port, _seed, _rate, _result, serviceId);
		if (portEq0) {
            // Open port was chosen randomly, override the applications configuration by giving the port via commandline
	        execString += " --remote-port " + to_string(_port);
        }

		// DEBUG:
//        execString = std::string("echo '##### Bypassing app start for debugging (expecting app on port " + to_string(_port) + ") ####'");

		m_syncManager->RecognizeNewApplication(appHandler);

		// Loop fixed nodes and install the application on nodes defined by the configuration file
		for (vector<char*>::iterator stationIt = stations.begin(); stationIt != stations.end(); stationIt++) {
			char* stationId = *stationIt;
			ITetrisNode* node = NULL;
			int auxi = StringUtils::toInt(stationId);
			node = m_syncManager->GetNodeByIcsId(auxi);
			if (node == NULL) {
#ifdef LOG_ON
    stringstream log;
    cout << "ReadAppConfigFile() The ID defined in the configuration file does not match with any vehicle or infrastructure" << endl;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
			} else {
				node->m_applicationHandlerInstalled->push_back(appHandler);
				ResultContainer * result = ResultContainer::CreateResultContainer(appHandler->m_resultType,node->m_icsId,appHandler->m_id);
				if(result == NULL)
					return EXIT_FAILURE;
				node->m_resultContainerCollection->push_back(result);
			}
		}

		// Execute the application in a separate thread
		pthread_t myThread;
		m_applicationsThreads.push_back(myThread);
		pthread_create(&(m_applicationsThreads[i]), NULL, launchApplication,(void *) execString.c_str());
		Sleep(1000);
	}

	return EXIT_SUCCESS;
}

}
