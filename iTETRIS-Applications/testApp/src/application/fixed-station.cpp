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

#include "fixed-station.h"
#include "subscription-helper.h"
#include "log/log.h"
#include "split.h"

namespace testapp
{
	namespace application
	{

		FixedStation::FixedStation(int id) :
				Node(id)
		{
			m_type = NT_RSU;
			m_mobilitySubscription = m_positionUpdated = m_trafficLightSubscription = false;
			m_rsuData = ProgramConfiguration::GetRsuData(id);
			m_trafficLight = NULL;
			init();
		}

		FixedStation::~FixedStation()
		{
			delete m_trafficLight;
			m_trafficLight = NULL;
		}

		void FixedStation::addSubscriptions()
		{
			//Subscribe to receive messages
			Node::addSubscriptions();
			if (!m_mobilitySubscription)
			{
				m_mobilitySubscription = true;
				m_toSubscribe.push(SubscriptionHelper::GetMobilityInformation());
			}

			if (ProgramConfiguration::GetTestCase() != TEST_CASE_NONE) {
			    // in original demo-app this was included, but not needed for most simple test cases
                if (!m_trafficLightSubscription)
                {
                    m_trafficLightSubscription = true;
                    m_toSubscribe.push(SubscriptionHelper::GetTrafficLightInformation());
                }
			}
		}

		void FixedStation::updateMobilityInformation(MobilityInfo * info)
		{
			if (!m_positionUpdated)
			{
				std::ostringstream oss;
				oss << "Rsu " << m_id << " position update: " << info->position;
				Log::WriteLog(oss);
				m_rsuData.position = info->position;
				m_positionUpdated = true;
			}
			delete info;
		}

		void FixedStation::mobilityInformationHasRun()
		{
		}

		Vector2D FixedStation::getPosition()
		{
			return m_rsuData.position;
		}

		Vector2D FixedStation::getVelocity()
		{
			static Vector2D velocity;
			return velocity;
		}

		double FixedStation::getDirection()
		{
			return DIR_INVALID;
		}

		int stringToInt(std::string value)
		{
			int ret;
			std::stringstream str;
			str << value;
			str >> ret;
			return ret;
		}

		void FixedStation::trafficLightInformation(const bool error, const std::vector<std::string> & data)
		{
			std::vector<std::string> tokens;
			if (error)
			{
				split(tokens, data[0], ":");
				std::ostringstream oss;
				oss << "[TrafficLightInformation] error: " << tokens[1] << ". Will unsubscribe " << tokens[0];
				Log::Write(oss, LOG_ERROR);
				int subNo = stringToInt(tokens[0]);
				setToUnsubscribe(subNo);
			} else
			{
				for (int i = 0; i < data.size(); ++i)
				{
					split(tokens, data[i], " ");
					if (tokens.size() > 2)
					{
						int numLanes = stringToInt(tokens[2]);
						m_trafficLight = TrafficLight::CreateTrafficLight(i + 1, numLanes, data, tokens[0], m_rsuData);
						i += numLanes;
					}
					m_trafficLight->setState(tokens[1]);
					break; //Will use only the first traffic light from the subscription
				}
				std::ostringstream oss;
				oss << "[TrafficLightInformation] Traffic light status update: " << m_trafficLight->getState();
				Log::WriteLog(oss);
			}
		}

	} /* namespace application */
} /* namespace protocol */
