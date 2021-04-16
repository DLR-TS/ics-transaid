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
 * Copyright (c) 205 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-20-8, grant agreement no. 38622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * . Redistributions of source code must retain the above copyright notice,
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
 * Author Vasilios Karagounis
 ***************************************************************************************/

#include "behaviour-uc-rsu.h"
#include <app-commands-subscriptions-constants.h>
//#include "log/console.h"
#include "vehicleManager.h"

using namespace baseapp;
using namespace baseapp::application;

const static bool debug(false);

namespace ucapp {
namespace application {
//-------------------------------------------------------------------------------------------------------
//BehaviourUCRSU implementation
//-------------------------------------------------------------------------------------------------------
BehaviourUCRSU::BehaviourUCRSU(iCSInterface* controller, const bool use_ns3) :
    BehaviourRsu(controller),
    m_use_ns3(use_ns3),
    m_optimizeForSpeed(false),
    m_debugCamMessages(false),
    m_debugDenmMessages(false),
    rsuSumoName(""),
    rsuWithId("") {

    std::cout << "BehaviourUCRSU created" << GetController()->GetNode()->getId() << std::endl;

    if (m_use_ns3) {
        //find rsu name for this node
        for (std::pair<std::string, int> rsu : VehicleManager::get().rsus) {
            if (GetController()->GetNode()->getId() == rsu.second) {
                rsuSumoName = rsu.first;
                rsuWithId = std::string("RSU (") + std::to_string(GetController()->GetNode()->getId()) + std::string(")");
                break;
            }
        }

        try {
            m_optimizeForSpeed = JsonReader::get()["optimizeForSpeed"].get<std::string>() == "True";

            m_debugCamMessages = JsonReader::get()["camMessageDebug"].get<std::string>() == "True";
            m_debugDenmMessages = JsonReader::get()["denmMessageDebug"].get<std::string>() == "True";

            if (!rsuSumoName.empty()) {
                if (JsonReader::get().contains(rsuSumoName)) {
                    rsu_request.unicast = JsonReader::get()[rsuSumoName]["unicast"].get<std::string>() == "True";
                    rsu_request.geobroadcast = JsonReader::get()[rsuSumoName]["geobroadcast"].get<std::string>() == "True";

                    if (JsonReader::get()[rsuSumoName]["denmMsgEnable"].get<std::string>() == "True") {
                        denm_message.enable = true;
                        denm_message.type = (DenmType)JsonReader::get()[rsuSumoName]["denmMsgType"].get<int>();
                        denm_message.frequency = JsonReader::get()[rsuSumoName]["denmFrequency"].get<double>();
                        denm_message.startingPoint = JsonReader::get()[rsuSumoName]["denmStartingPoint"].get<double>();
                        denm_message.endPoint = JsonReader::get()[rsuSumoName]["denmEndPoint"].get<double>();

                        denm_message.highlight = JsonReader::get()[rsuSumoName]["denmHighlight"].get<std::string>() == "True";
                        denm_message.initialize = false;
                        denm_message.eventId = 0;
                    } else {
                        denm_message.enable = false;
                    }

                    if (debug) {
                        std::cout << rsuWithId << " created." << std::endl;
                    }
                } else {
                    rsu_request.unicast = false;
                    rsu_request.geobroadcast = false;

                    denm_message.enable = false;

                    std::cout << "Warning : " << rsuSumoName << " didn't found. Disable all msgs" << std::endl;
                }
            } else {
                m_debugCamMessages = false;
                m_debugDenmMessages = false;

                rsu_request.unicast = false;
                rsu_request.geobroadcast = false;

                denm_message.enable = false;

                std::cout << "Warning : No json data found for RSU : " << GetController()->GetNode()->getId() << std::endl;
            }
        } catch (json::exception& e) {
            std::cout << "ERROR : RSU " << rsuSumoName << " json error : " << e.what() << '\n'
                      << " Exception id : " << e.id << std::endl;

            exit(0);
        }

        if (m_optimizeForSpeed) {
            m_debugCamMessages = false;
            m_debugDenmMessages = false;

            denm_message.highlight = false;
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
BehaviourUCRSU::~BehaviourUCRSU() {
    if (m_use_ns3) {
        if (denm_message.enable) {
            Scheduler::Cancel(denm_message.eventId);
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::Start() {
    if (!m_enabled) {
        return;
    }

    BehaviourRsu::Start();

    if (rsu_request.unicast) {
        GetController()->startReceivingUnicast();
    }
    if (rsu_request.geobroadcast) {
        GetController()->startReceivingGeobroadcast(MSGCAT_TRANSAID);
    }

    if (debug) {
        std::cout << rsuWithId << " started at " << CurrentTime::Now() << std::endl;
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::OnAddSubscriptions() {
    if (!m_use_ns3) {
        return;
    }

    if (debug) {
        std::cout << "RSU vehicle OnAddSubscriptions " << GetController()->GetNode()->getId() << std::endl;
    }

    if (denm_message.enable && !denm_message.initialize) {
        sendDENMInfoRepeated();
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool BehaviourUCRSU::IsSubscribedTo(ProtocolId pid) const {
    return pid == PID_TRANSAID;
}

//-------------------------------------------------------------------------------------------------------
//CAM = Cooperative Awareness Message
//DENM = Decentralised Environmental Notification Message
//MCM = Manoeuvre Coordination Message
//RSU = Road-Side Unit
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::Receive(server::Payload* payload, double snr) {
    if (!m_use_ns3) {
        return;
    }

    TransaidHeader* receivedHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_END, receivedHeader);

    if (receivedHeader->getMessageType() == TRANSAID_DENM) {

    } else if (receivedHeader->getMessageType() == TRANSAID_MCM_VEHICLE) {

    } else if (receivedHeader->getMessageType() == TRANSAID_CAM) {
        handle_CAM_msg(receivedHeader);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool BehaviourUCRSU::Execute(DirectionValueMap& data) {
    return false;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages) {
    /*	std::stringstream ss;
    	for (const CAMdata &d : receivedCAMmessages)
    		ss << d.senderID << " ";

    	std::cout << "BehaviourUCRSU::processCAMmessagesReceived : " << ss.str() << std::endl;
    */
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::handle_CAM_msg(TransaidHeader* receivedHeader) {
    // this is a messsage from vehicles

    const TransaidHeader::CamInfo* ci = receivedHeader->getCamInfo();

    std::string vehId = GetController()->GetSumoID(ci->senderID);

    if (m_debugCamMessages) {
        // received CAM message
        std::cout << rsuWithId << " received CAM msg at " << CurrentTime::Now()
                  << " from " << vehId << " (" << ci->senderID << ")"
                  << ", generationTime=" << ci->generationTime
                  << ", position=" << ci->position
                  << ", speed=" << ci->speed
                  << ", laneIndex=" << ci->laneIndex
                  << ", acceleration=" << ci->acceleration
                  << ", heading=" << ci->heading
                  << std::endl;
    }
}

//-------------------------------------------------------------------------------------------------------
// send DENM (work zone for UC42) info repeated
// TODO, would be good to send both messages (ROAD_WORKS and NO_AD_ZONE) if needed
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::sendDENMInfoRepeated() {
    if (!m_use_ns3 || CurrentTime::Now() < 0) {
        return;
    }

    denm_message.initialize = true;

    sendDENMInfo();

    denm_message.eventId = Scheduler::Schedule(denm_message.frequency, &BehaviourUCRSU::sendDENMInfoRepeated, this);
}

//-------------------------------------------------------------------------------------------------------
//RSU periodically broadcasts path information
//-------------------------------------------------------------------------------------------------------
void BehaviourUCRSU::sendDENMInfo() {
    if (m_debugDenmMessages) {
        std::cout << rsuWithId << " sends DENM info type (" << denm_message.type << ") at time " << CurrentTime::Now() << std::endl;
    }

    TransaidHeader::DenmInfo* denmInfo = new TransaidHeader::DenmInfo(GetController()->GetId(), CurrentTime::Now(), denm_message.type);

    int messageSize = sizeof(TransaidHeader::DenmInfo);
    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_DENM, denmInfo, messageSize);

    denmInfo->startingPoint = denm_message.startingPoint;
    denmInfo->endPoint = denm_message.endPoint;

    GetController()->Send(NT_VEHICLE_FULL, header, PID_TRANSAID, MSGCAT_TRANSAID);

    if (denm_message.highlight && !rsuSumoName.empty()) {
        GetController()->Highlight(sendColors[TRANSAID_DENM], HighlightSize[DENM_RSU], TRANSAID_DENM, HighlightDuration[DENM_RSU], rsuSumoName);
    }
}

} /* namespace application */
} /* namespace ucapp */
