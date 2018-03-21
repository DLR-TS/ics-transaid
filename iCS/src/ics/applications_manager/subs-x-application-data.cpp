/****************************************************************************/
/// @file    subs-x-application-data.h
/// @author  Jerome Haerri (EURECOM)
/// @date    June 17th 2012
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

#include "subs-x-application-data.h"
#include "app-result-generic.h"
#include "subscriptions-type-constants.h"
#include "app-commands-subscriptions-constants.h"
#include "../itetris-node.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"

#include "application-handler.h"

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int SubsXApplicationData::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
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
		if (typeinfo == typeid(SubsXApplicationData*))
		{
			SubsXApplicationData* subsXApplicationData = static_cast<SubsXApplicationData*>(sub);
			if (subsXApplicationData->m_nodeId == stationID)
			{
				delete subsXApplicationData;
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
SubsXApplicationData::SubsXApplicationData(int appId, ics_types::stationID_t stationId, SyncManager* syncManager,
		unsigned char* msg, int msgSize) :
		Subscription(stationId), m_msg(msg, msgSize)
{

	m_id = ++m_subscriptionCounter;

	m_name = "RETURN CROSS APPLICATION DATA";

	m_appId = appId;

	m_nodeId = stationId;

#ifdef LOG_ON
	stringstream log;
	log << "[INFO] SubsXApplicationData() - appID: " << appId;
	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

	m_data = pull(syncManager);

}

std::vector<unsigned char> SubsXApplicationData::pull(SyncManager* syncManager)
{

	vector<unsigned char> info;
	unsigned int index = 0;

	unsigned char cmdMode = m_msg.readUnsignedByte(); // Defines the command mode and thus the way the rest of the msg will be read.

	switch (cmdMode)
	{

	case VALUE_GET_DATA_BY_RESULT_CONTAINER_ID:
	{

		int cmd = m_msg.readUnsignedByte();
		std::string tag = m_msg.readString();

		bool found = false;
		// Loop Applications
		ITetrisNode* node_local = syncManager->GetNodeByIcsId(m_nodeId);
#ifdef LOG_ON
        stringstream log;
        log << "SubsXApplicationData::pull node_local->m_applicationHandlerInstalled.size()"
                << node_local->m_applicationHandlerInstalled->size();
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		for (vector<ApplicationHandler*>::iterator appHandlerIt = node_local->m_applicationHandlerInstalled->begin();
				appHandlerIt < node_local->m_applicationHandlerInstalled->end(); appHandlerIt++)
		{
			ApplicationHandler* appHandler = (*appHandlerIt);

			if (appHandler->m_id == m_appId)
			{

#ifdef LOG_ON
                stringstream log;
                log << "iCS --> Application " << appHandler->m_id << " Matches the subscribed application ID.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

				for (vector<ResultContainer*>::iterator resultIt = node_local->m_resultContainerCollection->begin();
						resultIt != node_local->m_resultContainerCollection->end(); ++resultIt)
				{
					ResultContainer* result = (*resultIt);
					ResultContainer& reference = *result;

					const type_info& type = typeid(reference);
					if (type == typeid(ResultGeneric))
					{  // the sub-x-application only works with Generic Result Containers
						ResultGeneric* resultGeneric;
						resultGeneric = static_cast<ResultGeneric*>(result);
						vector<unsigned char> tmp = resultGeneric->pull(cmd, tag);
#ifdef LOG_ON
						stringstream log;
						log << "SubsXApplicationData::pull() size returned " << tmp.size() << " for tag " << tag << " appId " << resultGeneric->m_applicationHandlerId << " size " << node_local->m_resultContainerCollection->size();
						IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
						return tmp;
					}
				}
				break;
			}
		}
		break;
	}
	default:
	{
		std::cout << "Impossible to find application for result container " << std::endl;
		return info;
	}

	}
	return info;
}

std::vector<unsigned char> SubsXApplicationData::returnStatus()
{
	return m_data;
}

SubsXApplicationData::~SubsXApplicationData()
{
}

}
