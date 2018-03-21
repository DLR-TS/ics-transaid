/****************************************************************************/
/// @file    subs-app-cmd-traff-sim.cpp
/// @author  Jerome Haerri (EURECOM)
/// @date    December 15th, 2010
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <cstring>

#include "subs-app-cmd-traff-sim.h"
#include "subscriptions-helper.h"
#include "subscriptions-type-constants.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../facilities/mapFacilities/road/Edge.h"
#include "../facilities/mapFacilities/road/Lane.h"
#include "../facilities/mapFacilities/road/Junction.h"
#include "../../utils/ics/geometric/Shapes.h"

namespace ics
{

// ===========================================================================
// Constants
// ===========================================================================

#define TYPE_ERROR			       			  0x00
#define TYPE_EDGE_ID						  0x01
#define	TYPE_STATION_ID						  0x02
#define TYPE_EDGE_TRAVELTIME       			  0x03
#define TYPE_EDGES_ID_FROM_ROAD_ID      	  0x04
// ===========================================================================
// static member definitions
// ===========================================================================
	int SubsAppCmdTraffSim::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
	{
		if (subscriptions == NULL)
		{
			return EXIT_FAILURE;
		}

		vector<Subscription*>::iterator it;
		for (it = subscriptions->begin(); it < subscriptions->end(); it++)
		{
			Subscription* sub = *it;
			const type_info& typeinfo = typeid(sub);
			if (typeinfo == typeid(SubsAppCmdTraffSim*))
			{
				SubsAppCmdTraffSim* subsAppCmdTraffSim = static_cast<SubsAppCmdTraffSim*>(sub);
				if (subsAppCmdTraffSim->m_nodeId == stationID)
				{
					delete subsAppCmdTraffSim;
					delete sub;
					return EXIT_SUCCESS;
				}
			}
		}
		return EXIT_SUCCESS;
	}

// ===========================================================================
// member method definitions
// ===========================================================================

	SubsAppCmdTraffSim::SubsAppCmdTraffSim(int appId, ics_types::stationID_t stationId, SyncManager* syncManager,
			unsigned char* msg, int msgSize) :
			Subscription(stationId), m_msg(msg, msgSize)
	{
		// Read parameters
		m_id = ++m_subscriptionCounter;

		m_name = "SEND A Command To the Traffic Simulator";

		m_appId = appId;

		m_resultStatus = push(syncManager);

	}

	SubsAppCmdTraffSim::~SubsAppCmdTraffSim()
	{
	}

	bool SubsAppCmdTraffSim::push(SyncManager* syncManager)
	{
		unsigned int index = 0;

		unsigned char appMsgType = m_msg.readChar();       // HEADER__APP_MSG_TYPE
		unsigned char cmdMode = m_msg.readChar(); // Defines the command mode and thus the way the rest of the msg will be read.

		switch (cmdMode)
		{
		case VALUE_SET_EDGE_TRAVELTIME:
		{
			IcsLog::LogLevel("SubsAppResultTraffSim: VALUE_SET_EDGE_TRAVELTIME.", kLogLevelInfo);
			ics_types::roadElementID_t edgeID = "";
			//short weight = 0;
			float weight = 2.0;

			ics_types::stationID_t m_iCS_ID = (ics_types::stationID_t) m_msg.readInt();
			//unsigned int numWeights = (unsigned int) m_msg.readChar();
			int numWeights = m_msg.readInt();
			//for (unsigned int i = 0; i < numWeights; i++)
			for (int i = 0; i < numWeights; i++) 
			{
				unsigned char typeEdgeID = m_msg.readChar();
				if (typeEdgeID == TYPE_EDGE_ID)
				{
					edgeID = m_msg.readString();
					unsigned char typeWeight = m_msg.readChar();
					if (typeWeight == TYPE_EDGE_TRAVELTIME)
					{
						weight = m_msg.readFloat();
						//ChangeEdgeWeight(const ITetrisNode &node, string edgeId, float weight)
						syncManager->m_trafficSimCommunicator->SetEdgeWeight(edgeID, weight);
						/* ITetrisNode *node = syncManager->GetNodeByIcsId(m_iCS_ID);
						 if (node->m_type==STATION_MOBILE) {
						 syncManager->m_trafficSimCommunicator->ChangeEdgeWeight(*node,edgeID,weight);
						 }*/
				syncManager->m_trafficSimCommunicator->SetEdgeWeight(edgeID, weight);
					} else
					{
						IcsLog::LogLevel(
								"To send a SET_WEIGHT command a Weight in the structure <EdgeID, Weight> must be  specified.",
								kLogLevelError);
						return false;
					}
				} else
				{
					IcsLog::LogLevel(
							"To send a SET_WEIGHT command an EdgeID in the structure <EdgeID, Weight> must be  specified.",
							kLogLevelError);
					return false;
				}
			}
			return true;
		}
		case VALUE_RE_ROUTE:
		{
			IcsLog::LogLevel("SubsAppResultTraffSim: VALUE_RE_ROUTE.", kLogLevelInfo);
			unsigned char typeID = m_msg.readChar();
			//unsigned int numIDs = (unsigned int) m_msg.readChar();
			 int numIDs = m_msg.readInt();
			if (typeID == TYPE_STATION_ID)
			{
				//for (unsigned int i = 0; i < numIDs; i++)
				for (int i = 0; i < numIDs; i++) 
				{
					ics_types::stationID_t m_iCS_ID = m_msg.readInt();
					ITetrisNode *node = syncManager->GetNodeByIcsId(m_iCS_ID);
					if (node==NULL) std::cout << " VALUE_RE_ROUTE > nodeId="<<m_iCS_ID <<" not found" << std::endl;
              				else std::cout << " VALUE_RE_ROUTE for nodeId="<< m_iCS_ID << std::endl;      
					syncManager->m_trafficSimCommunicator->ReRoute(*node);
				}
			} else
			{
				IcsLog::LogLevel("To send a RE_ROUTE command a list of nodeIDs must be  specified.", kLogLevelError);
				return false;
			}
			return true;
		}
		case VALUE_SET_SPEED:
		{
			IcsLog::LogLevel("SubsAppResultTraffSim: VALUE_SET_SPEED.", kLogLevelInfo);
			ics_types::stationID_t m_iCS_ID = m_msg.readInt();
			ITetrisNode *node = syncManager->GetNodeByIcsId(m_iCS_ID);

			if (syncManager->m_trafficSimCommunicator->CommandSetMaximumSpeed(*node, m_msg.readFloat()) == EXIT_FAILURE)
			{
				IcsLog::LogLevel("SubsAppResultTraffSim: CommandSetMaximumSpeed error.", kLogLevelError);
				return false;
			}
			return true;
		}
		case VALUE_SET_TRAFFIC_LIGHT:
		{
			std::string trafficLightId = m_msg.readString();
			std::string lightStatus = m_msg.readString();
			if (!syncManager->m_trafficSimCommunicator->ChangeTrafficLightStatus(trafficLightId, lightStatus))
			{
				IcsLog::LogLevel("SubsAppResultTraffSim: ChangeTrafficLightStatus error.", kLogLevelError);
				return false;
			}
			return true;
		}
		default:
		{
			IcsLog::LogLevel("Impossible to send the command. Command not recognized.", kLogLevelError);
			return false;
		}
		}

	}

//===================================================================

	bool SubsAppCmdTraffSim::returnStatus()
	{
		return m_resultStatus;
	}

} // end namespace ics
