/****************************************************************************/
/// @file    sync-manager.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 ***************************************************************************************/

/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Jerome Haerri (jerome.haerri@eurecom.fr) and Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <map>
#include <math.h>
#include <sys/timeb.h>

#include <utils/common/RandHelper.h>
#include "sync-manager.h"
#include "vehicle-node.h"
#include "fixed-node.h"
#include "tmc-node.h"
#include "wirelesscom_sim_communicator/ns3-client.h"
#include "traffic_sim_communicator/traci-client.h"
#include "wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "wirelesscom_sim_message_tracker/V2X-cam-area.h"
#include "wirelesscom_sim_message_tracker/V2X-geobroadcast-area.h"
#include "applications_manager/subscription.h"
#include "applications_manager/app-commands-subscriptions-constants.h"
#include "applications_manager/app-result-container.h"
#include "applications_manager/application-handler.h"
#include "applications_manager/app-message-manager.h"
#include "applications_manager/subs-set-cam-area.h"
#include "applications_manager/subs-start-travel-time-calculation.h"
#include "applications_manager/subs-stop-travel-time-calculation.h"
#include "applications_manager/subs-app-message-send.h"
#include "applications_manager/subs-app-message-receive.h"
#include "FacilitiesManager.h"
#include "ics.h"
#include "../utils/ics/log/ics-log.h"
#include "../utils/ics/iCSGeoUtils.cpp"
#include <cmath>
#include "applications_manager/subs-get-facilities-info.h"
#include "applications_manager/subs-app-control-traci.h"

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#ifndef _WIN32
#include "config.h"
#endif
using namespace std;


// #define _DEBUG_MOBILITY



namespace ics
{

//LogFunction
int GetMilliCount()
{
	// Something like GetTickCount but portable
	// It rolls over every ~ 12.1 days (0x100000/24/60/60)
	// Use GetMilliSpan to correct for rollover
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int GetMilliSpan(int nTimeStart)
{
	int nSpan = GetMilliCount() - nTimeStart;
	if (nSpan < 0)
		nSpan += 0x100000 * 1000;
	return nSpan;
}

/**
 * @class veh_by_id_finder
 * @brief A class to find the matching lane wrapper
 */
class veh_by_id_finder
{
public:
	/** @brief constructor */
	explicit veh_by_id_finder(const std::string id) :
	myID(id)
	{
	}

	/** @brief the comparing function */
	bool operator()(ITetrisNode *node)
	{
		VehicleNode *veh = dynamic_cast<VehicleNode *>(node);
		return veh != 0 && veh->m_tsId == myID;
	}

private:
	/// @brief The id of the searched object
	std::string myID;
};

// ===========================================================================
// static member definitions
// ===========================================================================
icstime_t SyncManager::m_simStep = -1000;
int SyncManager::m_timeResolution;
FacilitiesManager* SyncManager::m_facilitiesManager;
TrafficSimulatorCommunicator* SyncManager::m_trafficSimCommunicator;
WirelessComSimulatorCommunicator* SyncManager::m_wirelessComSimCommunicator;
V2xMessageManager* SyncManager::m_v2xMessageTracker;

// ===========================================================================
// member method definitions
// ===========================================================================
SyncManager::SyncManager(int ns3Port, int sumoPort, string sumoHost, string ns3Host, int beginTime, int endTime,
		int resolution) :
		m_firstTimeStep(beginTime * 1000),
		m_lastTimeStep(endTime * 1000),
		m_trafficSimstep(-1)
{
	m_ns3Client.m_port = ns3Port;
	m_ns3Client.m_host = ns3Host;
	m_wirelessComSimCommunicator = &m_ns3Client;

	m_traci.m_port = sumoPort;
	m_traci.m_host = sumoHost;
	m_trafficSimCommunicator = &m_traci;

	if (m_firstTimeStep >= m_lastTimeStep)
	{
		cout << "[WARNING] The defined first time step is equal or higher than the ending time step." << endl;
		cout << "[WARNING] The first time step is automatically set to 0." << endl;
		m_firstTimeStep = 0;
	}
	m_timeResolution = resolution;

	m_iTetrisNodeMap = new NodeMap();
	m_NS3IdToIcsIdMap = new NS3IdToIcsIdMap();
	m_SumoIdToIcsIdMap = new SumoIdToIcsIdMap();
	AddNode(TmcNode::GetInstance());

	m_applicationHandlerCollection = new vector<ApplicationHandler*>();
	m_v2xMessageTracker = new V2xMessageManager();
	m_subscriptionCollectionManager = new vector<Subscription*>();
	m_facilitiesManager = new FacilitiesManager();
	m_messageId = 0;
}

SyncManager::~SyncManager()
{
	delete m_trafficSimCommunicator;
	delete m_wirelessComSimCommunicator;
	//		delete m_iTetrisNodeCollection;
	delete m_applicationHandlerCollection;
	delete m_v2xMessageTracker;
	delete m_subscriptionCollectionManager;
	delete m_facilitiesManager;
	for (NodeMap::iterator it = m_iTetrisNodeMap->begin(); it != m_iTetrisNodeMap->end(); ++it)
		delete it->second;
	m_iTetrisNodeMap->clear();
	delete m_iTetrisNodeMap;
	delete m_NS3IdToIcsIdMap;
}

int SyncManager::Run(bool interactive)
{
	m_simStep = 0;

	cout << "\t\tiCS --> Global simulation timestep is: " << m_simStep << "; last step is: " << m_lastTimeStep << endl;
	cout << "\t\t============================================================" << endl;
	log_msgNumber = -1;
	log_stepTime = -1;
	log_ns3Time = -1;
	//Set facilities clock to zero
	m_facilitiesManager->updateClock(m_simStep);
	while (m_lastTimeStep >= m_simStep)
	{

#ifdef LOG_ON
		stringstream log;
		log << "Tics=" << m_simStep;
		if (log_stepTime != -1)
		{
			log_stepTime = GetMilliSpan(log_stepTime);
			log << " StepT=" << log_stepTime;
			log << " ns3T=" << log_ns3Time;
			log << " %ns3=" << (((float) log_ns3Time) / log_stepTime * 100);
		}
		if (log_msgNumber != -1)
			log << " DispMsgs=" << log_msgNumber;
		log << " MsgQueue=" << m_messageMap.size();
		IcsLog::Log((log.str()).c_str());
		log_stepTime = GetMilliCount();
		log_msgNumber = 0;
		//			IcsLog::Log("*************************************************************************************");
#endif

		if (interactive)
		{
			cout << endl;
			utils::Conversion::Wait("Press <Enter> to run the next ns-3 timestep");
			cout << endl;
			cout << "STEP 5 - SIMULATION PHASE - Tics = " << m_simStep << endl;
			cout << "======================================" << endl;
			cout << "[ns-3] EXECUTING EVENTS Tns-3 = Tics + 1" << endl;
			cout << "[APP]  WAITING." << endl;
			cout << "[iCS]  PROCESS THE STATUS OF THE MESSAGES RECEIVED IN ns-3." << endl;
			cout << "[SUMO] WAITING." << endl;
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		} else
		{
			cout << "STEP 5 - SIMULATION PHASE - Tics = " << m_simStep << endl;
		}

#ifdef NS3_ON

		if (m_simStep >= m_firstTimeStep)
		{
			log_ns3Time = GetMilliCount();
			if (RunOneNs3TimeStep() == EXIT_FAILURE)
			{
				utils::Conversion::Wait("iCS --> [ERROR] RunOneNs3TimeStep()");
				return EXIT_FAILURE;
			}
			log_ns3Time = GetMilliSpan(log_ns3Time);
			if (GetDataFromNs3() == EXIT_FAILURE)
			{
				utils::Conversion::Wait("iCS --> [ERROR] GetDataFromNs3()");
				return EXIT_FAILURE;
			}
		}
#endif

		if (interactive)
		{
			cout << endl << endl;
			utils::Conversion::Wait("Press <Enter> to run the next SUMO timestep");
			cout << endl;
			cout << "STEP 6 - SIMULATION PHASE - Tics = " << m_simStep << endl;
			cout << "======================================" << endl;
			cout << "[ns-3] CREATE NEW NODES." << endl;
			cout << "[APP]  WAITING." << endl;
			cout << "[iCS]  REGISTER NEW NODES, ASSIGN RAT AND APPLICATIONS. UPDATE POSITIONS." << endl;
			cout << "[SUMO] EXECUTING EVENTS = Tics" << endl;
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		} else
		{
			cout << "STEP 6 - SIMULATION PHASE - Tics = " << m_simStep << endl;
		}

#ifdef SUMO_ON
		if (m_simStep % m_trafficSimstep == 0)
		{
		    if(m_simStep > m_trafficSimCommunicator->getStartTime()) {
		        if (RunOneSumoTimeStep() == EXIT_FAILURE)
		        {
		            utils::Conversion::Wait("iCS --> [ERROR] RunOneSumoTimeStep()");
		            return EXIT_FAILURE;
		        }
                if (updateTrafficLightInformation() == EXIT_FAILURE)
                {
                    utils::Conversion::Wait("iCS --> [ERROR] updateTrafficLightInformation()");
                    return EXIT_FAILURE;
                }
		    } else {
		        // Don't run a sumo time step
		        IcsLog::LogLevel("RunOneSumoTimeStep() Skipping SUMO simulation step since the current iCS time is less than the next traffic simulator time.",
		                kLogLevelInfo);
		        // TrafficLight info needs to be present already at the first iCS simulation step, though
                if (updateTrafficLightInformation() == EXIT_FAILURE)
                {
                    utils::Conversion::Wait("iCS --> [ERROR] updateTrafficLightInformation()");
                    return EXIT_FAILURE;
                }
		    }
		}
#endif

		if (interactive)
		{
			utils::Conversion::Wait("Press <Enter> to run the next Application round");
			cout << endl;
			cout << "STEP 7 - SIMULATION PHASE - Tics = " << m_simStep << endl;
			cout << "======================================" << endl;
			cout << "[ns-3] WAITING" << endl;
			cout << "[APP]  WAITING." << endl;
			cout << "[iCS]  RUN INTERACTION WITH APPLICATIONS." << endl;
			cout << "[SUMO] WAITING" << endl;
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		} else
		{
			cout << "STEP 7 - SIMULATION PHASE - Tics = " << m_simStep << endl;
		}

#ifdef APPLICATIONS_ON
		if (m_simStep >= m_firstTimeStep)
		{
			if (RunApplicationLogic() == EXIT_FAILURE)
			{
				utils::Conversion::Wait("iCS --> [ERROR] RunApplicationLogic()");
				return EXIT_FAILURE;
			}
		}
#endif

		if (interactive)
		{
			utils::Conversion::Wait("Press <Enter> to process the result of Applications");
			cout << endl;
			cout << "STEP 8 - SIMULATION PHASE - Tics = " << m_simStep << endl;
			cout << "======================================" << endl;
			cout << "[ns-3] WAITING" << endl;
			cout << "[APP]  WAITING." << endl;
			cout << "[iCS]  PROCESS RESULTS OF THE APPLICATIONS." << endl;
			cout << "[SUMO] WAITING" << endl;
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		} else
		{
			cout << "STEP 8 - SIMULATION PHASE - Tics = " << m_simStep << endl;
		}
#ifdef APPLICATIONS_ON
		if (m_simStep >= m_firstTimeStep)
		{

			if (ProcessApplicationResults() == EXIT_FAILURE)
			{
				//                 utils::Conversion::Wait(
				cout << "iCS --> [ERROR] ProcessApplicationResults()" << endl/*)*/;
				return EXIT_FAILURE;
			}
		}
#endif

		if (interactive)
		{
			cout << endl;
			utils::Conversion::Wait("Press <Enter> to update positions in ns-3");
			cout << endl;
			cout << "STEP 9 - SIMULATION PHASE - Tics = " << m_simStep << endl;
			cout << "========================================" << endl;
			cout << "[ns-3] UPDATE POSITIONS." << endl;
			cout << "[APP]  WAITING." << endl;
			cout << "[iCS]  WAITING." << endl;
			cout << "[SUMO] WAITING." << endl;
			utils::Conversion::Wait("Press <Enter> to continue...");
			cout << endl;
		} else
		{
			cout << "STEP 9 - SIMULATION PHASE - Tics = " << m_simStep << endl;
		}

#ifdef NS3_ON
		if (m_simStep >= m_firstTimeStep)
		{
			if (ScheduleV2xMessages() == EXIT_FAILURE)
			{
				utils::Conversion::Wait("iCS --> [ERROR] ScheduleV2xMessages()");
				return EXIT_FAILURE;
			}
		}

		if (UpdatePositionsInNs3() == EXIT_FAILURE)
		{
			utils::Conversion::Wait("iCS --> [ERROR] UpdatePositionsInNs3()");
			return EXIT_FAILURE;
		}

#endif

		//Increase global time simulation counter
		m_simStep += m_timeResolution;

		//Update facilities timestep
		m_facilitiesManager->updateClock(m_simStep);

		// To not display the last time step header
		if (m_lastTimeStep >= m_simStep)
		{
			cout << "\t\tiCS --> Global simulation timestep is: " << m_simStep << "; last step is: " << m_lastTimeStep
					<< endl;
			cout << "\t\t============================================================" << endl;
		}
	}

#ifdef LOG_ON
	stringstream log;
	log << "[Run] The runtime is over. Last time step reached [LastTimeStep] [" << m_lastTimeStep << "]";
	IcsLog::Log((log.str()).c_str());
#endif

	return EXIT_SUCCESS;
}

int SyncManager::Stop()
{
	bool success = true;

#ifdef NS3_ON
	if (CloseNs3() == EXIT_FAILURE)
	{
		success = false;
		Sleep(2000);
	}
#endif

#ifdef SUMO_ON
	if (CloseSumo() == EXIT_FAILURE)
	{
		success = false;
		Sleep(2000);
	}
#endif

#ifdef APPLICATIONS_ON

	if (CloseApps() == EXIT_FAILURE)
	{
		success = false;
		Sleep(2000);
	}
#endif

	if (success)
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}

int SyncManager::RunOneSumoTimeStep()
{
	std::vector<std::string> departed;
	std::vector<std::string> arrived;
	if (m_trafficSimCommunicator->CommandSimulationStep(m_simStep, departed, arrived) == EXIT_FAILURE)
	{
		IcsLog::LogLevel("RunOneSumoTimeStep() Error trying to command simulation step in traffic simulator.",
				kLogLevelError);
		return EXIT_FAILURE;
	}

	// remove vehicles that left the simulation
	{
#ifdef LOG_ON
		stringstream log;
		log << "RunOneSumoTimeStep() Number of vehicles that left the simulation: " << arrived.size();
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
	}

	for (vector<string>::const_iterator i = arrived.begin(); i != arrived.end(); ++i)
	{
		VehicleNode * node = dynamic_cast<VehicleNode*>(GetNodeBySumoId(*i));

		if (node != NULL)
		{
#ifdef LOG_ON
			stringstream log;
			log << "RunOneSumoTimeStep() Node " << node->m_icsId << "('" << node->m_tsId << "') left the simulation.";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			((Station *) m_facilitiesManager->getStation(node->m_icsId))->isActive = false;
			m_vehiclesToBeDeactivated.push_back(node->m_nsId);
			// Inform apps about node removal
			for (auto ah : *m_applicationHandlerCollection) {
				ah->RemoveVehicleNode(node);
			}
			RemoveNodeInTheArea(node);
			DeleteNode(node);
		}
	}

	// assign RATs to new vehicles and create the node in ns-3
	{
#ifdef LOG_ON
		stringstream log;
		log << "RunOneSumoTimeStep() Number of vehicles that entered the simulation: " << departed.size();
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
	}

	// Container of nodes to be activated in wireless communication simulator
	vector<int> nodesToActivateInNs3;
	nodesToActivateInNs3.reserve(departed.size());

	for (vector<string>::const_iterator i = departed.begin(); i != departed.end(); ++i)
	{
		if (ITetrisSimulationConfig::HasRat(*i))
		{
			VehicleNode* vehicle = new VehicleNode(*i);

			{
#ifdef LOG_ON
				stringstream log;
				log << "RunOneSumoTimeStep() vehicle " << vehicle->m_tsId << "  entered the simulation. Assigned iCS-ID " << vehicle->m_icsId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			}

			// Get additional info from SUMO and Create the new station in the facilities
			std::pair<float, float> pos = m_trafficSimCommunicator->GetPosition(*vehicle);
			float speed = m_trafficSimCommunicator->GetSpeed(*vehicle);
			vehicle->m_SumoType = m_trafficSimCommunicator->GetVehicleType(*vehicle);
			vehicle->m_SumoClass = m_trafficSimCommunicator->GetVehicleClass(*vehicle);

			TMobileStationDynamicInfo info;
            fillDynamicInfo(info, vehicle, pos, speed);
			m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);

			AssignApplication(vehicle);
			vector<RATID>* rats = m_facilitiesManager->getStationActiveRATs(vehicle->m_icsId);
			vector<string> techList;
			if (rats->size() == 0)
			{
				cout << "[ERROR] getStationActiveRATs returned 0 size" << endl;
				return EXIT_FAILURE;
			} else
			{
				for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++)
				{
					switch (*it)
					{
					case WAVE:
					{
						techList.push_back("WaveVehicle");
						vehicle->m_rats.insert("WaveVehicle");
						break;
					}
					case UMTS:
					{
						techList.push_back("UmtsVehicle");
						vehicle->m_rats.insert("UmtsVehicle");
						break;
					}
					case WiMAX:
					{
						techList.push_back("WimaxVehicle");
						vehicle->m_rats.insert("WimaxVehicle");
						//  utils::Conversion::Wait("iCS --> Wimax not implemented yet.");
						return EXIT_FAILURE;
						break;
					}
					case DVBH:
					{
						techList.push_back("DvbhVehicle");
						vehicle->m_rats.insert("DvbhVehicle");
						break;
					}
				   case LTE: {
				        techList.push_back("LteVehicle");
				        vehicle->m_rats.insert("LteVehicle");
				        break;
				    }
					default:
					{
						cout << "[ERROR] RunOneSumoTimeStep() There is no match for the type of RAT" << endl;
						delete rats;
						return EXIT_FAILURE;
					}
					}
				}
			}
			delete rats;

			// Create the node in ns-3
#ifdef NS3_ON
			int32_t id = m_wirelessComSimCommunicator->CommandCreateNode2(vehicle->GetPositionX(), vehicle->GetPositionY(),
					vehicle->GetSpeed(), vehicle->GetHeading(), vehicle->GetLane(), techList);
#else
			int32_t id = 1;
#endif
			vehicle->m_nsId = id; // Assign the ns-3 node ID returned by ns-3
			if (!AddNode(vehicle)) {
                return EXIT_FAILURE;
			}
			nodesToActivateInNs3.push_back(vehicle->m_nsId);  // Add vehicle to activate

			// Inform Apps of mobile node creation
			CreateNodeApplication(vehicle);
		}
	}

	// Activate the new nodes in wireless
#ifdef NS3_ON
	m_wirelessComSimCommunicator->CommandActivateNode(nodesToActivateInNs3);
#endif
		// update positions + other data
	for (NodeMap::const_iterator it = m_iTetrisNodeMap->begin(); it != m_iTetrisNodeMap->end(); ++it)
	{
		if (it->second->m_type != staType_CAR)
			continue;
		VehicleNode* vehicle = (VehicleNode*) it->second;

		// Get additional info from SUMO and Create the new station in the facilities
		std::pair<float, float> pos = m_trafficSimCommunicator->GetPosition(*vehicle);
		vehicle->CheckPosition(pos);
		float speed = m_trafficSimCommunicator->GetSpeed(*vehicle);

		TMobileStationDynamicInfo info;
        fillDynamicInfo(info, vehicle, pos, speed);
        m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);

	}

    return EXIT_SUCCESS;
}

int SyncManager::updateTrafficLightInformation()
    {
	// Update traffic lights
	std::vector<ics_types::trafficLightID_t> trafficLigthIds;
	if (m_trafficSimCommunicator->GetTrafficLights(trafficLigthIds) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	bool hadProblems = false;
	for (std::vector<ics_types::trafficLightID_t>::iterator i = trafficLigthIds.begin(); i != trafficLigthIds.end();
			++i)
	{
		std::string state;
		if (m_trafficSimCommunicator->GetTrafficLightStatus(*i, state) == EXIT_FAILURE)
		{
			hadProblems = true;
		} else
		{
			m_facilitiesManager->updateTrafficLightDynamicInformation(*i, state);
		}
	}
	if (hadProblems)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int SyncManager::RunOneNs3TimeStep()
{
	//ns-3 time step is one step ahead
	int ns3timeStep = m_simStep + m_timeResolution;

	//if (m_wirelessComSimCommunicator->CommandSimulationStep(ns3timeStep))
	if (m_wirelessComSimCommunicator->CommandSimulationStep(m_simStep))
	{
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int SyncManager::RunApplicationLogic()
{
    // Send current time step to all applications
    for (auto& a : *m_applicationHandlerCollection)
    {
        if (!a->m_appMessageManager->sendSimStep()) {
            cout << "iCS --> [ERROR] RunApplicationLogic() in NewSubscriptions." << endl;
            return EXIT_FAILURE;
        }
    }

    bool success = true;

	for (NodeMap::iterator nodeIt = m_iTetrisNodeMap->begin(); nodeIt != m_iTetrisNodeMap->end(); ++nodeIt)
	{
		ITetrisNode* currentNode = nodeIt->second;
		if (currentNode->m_applicationHandlerInstalled->size() != 0)
		{
			if (NewSubscriptions(currentNode) == EXIT_FAILURE)
			{
				cout << "iCS --> [ERROR] RunApplicationLogic() in NewSubscriptions." << endl;
				return EXIT_FAILURE;
			}

			map<stationID_t, icstime_t >::iterator itTime = m_firstTimeOutOfZone.find(currentNode->m_icsId);
			if (itTime != m_firstTimeOutOfZone.end()) {
			    if ((itTime->second != m_simStep) && (itTime->second/1000) == (m_simStep/1000)){
			        //get position from mobility history
#ifdef _DEBUG_MOBILITY
			        std::cout<<"m_firstTimeOutOfZone timestep "<<itTime->second <<", current timestep "<<m_simStep<<std::endl;
#endif
			        Point2D pos = m_facilitiesManager->getStationPositionsFromMobilityHistory(itTime->second, currentNode->m_icsId);
			        //If position is correct, send information to NS-3
			        if ((pos.x() > -100.0) || (pos.y() > -100.0)){
			            VehicleNode* vehicle = dynamic_cast<VehicleNode*>(currentNode);
			            std::pair<float, float> posFromSUMO = m_trafficSimCommunicator->GetPosition(*vehicle);
			            //if the same pos as from SUMO -> do nothing
			            if ((pos.x() != posFromSUMO.first) || (pos.y() != posFromSUMO.second)){
			                // Get additional info from SUMO, Create the new station in the facilities and update...
			                float speed = m_trafficSimCommunicator->GetSpeed(*vehicle);
			                TMobileStationDynamicInfo info;
                            fillDynamicInfo(info, vehicle, make_pair(pos.x(),pos.y()), speed);
			                m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);
#ifdef _DEBUG_MOBILITY
			                cout << "iCS -->SubsAppControlTraci  Updated node's position: (node " << currentNode->m_icsId << "), SUMO - pos (" << posFromSUMO.first<<","<<posFromSUMO.second<< ")"<< ", New pos (" << pos.x()<<","<<pos.y()<< ") "<<  " at TS " << m_simStep << " "<< endl;
#endif
			            }
			        }

			    }
			}

			if (DropSubscriptions(currentNode) == EXIT_FAILURE)
			{
				cout << "iCS --> [ERROR] RunApplicationLogic() in DropSubscriptions." << endl;
				return EXIT_FAILURE;
			}

			if (ForwardSubscribedDataToApplication(currentNode) == EXIT_FAILURE)
			{
				cout << "iCS --> [ERROR] RunApplicationLogic() in ForwardSubscribedDataToApplication." << endl;
				return EXIT_FAILURE;
			}

			if (DeliverMessageStatus(currentNode) == EXIT_FAILURE)
			{
				cout << "iCS --> [ERROR] RunApplicationLogic() in DeliverMessageStatus." << endl;
				return EXIT_FAILURE;
			}

			if (ExecuteApplicationMainFunction(currentNode) == EXIT_FAILURE)
			{
				cout << "iCS --> [ERROR] RunApplicationLogic() in ExecuteApplicationMainFunction." << endl;
				return EXIT_FAILURE;
			}
		}
	} //end For

	if (success)
	{
		cout << endl;
		return EXIT_SUCCESS;
	} else
	{
		return EXIT_FAILURE;
	}
}


void SyncManager::fillDynamicInfo(TMobileStationDynamicInfo& info, VehicleNode * vehicle, const pair<double,double>& pos, const double speed) {
    info.speed = speed;
    info.acceleration = vehicle->ChangeSpeed(speed);
    info.direction = m_trafficSimCommunicator->GetDirection(*vehicle);
    info.exteriorLights = m_trafficSimCommunicator->GetExteriorLights(*vehicle);
    info.positionX = pos.first;
    info.positionY = pos.second;
    info.length = m_trafficSimCommunicator->GetVehicleLength(*vehicle);
    info.width = m_trafficSimCommunicator->GetVehicleWidth(*vehicle);
    info.lane = m_trafficSimCommunicator->GetLane(*vehicle);
    info.timeStep = m_simStep;
}


int SyncManager::ConnectNs3()
{
	if (m_wirelessComSimCommunicator->Connect())
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

int SyncManager::CloseNs3()
{
	if (m_wirelessComSimCommunicator->CommandClose() == EXIT_FAILURE)
	{
		cout << "iCS --> [ERROR] CloseNs3() Finishing NS-3" << endl;
		return EXIT_FAILURE;
	}

	if (m_wirelessComSimCommunicator->Close() == EXIT_FAILURE)
	{
		cout << "iCS --> [ERROR] CloseNs3() Closing socket." << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int SyncManager::ConnectSumo()
{
	if (m_trafficSimCommunicator->Connect()){
		return setTrafficSimstep();
	}
	return EXIT_FAILURE;
}

int SyncManager::setTrafficSimstep() {
    m_trafficSimstep = int(round(1000 * m_trafficSimCommunicator->getSimstepLength()));
    // Check whether traffic simstep is a multiple of the iCS simsteplength, fail otherwise
    if (m_trafficSimstep % m_timeResolution != 0) {
        stringstream msg;
        msg << "setTrafficSimStep() traffic simstep length (" << m_trafficSimstep << " ms.) "
                << "is no multiple of iCS simstep length (" << m_timeResolution << "ms.)";
        cout << "iCS --> [ERROR] " <<  msg.str() << endl;
        IcsLog::LogLevel((msg.str()).c_str(), kLogLevelError);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SyncManager::CloseSumo()
{
	if (m_trafficSimCommunicator->CommandClose() == EXIT_FAILURE)
	{
		cout << "iCS --> [ERROR] CloseSumo() Closing SUMO" << endl;
		return EXIT_FAILURE;
	}

	if (m_trafficSimCommunicator->Close() == EXIT_FAILURE)
	{
		cout << "iCS --> [ERROR] CloseSumo() Closing socket." << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int SyncManager::ConnectToApps()
{
	bool success = true;

	// Loop Applications
	for (vector<ApplicationHandler*>::iterator appHandlerIt = m_applicationHandlerCollection->begin();
			appHandlerIt < m_applicationHandlerCollection->end(); appHandlerIt++)
	{
		ApplicationHandler* appHandler = (*appHandlerIt);
		success = appHandler->m_appMessageManager->Connect(appHandler->m_host, appHandler->m_port);
		success = success && appHandler->m_appMessageManager->initSUMOStepLength(m_trafficSimstep);
		if (success)
		{
			cout << "iCS --> Application " << appHandler->m_name << " connected." << endl;
		} else
		{
			success = false;
		}
	}

	if (success)
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}

int SyncManager::CloseApps()
{
	bool success = true;

	// Loop all apps
	for (vector<ApplicationHandler*>::iterator appHandlerIt = m_applicationHandlerCollection->begin();
			appHandlerIt < m_applicationHandlerCollection->end(); appHandlerIt++)
	{
		ApplicationHandler* appHandler = (*appHandlerIt);

		if (appHandler->m_appMessageManager->CommandClose() == EXIT_FAILURE)
		{
			cout << "iCS --> Application " << appHandler->m_name << " was not closed" << endl;
			success = false;
		}
	}

	if (success)
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}

int SyncManager::SetFixedStationInNs3()
{

	if (m_iTetrisNodeMap->size() == 0)
	{

		stringstream log;
		log << "SetFixedStationInNs3() There are not fixed stations";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

		return EXIT_SUCCESS;
	}
	

	for (NodeMap::iterator nodeIt = m_iTetrisNodeMap->begin(); nodeIt != m_iTetrisNodeMap->end(); ++nodeIt)
	{
		ITetrisNode *node = nodeIt->second;
		if (node->m_type == staType_BASICRSU)
		{ // Only send message to nodes that are CIUs (fixed stations)
			FixedNode* fixedNode = static_cast<FixedNode*>(node);
			vector<string> techList;
			string tech = m_facilitiesManager->getFixedStationCommunicationProfile(fixedNode->m_icsId);
		
			techList.push_back(tech);
			int id = m_wirelessComSimCommunicator->CommandCreateNode(fixedNode->GetPositionX(), fixedNode->GetPositionY(),
					techList);

			if (id == -1)
			{ // Check status
#ifdef LOG_ON
				stringstream log;
				log << "SetFixedStationInNs3() Error in the message to create fixed station in ns-3";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif

				cout << "iCS --> [ERROR] SetFixedStationInNs3() Error in the message to create fixed station in ns-3" << endl;
				return EXIT_FAILURE;
			}

			fixedNode->m_nsId = id; // Assign the ID returned by ns-3
			UpdateNodeId(fixedNode, true, false);
		}
	}

	return EXIT_SUCCESS;

}

int SyncManager::UpdatePositionsInNs3()
{
	// Deactivate nodes that left the scenario
	if (m_vehiclesToBeDeactivated.size() > 0)
	{
		if (m_wirelessComSimCommunicator->CommandDeactivateNode(m_vehiclesToBeDeactivated))
		{
#ifdef LOG_ON
			stringstream log;
			log << "UpdatePositionsInNs3() Deactivated nodes: ";
			for (vector<int>::iterator it = m_vehiclesToBeDeactivated.begin(); it < m_vehiclesToBeDeactivated.end(); it++)
			{
				log << (*it) << " ";
			}
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			m_vehiclesToBeDeactivated.clear(); // Reset the container
		} else
		{

			stringstream log;
			log << "UpdatePositionsInNs3() Node deactivation incorrect";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

		}
	}

	// Update the position of the vehicles.
	for (NodeMap::iterator nodeIt = m_iTetrisNodeMap->begin(); nodeIt != m_iTetrisNodeMap->end(); ++nodeIt)
	{
		// Discard the node that are not mobile
		if (nodeIt->second->m_type == staType_CAR)
		{
			VehicleNode* vehicle = static_cast<VehicleNode*>(nodeIt->second);
			if (vehicle->m_moved)
			{
				if (m_wirelessComSimCommunicator->CommandUpdateNodePosition2(vehicle->m_nsId, vehicle->GetPositionX(),
						vehicle->GetPositionY(), vehicle->GetSpeed(), vehicle->GetHeading(), vehicle->GetLane()) == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}
				// Reset the moved info value
				vehicle->m_moved = false;
			}
		}
	}

	return EXIT_SUCCESS;

}

ITetrisNode* SyncManager::GetNodeByNs3Id(int nodeId)
{
	NS3IdToIcsIdMap::iterator node = m_NS3IdToIcsIdMap->find(nodeId);
	if (node != m_NS3IdToIcsIdMap->end())
		return GetNodeByIcsId(node->second);
	return NULL;

}

ITetrisNode* SyncManager::GetNodeBySumoId(const std::string &nodeId)
{
	SumoIdToIcsIdMap::iterator node = m_SumoIdToIcsIdMap->find(nodeId);
	if (node != m_SumoIdToIcsIdMap->end())
		return GetNodeByIcsId(node->second);
	return NULL;

}

ITetrisNode* SyncManager::GetNodeByIcsId(stationID_t nodeId)
{
	NodeMap::iterator node = m_iTetrisNodeMap->find(nodeId);
	if (node != m_iTetrisNodeMap->end())
		return node->second;
	return NULL;
}

int SyncManager::ForwardSubscribedDataToApplication(ITetrisNode *node)
{
	if (node == NULL)
	{
		cout << "iCS --> [ERROR] ForwardSubscribedDataToApplication() Node is null." << endl;
		return EXIT_FAILURE;
	}

	if (node->m_subscriptionCollection->size() == 0)
	{

		stringstream log;
		log << "iCS --> There is 0 subscription (node " << node->m_icsId << ")";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
		return EXIT_SUCCESS;
	}

#ifdef LOG_ON
{
    stringstream log;
	log << "ForwardSubscribedDataToApplication() subscription in node [iCS-ID] [" << node->m_icsId
			<< "] getting facilities data.";
	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
}
#endif

	vector<ApplicationHandler*>::iterator appsIt_1;
	vector<ApplicationHandler*>* apps_1 = node->m_applicationHandlerInstalled;

	// Loop the subscriptions of the node
	for (vector<Subscription*>::iterator subIt = node->m_subscriptionCollection->begin();
			subIt < node->m_subscriptionCollection->end(); subIt++)
	{
		Subscription* subscription = (*subIt);
		Subscription* subscription_2 = (*subIt);
		const std::type_info& typeinfo = typeid((*subscription_2));

		vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

		// Loop the applications the node has installled
		for (vector<ApplicationHandler*>::iterator appIt = apps->begin(); appIt < apps->end(); appIt++)
		{
			ApplicationHandler* app = (*appIt);
			if (app->m_id == subscription->m_appId)
			{ // Check if the subscription was created by the application
				int nodeId = node->m_icsId;

				if (app->SendSubscribedData(nodeId, subscription, m_iTetrisNodeMap) == EXIT_FAILURE)
				{
					cout << "iCS --> [ERROR] ForwardSubscribedDataToApplication() Error sending subscribed data." << endl;
					return EXIT_FAILURE;
				}
			}
		}
		/*
		 *  the SubsGetFacilitiesInfo is a single shot subscription. But it cannot be unsubscribed right after having
		 *                     been subscribed, as it is triggered in the "SendSubscribedData" phase of iCS, after the askForUnsubscribe.
		 *                     So, we have to manually erase it, as the iCS consideres it as a recuring subscription otherwise.
		 */
		if (typeinfo == typeid(SubsGetFacilitiesInfo))
		{
#ifdef LOG_ON
{

            stringstream log;
			log
			<< "[iCS]->[SyncManager::ForwardSubscribedDataToApplication]: SubsGetFacilitiesInfo - single short subscription erasing itself...";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
}
#endif
			delete *subIt;
			node->m_subscriptionCollection->erase(subIt);
			subIt--;

#ifdef LOG_ON
{
            stringstream log;
            log
			<< "[iCS]->[SyncManager::ForwardSubscribedDataToApplication]: SubsGetFacilitiesInfo - single short subscription...deleted.";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
}
#endif
		}
	}

	return EXIT_SUCCESS;
}

bool SyncManager::AssignApplication(ITetrisNode *node)
{
	if (node == nullptr)
		return false;

	for (ApplicationHandler* const appHand : *m_applicationHandlerCollection)
	{
		// Decide if the application will be assigned based on the penetration rates
		if (RandHelper::rand(100, &appHand->m_rng) < appHand->m_rate)
		{
			node->m_applicationHandlerInstalled->push_back(appHand);
			ResultContainer* result = ResultContainer::CreateResultContainer(appHand->m_resultType, node->m_icsId,
					appHand->m_id);
			if (result == nullptr)
				return false;
			node->m_resultContainerCollection->push_back(result);
		}
	}

	return true;
}

int SyncManager::CreateNodeApplication(ITetrisNode *node)
{
	if (node == NULL)
		return EXIT_FAILURE;
#ifdef LOG_ON
	stringstream log;
	log << "iCS --> Notifying the application of the creation of node " << node->m_icsId << " at TS " << m_simStep;
	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
	if (node->m_type != staType_CAR)
		return EXIT_SUCCESS;
	VehicleNode* vNode = static_cast<VehicleNode*>(node);
	for (vector<ApplicationHandler*>::iterator appsIt = node->m_applicationHandlerInstalled->begin();
			appsIt != node->m_applicationHandlerInstalled->end(); ++appsIt)
	{
		ApplicationHandler* appHandler = (*appsIt);
		if (!appHandler->CreateVehicleNodeApplication(vNode))
		{
			cerr << "iCS --> Error occurred when creating node in applications (node " << node->m_icsId << ")" << endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int SyncManager::NewSubscriptions(ITetrisNode *node)
{
	if (node == NULL)
		return EXIT_FAILURE;


#ifdef LOG_ON
	stringstream log;
	log << "iCS --> Asking new subscriptions to node " << node->m_icsId << " at TS " << m_simStep << " ";
	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

	vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

	vector<Subscription*>* newSubs = new vector<Subscription*>();

	// Loop the applications installed to ask for subscriptions
	for (vector<ApplicationHandler*>::iterator appsIt = apps->begin(); appsIt < apps->end(); appsIt++)
	{
		ApplicationHandler* appHandler = (*appsIt);
		bool success = true;
		success = appHandler->AskForNewSubscriptions(node->m_icsId, newSubs);
		if (!success)
		{
			cerr << "iCS --> Error occurred when asking for new subscriptions (node " << node->m_icsId << ")" << endl;
			return EXIT_FAILURE;
		}
	}

	if (newSubs->size() == 0)
	{

#ifdef LOG_ON
		stringstream log;
		log << "iCS --> NewSubscriptions() The node " << node->m_icsId << " requested 0 subscriptions";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		return EXIT_SUCCESS;
	}

	// Loop the new subscription, linked them and process if needed (for example, creating CAM areas)
	bool controlTraci = false;
	for (vector<Subscription*>::iterator subIt = newSubs->begin(); subIt < newSubs->end(); subIt++)
	{

		Subscription* subscription = *subIt;
		node->m_subscriptionCollection->push_back(subscription);

		Subscription& sub = *subscription;
		const std::type_info& typeinfo = typeid(sub);

		if (typeinfo == typeid(SubsSetCamArea))
		{
			m_subscriptionCollectionManager->push_back(subscription);
			SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);
			int payloadLength = 20;
			m_v2xMessageTracker->CreateV2xCamArea(subSetCamArea->m_id, subSetCamArea->GetFrequency(), payloadLength);

		}
		//find the SubsAppControlTraci
		else if(typeinfo == typeid(SubsAppControlTraci)){
			controlTraci = true;
#ifdef _DEBUG_MOBILITY
			cout << "iCS -->SubsAppControlTraci  (node " << node->m_icsId << ")" << " at TS " << m_simStep << " "<< endl;
#endif
			m_subscriptionCollectionManager->push_back(subscription);
			SubsAppControlTraci* subAppControlTraci = static_cast<SubsAppControlTraci*>(subscription);

			//get node's position from App Msg
			std::pair<float, float> pos = subAppControlTraci->getPositionFromMsg();

			//If position is correct, send information to NS-3
			if ((pos.first > -100.0) || (pos.second > -100.0)){
				VehicleNode* vehicle = dynamic_cast<VehicleNode*>(node);
				std::pair<float, float> posFromSUMO = m_trafficSimCommunicator->GetPosition(*vehicle);
				//if the same pos as from SUMO -> do nothing
				if ((pos.first != posFromSUMO.first) || (pos.second != posFromSUMO.second)){
					// Get additional info from SUMO and Create the new station in the facilities
					float speed = m_trafficSimCommunicator->GetSpeed(*vehicle);
					TMobileStationDynamicInfo info;
                    fillDynamicInfo(info, vehicle, pos, speed);
					m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);
#ifdef _DEBUG_MOBILITY
					cout << "iCS -->SubsAppControlTraci  Updated node's position: (node " << node->m_icsId << "), SUMO - pos (" << posFromSUMO.first<<","<<posFromSUMO.second<< ")"<< ", New pos (" << pos.first<<","<<pos.second<< ") "<<  " at TS " << m_simStep << " "<< endl;
#endif
					//update TimeoutofZone
					map<stationID_t, icstime_t >::iterator itTime;
					itTime = m_firstTimeOutOfZone.find(node->m_icsId);
					if (itTime != m_firstTimeOutOfZone.end()){ //update
						itTime->second = m_simStep;
#ifdef _DEBUG_MOBILITY
						std::cout<<"[m_firstTimeOutOfZone] UPDATEE, id "<<node->m_icsId <<", simstep "<<m_simStep<<std::endl;
#endif
					}else{ //insert
						m_firstTimeOutOfZone.insert(pair<stationID_t, icstime_t> (node->m_icsId, m_simStep));
#ifdef _DEBUG_MOBILITY
						std::cout<<"[m_firstTimeOutOfZone] insert id, "<<node->m_icsId <<" , simstep "<<m_simStep<<std::endl;
#endif
					}
				}
			}
			//Enable to test CMD_GET_VEHICLE_VARIABLE
			//subAppControlTraci->printGetSpeedMessage();
		}
	}

	delete newSubs;

	return EXIT_SUCCESS;
}

int SyncManager::DropSubscriptions(ITetrisNode *node)
{
	if (node == NULL)
		return EXIT_FAILURE;

	if (node->m_subscriptionCollection->size() == 0)
	{

		stringstream log;
		log << "DropSubscriptions() The node " << node->m_icsId << " has 0 subscriptions.";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

		return EXIT_SUCCESS;
	}

	vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

	// Loop applications installed in the node
	for (vector<ApplicationHandler*>::iterator appsIt = apps->begin(); appsIt < apps->end(); appsIt++)
	{
		ApplicationHandler* appHandler = *appsIt;
		bool success = true;
		success = appHandler->AskForUnsubscriptions(node->m_icsId, node->m_subscriptionCollection);
		if (!success)
		{
			cout << "iCS --> Error occurred when asking to drop subscriptions (node " << node->m_icsId << ")" << endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int SyncManager::ExecuteApplicationMainFunction(ITetrisNode *node)
{
	if (node == NULL)
		return EXIT_FAILURE;

	vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

	// Loop all the application to ask for execution
	for (vector<ApplicationHandler*>::iterator appsIt = apps->begin(); appsIt < apps->end(); appsIt++)
	{
		ApplicationHandler* appHandler = *appsIt;

		// Look for the appropriated result container of the application
		for (vector<ResultContainer*>::iterator resultIt = node->m_resultContainerCollection->begin();
				resultIt < node->m_resultContainerCollection->end(); resultIt++)
		{
			ResultContainer* resultContainer = *resultIt;
			if ((*appsIt)->m_id == (*resultIt)->m_applicationHandlerId)
			{   //Find the matching result container of the application
				bool success = EXIT_SUCCESS;
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> executing application (node " << node->m_icsId << ")";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				success = appHandler->ExecuteApplication(node->m_icsId, resultContainer);
				if (!success)
				{
					cout << "iCS --> Error occurred when asking to execute application (node " << node->m_icsId << ")" << endl;
					return EXIT_FAILURE;
				}
			}
		}
	}
	return EXIT_SUCCESS;
}

int SyncManager::DeliverMessageStatus(ITetrisNode* node)
{
	// Loop applications installed in the node
	for (vector<ApplicationHandler*>::iterator appsIt = node->m_applicationHandlerInstalled->begin();
			appsIt < node->m_applicationHandlerInstalled->end(); appsIt++)
	{
		int appId = (*appsIt)->m_id;

		// Loop results attached to the node
		for (vector<ResultContainer*>::iterator resultIt = node->m_resultContainerCollection->begin();
				resultIt < node->m_resultContainerCollection->end(); resultIt++)
		{
			if (appId == (*resultIt)->m_applicationHandlerId)
			{ //Find the matching result container of the application
				if ((*appsIt)->SendMessageStatus(node->m_icsId, (*resultIt)) == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}
			}
		}
	}

	return EXIT_SUCCESS;
}

int SyncManager::ProcessApplicationResults()
{
	for (NodeMap::iterator nodeIt = m_iTetrisNodeMap->begin(); nodeIt != m_iTetrisNodeMap->end(); ++nodeIt)
	{

		ITetrisNode* node = nodeIt->second;
		vector<ResultContainer*>::iterator resultIt;

		// Loop all the result of the station
		for (resultIt = node->m_resultContainerCollection->begin(); resultIt < node->m_resultContainerCollection->end();
				resultIt++)
		{
			ResultContainer* result = *resultIt;

			ApplicationHandler* appHandler = NULL;
			// Loop applications installed in the station
			for (vector<ApplicationHandler*>::iterator appsIt = node->m_applicationHandlerInstalled->begin();
					appsIt < node->m_applicationHandlerInstalled->end(); appsIt++)
			{
				if (result->m_applicationHandlerId == (*appsIt)->m_id)
				{
					appHandler = (*appsIt);
					break;
				}
			}

			// Check there is app handler attached.
			if (appHandler == NULL)
			{
				cout << "iCS --> The result is not attached to any application handler." << endl;
				return EXIT_FAILURE;
			}
			bool noResult = result->ResultContainerIsEmpty(); // Check wether there is a new result
			if (!noResult)
			{
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> [ProcessApplicationResults] on node " << node->m_icsId << " ";
                if (node->m_tsId != "") {
                    log << " (SUMO-id " << node->m_tsId << ")";
                }
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				if (result->ApplyResult(GetAddress()) == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}
			} else
			{
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> [ProcessApplicationResults] on node " << node->m_icsId;
                if (node->m_tsId != "") {
                    log << " (SUMO-id " << node->m_tsId << ")";
                }
                log << " NO RESULTS TO PROCESS";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			}
		}
	}

	return EXIT_SUCCESS;
}

void SyncManager::RecognizeNewApplication(ApplicationHandler* appHandler)
{
	m_applicationHandlerCollection->push_back(appHandler);
}

void SyncManager::RemoveNodeInTheArea(ITetrisNode* node)
{
	vector<V2xCamArea*>* camAreas = m_v2xMessageTracker->m_v2xCamAreaCollection;

	// Loop CAM areas
	for (vector<V2xCamArea*>::iterator camAreasIt = camAreas->begin(); camAreasIt < camAreas->end(); camAreasIt++)
	{
		V2xCamArea* camArea = *camAreasIt;
		int subscriptionId = camArea->m_subscriptionId;
		Subscription* subscription = NULL;
		for (vector<Subscription*>::iterator  subIt = m_subscriptionCollectionManager->begin(); subIt < m_subscriptionCollectionManager->end(); subIt++)
		{
			subscription = *subIt;
			if (subscriptionId == subscription->m_id)
			{
				break;
			}
		}

		if (subscription != NULL)
		{
			SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);
			vector<ITetrisNode*>* oldNodesInArea = subSetCamArea->getNodesInArea();
			subSetCamArea->RemoveNodeFromVector(node);
		}
	}
}

int SyncManager::ScheduleV2xMessages()
{
	if (ScheduleV2xCamAreaMessages() == EXIT_FAILURE)
	{
		cout << "iCS --> [ScheduleV2xMessages] Failure scheduling CAM messages." << endl;
		return EXIT_FAILURE;
	}

	if (ScheduleV2xGeobroadcastMessages() == EXIT_FAILURE)
	{
		cout << "iCS --> [ScheduleV2xGeobroadcastMessages] Failure scheduling Geobroadcast." << endl;
		return EXIT_FAILURE;
	}

	//Refresh the scheduled messages tables
    RefreshScheduledCamMessageTable();
    RefreshScheduledMessageMap();

	return EXIT_SUCCESS;
}

int SyncManager::GetDataFromNs3()
{
	// STEP 1 GET RECEIVED MESSAGES FROM NS-3

	bool camMessageReceived = false; // Flag to know if any of the received messages is CAM
	std::map<int, std::vector<Message> >* nodeMessages = new std::map<int, std::vector<Message> >();
	if (!m_wirelessComSimCommunicator->CommandGetAllReceivedMessages(nodeMessages, m_timeResolution))
    {
#ifdef LOG_ON
		stringstream log;
		log << "iCS --> [GetDataFromNs3] ERROR occurred when trying to obtain received messages";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
		return EXIT_FAILURE;
	}

	//Loop all nodes who have received a message
	for (std::map<int, std::vector<Message> >::iterator messageIt = nodeMessages->begin();
			messageIt != nodeMessages->end(); ++messageIt)
    {
		ITetrisNode* node = GetNodeByNs3Id(messageIt->first);
		if (node == NULL)
		{
#ifdef LOG_ON
            ostringstream log;
			log << "GetDataFromNs3() Node " << messageIt->first
					<< " is NULL. It has probably left the simulation. Will be skipped";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
			//I need to delete all the containers
			for (vector<Message>::iterator it = messageIt->second.begin(); it != messageIt->second.end(); ++it)
				delete it->packetTagContainer;
			messageIt->second.clear();
			continue;
		}
		vector<Message>* receivedMessages = &messageIt->second;

#ifdef LOG_ON
		ostringstream log;
		log << "GetDataFromNs3() Node " << node->m_icsId << " received " << receivedMessages->size() << " messages";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

		//dispatched message count
		log_msgNumber += receivedMessages->size();
		// Loop received messages
		for (vector<Message>::iterator receivedIterator = receivedMessages->begin();
				receivedIterator != receivedMessages->end(); ++receivedIterator)
		{
			Message receivedMessage = *receivedIterator;
			receivedMessage.receiverIcsId = node->m_icsId;
			// Check received message type
			switch (receivedMessage.messageType)
			{
			case CAM:
            {
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> CAM message from NS3 from: " << receivedMessage.senderNs3Id;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				camMessageReceived = true; // Change flag, at least one message is CAM
				ScheduledCamMessageData rcvMessage;
				rcvMessage.senderNs3ID = receivedMessage.senderNs3Id;
				rcvMessage.messageType = receivedMessage.messageType;
				rcvMessage.timeStep = receivedMessage.timeStep;
				rcvMessage.sequenceNumber = receivedMessage.sequenceNumber;
				rcvMessage.received = true;

				if (ScheduledCamMessageTable.size() > 0)
				{
					// Loop the scheduled message table to get the action ID associated
					for (vector<ScheduledCamMessageData>::iterator messageIterator = ScheduledCamMessageTable.begin();
							messageIterator != ScheduledCamMessageTable.end(); messageIterator++)
					{
						if (m_v2xMessageTracker->CompareCamRows(rcvMessage, (*messageIterator)))
						{
							rcvMessage.actionID = (*messageIterator).actionID;

							// Groups the receivers that received the same message
							// Facilities function needs all the receivers associated with the same Action ID
							m_v2xMessageTracker->UpdateIdentifiersTable(IdentifiersStorageTable, messageIterator->actionID,
									messageIterator->senderIcsID, node->m_icsId);

							// Change the status of the message (marks as received but more nodes could receive it)
							messageIterator->received = true;
						}
					}
				} else
				{
					IcsLog::LogLevel("GetDataFromNs3() There isn't any scheduled CAM message for the received message",
							kLogLevelWarning);
				}
				//Delete the container.
				delete receivedMessage.packetTagContainer;
				break;
			}

            case DENM:   // JHNOTE (20/03/2018) not implemented in this version
            {
                //Delete the container.
				delete receivedMessage.packetTagContainer;
				break;
			}

			case UNICAST:
            {
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> ProcessUnicastMessages() from NS3 from: " << receivedMessage.senderNs3Id << " messageType:"
						<< toString(receivedMessage.messageType);
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                if (ProcessUnicastMessages(receivedMessage, node) == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
				break;
			}
			case GEOBROADCAST:
            {
				receivedMessage.received = true;

				ITetrisNode* sender = GetNodeByNs3Id(receivedMessage.senderNs3Id);
				if (sender == NULL)
				{
#ifdef LOG_ON
					stringstream log;
					log << "iCS --> ProcessGeoBroadcastMessages() The sender is NULL. It has probably left the simulation."
							<< toString(receivedMessage.messageType);
					IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
					continue;
#endif
				}
				receivedMessage.senderIcsId = sender->m_icsId;
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> ProcessGeoBroadcastMessages() from NS3 from: " << receivedMessage.senderNs3Id
						<< " messageType:" << toString(receivedMessage.messageType);
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				if (ProcessGeobroadcastMessages(receivedMessage) == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}
				break;

			}
			case TOPOBROADCAST:
            {
#ifdef LOG_ON
				stringstream log;
				log << "iCS --> ProcessTopobroadcastMessages() from NS3 from: " << receivedMessage.senderNs3Id
						<< " messageType:" << toString(receivedMessage.messageType) << " nsId " << node->m_nsId << " icsId "
						<< node->m_icsId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
				receivedMessage.received = true;
				if (ProcessTopobroadcastMessages(receivedMessage) == EXIT_FAILURE)
				{
					return EXIT_FAILURE;
				}
				break;
			}
			default:
			{
#ifdef LOG_ON
				stringstream log;
				log << "[ERROR] GetDataFromNs3() The message received has a non defined message type.";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
				return EXIT_FAILURE;
				break;
			}
			}
		} // received messages
		receivedMessages->clear();
	}

	//delete the map outside the for cycle
	delete nodeMessages;


	// STEP 2: Transfer received Action ID (messages) to the facilities

	// If at least one message is CAM trigger the CAM message process
	if (camMessageReceived)
    {
		// Loop received Action IDs to get
		if (IdentifiersStorageTable.size() > 0)
        {
			vector<IdentifiersStorageStruct>::iterator idIterator = IdentifiersStorageTable.begin();
			while (idIterator != IdentifiersStorageTable.end())
            {
				if ((*idIterator).stored)
				{
					idIterator = IdentifiersStorageTable.erase(idIterator);
				} else
				{
					vector<stationID_t> vReceivers;
					vReceivers = m_v2xMessageTracker->GroupReceivers(IdentifiersStorageTable, (*idIterator).actionID,
							(*idIterator).senderID);

					if (vReceivers.size() != 0)
					{
						m_facilitiesManager->storeMessage((*idIterator).actionID, vReceivers);
					} else
					{
						cout << "[GetDataFromNs3] ERROR: There isn't any receiver for this message" << endl;
						return EXIT_FAILURE;
					}
					++idIterator; // Go to next identifier
				}
			}
		}

		// Optimization
		if (IdentifiersStorageTable.size() > 0)
		{
			IdentifiersStorageTable.clear();
		}

		// Erase from the CAM scheduling table in case there is no more receivers (received)
		if (ScheduledCamMessageTable.size() > 0)
        {
			vector<ScheduledCamMessageData>::iterator mIterator = ScheduledCamMessageTable.begin();
			while (mIterator != ScheduledCamMessageTable.end())
			{
				if ((*mIterator).received)
				{
#ifdef LOG_ON
                    stringstream log;
					log
					<< "GetDataFromNs3() A received message has been erased from Scheduled CAM Message Table [senderID|time|seqN]: "
					<< "[" << (*mIterator).senderNs3ID << "|" << (*mIterator).timeStep << "|" << (*mIterator).sequenceNumber
					<< "]";
					IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
					mIterator = ScheduledCamMessageTable.erase(mIterator);
				} else
				{
					++mIterator; // Go to the next schedule message
				}
			}
		} else
		{
#ifdef LOG_ON
			stringstream log;
			log << "GetDataFromNs3() There isn't any scheduled CAM message to be erased.";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
		}

	}
	return EXIT_SUCCESS;


}


int SyncManager::ProcessTopobroadcastMessages(Message & message)
{
	MessageMap::iterator scheduledIt = m_messageMap.find(message.messageId);
	if (scheduledIt != m_messageMap.end())
	{
		message.senderIcsId = scheduledIt->second.senderIcsId;
		message.appMessageId = scheduledIt->second.appMessageId;
		message.actionId = scheduledIt->second.actionId;
		message.appMessageId = scheduledIt->second.appMessageId;
		// Call the Facilities
		vector<stationID_t> vReceiver;
		vReceiver.push_back(message.receiverIcsId);
		m_facilitiesManager->storeMessage(message.actionId, vReceiver);

#ifdef LOG_ON
		stringstream log;
		log << "iCS --> ProcessAppMessages(appMessage) sender: " << message.senderIcsId << " receiver "
				<< message.receiverIcsId;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		if (message.receiverIcsId == 0)
		{
			IcsLog::LogLevel("receiverIcsID==0", kLogLevelInfo);
			return EXIT_SUCCESS;
		}
		if (ProcessAppMessages(message) == EXIT_FAILURE)
		{
#ifdef LOG_ON
			stringstream log;
			log << "[ERROR] GetDataFromNs3() Processing AppMessage failed.";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif

		}

	} else
	{
		ostringstream log;
		log << "ProcessTopobroadcastMessages() There isn't any scheduled message with id " << message.messageId;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
		//			I have to delete the contained if the message is not scheduled.
		delete message.packetTagContainer;
	}
	return EXIT_SUCCESS;
}

int SyncManager::InitializeFacilities(const std::string& file)
{
	if (m_facilitiesManager->configureICSFacilities(file))
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}

int SyncManager::ScheduleV2xCamAreaMessages()
{
	vector<V2xCamArea*>* camAreas = m_v2xMessageTracker->m_v2xCamAreaCollection;

	vector<string> idNodesToStop;
	vector<string> idNodesToStart;
	// Loop on each station
	for (NodeMap::iterator nodeIt = m_iTetrisNodeMap->begin(); nodeIt != m_iTetrisNodeMap->end(); ++nodeIt)
	{

		if (nodeIt->second->m_icsId == 0)  // Skip TMC node
			continue;

		// Flag to know if the current node is inside at least one CAM area.
		bool isNodeInAnyArea = false;
		// Initialize the frequency to send CAM messages, if necessary.
		float maxFrequency = 0.0;
		// Initialize the payload length of the CAM messages to be sent, if necessary.
		unsigned int maxPayloadLength = 0;

		// Loop CAM areas
		for (vector<V2xCamArea*>::iterator camAreasIt = camAreas->begin(); camAreasIt < camAreas->end(); camAreasIt++)
		{

			// Get the subscriptionId of to the current V2xcamArea
			V2xCamArea* camArea = *camAreasIt;
			int subscriptionId = camArea->m_subscriptionId;
			Subscription* subscription = NULL;

			// Find the subscription that created the CAM area
			for (vector<Subscription*>::iterator subIt = m_subscriptionCollectionManager->begin();
					subIt < m_subscriptionCollectionManager->end(); subIt++)
			{
				subscription = *subIt;
				if (subscriptionId == subscription->m_id)
				{
					break;
				}

			}
			if (subscription == NULL)
			{
				cout << "iCS --> [ScheduleV2xCamAreaMessages] ERROR The CAM area was not created by a subscription." << endl;
				return EXIT_FAILURE;
			} else
			{

				// Cast subscription to the SubsSetCamArea type
				SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);

				nodeStatusInArea_t nodeStatus = subSetCamArea->checkNodeStatus(nodeIt->second);

				switch (nodeStatus)
				{
                case (JustArrivedInArea):
				{
					subSetCamArea->AddNodeToVector(nodeIt->second);
					if (maxFrequency < subSetCamArea->GetFrequency())
						maxFrequency = subSetCamArea->GetFrequency();

					if (maxPayloadLength < camArea->m_payloadLength)
						maxPayloadLength = camArea->m_payloadLength;

					isNodeInAnyArea = true;
					break;
				}
				case (JustLeftArea):
				{
					subSetCamArea->RemoveNodeFromVector(nodeIt->second);
					break;
				}
				case (StillInsideArea):
				{
					isNodeInAnyArea = true;
					if (maxFrequency < subSetCamArea->GetFrequency())
						maxFrequency = subSetCamArea->GetFrequency();

					if (maxPayloadLength < camArea->m_payloadLength)
						maxPayloadLength = camArea->m_payloadLength;

					break;
				}
				case (StillOutsideArea):
				{
					break;
				}
				default:
				{
					cout << "iCS --> [ScheduleV2xCamAreaMessages] ERROR It is not possible to determine the status of node "
							<< nodeIt->second->m_icsId;
					cout << " within the current V2X-cam-area." << endl;
					return EXIT_FAILURE;
				}
				}
			}
		}

		if (SubsSetCamArea::isNodeInGeneralCamSubscriptionVector(nodeIt->second))
		{
			if (isNodeInAnyArea
					&& (maxFrequency != SubsSetCamArea::getCamFrequencyFromCamSubscriptionVector(nodeIt->second)
			|| maxPayloadLength != SubsSetCamArea::getCamPayloadLengthFromCamSubscriptionVector(nodeIt->second)))
			{
				// Stop sending CAM messages (indicate the node by its name known to the wireless simulator)
				stringstream outStop;
				outStop << nodeIt->second->m_nsId;
				idNodesToStop.push_back(outStop.str());

				// Update new maxFrequency to the generalCamSubscriptionVector
				SubsSetCamArea::setNewCamFrequencyInCamSubscriptionVector(nodeIt->second, maxFrequency);

				// Update new maxPayloadLength to the generalCamSubscriptionVector
				SubsSetCamArea::setNewPayloadLengthInCamSubscriptionVector(nodeIt->second, maxPayloadLength);

				// Start sending CAM messages with new frequency
				stringstream outStart;
				outStart << nodeIt->second->m_nsId;
				idNodesToStart.push_back(outStart.str());
			}

			if (!isNodeInAnyArea)
			{
				// Stop sending messages
				stringstream out;
				out << nodeIt->second->m_nsId;
				idNodesToStop.push_back(out.str());

				// remove the node from the generalCamSubscriptionVector
				SubsSetCamArea::removeNodeFromCamSubscriptionVector(nodeIt->second->m_icsId);
			}
		} else
		{
			if (isNodeInAnyArea)
			{
				// Add the node to the generalCamSubscriptionVector
				SubsSetCamArea::addNodeToCamSubscriptionVector(nodeIt->second, maxFrequency, maxPayloadLength);

				// Start sending CAM messages
				stringstream outStart;
				outStart << nodeIt->second->m_nsId;
				idNodesToStart.push_back(outStart.str());
			}
		}

	}

	// Stop CAM transmission for the nodes in the vector idNodesToStop
	if (idNodesToStop.size() > 0)
	{
		m_wirelessComSimCommunicator->CommandStopSendingCam(idNodesToStop);
	}

	//TODO - For sending CAM messages we are using the method CommandStartSendingCam(sendersId, payloadLength, frequency)
	//       where the payloadLength and the frequency is common for all the areas. This might NOT happen (in fact, each subscription has
	//       its own payloadLength and frequency).
	//       In order to keep the implementation compliant with the current implementation, we retrieve from the generalCamSubscriptionVector
	//       the max frequency and payloadLeght among all the nodes.
	//       However, this method has to be changed by passing per each station its frequency and payloadLength.

	// Get the max frequency for the nodes (to be removed upon update of the CommandStartSendingCam() method)
	float maxGeneralFrequency = SubsSetCamArea::getMaxFrequencyFromCamSubscriptionVector();
	// Get the max payloadLength for the nodes (to be removed upon update of the CommandStartSendingCam() method)
	unsigned int maxGeneralPayloadLength = SubsSetCamArea::getMaxPayloadLengthFromCamSubscriptionVector();

	// Start CAM transmission for the nodes in the vector idNodesToStart
	if (idNodesToStart.size() > 0)
	{
		if (!m_wirelessComSimCommunicator->CommandStartSendingCam(idNodesToStart, maxGeneralPayloadLength,
				maxGeneralFrequency))
		{
			cout
			<< "iCS --> [ScheduleV2xCamAreaMessages] ERROR While commanding to start sending CAM in the network simulator."
			<< endl;
			return EXIT_FAILURE;
		}
	}
	// --- Create Payload and store CAMs
	//TODO: We only consider a unique frequency and payload until the method CommandStartSendingCam() is fixed

	vector<TGeneralCamSubscription>::iterator itGenCamSubsVect;
	for (itGenCamSubsVect = SubsSetCamArea::generalCamSubscriptions.begin();
			itGenCamSubsVect != SubsSetCamArea::generalCamSubscriptions.end(); itGenCamSubsVect++)
	{
		// Loop the nodes to get and store Facilities Action ID

		TGeneralCamSubscription currGCS = *itGenCamSubsVect;

		// If the node does not exist anymore in the simulation, delete it from the subscription
		bool nodeInSimulation = false;
		if (currGCS.node != NULL)
		{
			ITetrisNode* node = GetNodeByIcsId(currGCS.stationId);
			if (node != NULL)
				nodeInSimulation = true;
		}

		if (nodeInSimulation)
		{
#ifdef LOG_ON
			{
				stringstream log;
				log << "[INFO] ScheduleV2xCamAreaMessages() create and store cams for node: " << itGenCamSubsVect->stationId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
			}
#endif
			bool nodeWithWave = false;
			for (set<string>::iterator itRats = currGCS.node->m_rats.begin(); itRats != currGCS.node->m_rats.end();
					itRats++)
			{
				if ((*itRats) == "WaveVehicle" || (*itRats) == "WaveRsu")
				{
					nodeWithWave = true;
					break;
				}
			}
			if (nodeWithWave)
			{
				ScheduledCamMessageData messageData;

				int numberOfMessages = (unsigned int) maxGeneralFrequency; // TODO: To delete when the method CommandStartSendingCam() is fixed

				// Stores one Action ID per message in the timestep (i.e. frequency)
				for (int i = 0; i < numberOfMessages; i++)
				{
					messageData.senderNs3ID = currGCS.node->m_nsId;
					messageData.senderIcsID = currGCS.stationId;
					messageData.messageType = CAM; // NOTE, this is dynamic, should change to DNEM or so
					messageData.timeStep = m_simStep + m_timeResolution;
					messageData.sequenceNumber = i;
					messageData.actionID = m_facilitiesManager->createCAMpayload(currGCS.stationId); // NOTE, this is dynamic
					messageData.received = false;
					m_v2xMessageTracker->InsertCamRow(ScheduledCamMessageTable, messageData);
				}
			} else
			{
#ifdef LOG_ON
				stringstream log;
				log << "[WARNING] ScheduleV2xCamAreaMessages() Wave technology hasn't been installed in node "
						<< currGCS.stationId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
			}
		}
	}
	return EXIT_SUCCESS;
}

int SyncManager::ScheduleV2xGeobroadcastMessages()
{

	return EXIT_SUCCESS;
}

int SyncManager::ScheduleV2xUnicastMessages(stationID_t sender, int appHandlerId, ITetrisNode* destination,
		int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
		unsigned char commProfile, unsigned char preferredRATs, unsigned int msgLifetime)
{

	string extra;
	std::vector<unsigned char> *container = new std::vector<unsigned char>();
	return ScheduleV2xUnicastMessages(sender, appHandlerId, destination, appMessageId, appMessageType, frequency,
			payloadLength, msgRegenerationTime, commProfile, preferredRATs, msgLifetime, extra, m_simStep, container);

}

int SyncManager::ScheduleV2xUnicastMessages(stationID_t sender, int appHandlerId, ITetrisNode* destination,
		int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
		unsigned char commProfile, unsigned char preferredRATs, unsigned int msgLifetime, string & extra, double time,
		std::vector<unsigned char> *genericContainer)
{
	if (destination == NULL)
	{
		IcsLog::LogLevel("ScheduleV2xUnicastMessages() Destination node is null pointer.", kLogLevelError);
		return EXIT_FAILURE;
	}

	vector<RATID> *rats = m_facilitiesManager->getStationActiveRATs(sender);
	vector<string> techList;
	if (rats->size() == 0)
	{
		cout << "[ScheduleResult] getStationActiveRATs returned 0 size" << endl;
		return EXIT_FAILURE;
	} else
	{
		for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++)
		{
			switch (*it)
			{
			case WAVE:
			{
				// Check if the available technology is also among the preferred for the message
				if ((preferredRATs & 0x01) == 0x01)
				{
					techList.push_back("WaveVehicle");
				}
				break;
			}
			case UMTS:
			{
				// Check if the available technology is also among the preferred for the message
				if ((preferredRATs & 0x02) == 0x02)
				{
					techList.push_back("UmtsVehicle");
				}
				break;
			}
			case WiMAX:
			{
                // Check if the available technology is also among the preferred for the message
				if ((preferredRATs & 0x04) == 0x04)
				{
					techList.push_back("WimaxVehicle");
				}
				break;
			}
			case DVBH:
			{
                if ((preferredRATs & 0x08) == 0x08)
                {
                    techList.push_back("DvbhVehicle");
                }
				break;
			}
            //new LTE support for iTETRIS
		    case LTE: { 
                if ((preferredRATs & 0x10) == 0x10)
				{
		       		 techList.push_back("LteVehicle");
				}
		    	break;
		    }

			default:
			{
				cout << "[ERROR] ScheduleV2xUnicastMessages() There is no match for the type of RAT" << endl;
				delete rats;
				return EXIT_FAILURE;
			}
			}
		}
	}
	// Convert sender ID to string in a vector
	vector<string> sendersId;
	int senderNs3Id = GetNodeByIcsId(sender)->m_nsId;
	sendersId.push_back(utils::Conversion::int2String(senderNs3Id));
	// Convert receiver
	unsigned int destinationNs3Id = destination->m_nsId;

	string serviceId = "NULL";
	for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin();
			it != m_applicationHandlerCollection->end(); ++it)
	{
		if ((*it)->m_id == appHandlerId)
		{
			serviceId = (*it)->m_serviceId.unicastServiceId;
			break;
		}
	}
	if (serviceId == "NULL")
	{
		cout << "iCS --> [ERROR] ScheduleV2xUnicastMessages() Service ID is not correct" << endl;
		return EXIT_FAILURE;
	}

	// 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES

	// Stores one Action ID per message in the timestep (i.e. frequency)
	for (int i = 0; i < frequency; i++)
	{
		Message messageData;
		messageData.senderNs3Id = senderNs3Id;
		messageData.senderIcsId = sender;
		messageData.receiverNs3Id = destination->m_nsId;
		messageData.receiverIcsId = destination->m_icsId;
		messageData.messageType = UNICAST;
		messageData.timeStep = m_simStep;
		messageData.sequenceNumber = i;
		messageData.appMessageId = appMessageId;
		messageData.messageId = ++m_messageId;

#ifdef LOG_ON
		ostringstream log;
		log << "iCS -->  [ScheduleV2xUnicastMessages] try to schedule UNICAST from " << messageData.senderIcsId << " to "
				<< messageData.receiverIcsId << " Id " << appMessageId << " SN " << messageData.sequenceNumber << " at "
				<< messageData.timeStep;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

		//Call facilities
		TApplicationMessageDestination appDest;
		appDest.dest_station = messageData.receiverIcsId;
		messageData.actionId = m_facilitiesManager->createApplicationMessagePayload(messageData.senderIcsId,
				messageData.messageType, appDest, preferredRATs, payloadLength, commProfile, appHandlerId, appMessageType,
				appMessageId, extra);
		m_messageMap[messageData.messageId] = messageData;

		// 2. SCHEDULE MESSAGE IN ns-3
		ITetrisNode& receiverReference = *destination;
		ITetrisNode* senderNode = GetNodeByIcsId(sender);
		ITetrisNode& senderReference = *senderNode;
		const type_info& typeinfoRcv = typeid(receiverReference);
		const type_info& typeinfoSender = typeid(senderReference);

#ifdef LOG_ON
		ostringstream log2;
		log2 << "iCS -->  [ScheduleV2xUnicastMessages] serviceId " << serviceId << " commProfle " << (int) commProfile
				<< " frequency " << frequency << " payloadLength " << payloadLength << " sourceNs3Id " << senderNs3Id
				<< " destinationNs3Id " << destinationNs3Id << " msgRegenerationTime " << msgRegenerationTime
				<< " msgLifetime " << msgLifetime << " messageId " << messageData.messageId;
		IcsLog::LogLevel((log2.str()).c_str(), kLogLevelInfo);
#endif

		// Decide whether the tx is from the a RSU to a vehicle or viceversa
		if ((typeinfoSender == typeid(VehicleNode)
				|| ((typeinfoSender == typeid(FixedNode)) && techList.front() == "WaveVehicle")))
		{
			//Sender->vehicle  Receiver->RSU
			if (!(SyncManager::m_wirelessComSimCommunicator->CommandStartIdBasedTxon(sendersId, serviceId, commProfile,
					techList, frequency, payloadLength, destinationNs3Id, msgRegenerationTime, msgLifetime,
					time + m_timeResolution, messageData.messageId, genericContainer)))
			{
#ifdef LOG_ON
				stringstream log;
				log << "iCS -->  [ScheduleV2xUnicastMessages] [ERROR] ScheduleResult() Error while scheduling id based Txon";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif

				delete rats;
				return EXIT_FAILURE;
			} else
			{
#ifdef LOG_ON
				stringstream log;
				log << "iCS -->  [ScheduleV2xUnicastMessages] message correctly scheduled for transmission in ns-3. MsgId="
						<< messageData.messageId;
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
			}

		} else
		{
			//Sender->RSU  Receiver->vehicle
			//TODO add message id
			if (!(SyncManager::m_wirelessComSimCommunicator->CommandStartIpCiuTxon(sendersId, serviceId, frequency,
					payloadLength, destinationNs3Id, msgRegenerationTime, genericContainer)))
			{
#ifdef LOG_ON
				stringstream log;
				log << "iCS -->  [ScheduleV2xUnicastMessages] [ERROR] ScheduleResult() Error while scheduling IP based Txon";
				IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
				delete rats;
				return EXIT_FAILURE;
			}
		}
	}
	delete rats;
	return EXIT_SUCCESS;
}

int SyncManager::ScheduleV2XApplicationMessages(stationID_t sender, int appHandlerId, ITetrisNode* destination,
		int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
		unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, short numHops,
		unsigned int msgLifetime)
{
	std::vector<unsigned char> *container = new std::vector<unsigned char>();
	return ScheduleV2XApplicationMessages(sender, appHandlerId, destination, appMessageId, appMessageType, frequency,
			payloadLength, msgRegenerationTime, commProfile, preferredRATs, areas, numHops, msgLifetime, container);

}

int SyncManager::ScheduleV2XApplicationMessages(stationID_t sender, int appHandlerId, ITetrisNode* destination,
		int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
		unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, short numHops,
		unsigned int msgLifetime, std::vector<unsigned char> *genericContainer)
{

	if ((destination == NULL) && (areas.size() == 0))
	{
		IcsLog::LogLevel(
				"ScheduleV2XApplicationMessages() At least Destination node OR Gegraphic Area MUST NOT be null pointer.",
				kLogLevelError);
		return EXIT_FAILURE;
	}
	string extra;
	if (appMessageType == UNICAST)
	{
		if (ScheduleV2xUnicastMessages(sender, appHandlerId, destination, appMessageId, appMessageType, frequency,
				payloadLength, msgRegenerationTime, commProfile, preferredRATs, msgLifetime, extra, m_simStep,
				genericContainer) == EXIT_FAILURE)
		{

			IcsLog::LogLevel("iCS --> [ScheduleV2xUnicastMessages] Failure scheduling Geobroadcast.", kLogLevelError);
			return EXIT_FAILURE;
		}
	} else if (appMessageType == GEOBROADCAST)
	{
		if (ScheduleV2xGeobroadcastMessages(sender, appHandlerId, appMessageId, frequency, payloadLength,
				msgRegenerationTime, appMessageType, commProfile, preferredRATs, areas, msgLifetime, extra, m_simStep,
				genericContainer) == EXIT_FAILURE)
		{

			IcsLog::LogLevel("iCS --> [ScheduleV2xGeobroadcastMessages] Failure scheduling Geobroadcast.", kLogLevelError);
			return EXIT_FAILURE;
		}
	} else if (appMessageType == TOPOBROADCAST)
	{
		if (ScheduleV2xTopobroadcastMessages(sender, appHandlerId, appMessageId, frequency, payloadLength,
				msgRegenerationTime, appMessageType, commProfile, preferredRATs, numHops, msgLifetime, extra,
				genericContainer) == EXIT_FAILURE) // TODO add numHops in the subs
		{
			IcsLog::LogLevel("iCS --> [ScheduleV2xTopobroadcastMessages] Failure scheduling Geobroadcast.", kLogLevelError);
			return EXIT_FAILURE;
		}

	} else if (appMessageType == CAM)
	{
		IcsLog::LogLevel(
				"iCS --> [ScheduleV2XApplicationMessages()] CAM scheduling should not be sent using this subscription. Please use ScheduleCAMAreaMessage() instead",
				kLogLevelError);
		return EXIT_FAILURE;

	} else if (appMessageType == DENM)
	{
		IcsLog::LogLevel("iCS --> [ScheduleV2XApplicationMessages()] Dedicated DENM scheduling is not supported yet.",
				kLogLevelError);
		return EXIT_FAILURE;

	} else
	{
		IcsLog::LogLevel("ScheduleV2XApplicationMessages() Unsupported appMessageType.", kLogLevelError);
		return EXIT_FAILURE;
	}

	if (m_messageMap.size() > 0)
		RefreshScheduledMessageMap();

	return EXIT_SUCCESS;
}

int SyncManager::ScheduleV2xTopobroadcastMessages(stationID_t sender, int appHandlerId, int appMessageId,
		int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType,
		unsigned char commProfile, unsigned char preferredRATs, short numHops, unsigned int msgLifetime)
{
	string extra;
	std::vector<unsigned char> *container = new std::vector<unsigned char>();
	return ScheduleV2xTopobroadcastMessages(sender, appHandlerId, appMessageId, frequency, payloadLength, msgRegenerationTime,
			appMessageType, commProfile, preferredRATs, numHops, msgLifetime, extra, container);

}

int SyncManager::ScheduleV2xTopobroadcastMessages(stationID_t sender, int appHandlerId, int appMessageId,
		int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType,
		unsigned char commProfile, unsigned char preferredRATs, short numHops, unsigned int msgLifetime, string & extra,
		std::vector<unsigned char> *genericContainer)
{

	vector<RATID> *rats = m_facilitiesManager->getStationActiveRATs(sender);
	vector<string> techList;
	if (rats->size() == 0)
	{
		IcsLog::LogLevel("[ScheduleResult] getStationActiveRATs returned 0 size", kLogLevelError);
		return EXIT_FAILURE;
	} else
	{
		bool nodeWithWave = false;
		for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++)
		{
			if ((*it) == WAVE)
			{
				nodeWithWave = true;
				break;
			}
		}

		if (!nodeWithWave)
		{
			IcsLog::LogLevel("[ERROR] ScheduleV2xTopobroadcastMessages() There is no match for the type of RAT",
					kLogLevelError);
			return EXIT_FAILURE;
		}

		if (nodeWithWave && ((preferredRATs & 0x01) != 0x01))
		{
			IcsLog::LogLevel(
					"[ERROR] ScheduleV2xTopobroadcastMessages() Topobroadcast can be performed only with WAVE, and this technology was not set in the preferred technologies in the application message header.",
					kLogLevelError);
			return EXIT_FAILURE;
		}
	}

	// Convert sender ID to string in a vector
	vector<string> sendersId;
	int senderNs3Id = GetNodeByIcsId(sender)->m_nsId;
	sendersId.push_back(utils::Conversion::int2String(senderNs3Id));

	string serviceId = "NULL";
	for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin();
			it != m_applicationHandlerCollection->end(); ++it)
	{
		if ((*it)->m_id == appHandlerId)
		{
			serviceId = (*it)->m_serviceId.topoBroadcastServiceId;
			break;
		}
	}

	if (serviceId == "NULL")
	{
		IcsLog::LogLevel("iCS --> [ERROR] ScheduleV2xTopobroadcastMessages() Service ID is not correct", kLogLevelError);
		return EXIT_FAILURE;
	}

	// 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES

	// Stores one Action ID per message in the timestep (i.e. frequency)
	for (int i = 0; i < frequency; i++)
	{
		Message messageData;
		messageData.senderNs3Id = senderNs3Id;
		messageData.senderIcsId = sender;
		messageData.messageType = TOPOBROADCAST;
		messageData.timeStep = m_simStep;
		messageData.sequenceNumber = i;
		messageData.appMessageId = appMessageId;
		messageData.messageId = ++m_messageId;

		//Call facilities
		TApplicationMessageDestination appDest;
		appDest.dest_numHops = numHops;
		messageData.actionId = m_facilitiesManager->createApplicationMessagePayload(messageData.senderIcsId,
				messageData.messageType, appDest, preferredRATs, payloadLength, commProfile, appHandlerId, appMessageType,
				appMessageId, extra);

		m_messageMap[messageData.messageId] = messageData;
		//			m_v2xMessageTracker->InsertTopobroadcastRow(ScheduledTopobroadcastM.essageTable, messageData);

		// 2. SCHEDULE MESSAGE IN ns-3
		ITetrisNode* senderNode = GetNodeByIcsId(sender);
		ITetrisNode& senderReference = *senderNode;
		const type_info& typeinfoSender = typeid(senderReference);

		if (sender != 0)
		{
			IcsLog::LogLevel(
					"iCS --> [ERROR] ScheduleV2xTopobroadcastMessages() MessageId is not implemented. It will not work!",
					kLogLevelError);
			//TODO add message id
			if (!SyncManager::m_wirelessComSimCommunicator->CommandStartTopoTxon(sendersId, "serviceIdTopobroadcast",
					commProfile, techList, frequency, payloadLength, msgRegenerationTime, 1, (unsigned int) numHops,
					genericContainer))
			{
				cout << "[ERROR] ScheduleResult() Error while scheduling TopoBroadcast txon" << endl;
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

int SyncManager::ScheduleV2xGeobroadcastMessages(ics_types::stationID_t sender, int appHandlerId, int appMessageId,
		int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType,
		unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, unsigned int msgLifetime)
{
	string extra;
	std::vector<unsigned char> *container = new std::vector<unsigned char>();
	return ScheduleV2xGeobroadcastMessages(sender, appHandlerId, appMessageId, frequency, payloadLength, msgRegenerationTime,
			appMessageType, commProfile, preferredRATs, areas, msgLifetime, extra, m_simStep, container);

}

int SyncManager::ScheduleV2xGeobroadcastMessages(ics_types::stationID_t sender, int appHandlerId, int appMessageId,
		int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType,
		unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, unsigned int msgLifetime,
		string &extra, double time, std::vector<unsigned char> *genericContainer)
{

	vector<RATID> *rats = m_facilitiesManager->getStationActiveRATs(sender);
	std::vector<std::string> techList;
	if (rats->size() == 0)
	{
		IcsLog::LogLevel("[ScheduleResult] getStationActiveRATs returned 0 size", kLogLevelError);
		return EXIT_FAILURE;
    }
    else if (sender == 0)  // TMC node
	{
		// TMC node does not hold any techno interface
		// it only fills the technoList vector for later processing at the techno selector.
#ifdef LOG_ON
		stringstream log;
		log << "[ScheduleResult] TMC node scheduled to send geobroadcast msg";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

		if ((preferredRATs & 0x01) == 0x01)
			techList.push_back("WaveVehicle");

		if ((preferredRATs & 0x02) == 0x02)
			techList.push_back("UmtsVehicle");

		if ((preferredRATs & 0x04) == 0x04)
			techList.push_back("WimaxVehicle");

		if ((preferredRATs & 0x08) == 0x08)
			techList.push_back("DvbhVehicle");
        // LTE Support for iTETRIS
        if ((preferredRATs & 0x10) == 0x10)
			techList.push_back("LteVehicle");


		if (((preferredRATs & 0x08) != 0x08) && ((preferredRATs & 0x04) != 0x04) && ((preferredRATs & 0x02) != 0x02)
                && ((preferredRATs & 0x01) != 0x01)&& ((preferredRATs & 0x10) != 0x10))
		{
			// must return error as none of the preferred technologies are supported
#ifdef LOG_ON
			stringstream log;
			log << "ScheduleV2xGeobroadcastMessages() Geobroadcast preferred technology not supported";
			IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
			return EXIT_FAILURE;
		}

    }
    else  // at least one preferred techno and not TMC
	{
		bool nodeWithWave = false;
		for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++)
		{
			if ((*it) == WAVE)
			{
				nodeWithWave = true;
				break;
			}
		}

        if (!nodeWithWave) // in current ETSI standard, Geobroadcast can only be done by WAVE
		{
			IcsLog::LogLevel("[ERROR] ScheduleV2xGeobroadcastMessages() There is no match for the type of RAT",
					kLogLevelError);
			return EXIT_FAILURE;
		}

		if (nodeWithWave && ((preferredRATs & 0x01) != 0x01))
		{
			IcsLog::LogLevel("[ERROR] ScheduleV2xGeobroadcastMessages() Geobroadcast can be performed only with WAVE, "
					"and this technology was not set in the preferred technologies in the application message header.",
					kLogLevelError);
			return EXIT_FAILURE;
		}
	}

	// Convert sender ID to string in a vector
	std::vector<std::string> sendersId;
	ITetrisNode * senderNode = GetNodeByIcsId(sender);
	int senderNs3Id = senderNode->m_nsId;
	sendersId.push_back(utils::Conversion::int2String(senderNs3Id));

	string serviceId = "NULL";
	for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin();
			it != m_applicationHandlerCollection->end(); ++it)
	{
		if ((*it)->m_id == appHandlerId)
		{
			serviceId = (*it)->m_serviceId.geoBroadcastServiceId;
			break;
		}
	}

	if (serviceId == "NULL")
	{
		IcsLog::LogLevel("iCS --> [ERROR] ScheduleV2xGeobroadcastMessages() Service ID is not correct", kLogLevelError);
		return EXIT_FAILURE;
	}

	// 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES

	// Stores one Action ID per message in the timestep (i.e. frequency)
	for (int i = 0; i < frequency; i++)
	{
		Message messageData;
		messageData.senderNs3Id = senderNs3Id;
		messageData.senderIcsId = sender;
		messageData.messageType = GEOBROADCAST;
		messageData.timeStep = m_simStep;
		messageData.sequenceNumber = i;
		messageData.appMessageId = appMessageId;
		messageData.messageId = ++m_messageId;

		//Call facilities
		TApplicationMessageDestination appDest;
		appDest.dest_areas = areas;
		messageData.actionId = m_facilitiesManager->createApplicationMessagePayload(messageData.senderIcsId,
				messageData.messageType, appDest, preferredRATs, payloadLength, commProfile, appHandlerId, appMessageType,
				appMessageId, extra);
		m_messageMap[messageData.messageId] = messageData;
		//				m_v2xMessageTracker->InsertGeobroadcastRow(ScheduledGeobroadcastMessageTable, messageData);

		// 2. SCHEDULE MESSAGE IN ns-3

		CircularGeoAddress destination;
		Area2D* area2D = m_facilitiesManager->getWholeArea(areas);
		Circle circle = m_facilitiesManager->getCircleFromArea(area2D);

		//Need to check if the returned pointer is not the one inside the vector
		if (areas.size() == 1 && areas[0] != area2D)
			delete area2D;

		destination.lat = (uint32_t) (circle.getCenter().x()<0?0:circle.getCenter().x()); // Modified by acorrea to use cartesian coordinates
		destination.lon = (uint32_t) (circle.getCenter().y()<0?0:circle.getCenter().y()); // Modified by acorrea to use cartesian coordinates
		destination.areaSize = (uint32_t) circle.getArea();

		if (sender == 0)
		{
			IcsLog::LogLevel("[ERROR] CommandStartMWTxon does not support messageid. It will not work", kLogLevelError);
			//TODO Add message id
			if (!SyncManager::m_wirelessComSimCommunicator->CommandStartMWTxon(sendersId, serviceId, commProfile, techList,
					destination, (float) frequency, (uint32_t) payloadLength, (double) msgRegenerationTime,
					(uint8_t) msgLifetime, genericContainer))
			{
				IcsLog::LogLevel("[ERROR] ScheduleResult() Error while scheduling MWtxon", kLogLevelError);
				return EXIT_FAILURE;
			}
        }
        else
		{
			if (!SyncManager::m_wirelessComSimCommunicator->StartGeobroadcastTxon(sendersId, serviceId, commProfile,
					techList, destination, frequency, payloadLength, msgRegenerationTime, msgLifetime,
					time + m_timeResolution /*sendTime*/, messageData.messageId, genericContainer))
			{
				IcsLog::LogLevel("[ERROR] ScheduleResult() Error while scheduling GeoBroadcast txon", kLogLevelError);
				return EXIT_FAILURE;
			} else
			{
				ostringstream log2;
				log2 << "ScheduleV2xGeobroadcastMessages() Geobroadcast message scheduled. MsgId=" << messageData.messageId;
				IcsLog::LogLevel((log2.str()).c_str(), kLogLevelInfo);
			}
		}
	}

	return EXIT_SUCCESS;
}

int SyncManager::RefreshScheduledCamMessageTable()
{
	if (ScheduledCamMessageTable.size() > 0)
	{
		vector<ScheduledCamMessageData>::iterator messageIterator = ScheduledCamMessageTable.begin();
		while (messageIterator != ScheduledCamMessageTable.end())
		{
			if (m_simStep >= ((*messageIterator).timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp))
			{
				messageIterator = ScheduledCamMessageTable.erase(messageIterator);
			} else
			{
				++messageIterator;
			}
		}
	} else
	{
//#ifdef LOG_ON
//		stringstream log;
//		log << "[WARNING] RefreshScheduledCamMessageTable() CAM scheduled table is empty";
//		IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
//#endif
	}

	return EXIT_SUCCESS;
}



int SyncManager::ProcessUnicastMessages(Message& receivedMessage, ITetrisNode const * node) {
    bool messageProcessed = false;
    MessageMap::iterator scheduledIt = m_messageMap.find(receivedMessage.messageId);
    if (scheduledIt != m_messageMap.end())
    {
        receivedMessage.senderIcsId = scheduledIt->second.senderIcsId;
        receivedMessage.receiverIcsId = scheduledIt->second.receiverIcsId;
        receivedMessage.actionId = scheduledIt->second.actionId;
        receivedMessage.appMessageId = scheduledIt->second.appMessageId;
        // Call the Facilities
        vector<stationID_t> vReceiver;
        vReceiver.push_back(receivedMessage.receiverIcsId);
        m_facilitiesManager->storeMessage(receivedMessage.actionId, vReceiver);

        // Loop the SUBSCRIPTIONs of the node
        for (vector<Subscription*>::iterator subsIt = node->m_subscriptionCollection->begin();
                subsIt != node->m_subscriptionCollection->end(); ++subsIt)
        {
            Subscription* subscription = (*subsIt);
            Subscription& reference = *subscription;
            const type_info& typeofSubscription = typeid(reference);
            if (typeofSubscription == typeid(SubsAppMessageReceive))
            {
                SubsAppMessageReceive* appMsgReceive = static_cast<SubsAppMessageReceive*>(subscription);
                if (appMsgReceive->ProcessReceivedAppMessage(receivedMessage, GetAddress()) == EXIT_FAILURE)
                {
                    IcsLog::LogLevel("[ProcessUnicastMessages] Error processing App message.", kLogLevelError);
                    return EXIT_FAILURE;
                } else
                {
                    if (appMsgReceive->getLastMessageAddedToReceived())
                    {
                        messageProcessed = true;
#ifdef LOG_ON
                        stringstream log;   //only print the log if successful
                        log << "[ProcessUnicastMessages] for APP_MSG_RECEIVE subscriptions:  senderID "
                                << receivedMessage.senderIcsId << " receiverID " << receivedMessage.receiverIcsId << " appID "
                                << receivedMessage.appMessageId << " ActionID " << receivedMessage.actionId;
                        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                    }
                }
            }
        }

        m_messageMap.erase(scheduledIt);
    } else
    {
        ostringstream log;
        log << "iCS --> sync-manager - GetDataFromNs3() There isn't a scheduled message with id "
                << receivedMessage.messageId;
        IcsLog::LogLevel(log.str().c_str(), kLogLevelWarning);
    }
    if (!messageProcessed)
        //                      I have to delete the contained if the message is not processed. This can happen when penetration ration is less than 100%
        //                      since the shadow nodes do not have any subscription.
        delete receivedMessage.packetTagContainer;
    return EXIT_SUCCESS;
}


int SyncManager::ProcessGeobroadcastMessages(Message& message)
{
	// Check if the message was scheduled
#ifdef LOG_ON
    stringstream tmp;
	tmp << "sender " << message.senderNs3Id << " type " << message.messageType << " time " << message.timeStep
			<< " sequence " << message.sequenceNumber << " msgId " << message.messageId;
	IcsLog::LogLevel((tmp.str()).c_str(), kLogLevelInfo);
#endif

	MessageMap::iterator scheduledIt = m_messageMap.find(message.messageId);
	if (scheduledIt != m_messageMap.end())
	{
		message.actionId = scheduledIt->second.actionId;
		// Change the status of the message (marks as received but more nodes could receive it)
		scheduledIt->second.received = true;
		message.appMessageId = scheduledIt->second.appMessageId;
		// Call the Facilities.
		vector<stationID_t> vReceiver;
		vReceiver.push_back(message.receiverIcsId);
		m_facilitiesManager->storeMessage(message.actionId, vReceiver);
	} else
	{
#ifdef LOG_ON
        ostringstream log;
		log << "ProcessGeobroadcastMessages() There isn't any scheduled message with the id " << message.messageId;
        IcsLog::LogLevel(log.str().c_str(), kLogLevelWarning);
#endif
		//			I have to delete the contained if the message is not scheduled.
		delete message.packetTagContainer;
		return EXIT_SUCCESS;
	}

	if (ProcessAppMessages(message) == EXIT_FAILURE)
	{
#ifdef LOG_ON
		stringstream log;
		log << "[ERROR] GetDataFromNs3() Processing AppMessage in ProcessGeoBroadcastMessages() failed.";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int SyncManager::ProcessAppMessages(Message & appMessage)
{
	// checking for APP msg subscriptions
	// 1. Get reference to receiver
	ITetrisNode* receiver = GetNodeByIcsId(appMessage.receiverIcsId);

	// 2. Check whether the receiver runs app that has subscription able to process the message
	bool messageProcessed = false;
	bool found = false;
#ifdef LOG_ON
    ostringstream oss;
#endif
	for (vector<Subscription*>::iterator it = receiver->m_subscriptionCollection->begin();
			it != receiver->m_subscriptionCollection->end(); ++it)
	{
		Subscription& reference = *(*it);
		const type_info& typeofSubscription = typeid(reference);
		if (typeofSubscription == typeid(SubsAppMessageReceive))
		{
			SubsAppMessageReceive* appMsgReceive = static_cast<SubsAppMessageReceive*>(*it);
#ifdef LOG_ON
            oss << "sub id: " << appMsgReceive->m_id << " type: " << (int) appMsgReceive->m_appMsgType << " message type: "
					<< appMessage.appMessageId << endl;
#endif
			//3 check if the message is of the same type
			if (appMsgReceive->m_appMsgType == appMessage.appMessageId)
			{
				//4 process subscription
				found = true;
				if (appMsgReceive->ProcessReceivedAppMessage(appMessage, GetAddress()) == EXIT_FAILURE)
				{
					IcsLog::LogLevel("ProcessAppMessages() Error processing App message.", kLogLevelError);
					return EXIT_FAILURE;
				}
				if (appMsgReceive->getLastMessageAddedToReceived())
					messageProcessed = true;
				break;
			}
		}
	}
	if (!found)
	{
#ifdef LOG_ON
        IcsLog::LogLevel(oss.str().c_str(), kLogLevelWarning);
		stringstream log;
		log << "ProcessAppMessages() The message has no subscription to be processed.";
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
	}
	if (!messageProcessed)
		//		I have to delete the contained if the message is not processed. This can happen when penetration ration is less than 100%
		//		since the shadow nodes do not have any subscription.
		delete appMessage.packetTagContainer;
	return EXIT_SUCCESS;
}

bool SyncManager::AddNode(ITetrisNode * node, bool assingToOtherTables)
{
    //    cout << "Adding node " << node->m_icsId << ".\n"
    //            << "SUMO-ID: '" << node->m_tsId << "', "
    //            << "ns3-ID: '" << node->m_nsId << "'" << endl;
    m_iTetrisNodeMap->operator[](node->m_icsId) = node;
    if (assingToOtherTables)
    {
        if (m_SumoIdToIcsIdMap->find(node->m_tsId) != m_SumoIdToIcsIdMap->end()) {
#ifdef LOG_ON
            stringstream log;
            log << "Tried to add existing node '"<< node->m_tsId <<"' to SUMO-ID map.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
            return false;
        } else {
            m_SumoIdToIcsIdMap->operator[](node->m_tsId) = node->m_icsId;
            m_NS3IdToIcsIdMap->operator[](node->m_nsId) = node->m_icsId;
        }
    }
#ifdef LOG_ON
    ostringstream log;
    log << "AddNode() Added node. icsId=" << node->m_icsId << ", ns3Id=" << node->m_nsId << ", sumoId=" << node->m_tsId
            << ". Assing=" << assingToOtherTables;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    return true;
}

void SyncManager::DeleteNode(ITetrisNode * node)
{
	m_iTetrisNodeMap->erase(node->m_icsId);
	m_NS3IdToIcsIdMap->erase(node->m_nsId);
	m_SumoIdToIcsIdMap->erase(node->m_tsId);
	delete node;
}

void SyncManager::RefreshScheduledMessageMap()
{
	int num = 0;
	for (MessageMap::iterator it = m_messageMap.begin(); it != m_messageMap.end();)
	{
		if (m_simStep >= (it->second.timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp))
		{
			++num;
			m_messageMap.erase(it++);
		} else
			++it;
	}
#ifdef LOG_ON
	if (m_messageMap.size() > 0 || num > 0) {
	    ostringstream log;
	    log << "RefreshScheduledMessageMap() Current size=" << m_messageMap.size() << ". Removed=" << num << " time"
	            << ITetrisSimulationConfig::m_scheduleMessageCleanUp;
	    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
	}
#endif
}

void SyncManager::UpdateNodeId(ITetrisNode * node, bool addNs3, bool addSumo)
{
#ifdef LOG_ON
    ostringstream log;
	log << "UpdateNodeId() Update node " << node->m_icsId;
#endif
	if (addNs3)
	{
		m_NS3IdToIcsIdMap->operator[](node->m_nsId) = node->m_icsId;
#ifdef LOG_ON
        log << ". Added ns3Id=" << node->m_nsId;
#endif
	}
	if (addSumo)
	{
		m_SumoIdToIcsIdMap->operator[](node->m_tsId) = node->m_icsId;
#ifdef LOG_ON
		log << ". Added sumoId=" << node->m_tsId;
#endif
	}
#ifdef LOG_ON
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
}

}  //syncmanager
