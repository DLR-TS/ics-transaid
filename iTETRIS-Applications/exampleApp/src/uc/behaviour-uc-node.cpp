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
 * Author Vasilios Karagounis
 ***************************************************************************************/

#include "behaviour-uc-node.h"
#include <app-commands-subscriptions-constants.h>
//#include "log/console.h"
#include "vehicleManager.h"

using namespace baseapp;
using namespace baseapp::application;

const static bool debug(false);

namespace ucapp {
namespace application {
//-------------------------------------------------------------------------------------------------------
//BehaviourUCNode implementation
//-------------------------------------------------------------------------------------------------------
BehaviourUCNode::BehaviourUCNode(iCSInterface* controller, const bool use_ns3) :
    BehaviourNode(controller),
    m_msgScheduler(nullptr),
    m_use_ns3(use_ns3),
    m_info_set(false),
    m_optimizeForSpeed(false),
    m_debugCamMessages(false),
    m_vehID(controller->GetNode()->getSumoId()),
    m_node_id(controller->GetNode()->getId()) {
    //getDepartedIDList() traci (in vehicleManager.cpp) call is not working as expected with c++ code so i had to add this line
    VehicleManager::getInstance().addVehicle(m_vehID);

    if (m_use_ns3) {
        const unsigned int numElements = sizeof(vehicleList) / sizeof(vehicleList[0]);

        std::string vehIdName("");

        for (unsigned int i = 1; i < numElements; i++) {
            if (m_vehID.find(vehicleList[i]) != std::string::npos) {
                vehIdName = vehicleList[i];
                break;
            }
        }

        try {
            m_optimizeForSpeed = JsonReader::get()["optimizeForSpeed"].get<std::string>() == "True";

            m_debugCamMessages = JsonReader::get()["camMessageDebug"].get<std::string>() == "True";

            if (JsonReader::get()[vehIdName]["camEnable"].get<std::string>() == "True") {
                cam_message.enable = true;

                cam_message.autoSend = JsonReader::get()[vehIdName]["camAuto"].get<std::string>() == "True";
                cam_message.frequency = JsonReader::get()[vehIdName]["camFrequency"].get<double>();
                cam_message.highlight = JsonReader::get()[vehIdName]["camHighlight"].get<std::string>() == "True";
                cam_message.initialize = false;
                cam_message.eventId = 0;
            } else {
                cam_message.enable = false;
            }

        } catch (json::exception& e) {
            std::cout << "Node vehicle : " << m_vehID << " Json error : " << e.what() << '\n'
                      << " Exception id : " << e.id << std::endl;

            exit(0);
        }

        if (m_optimizeForSpeed) {
            m_debugCamMessages = false;
            cam_message.highlight = false;
        }
    }

    if (debug) {
        std::cout << "Node vehicle " << m_vehID << " (" << m_node_id << ") created." << std::endl;
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
BehaviourUCNode::~BehaviourUCNode() {
    if (m_use_ns3) {
        if (cam_message.enable && !cam_message.autoSend) {
            Scheduler::Cancel(cam_message.eventId);
        }
    }

    SAFE_DELETE(m_msgScheduler);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCNode::Start() {
    if (m_use_ns3 && m_enabled) {
        BehaviourNode::Start();

        // Start receiving messages

        //INFO: in itetris_cfg_template file, do not leave <vehicleSelector value=""/> empty.
        //Put at least one type of vehicle, so no node will created if these type not exist.
        //In 'manual' scenario, if we do not fill (empty) vehicleSelector value, then all vehicles are created as nodes.
        //And this is very bad in simulation overall time completion.

        GetController()->startReceivingGeobroadcast(MSGCAT_TRANSAID);
        GetController()->startReceivingUnicast();

        //select base class scheduler to send those 3 msgs.
        if (cam_message.enable && cam_message.autoSend) {
            // Scheduler sending CAMs, CPMs and MCMs (ALL THREE MSGS) (AUTOMATICALLY)
            m_msgScheduler = new MessageScheduler(GetController());

            // Activate highlighting
            if (cam_message.highlight) {
                m_msgScheduler->switchOnHighlight();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCNode::sendCAMInfoRepeated() {
    if (!m_use_ns3 || CurrentTime::Now() < 0) {
        return;
    }

    cam_message.initialize = true;

    sendCAMInfo();

    cam_message.eventId = Scheduler::Schedule(cam_message.frequency, &BehaviourUCNode::sendCAMInfoRepeated, this);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCNode::sendCAMInfo() {
    const Node* veh = GetController()->GetNode();

    if (!veh || veh->getLaneIndex() < 0) {
        // Vehicle is not completely initialized (no mobility info received)
        return;
    }

    if (m_debugCamMessages) {
        std::cout << "Node vehicle " << m_vehID << " (" << GetController()->GetId() << ") sends a CAM message at time " << CurrentTime::Now() << std::endl;
    }

    TransaidHeader::CamInfo* camInfo = new TransaidHeader::CamInfo(GetController()->GetId(),
            CurrentTime::Now(),
            veh->getLaneIndex(),
            veh->getPosition(),
            veh->getSpeed(),
            veh->getAcceleration(),
            veh->getDirection());
    int messageSize = sizeof(TransaidHeader::CamInfo);
    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_CAM, camInfo, messageSize);

    GetController()->Send(NT_ALL, header, PID_TRANSAID, MSGCAT_TRANSAID);

    if (cam_message.highlight) {
        GetController()->Highlight(sendColors[TRANSAID_CAM], HighlightSize[CAM_VEH], TRANSAID_CAM, HighlightDuration[CAM_VEH]);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCNode::OnAddSubscriptions() {
    if (!m_use_ns3) {
        return;
    }

    if (debug) {
        std::cout << "Node vehicle OnAddSubscriptions " << m_vehID << std::endl;
    }

    BehaviourNode::OnAddSubscriptions();

    if (cam_message.enable && !cam_message.autoSend && !cam_message.initialize) {
        sendCAMInfoRepeated();
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool BehaviourUCNode::IsSubscribedTo(ProtocolId pid) const {
    return pid == PID_TRANSAID;
}

//-------------------------------------------------------------------------------------------------------
//CAM = Cooperative Awareness Message
//DENM = Decentralised Environmental Notification Message
//MCM = Manoeuvre Coordination Message
//RSU = Road-Side Unit
//-------------------------------------------------------------------------------------------------------
void BehaviourUCNode::Receive(server::Payload* payload, double snr) {
    if (!m_enabled) {
        return;
    }

    //forward message to vehicle
    auto it = VehicleManager::get().vehicles.find(m_vehID);
    if (it != VehicleManager::get().vehicles.end()) {
        it->second->receiveMessage(payload, snr);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool BehaviourUCNode::Execute(DirectionValueMap& data) {
    if (!m_info_set) {
        m_info_set = VehicleManager::getInstance().setInfo(m_vehID, m_node_id, GetController());
    }

    return false;
}

} /* namespace application */
} /* namespace ucapp */
