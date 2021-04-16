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

#include "application/model/behaviour-rsu.h"
#include "behaviour-test-rsu.h"
#include "ics-interface.h"
#include "fixed-station.h"
#include "program-configuration.h"
#include "node.h"
#include <app-commands-subscriptions-constants.h>
#include "current-time.h"
#include "log/console.h"
#include "libsumo/TraCIDefs.h"
#include "message-scheduler-helper.h"

using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {

///BehaviourTestRSU implementation
BehaviourTestRSU::BehaviourTestRSU(iCSInterface* controller) :
    BehaviourRsu(controller), m_firstBroadcast(true), m_broadcastInterval(1000), m_broadcastCheckInterval(100),
    m_broadcastActive(true), m_eventBroadcast(0), m_eventAbortBroadcast(0),
    m_lastVType("NONE"), m_eventBroadcastCAM(0), m_eventBroadcastDENM(0), m_eventBroadcastCPM(0), m_eventBroadcastMCM(0), m_eventBroadcastIVI(0), m_eventBroadcastMAP(0) {
    m_mobilitySubscription = m_trafficLightSubscription = m_setCAMareaSubscription = m_subReceiveMessage = false;
    m_lastCAMsent = nullptr;
    m_lastCPMsent = nullptr;
    m_lastDENMsent = nullptr;
    m_lastIVIsent = nullptr;
    m_lastMAPsent = nullptr;
    m_lastMCMsent = nullptr;
    m_MessageScheduler = nullptr;
}

BehaviourTestRSU::~BehaviourTestRSU() {
    Scheduler::Cancel(m_eventBroadcast);
    Scheduler::Cancel(m_eventAbortBroadcast);
    Scheduler::Cancel(m_eventBroadcastCAM);
    Scheduler::Cancel(m_eventBroadcastDENM);
    Scheduler::Cancel(m_eventBroadcastCPM);
    Scheduler::Cancel(m_eventBroadcastMCM);
    Scheduler::Cancel(m_eventBroadcastMAP);
    Scheduler::Cancel(m_eventBroadcastIVI);
    if (ProgramConfiguration::GetTestCase() == "testMessageScheduler") {
        delete m_MessageScheduler;
    }
}

void BehaviourTestRSU::Start() {
    if (!m_enabled) {
        return;
    }
    BehaviourRsu::Start();

    //Example use of a traci command subscription
    if (ProgramConfiguration::GetTestCase() == "simpleExecute") {
        // rsu does nothing
    } else if (ProgramConfiguration::GetTestCase() == "setVType") {
        // rsu does nothing
    } else if (ProgramConfiguration::GetTestCase() == "commSimple" || ProgramConfiguration::GetTestCase() == "commSimple2") {

        // Subscribe to receive geobroadcast messages
        GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
        m_subReceiveMessage = true;
        GetController()->requestMobilityInfo();

        if (ProgramConfiguration::GetTestCase() == "commSimple2") {
            // Start broadcasting at 5000
            int broadcastStart = 5000;
            int broadcastEnd = 10000;
            m_eventBroadcast = Scheduler::Schedule(broadcastStart, &BehaviourTestRSU::RSUBroadcastCommSimple2, this);
            m_eventAbortBroadcast = Scheduler::Schedule(broadcastEnd, &BehaviourTestRSU::abortBroadcast, this);
            NS_LOG_INFO(Log() << "RSU scheduled broadcast start at " << broadcastStart << " and broadcastEnd at " << broadcastEnd);
        }
    } else if (ProgramConfiguration::GetTestCase() == "CAMsimple") {
        if (!m_setCAMareaSubscription) {
            GetController()->StartSendingCAMs();
            GetController()->startReceivingCAMs();
            m_setCAMareaSubscription = true;
        }
    } else if (ProgramConfiguration::GetTestCase() == "acosta" || ProgramConfiguration::GetTestCase() == "") {
        // in original demo-app this was included, but not needed for most simple test cases
        if (!m_mobilitySubscription) {
            GetController()->requestMobilityInfo();
            m_mobilitySubscription = true;
        }

        if (!m_trafficLightSubscription) {
            GetController()->requestTrafficLightInfo();
            m_trafficLightSubscription = true;
        }

        if (!m_subReceiveMessage) {
            //Subscribe to geobroadcast and unicast
            GetController()->startReceivingUnicast();
            GetController()->startReceivingGeobroadcast(PROTOCOL_MESSAGE);
            m_subReceiveMessage = true;
        }
    } else if (ProgramConfiguration::GetTestCase() == "testV2XmsgSet") {

        GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
        m_subReceiveMessage = true;
        GetController()->requestMobilityInfo();

        m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &BehaviourTestRSU::RSUBroadcastTestV2XmsgSet, this);


    } else if (ProgramConfiguration::GetTestCase() == "TMCBehaviour" || ProgramConfiguration::GetTestCase() == "TMCBehaviour_multiRSU") {
        if (GetController()->GetId() == 5000) {
            GetController()->requestMobilityInfo();
        }
        GetController()->startReceivingGeobroadcast(MSGCAT_TESTAPP);
        m_subReceiveMessage = true;


    } else if (ProgramConfiguration::GetTestCase() == "testMessageScheduler") {

        GetController()->startReceivingGeobroadcast(MSGCAT_TRANSAID);
        m_subReceiveMessage = true;
        GetController()->requestMobilityInfo();

        // std::cout << "Starting test Message Scheduler at RSU"  << std::endl;


        m_MessageScheduler = new MessageScheduler(GetController());

    }

}

bool BehaviourTestRSU::IsSubscribedTo(ProtocolId pid) const {
    if (ProgramConfiguration::GetTestCase() == "testMessageScheduler") {
        return pid == PID_TRANSAID;
    } else {
        return pid == PID_UNKNOWN;
    }
}

void BehaviourTestRSU::Receive(server::Payload* payload, double snr) {
    NS_LOG_FUNCTION(Log());
    if (!m_enabled) {
        return;
    }
    CommHeader* commHeader;
    GetController()->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);

    if (ProgramConfiguration::GetTestCase() == "testV2XmsgSet" || ProgramConfiguration::GetTestCase() == "testMessageScheduler") {

        TransaidHeader* transaidHeader;
        GetController()->GetHeader(payload, server::PAYLOAD_END, transaidHeader);
        Header* receivedHeader = payload->getHeader(baseapp::server::PAYLOAD_END);
        TransaidHeader* receivedTestHeader = dynamic_cast<TransaidHeader*>(receivedHeader);

        NS_LOG_INFO(Log() << "Received a test message from node " << commHeader->getSourceId() << " with message type: " << transaidHeader->getMessageType());

        if (commHeader->getMessageType() == TRANSAID_CAM) {
            const TransaidHeader::CamInfo* camInfo = transaidHeader->getCamInfo();
            std::cout << "Received CAM at node " << GetController()->GetId() << "  sender " << camInfo->senderID << " time " << camInfo->generationTime <<  " position " << camInfo->position <<
                      " speed " << camInfo->speed  <<  " acceleration " << camInfo->acceleration  << std::endl;

            return;
        }

        if (commHeader->getMessageType() == TRANSAID_CPM) {
            const TransaidHeader::CpmInfo* cpmInfo = transaidHeader->getCpmInfo();


            std::cout << "Received CPM at node " << GetController()->GetId() << "  sender " << cpmInfo->senderID << " time " << cpmInfo->generationTime << " Number OF Received Obstacles in the CPM " << cpmInfo->numObstacles << " CPM Message Size " << cpmInfo->CPM_message_size << std::endl;
            /*
            for(int i=0; i < cpmInfo->numObstacles ; ++i )
            {
            	std::cout << "Received Object ID : " << cpmInfo->CPM_detected_objectID[i] <<std::endl;
            } */

            return;
        }

        if (commHeader->getMessageType() == TRANSAID_MCM_VEHICLE) {
            const TransaidHeader::McmVehicleInfo* mcmInfo = transaidHeader->getMcmVehicleInfo();

            std::cout << "Received MCM at node " << GetController()->GetId() << "  sender " << mcmInfo->senderID << " time " << mcmInfo->generationTime  << std::endl;

            return;
        }

    } else {


        if (commHeader->getMessageType() != MT_TEST_RESPONSE) {
            NS_LOG_WARN(Log() << "Received an unknown message " << commHeader->getMessageType());
            return;
        }
        TestHeader* testHeader;
        GetController()->GetHeader(payload, server::PAYLOAD_END, testHeader);
        Header* receivedHeader = payload->getHeader(baseapp::server::PAYLOAD_END);
        TestHeader* receivedTestHeader = dynamic_cast<TestHeader*>(receivedHeader);

        NS_LOG_INFO(Log() << "Received a test message with content: " << testHeader->getMessage());

        if (ProgramConfiguration::GetTestCase() == "commSimple2") {
            double responseTime = m_rnd.GetValue(m_responseTimeSpacing, testHeader->getMaxResponseTime() - m_responseTimeSpacing);
            Scheduler::Cancel(m_eventResponse);
            if (receivedTestHeader->getMessage() == "Vehicle response to RSU Vehicle acknowledgement") {
                // Random offset for responseTime
                //                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends stop advice response on reception of Vehicle response to RSU Vehicle acknowledgement.");
                NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends stop advice response on reception of Vehicle response to RSU Vehicle acknowledgement.");
                TestHeader::ResponseInfo response;
                response.message = "RSU Stop advice";
                response.targetID = commHeader->getSourceId();
                response.stopEdge = "CE";
                response.stopPosition = 50;
                m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestRSU::EventSendResponse, this, response);
                NS_LOG_INFO(Log() << "scheduled a test response to Vehicle response to RSU Vehicle acknowledgement in " << responseTime);
            } else if (receivedTestHeader->getMessage() == "Vehicle response to RSU broadcast") {
                // Random offset for responseTime
                //                    NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends acknowledgement response on reception of vehicle's first response.");
                NS_LOG_DEBUG(Log() << "RSU " << GetController()->GetNode()->getId() << " sends acknowledgement response on reception of vehicle's first response.");
                TestHeader::ResponseInfo response;
                response.message = "RSU Vehicle acknowledgement";
                response.targetID = commHeader->getSourceId();
                m_eventResponse = Scheduler::Schedule(responseTime, &BehaviourTestRSU::EventSendResponse, this, response);
                NS_LOG_INFO(Log() << "scheduled a test response to Vehicle regular broadcast in " << responseTime);
            }
        }
    }
}

bool BehaviourTestRSU::Execute(DirectionValueMap& data) {
    if (ProgramConfiguration::GetTestCase() == "setVType") {

        std::string vehID = "veh0";
        std::pair<int, std::shared_ptr<libsumo::TraCIResult> > response = GetLastTraCIResponse(vehID, libsumo::VAR_TYPE);
        if (response.second != nullptr) {
            // response exists
            std::string vType = std::dynamic_pointer_cast<libsumo::TraCIString>(response.second)->value;
            if (vType != m_lastVType) {
                // Report new vType info
                std::cout << "Updated vType information for Vehicle '" << vehID << "' from '" << m_lastVType
                          << "' to '" << vType << "' at time " << response.first << std::endl;
                m_lastVType = vType;
            }
        }
    } else if (ProgramConfiguration::GetTestCase() == "inductionLoop") {
        // constantly query induction loop status via RSU
        GetController()->AddTraciSubscription("WC", libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::LAST_STEP_VEHICLE_NUMBER);
    } else if (ProgramConfiguration::GetTestCase() == "commSimple") {
        // RSU constantly broadcasts for 5 secs (starting at t=5000)
        if (CurrentTime::Now() < 10000) {
            TestHeader* header = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU regular broadcast message");
            GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_TESTAPP);
        }
    } else if (ProgramConfiguration::GetTestCase() == "commSimple2") {
        // do nothing
    } else if (ProgramConfiguration::GetTestCase() == "commRSU2Vehicle") {
        if (CurrentTime::Now() % 3000 == 0) {
            SendMCMTo("veh0");
        }
    } else if (ProgramConfiguration::GetTestCase() == "testMessageScheduler") {
        if (CurrentTime::Now() == 6550) {
            TransaidHeader::DenmInfo* message = new TransaidHeader::DenmInfo() ;
            message->generationTime = CurrentTime::Now();
            message->senderID = GetController()->GetId();

            m_MessageScheduler->SendDENM(message);

        }
    }
    return false;
}


void BehaviourTestRSU::EventSendResponse(TestHeader::ResponseInfo response) {
    NS_LOG_FUNCTION(Log());

    // React to perception of vehicle.
    TestHeader* responseHeader = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, response);
    GetController()->SendTo(response.targetID, responseHeader, PID_UNKNOWN, MSGCAT_TESTAPP);

    NS_LOG_DEBUG(Log() << "Sent test response to vehicle " << response.targetID);
}


void BehaviourTestRSU::RSUBroadcastCommSimple2() {
    if (m_firstBroadcast) {
        NS_LOG_FUNCTION(Log());
        m_firstBroadcast = false;
        NS_LOG_DEBUG(Log() << "Starting vehicle broadcast");
    }

    if (!m_broadcastActive) {
        return;
    }

    TestHeader* header = new TestHeader(PID_UNKNOWN, MT_RSU_TEST, "RSU regular broadcast message");
    GetController()->Send(NT_VEHICLE_FULL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

    // Schedule next broadcast with random offset
    double nextTime = m_rnd.GetValue(m_broadcastInterval, m_broadcastInterval + 100);
    NS_LOG_DEBUG(Log() << "Scheduled next RSU broadcast at time " << nextTime);
    m_eventBroadcast = Scheduler::Schedule(nextTime, &BehaviourTestRSU::RSUBroadcastCommSimple2, this);
}

void BehaviourTestRSU::RSUBroadcastTestV2XmsgSet() {
    if (m_firstBroadcast) {
        NS_LOG_FUNCTION(Log());
        m_firstBroadcast = false;
        NS_LOG_DEBUG(Log() << "Starting vehicle broadcast");
    }

    if (!m_broadcastActive) {
        return;
    }

    //std::cout << "Check the transmission of messages at node  " << GetController()->GetId() << " at time " << CurrentTime::Now() <<  std::endl;

    // Check CAM Tx

    if (m_lastCAMsent == nullptr || (CurrentTime::Now() - m_lastCAMsent->generationTime) > 1000) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collissions
        m_eventBroadcastCAM = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendCAM, this);
    }

    // Check DENM Tx

    if (m_lastDENMsent == nullptr || (CurrentTime::Now() - m_lastDENMsent->generationTime) > 2000) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collissions
        m_eventBroadcastDENM = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendDENM, this);
    }

    // Check CPM Tx

    m_NodeMap  = GetController()->GetAllNodes() ;

    for (NodeMap::const_iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it) {
        std::cout <<  "Obtained information about node " << it->second->getId()  << " of type " <<   it->second->getNodeType() << std::endl;
    }


    if (m_lastCPMsent == nullptr || (CurrentTime::Now() - m_lastCPMsent->generationTime) > 1000) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
        m_eventBroadcastCPM = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendCPM, this);
    }

    // Check MCM Tx

    if (m_lastMCMsent == nullptr || (CurrentTime::Now() - m_lastMCMsent->generationTime) > 3000) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
        m_eventBroadcastMCM = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendMCM, this);

    }

    // Check MAP Tx

    if (m_lastMAPsent == nullptr || (CurrentTime::Now() - m_lastMAPsent->generationTime) > 1500) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
        m_eventBroadcastMAP = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendMAP, this);

    }

    // Check IVI Tx

    if (m_lastIVIsent == nullptr || (CurrentTime::Now() - m_lastIVIsent->generationTime) > 2000) {

        double nextTime = m_rnd.GetValue(0, 50); // introduce random value to avoid collisions
        m_eventBroadcastIVI = Scheduler::Schedule(nextTime, &BehaviourTestRSU::SendIVI, this);

    }


    // Schedule next broadcast for check transmission of messages
    m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &BehaviourTestRSU::RSUBroadcastTestV2XmsgSet, this);
}

void BehaviourTestRSU::SendCAM() {

    TransaidHeader::CamInfo* message = new TransaidHeader::CamInfo();
    message->generationTime = CurrentTime::Now();
    message->senderID = GetController()->GetId();
    message->position = GetController()->GetPosition(); // TODO update correctly
    message->speed = 0 ; // TODO update correctly
    message->acceleration = 0 ; //TODO update correctly

    m_lastCAMsent = message;

    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_CAM, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);
    std::cout << "Send CAM at node " << GetController()->GetId() <<  std::endl;
    GetController();
}

void BehaviourTestRSU::SendDENM() {

    TransaidHeader::DenmInfo* message = new TransaidHeader::DenmInfo();
    message->generationTime = CurrentTime::Now();
    message->senderID = GetController()->GetId();
    message->denmType = ROAD_WORKS; // TODO use the appropiate denmType for each use case

    m_lastDENMsent = message;

    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_DENM, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);
    std::cout << "Send DENM at node " << GetController()->GetId() <<  std::endl;
}

void BehaviourTestRSU::SendCPM() {

    TransaidHeader::CpmInfo* message = new TransaidHeader::CpmInfo();
    message->generationTime = CurrentTime::Now();
    message->senderID = GetController()->GetId();
    message->numObstacles = 1;

    m_lastCPMsent = message;

    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_CPM, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

    std::cout << "Send CPM at node " << GetController()->GetId() <<  std::endl;
}


void BehaviourTestRSU::SendMCM() {
    TransaidHeader::McmRsuInfo* message = new TransaidHeader::McmRsuInfo(GetController()->GetId(), CurrentTime::Now(), TOC);
    message->adviceInfo->adviceId = 1;
    std::shared_ptr<TransaidHeader::ToCAdvice> tocAdvice = std::dynamic_pointer_cast<TransaidHeader::ToCAdvice>(message->adviceInfo->advice);
    tocAdvice->tocEndPosition = 10;
    tocAdvice->tocStartPosition = 1;
    tocAdvice->tocTime = 2000;

    m_lastMCMsent = message;

    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_MCM_RSU, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

    std::cout << "Send MCM at node " << GetController()->GetId() <<  std::endl;
}

void BehaviourTestRSU::SendMCMTo(const std::string& vehID) {
    TransaidHeader::McmRsuInfo* mcmInfo = new TransaidHeader::McmRsuInfo(GetController()->GetId(), CurrentTime::Now(), TOC);
    mcmInfo->adviceInfo->adviceId = -1;
    mcmInfo->adviceInfo->targetId = GetController()->getICSID(vehID);
    std::shared_ptr<TransaidHeader::ToCAdvice> advice = std::dynamic_pointer_cast<TransaidHeader::ToCAdvice>(mcmInfo->adviceInfo->advice);

    int msgSize = sizeof(TransaidHeader::McmRsuInfo);
    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_MCM_RSU, mcmInfo, msgSize);
    GetController()->SendTo(GetController()->getICSID(vehID), header, PID_UNKNOWN, MSGCAT_TESTAPP);

    std::cout << "Send MCM from node " << GetController()->GetId() << " to node " << mcmInfo->adviceInfo->targetId <<  std::endl;
}

void BehaviourTestRSU::SendMAP() {

    TransaidHeader::MapInfo* message = new TransaidHeader::MapInfo();
    message->generationTime = CurrentTime::Now();
    message->senderID = GetController()->GetId();


    m_lastMAPsent = message;

    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_MAP, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

    std::cout << "Send MAP at node " << GetController()->GetId() <<  std::endl;
}

void BehaviourTestRSU::SendIVI() {

    TransaidHeader::IviInfo* message = new TransaidHeader::IviInfo();
    message->generationTime = CurrentTime::Now();
    message->senderID = GetController()->GetId();


    m_lastIVIsent = message;
    TransaidHeader* header = new TransaidHeader(PID_UNKNOWN, TRANSAID_IVI, message, 100);
    GetController()->Send(NT_ALL, header, PID_UNKNOWN, MSGCAT_TESTAPP);

    std::cout << "Send IVI at node " << GetController()->GetId() <<  std::endl;
}


void BehaviourTestRSU::abortBroadcast() {
    NS_LOG_FUNCTION(Log());
    m_broadcastActive = false;
    Scheduler::Cancel(m_eventBroadcast);
    NS_LOG_DEBUG(Log() << "RSU aborted broadcasting");
}


void BehaviourTestRSU::processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages) {
    NS_LOG_FUNCTION(Log());
    if (ProgramConfiguration::GetTestCase() == "CAMsimple") {
        NS_LOG_DEBUG(Log() << "Node " << nodeID <<   " received CAM messages");
        for (std::vector<CAMdata>::const_iterator it = receivedCAMmessages.begin(); it != receivedCAMmessages.end(); ++it) {
            NS_LOG_INFO(Log() << "CAM message received from node " << it->senderID << " at time " << it->generationTime);
        }
    }
}

void BehaviourTestRSU::processTraCIResult(const int result, const Command& command) {
    Behaviour::processTraCIResult(result, command);
    if (ProgramConfiguration::GetTestCase() == "inductionLoop") {
        if (command.commandId == libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE
                && command.variableId == libsumo::LAST_STEP_VEHICLE_NUMBER
                && result > 0) {
            std::cout << "Result in TraCIResponses:\n";
            const std::pair<double, std::shared_ptr<libsumo::TraCIResult> >& response = GetLastTraCIResponse(command.objId, command.variableId);
            if (response.second != nullptr) {
                std::cout << "   Number of vehicles on detector '" << command.objId << "' in step " << response.first << ": ";
                std::cout << std::dynamic_pointer_cast<libsumo::TraCIInt>(response.second)->value << std::endl;
            }
            GetController()->AddTraciSubscription(command.objId, libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::LAST_STEP_VEHICLE_ID_LIST);
            GetController()->AddTraciSubscription(command.objId, libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::LAST_STEP_MEAN_SPEED);
            GetController()->AddTraciSubscription(command.objId, libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, libsumo::LAST_STEP_OCCUPANCY);
        }
    }
}


} /* namespace application */
} /* namespace protocol */
