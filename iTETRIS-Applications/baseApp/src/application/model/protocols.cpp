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
#include "protocols.h"
#include "scheduler.h"
#include "log/log.h"

namespace testapp
{
	namespace application
	{

		///ExecuteBase
		TraceMap & ExecuteBase::GetTracedCallbacks()
		{
			return m_traceMap;
		}

		///CentralizedProtocol
		std::string CentralizedProtocol::TraceName = "FlowCheck";
		double CentralizedProtocol::SpaceThreshold = 50;
		bool CentralizedProtocol::ReturnData = false;

		CentralizedProtocol::CentralizedProtocol()
		{
			m_traceMap[TraceName] = &m_traceFlows;
		}

		CentralizedProtocol::~CentralizedProtocol()
		{
		}

		bool CentralizedProtocol::CompluteSpeed(const NodeInfo & latest, const NodeInfo & first, double & speedAll,
				double & speedFilter)
		{
			double distance = GetDistance(latest.position, first.position);
			speedAll = distance / (latest.lastSeen - first.lastSeen) * 1000;
			if (distance >= SpaceThreshold)
			{
				speedFilter = speedAll;
				return true;
			}
			return false;
		}

		void CentralizedProtocol::AggregateDataForDirection(const NodeDataMap& data, ValueMap& valueMap)
		{
			double speed = 0;
			double spaceMeanSpeed = 0;
			double speedTime = 0;
			double speedTimeFilter = 0;
			double speedTimeSingle = 0;
			int number = 0;
			int numberSpeedFilter = 0;
			double maxd = 0;
			double mind = 1000 * 1000;
			double time = 0;
			int numLastMex = 0;
			std::ostringstream oss;
			for (NodeDataMap::const_iterator node = data.begin(); node != data.end(); ++node)
			{
				//The latest message is in the last position of the set
				NodeInfo info = *(node->second.rbegin().operator *());
				//Do not count the nodes which have sent a no longer conformant message
				if (!info.toRemove)
				{
					++number;
					speed += info.currentSpeed;
					//Space Mean Speed
					spaceMeanSpeed += 1 / info.currentSpeed;
					oss << info.currentSpeed << " ";
					//Speed from messages
					if (node->second.size() > 1)
					{
						double spd, spdF;
						NodeInfo first = *(node->second.begin().operator *());
						if (CompluteSpeed(info, first, spd, spdF))
						{
							++numberSpeedFilter;
							speedTimeFilter += spdF;
						}
						speedTime += spd;
						NodeDataCollection::reverse_iterator rit = node->second.rbegin();
						++rit;
						NodeInfo last = *(rit.operator *());
						CompluteSpeed(info, last, spd, spdF);
						speedTimeSingle += spd;
					}
					if (info.distance > maxd)
						maxd = info.distance;
					if (info.distance < mind)
						mind = info.distance;
				}
				//Use also the data from no longer conformant nodes
				if (info.lastMessage && info.totalTime > 0)
				{
					++numLastMex;
					time += info.totalTime;
				}
			}
			if (numLastMex > 0)
				time /= numLastMex;
			else
				time = -1;
			if (number > 0)
			{
				speed /= number;
				//Space Mean Speed
				spaceMeanSpeed = number / spaceMeanSpeed;
				speedTime /= number;
				speedTimeSingle /= number;
				std::string tmp = oss.str();
				NS_LOG_INFO("SpaceMeanSpeed " << spaceMeanSpeed << " n="<<number<<" list=" << tmp);
			} else
			{
				mind = 0;
				speed = -1;
				spaceMeanSpeed = -1;
				speedTime = -1;
				speedTimeSingle = -1;
			}
			if (numberSpeedFilter > 0)
				speedTimeFilter /= number;
			else
				speedTimeFilter = -1;
			valueMap[NUMBER] = number;
			valueMap[SPEED] = speed;
			valueMap[TIME] = time;
			valueMap[MAX_DISTANCE] = maxd;
			valueMap[MIN_DISTANCE] = mind;
			valueMap[SPACE_MEAN_SPEED] = spaceMeanSpeed;
			valueMap[SPEED_TIME] = speedTime;
			valueMap[SPEED_TIME_FILTER] = speedTimeFilter;
			valueMap[SPEED_TIME_SINGLE] = speedTimeSingle;

		}

		bool CentralizedProtocol::Execute(DirectionValueMap &data, const DataMap &dataMap)
		{
			NS_LOG_FUNCTION("");
			std::vector<std::string> flows;
			std::stringstream str;
			int totNum = 0;
			for (DataMap::const_iterator dir = dataMap.begin(); dir != dataMap.end(); ++dir)
			{
				ValueMap valueMap;
				AggregateDataForDirection(dir->second, valueMap);
				totNum += valueMap[NUMBER];
				std::ostringstream out;
				out << dir->first << "=" << valueMap[NUMBER] << ":" << valueMap[MAX_DISTANCE] << ":" << valueMap[SPEED] << ":"
//						<< valueMap[TIME] << ":" << valueMap[MIN_DISTANCE] << ":" << valueMap[SPACE_MEAN_SPEED] << ":"
						<< valueMap[TIME] << ":" << valueMap[MIN_DISTANCE] << ":" << valueMap[SPEED_TIME_SINGLE] << ":"
						<< valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER];
				flows.push_back(out.str());
				if (valueMap[SPEED] == -1)
					valueMap[SPEED] = 0;
				if (valueMap[TIME] == -1)
					valueMap[TIME] = 0;
				data[dir->first] = valueMap;
				str << "dir=" << dir->first << " num=" << valueMap[NUMBER] << " spd=" << valueMap[SPEED] << " sms="
//						<< valueMap[SPACE_MEAN_SPEED] << " st=" << valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER]
						<< valueMap[SPEED_TIME_SINGLE] << " st=" << valueMap[SPEED_TIME] << ":" << valueMap[SPEED_TIME_FILTER]
						<< " ";
			}
			std::ostringstream out;
			out << "totc=" << totNum;
			flows.insert(flows.begin(), out.str());
			NS_LOG_INFO("CentralizedProtocol " << str.str());
			m_traceFlows(flows);
			return ReturnData;
		}

	} /* namespace application */
} /* namespace protocol */
