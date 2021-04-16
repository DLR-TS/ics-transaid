/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#include "subscription-helper.h"
#include "mobile-node.h"
#include "node-sampler.h"
#include <sstream>
#include "model/ics-interface.h"

namespace protocol {

namespace application {

MobileNode::MobileNode(int id) :
    Node(id) {
    selectNodeType();
    m_position = new MobilityInfo(id);
    init();
}

MobileNode::MobileNode(MobilityInfo* info) :
    Node(info->id) {
    selectNodeType();
    m_position = NULL;
    init();
    updateMobilityInformation(info);
}

MobileNode::MobileNode(const int nodeId, const int ns3NodeId, const std::string& sumoNodeId,
                       const std::string& sumoType, const std::string& sumoClass) :
    Node(nodeId) {
    m_ns3Id = ns3NodeId;
    m_sumoId = sumoNodeId;
    m_sumoType = sumoType;
    m_sumoClass = sumoClass;
    selectNodeType();
    m_position = new MobilityInfo(nodeId);
    init();
}

MobileNode::~MobileNode() {
    delete m_position;
}

void MobileNode::updateMobilityInformation(MobilityInfo* info) {
    if (m_controller != NULL) {
        m_controller->GetNodeSampler()->UpdatePosition(info);
    }
    delete m_position;
    m_position = info;
}

Vector2D MobileNode::getPosition() {
    return m_position->position;
}

Vector2D MobileNode::getVelocity() {
    double speed_x = (double) m_position->speed * cos((double) m_position->direction * M_PI / 180.0);
    double speed_y = (double) m_position->speed * sin((double) m_position->direction * M_PI / 180.0);
    return Vector2D(speed_x, speed_y);
}

double MobileNode::getDirection() {
    return m_position->direction;
}

void MobileNode::selectNodeType() {
//			TODO Add appropriate type according the type from sumo. Eg
//			if(m_type == "special")
//				m_type = NT_SPECIAL;
//			else if (mt_type == "police")
//				m_type = NT_POLICE;
//			else
    m_type = GetRandomNodeType();
    std::ostringstream oss;
    oss << "Selected class for node " << m_id << " is " << (int) m_type;
    Log::WriteLog(oss);
}

} /* namespace application */
} /* namespace protocol */
