/****************************************************************************/
/// @file    TrafficLight.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "TrafficLight.h"
#include "../../../sync-manager.h"

namespace ics_facilities
{
///TrafficLightLane implementation
TrafficLightLane::TrafficLightLane()
{
  m_active = false;
  m_currentStatus = UNKNOWN;
}
TrafficLightLane::~TrafficLightLane()
{
}
roadElementID_t TrafficLightLane::getControlledLaneID() const
{
  return m_controlledLaneID;
}
roadElementID_t TrafficLightLane::getFollowingLaneID() const
{
  return m_followingLaneID;
}
tlStatus TrafficLightLane::getStatus() const
{
  return m_currentStatus;
}
bool TrafficLightLane::isActive() const
{
  return m_active;
}
void TrafficLightLane::setStatus(tlStatus newStatus)
{
  m_currentStatus = newStatus;
}
void TrafficLightLane::setControlledLaneID(roadElementID_t& contrLaneID)
{
  m_controlledLaneID = contrLaneID;
}
void TrafficLightLane::setFollowingLaneID(roadElementID_t& followLaneID)
{
  m_followingLaneID = followLaneID;
}
void TrafficLightLane::setActive(bool active)
{
  m_active = active;
}

///TrafficLight implementation
TrafficLight::TrafficLight(trafficLightID_t trafficLightID)
{
  m_id = trafficLightID;
  m_active = false;
}
TrafficLight::~TrafficLight()
{
}
trafficLightID_t TrafficLight::getId() const
{
  return m_id;
}
const Point2D& TrafficLight::getPosition() const
{
  return position;
}
bool TrafficLight::isActive() const
{
  return m_active;
}
void TrafficLight::setPosition(Point2D pos)
{
  position = pos;
}
void TrafficLight::setActive(bool active)
{
  m_active = active;
}
const std::vector<TrafficLightLane>& TrafficLight::getLanes() const
{
  return m_lanes;
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
    m_lanes[i].setStatus(GetStatusFromChar(m_state[i]));
  }
  return true;
}
void TrafficLight::setActiveAllLanes(bool active)
{
  for (std::vector<TrafficLightLane>::iterator laneIt = m_lanes.begin(); laneIt != m_lanes.end(); ++laneIt)
  {
    laneIt->setActive(active);
  }
}
void TrafficLight::addTrafficLightLane(TrafficLightLane& newLane)
{
  m_lanes.push_back(newLane);
}

tlStatus TrafficLight::GetStatusFromChar(char statusChar)
{
		switch (statusChar)
		{
		case 'r':
			return RED;
		case 'R':
			return RED_MAIUSC;
		case 'y':
			return YELLOW;
		case 'Y':
			return YELLOW_MAIUSC;
		case 'g':
			return GREEN;
		case 'G':
			return GREEN_MAIUSC;
		case 'o':
			return OFF;
		case 'O':
			return OFF_MAIUSC;
		default:
			return UNKNOWN;
		}
}
TrafficLight TrafficLight::CreateTrafficLight(std::string id)
{
  TrafficLight tl(id);
  using namespace ics;
  std::vector<std::vector<std::string> > links;
  SyncManager::m_trafficSimCommunicator->GetTrafficLightControlledLinks(id, links);
  for (std::vector<std::vector<std::string> >::iterator oIt = links.begin(); oIt != links.end(); ++oIt)
  {
    TrafficLightLane lane;
    lane.setControlledLaneID(oIt->at(0));
    lane.setFollowingLaneID(oIt->at(1));
    tl.addTrafficLightLane(lane);
  }
  std::cout << "[CreateTrafficLight] Created new traffic light with id " << tl.getId() << " and "
      << tl.getLanes().size() << " lanes." << std::endl;
  return tl;
}

}
