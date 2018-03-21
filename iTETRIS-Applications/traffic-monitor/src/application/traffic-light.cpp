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

#include "traffic-light.h"
#include "log/log.h"
#include "log/ToString.h"
#include "split.h"

namespace protocol
{
namespace application
{

///TrafficLightLane implementation
TrafficLightLane::TrafficLightLane()
{
  m_currentStatus = UNKNOWN;
  m_direction = NO_DIRECTION;
}
TrafficLightLane::~TrafficLightLane()
{
}
std::string TrafficLightLane::getControlledLane() const
{
  return m_controlledLane;
}
std::string TrafficLightLane::getFollowingLane() const
{
  return m_followingLane;
}
TLStatus TrafficLightLane::getStatus() const
{
  return m_currentStatus;
}
void TrafficLightLane::setStatus(TLStatus newStatus)
{
  m_currentStatus = newStatus;
}
void TrafficLightLane::setControlledLane(std::string& contrLane)
{
  m_controlledLane = contrLane;
}
void TrafficLightLane::setFollowingLane(std::string& followLane)
{
  m_followingLane = followLane;
}
double TrafficLightLane::getDirection() const
{
  return m_direction;
}
void TrafficLightLane::setDirection(double direction)
{
  m_direction = direction;
}
void TrafficLightLane::setDirection(const std::vector<TLLane>& lanes)
{
  for (std::vector<TLLane>::const_iterator it = lanes.begin(); it != lanes.end(); ++it)
  {
    if (m_controlledLane == it->controlledLane && m_followingLane == it->followingLane)
    {
      m_direction = it->dir;
      m_friendlyName = it->friendlyName;
      return;
    }
  }
  m_direction = NO_DIRECTION;
  m_friendlyName = "No direction found";
}
const std::string& TrafficLightLane::getFriendlyName() const
{
  return m_friendlyName;
}

void TrafficLightLane::setFriendlyName(const std::string& friendlyName)
{
  m_friendlyName = friendlyName;
}

///TrafficLight implementation
TrafficLight::TrafficLight(std::string trafficLightID)
{
  m_id = trafficLightID;
}
TrafficLight::~TrafficLight()
{
  for (std::vector<TrafficLightLane*>::iterator it = m_lanes.begin(); it != m_lanes.end(); ++it)
    delete *it;
  m_lanes.clear();
  m_lanesDirection.clear();
}
std::string TrafficLight::getId() const
{
  return m_id;
}
const std::vector<TrafficLightLane*>& TrafficLight::getLanes() const
{
  return m_lanes;
}
const std::vector<TrafficLightLane*> TrafficLight::getLanesWithDirection(double direction) const
{
  std::map<double, std::vector<TrafficLightLane*> >::const_iterator it = m_lanesDirection.find(direction);
  if (it == m_lanesDirection.end())
    return std::vector<TrafficLightLane*>();
  return it->second;
}
const std::string& TrafficLight::getState() const
{
  return m_state;
}
bool TrafficLight::setState(const std::string& state)
{
  if (state.size() != m_lanes.size())
    return false;
  m_state = state;
  for (int i = 0; i < m_lanes.size(); ++i)
  {
    m_lanes[i]->setStatus(GetStatusFromChar(m_state[i]));
  }
  return true;
}
void TrafficLight::addTrafficLightLane(TrafficLightLane* newLane)
{
  m_lanes.push_back(newLane);
  std::map<double, std::vector<TrafficLightLane*> >::iterator it = m_lanesDirection.find(newLane->getDirection());
  if (it == m_lanesDirection.end())
  {
    std::vector<TrafficLightLane*> v;
    it = m_lanesDirection.insert(std::make_pair(newLane->getDirection(), v)).first;
  }
  it->second.push_back(newLane);
}
TLStatus TrafficLight::GetStatusFromChar(char statusChar)
{
  switch (statusChar)
  {
  case 'r':
  case 'R':
    return RED;
  case 'y':
  case 'Y':
    return YELLOW;
  case 'g':
  case 'G':
    return GREEN;
  case 'o':
  case 'O':
    return OFF;
  default:
    return UNKNOWN;
  }
}
TrafficLight* TrafficLight::CreateTrafficLight(const int index, const int numLanes,
    const std::vector<std::string> & data, const std::string& id, const RsuData & rsuData)
{
  TrafficLight* tl = new TrafficLight(id);
  std::ostringstream oss;
  oss << "[CreateTrafficLight] New traffic light with id " << id;
  Log::WriteLog(oss);
  std::vector<std::string> tokens;
  for (int i = index; i < index + numLanes; ++i)
  {
    split(tokens, data[i], " ");
    TrafficLightLane* lane = new TrafficLightLane();
    lane->setControlledLane(tokens[1]);
    lane->setFollowingLane(tokens[2]);
    lane->setDirection(rsuData.lanes);
    tl->addTrafficLightLane(lane);
    std::ostringstream oss;
    if (lane->getDirection() == NO_DIRECTION)
      oss << "[CreateTrafficLight] Added new lane to the traffic light. No match found. Controlled lane: " << lane->getControlledLane() << " Following: " << lane->getFollowingLane();
    else
      oss << "[CreateTrafficLight] Added new lane to the traffic light. Direction: " << toString(lane->getDirection())
          << ". Friendly name: " << lane->getFriendlyName();
    Log::WriteLog(oss);
  }
  return tl;
}

} /* namespace application */
} /* namespace protocol */
