/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/
/****************************************************************************************
 * Modified and Adapted for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

#include "subs-get-mobility-info.h"
#include "app-message-manager.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "foreign/tcpip/storage.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics
{

SubsGetMobilityInfo::SubsGetMobilityInfo(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize) :
    								Subscription(stationId)
{
	m_id = ++m_subscriptionCounter;
	m_name = "RETURN INFORMATION ABOUT THE POSITION OF ONE OR MORE NODES";
	m_appId = appId;
	tcpip::Storage message(msg, msgSize);
	m_mode = message.readUnsignedByte();
	if (m_mode == VALUE__LIST_ID)
	{
		short numNodes = message.readShort();
		m_listIds.reserve(numNodes);
		for (int i = 0; i < numNodes; ++i)
		{
			int nodeId = message.readInt();
			m_listIds.push_back(nodeId);
		}
	} else if (m_mode == VALUE__ZONE_ID)
	{
		float x = message.readFloat();
		float y = message.readFloat();
		float radius = message.readFloat();

		Point2D point(x, y);
		m_zone = Circle(point, radius);
	}
}

SubsGetMobilityInfo::~SubsGetMobilityInfo()
{
}

int SubsGetMobilityInfo::InformApp(AppMessageManager* messageManager)
{
	std::vector<TMobileStationDynamicInfo> * information = NULL;
	switch (m_mode)
	{
	case VALUE__ALL_ID:
		information = GetAllID();
		break;
	case VALUE__LIST_ID:
		information = GetListId();
		break;
	case VALUE__ZONE_ID:
		information = GetZoneId();
		break;
	default:
 #ifdef LOG_ON
		stringstream log;
		log << "SubsGetMobilityInfo::InformApp() Retrieve mode unknown: " << m_mode;
		IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
		return EXIT_SUCCESS;
	}
	if (information != NULL && information->size() > 0)
	{
		if (messageManager->CommandSendSubscriptionMobilityInfo(information, m_nodeId) == EXIT_FAILURE)
		{
			IcsLog::LogLevel("SubsGetMobilityInfo::InformApp() Could not send the result of the subscription",
					kLogLevelError);
			delete information;
			return EXIT_FAILURE;
		}
	}
	delete information;
	return EXIT_SUCCESS;
}

static TMobileStationDynamicInfo & GetData(const Station* station, TMobileStationDynamicInfo& data)
{
	//Use timeStep as id field
	data.timeStep = station->getID();
	data.positionX = station->getPosition().x();
	data.positionY = station->getPosition().y();
	if (station->getType() == STATION_FIXED)
		//Use exteriorLights as isMobileStation
		data.exteriorLights = false;
	else
	{
		data.exteriorLights = true;
		const MobileStation * mobileStation = static_cast<const MobileStation *>(station);
		data.acceleration = mobileStation->getAcceleration();
		data.speed = mobileStation->getSpeed();
		data.direction = mobileStation->getDirection();
		data.lane = mobileStation->getLaneID();
	}
	return data;
}

std::vector<TMobileStationDynamicInfo> * SubsGetMobilityInfo::GetAllID()
{
	const map<stationID_t, Station*> stations = SyncManager::m_facilitiesManager->getAllStations();
	std::vector<TMobileStationDynamicInfo> * info = new std::vector<TMobileStationDynamicInfo>();
	info->reserve(stations.size());
	for (map<stationID_t, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it)
	{
		if (it->second->isActive)
		{
			TMobileStationDynamicInfo data;
            //should use the most recent position from MobilityHistory
			data.timeStep = it->second->getID();
			Point2D itPos = SyncManager::m_facilitiesManager->getStationPositionsFromMobilityHistory(SyncManager::m_simStep, it->first);
			if ((itPos.x()>-100.0) && (itPos.y()>-100.0)){
				data.positionX = itPos.x(); //update position from Mobility History
				data.positionY = itPos.y(); //update position from Mobility History
#ifdef LOG_ON
				IcsLog::LogLevel("[GetAllID] get position from Mobility History", kLogLevelInfo);
#endif
			} else{
				data.positionX = it->second->getPosition().x(); //update position from sumo
				data.positionY = it->second->getPosition().y(); //update position from sumo
#ifdef LOG_ON
                IcsLog::LogLevel("[GetAllID] get position from SUMO", kLogLevelInfo);
#endif
			}

			if (it->second->getType() == STATION_FIXED)
				//Use exteriorLights as isMobileStation
				data.exteriorLights = false;
			else
			{
				data.exteriorLights = true;
				const MobileStation * mobileStation = static_cast<const MobileStation *>(it->second);
				data.acceleration = mobileStation->getAcceleration();
				data.speed = mobileStation->getSpeed();
				data.direction = mobileStation->getDirection();
				data.lane = mobileStation->getLaneID();
			}
			info->push_back(data);
		}
	}
	return info;
}

std::vector<TMobileStationDynamicInfo> * SubsGetMobilityInfo::GetListId()
{
	if (m_listIds.size() > 0)
	{
		std::vector<TMobileStationDynamicInfo> * info = new std::vector<TMobileStationDynamicInfo>();
		info->reserve(m_listIds.size());
		for (std::vector<int>::const_iterator it = m_listIds.begin(); it != m_listIds.end(); ++it)
		{
			const Station * station = SyncManager::m_facilitiesManager->getStation(*it);
			if (station)
			{
				if (station->isActive)
				{
					TMobileStationDynamicInfo data;
					info->push_back(GetData(station, data));
				}
			}
		}
		return info;
	}
    IcsLog::LogLevel("SubsGetMobilityInfo::InformApp() No id was specified", kLogLevelError);
	return NULL;
}

// get the mobility information in a given zone
std::vector<TMobileStationDynamicInfo> * SubsGetMobilityInfo::GetZoneId()
{

	map<stationID_t, const Station*>*  nodesInArea = SyncManager::m_facilitiesManager->getStationsInArea(m_zone);
	if (nodesInArea->size() != 0){

		std::vector<TMobileStationDynamicInfo> * info = new std::vector<TMobileStationDynamicInfo>();
		info->reserve(nodesInArea->size());

		for (std::map<stationID_t, const Station *>::const_iterator it = nodesInArea->begin(); it != nodesInArea->end(); ++it)
		{
			if (it->second)
			{

				if (it->second->isActive)
				{
					TMobileStationDynamicInfo data;
					//should use the most recent position from MobilityHistory
					data.timeStep = it->second->getID();
					Point2D itPos = SyncManager::m_facilitiesManager->getStationPositionsFromMobilityHistory(SyncManager::m_simStep, it->second->getID());
					if ((itPos.x()>-100.0) && (itPos.y()>-100.0)){
						data.positionX = itPos.x(); //update position from Mobility History
						data.positionY = itPos.y(); //update position from Mobility History
#ifdef LOG_ON
						IcsLog::LogLevel("[GetZoneId] get position from Mobility History", kLogLevelInfo);
#endif
					} else{
						data.positionX = it->second->getPosition().x(); //update position from sumo
						data.positionY = it->second->getPosition().y(); //update position from sumo
#ifdef LOG_ON
                        IcsLog::LogLevel("[GetZoneId] get position from SUMO", kLogLevelInfo);
#endif
                    }

					if (it->second->getType() == STATION_FIXED)
						//Use exteriorLights as isMobileStation
						data.exteriorLights = false;
					else
					{
						data.exteriorLights = true;
						const MobileStation * mobileStation = static_cast<const MobileStation *>(it->second);
						data.acceleration = mobileStation->getAcceleration();
						data.speed = mobileStation->getSpeed();
						data.direction = mobileStation->getDirection();
						data.lane = mobileStation->getLaneID();
					}
					info->push_back(data);

				}
			}
		}
		return info;

	}
	return NULL;
}

} /* namespace ics */
