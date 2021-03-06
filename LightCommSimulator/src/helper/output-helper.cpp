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
 * Author Enrico Zamagni
 * University of Bologna
 ***************************************************************************************/

/*
 * its-output-helper.cc
 *
 *  Created on: 30/set/2013
 *      Author: "Enrico Zamagni"
 */

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include "output-helper.h"
#include "common.h"
#include "current-time.h"
#include "payload.h"
#include "scheduler.h"
#include "headers.h"
#include "node.h"
#include "behaviour-rsu.h"
#include "data-manager.h"
#include "behaviour-node.h"
#include "node-handler.h"
#include "server.h"
#include "log/log.h"

namespace protocol {
namespace application {

OutputHelperWrapper::OutputHelperWrapper(iCSInterface* controller) :
    m_controller(controller) {
    RegisterCallbacks();
}

OutputHelperWrapper::~OutputHelperWrapper() {
    DisconnectCallbacks();
}

void OutputHelperWrapper::DisconnectCallbacks() {
    m_controller->TraceDisconnect("Send", MakeCallback(&OutputHelperWrapper::OnPacketSend, this));
    //      m_controller->TraceDisconnect ("Receive", MakeCallback (&OutputHelperWrapper::OnPacketReceive, this));
    if (m_controller->GetNodeType() == NT_RSU) {
        Behaviour* rsu = m_controller->GetBehaviour(BehaviourRsu::Type());
        rsu->TraceDisconnect("NodeReceiveData", MakeCallback(&OutputHelperWrapper::OnNodeReceiveData, this));
        rsu->TraceDisconnect("NodeNoLongerConforman",
                             MakeCallback(&OutputHelperWrapper::OnNodeNoLongerConforman, this));
        rsu->TraceDisconnect("NodeTimeOut", MakeCallback(&OutputHelperWrapper::OnNodeTimeOut, this));
        rsu->TraceDisconnect("NodeLastMessage", MakeCallback(&OutputHelperWrapper::OnNodeLastMessage, this));
        Behaviour* manager = m_controller->GetBehaviour(DataManager::Type());
        manager->TraceDisconnect("FlowCheck", MakeCallback(&OutputHelperWrapper::OnRsuFlowSample, this));
        manager->TraceDisconnect("MeanDensity", MakeCallback(&OutputHelperWrapper::OnMeanDensity, this));
        manager->TraceDisconnect("CurrentDensity", MakeCallback(&OutputHelperWrapper::OnCurrentDensity, this));
    } else {
        Behaviour* node = m_controller->GetBehaviour(BehaviourNode::Type());
        node->TraceDisconnect("NodeSendData", MakeCallback(&OutputHelperWrapper::OnNodeSendData, this));
    }
}

void OutputHelperWrapper::RegisterCallbacks() {
    m_controller->TraceConnect("Send", MakeCallback(&OutputHelperWrapper::OnPacketSend, this));
    //      m_controller->TraceConnect ("Receive", MakeCallback (&OutputHelperWrapper::OnPacketReceive, this));
    if (m_controller->GetNodeType() == NT_RSU) {
        Behaviour* rsu = m_controller->GetBehaviour(BehaviourRsu::Type());
        rsu->TraceConnect("NodeReceiveData", MakeCallback(&OutputHelperWrapper::OnNodeReceiveData, this));
        rsu->TraceConnect("NodeNoLongerConforman", MakeCallback(&OutputHelperWrapper::OnNodeNoLongerConforman, this));
        rsu->TraceConnect("NodeTimeOut", MakeCallback(&OutputHelperWrapper::OnNodeTimeOut, this));
        rsu->TraceConnect("NodeLastMessage", MakeCallback(&OutputHelperWrapper::OnNodeLastMessage, this));
        Behaviour* manager = m_controller->GetBehaviour(DataManager::Type());
        manager->TraceConnect("FlowCheck", MakeCallback(&OutputHelperWrapper::OnRsuFlowSample, this));
        manager->TraceConnect("MeanDensity", MakeCallback(&OutputHelperWrapper::OnMeanDensity, this));
        manager->TraceConnect("CurrentDensity", MakeCallback(&OutputHelperWrapper::OnCurrentDensity, this));
        OutputHelper::Instance()->AddRsu(m_controller);
    } else {
        Behaviour* node = m_controller->GetBehaviour(BehaviourNode::Type());
        node->TraceConnect("NodeSendData", MakeCallback(&OutputHelperWrapper::OnNodeSendData, this));
    }
}

iCSInterface* OutputHelperWrapper::GetController() {
    return m_controller;
}

void OutputHelperWrapper::OnPacketSend(server::Payload* payload) {
    OutputHelper::Instance()->OnPacketSend(m_controller, payload);
}

void OutputHelperWrapper::OnPacketReceive(server::Payload* payload) {
    OutputHelper::Instance()->OnPacketReceive(m_controller, payload);
}

void OutputHelperWrapper::OnNodeReceiveData(NodeInfo* info) {
    OutputHelper::Instance()->OnNodeReceiveData(m_controller, *info);
}
void OutputHelperWrapper::OnNodeNoLongerConforman(NodeInfo* info) {
    OutputHelper::Instance()->OnNodeNoLongerConforman(m_controller, *info);
}
void OutputHelperWrapper::OnNodeSendData(NodeInfo& info) {
    OutputHelper::Instance()->OnNodeSendData(m_controller, info);
}
void OutputHelperWrapper::OnNodeLastMessage(NodeInfo* info) {
    OutputHelper::Instance()->OnNodeLastMessage(m_controller, *info);
}
void OutputHelperWrapper::OnNodeTimeOut(NodeInfo* info) {
    OutputHelper::Instance()->OnNodeTimeOut(m_controller, *info);
}
void OutputHelperWrapper::OnRsuFlowSample(std::vector<std::string>& flows) {
    OutputHelper::Instance()->OnRsuFlowSample(m_controller, flows);
}
void OutputHelperWrapper::OnCurrentDensity(double density, int number) {
    OutputHelper::Instance()->OnCurrentDensity(m_controller, density, number);
}
void OutputHelperWrapper::OnMeanDensity(std::string outString) {
    OutputHelper::Instance()->OnMeanDensity(m_controller, outString);
}

int OutputHelper::SinkDistanceThresholdMin = 10;
int OutputHelper::SinkDistanceThresholdMax = 250;
double OutputHelper::SinkOrientationTolerance = 15;
int OutputHelper::SampleInterval = 1000;
bool OutputHelper::SamplePackets = false;
OutputHelper* OutputHelper::m_instance = NULL;

OutputHelper::OutputHelper(std::string outputFile) {
    m_lastMsTime = -1;
    // open output file to write
    out.open(outputFile.c_str());

    m_maxRsuDensity = m_rsuDensityCount = m_rsuDensityAccum = 0;
    m_psent = m_precv = 0;
    mp_sink_threshold_min = SinkDistanceThresholdMin;
    mp_sink_threshold_max = SinkDistanceThresholdMax;
    mp_sink_tolerance = SinkOrientationTolerance;
    mp_t_sample = SampleInterval;
    m_samplePackets = SamplePackets;

    m_rsuController = NULL;
    Scheduler::Schedule(mp_t_sample, &OutputHelper::GMSample, this);
}

OutputHelper::~OutputHelper() {
    OnSimulationEnd();
    for (std::map<int, OutputHelperWrapper*>::const_iterator it = m_wrappers.begin(); it != m_wrappers.end(); ++it) {
        delete it->second;
    }
    m_wrappers.clear();
    for (std::map<int, NodeMeta*>::const_iterator it = m_nodeMeta.begin(); it != m_nodeMeta.end(); ++it) {
        delete it->second;
    }
    m_nodeMeta.clear();
    m_instance = NULL;
}

OutputHelper* OutputHelper::Instance() {
    return m_instance;
}

void OutputHelper::RegisterNode(iCSInterface* controller) {
    m_wrappers.insert(std::make_pair(controller->GetId(), new OutputHelperWrapper(controller)));
}

void OutputHelper::RemoveNode(iCSInterface* controller) {
    std::map<int, OutputHelperWrapper*>::iterator it = m_wrappers.find(controller->GetId());
    if (it != m_wrappers.end()) {
        delete it->second;
        m_wrappers.erase(it);
    }
    //Do not delete because I will use it in OnSimulationEnd
    //std::map<int, NodeMeta *>::iterator it2 = m_nodeMeta.find(controller->GetId());
    //if (it2 != m_nodeMeta.end())
    //{
    //  delete it2->second;
    //  m_nodeMeta.erase(it2);
    //}
}

int OutputHelper::Start(std::string outputFile) {
    if (m_instance == NULL) {
        m_instance = new OutputHelper(outputFile);
    }
    return EXIT_SUCCESS;
}

void OutputHelper::AddRsu(iCSInterface* rsu) {
    m_rsuController = rsu;
    BehaviourRsu* bRsu = (BehaviourRsu*) rsu->GetBehaviour(BehaviourRsu::Type());
    m_rsuDirections = bRsu->GetDirections();
    for (std::vector<VehicleDirection>::const_iterator it = m_rsuDirections.begin(); it != m_rsuDirections.end();
            ++it) {
        Sector s(*it);
        m_sectors.push_back(s);
        NS_LOG_DEBUG("[AddRsu] Add sector " << s.direction);
    }
    OnSimulationStart();
}

void OutputHelper::Log(iCSInterface* controller, std::string msg) {
    std::ostringstream strs;
    int msTime = CurrentTime::Now();

    if (msTime > m_lastMsTime) {
        strs << "  @" << msTime << std::endl;
        m_lastMsTime = msTime;
    }

    NodeType nodeType = controller->GetNodeType();
    if (nodeType == NT_RSU) {
        strs << "#" << controller->GetId();
    } else {
        strs << "*";
        if (IsNodeType(nodeType, NT_VEHICLE_MEDIUM)) {
            strs << "m";
        } else {
            strs << "f";
        }
        strs << controller->GetId();
    }
    strs << " " << msg;

    out << strs.str() << std::endl;
}

/** CALLBACKS
 */
void OutputHelper::OnPacketSend(iCSInterface* controller, server::Payload* payload) {
    if (m_samplePackets) {
        // check if sender is inside a sector
        if (IsVehicle(controller->GetNodeType())) {
            // compute node distance
            Vector2D pos = controller->GetPosition();
            const Vector2D rsuPosition = m_rsuController->GetPosition();
            double distance = GetDistance(rsuPosition, pos);
            if (distance <= mp_sink_threshold_max) {
                // retrieve node direction by its position (reverted)
                double dx = rsuPosition.x - pos.x;
                double dy = rsuPosition.y - pos.y;
                double ang = atan2(dy, dx) * 180 / M_PI;
                for (std::vector<Sector>::iterator it = m_sectors.begin(); it != m_sectors.end(); ++it) {
                    if (iCSInterface::CheckDirectionAndMovement(ang, it->direction, mp_sink_tolerance, pos, rsuPosition)) {
                        it->packetCount++;
                        break;
                    }
                }
            }
        }
    }
    m_psent++;
}

void OutputHelper::OnPacketReceive(iCSInterface* controller, server::Payload* payload) {
    if (m_samplePackets) {
        std::ostringstream strs;
        strs << "p_recv " /*<< InspectHeader (payload)*/;
        Log(controller, strs.str());
    }
    m_precv++;
}

void OutputHelper::OnNodeReceiveData(iCSInterface* controller, NodeInfo& nodeInfo) {
    std::ostringstream strs;
    strs << "n_recv src=" << nodeInfo.nodeId << " p=" << nodeInfo.position << ":" << nodeInfo.distance << " d="
         << nodeInfo.direction << " cd=" << nodeInfo.conformantDirection.getId() << " s=" << nodeInfo.currentSpeed
         << ":" << nodeInfo.avgSpeedSmall << ":" << nodeInfo.avgSpeedHigh;
    if (nodeInfo.lastMessage) {
        strs << " last tt=" << nodeInfo.totalTime;
    }
    Log(controller, strs.str());
}
void OutputHelper::OnNodeNoLongerConforman(iCSInterface* controller, NodeInfo& nodeInfo) {
    std::ostringstream strs;
    strs << "n_nlc src=" << nodeInfo.nodeId << " p=" << nodeInfo.position << ":" << nodeInfo.distance << " d="
         << nodeInfo.direction << " cd=" << nodeInfo.conformantDirection.getId();
    Log(controller, strs.str());
}

void OutputHelper::OnNodeSendData(iCSInterface* controller, NodeInfo& nodeInfo) {
    std::ostringstream strs;
    strs << "n_send p=" << nodeInfo.position << " d=" << nodeInfo.direction << ":"
         << (nodeInfo.conformantDirection.vMov == APPROACHING ? "a" : "l") << " s=" << nodeInfo.currentSpeed << ":"
         << nodeInfo.avgSpeedSmall << ":" << nodeInfo.avgSpeedHigh << (nodeInfo.lastMessage ? " last" : "");
    Log(controller, strs.str());
}

void OutputHelper::OnNodeLastMessage(iCSInterface* controller, NodeInfo& nodeInfo) {
    std::ostringstream strs;
    strs << "n_last src=" << nodeInfo.nodeId;
    Log(controller, strs.str());
}

void OutputHelper::OnNodeTimeOut(iCSInterface* controller, NodeInfo& nodeInfo) {
    std::ostringstream strs;
    strs << "n_timeout src=" << nodeInfo.nodeId;
    Log(controller, strs.str());
}

void OutputHelper::OnRsuFlowSample(iCSInterface* controller, std::vector<std::string>& flows) {
    std::ostringstream strs;
    strs << "flow";
    for (std::vector<std::string>::const_iterator f = flows.begin(); f != flows.end(); f++) {
        strs << " " << *f;
    }
    Log(controller, strs.str());
}
void OutputHelper::OnCurrentDensity(iCSInterface* controller, double density, int number) {
    std::ostringstream strs;
    strs << "c_den den=" << density << " num=" << number;
    Log(controller, strs.str());
}
void OutputHelper::OnMeanDensity(iCSInterface* controller, std::string& outString) {
    std::ostringstream strs;
    strs << "m_den " << outString;
    Log(controller, strs.str());
}

//God Mode

void OutputHelper::GMSample() {
    using namespace server;
    Node* node;
    NodeMeta* meta;

    // check every node
    unsigned int nodeCount = 0;
    unsigned int nodeCountMobSampling = 0;
    std::vector<std::string> nList;
    NodeHandler* handler = Server::GetNodeHandler();
    const Vector2D rsuPosition = m_rsuController->GetPosition();
    for (NodeMap::const_iterator it = handler->begin(); it != handler->end(); ++it)
        //for (std::map<int, OutputHelperWrapper *>::const_iterator it = m_wrappers.begin(); it != m_wrappers.end(); ++it)
    {
        node = it->second;

        if (IsNodeType(node->getNodeType(), NT_RSU)) {
            continue;    // do not sample rsu nodes
        }

        // prepare node metadata
        meta = GetMeta(node, true);
        meta->ResetSample();

        // compute node distance
        Vector2D pos = node->getPosition();
        double distance = GetDistance(rsuPosition, pos);

        if (distance <= mp_sink_threshold_max) {
            // node is inside sink range
            nodeCount++;
            meta->flagAsEnteredRsuRange = true;
            meta->sample_enteredRsuRange = true;

            if (distance > mp_sink_threshold_min) {
                //get direction from sumo
                double ang = node->getDirection();
                // retrieve node direction by its position (reverted)
                /*double dx = rsuPosition.x - pos.x;
                 double dy = rsuPosition.y - pos.y;
                 double ang = atan2(dy, dx) * 180 / M_PI;*/
                /*
                 Vector2D dir = VectorToVector2D (mobility->GetVelocity ());
                 double ang = atan2 (dir.y, dir.x) * 180 / M_PI;
                 */
                bool found = false;
                for (std::vector<Sector>::iterator sIt = m_sectors.begin(); sIt != m_sectors.end(); ++sIt) {
                    Vector2D segmentRsuVehicle(rsuPosition.x - pos.x, rsuPosition.y - pos.y);
                    double segmentDir = atan2(segmentRsuVehicle.y, segmentRsuVehicle.x) * 180.0 / M_PI;
//							NS_LOG_DEBUG(
//									"[GMSample][" << node->getId() << "] dir=" << ang << " sd=" << segmentDir << " rp=" << rsuPosition << " np=" << pos);
                    if (iCSInterface::CheckDirectionAndMovement(ang, sIt->direction, mp_sink_tolerance, pos, rsuPosition)) {
                        found = true;
                        // node is inside sector
                        if (!meta->flagAsEnteredSector) {
                            // first time this node enters a sector
                            meta->flagAsEnteredSector = true;
                            meta->enteredRsuDirection = sIt->direction;
                        }
//            else if (meta->enteredRsuDirection != it->direction)
//              break; // node comes from another sector

                        meta->sample_enteredRsuDirection = sIt->direction;
                        sIt->nodeCount++;
                        NS_LOG_DEBUG("[GMSample][" << node->getId() << "] dir=" << sIt->direction);
                        sIt->avgSpeed += GetRelativeSpeed(node->getVelocity(), m_rsuController->GetNode()->getVelocity());
                        break;
                    }
                }
                if (!found) {
                    NS_LOG_DEBUG("[GMSample][" << node->getId() << "] dir=" << ang << " not found.");
                }
            }
        }
    }
    // print sample statistics
    std::ostringstream strs;
    strs << "real totc=" << nodeCount << " : ";
    for (std::vector<Sector>::const_iterator it = m_sectors.begin(); it != m_sectors.end(); ++it) {
        // for each sector:
        // log node count
        strs << it->direction.getId() << "=" << it->nodeCount << ";";
        // log avg speed
        if (it->nodeCount == 0) {
            strs << 0;
        } else {
            strs << (it->avgSpeed / it->nodeCount);
        }
        // log packet sent
        strs << ";" << it->packetCount << " ";
    }
    Log(m_rsuController, strs.str());
    SectorReset();

    // accumulate results
    m_rsuDensityAccum += nodeCount;
    m_rsuDensityCount++;
    if (nodeCount > m_maxRsuDensity) {
        m_maxRsuDensity = nodeCount;
    }

    // schedule next sample
    Scheduler::Schedule(mp_t_sample, &OutputHelper::GMSample, this);

}

double OutputHelper::GetRelativeSpeed(Vector2D first, Vector2D second) {
    double x = first.x - second.x;
    double y = first.y - second.y;
    return sqrt((x * x) + (y * y));
}

void OutputHelper::OnSimulationStart() {
    std::ostringstream strs;
    strs << "dirs ";
    for (std::vector<VehicleDirection>::const_iterator it = m_rsuDirections.begin(); it != m_rsuDirections.end();
            it++) {
        strs << it->getId() << ",";
    }
    strs << std::endl;
    out << strs.str();
}

void OutputHelper::OnSimulationEnd() {
    std::ostringstream strs;
    int msTime = CurrentTime::Now();
    double avgRsuDensity = m_rsuDensityAccum / (double) m_rsuDensityCount;
    unsigned int cNodesEnteredRsuRange = 0, cNodesClassShadow = 0, cNodesClassFull = 0, cNodesClassMedium = 0;
    iCSInterface* controller;
    // perform node counts
    for (std::map<int, NodeMeta*>::const_iterator it = m_nodeMeta.begin(); it != m_nodeMeta.end(); ++it) {
        if (!it->second->flagAsEnteredRsuRange) {
            continue;
        }

        if (it->second->type == NT_VEHICLE_SHADOW) {
            cNodesClassShadow++;
        } else if (it->second->type == NT_VEHICLE_FULL) {
            cNodesClassFull++;
        } else if (it->second->type == NT_VEHICLE_MEDIUM) {
            cNodesClassMedium++;
        }

        //if (it->second->flagAsEnteredRsuRange)
        ++cNodesEnteredRsuRange;
    }

    strs << "  @" << msTime << std::endl;
    strs << "end tot_nodes_rsu=" << cNodesEnteredRsuRange << " tot_nodes_rsu_shadow=" << cNodesClassShadow
         << " tot_nodes_rsu_medium=" << cNodesClassMedium << " tot_nodes_rsu_full=" << cNodesClassFull
         << " tot_packets_sent=" << m_psent << " tot_packets_recv=" << m_precv << " max_dens_rsu=" << m_maxRsuDensity
         << " avg_dens_rsu=" << avgRsuDensity << std::endl;

    out << strs.str();
    out.close();
}

/** UTILITIES
 */
std::string OutputHelper::InspectHeader(server::Payload* payload) {
    std::ostringstream strs;
    /*
     Header * tmp;
     payload->getHeader(tmp, server::PAYLOAD_FRONT);
     ItsHeader* itsHeader = dynamic_cast<ItsHeader*>(tmp);
     if (itsHeader != NULL)
     {
     strs << "(";
     itsHeader->Print(strs);
     strs << " - ";
     switch (itsHeader->GetProtocolId())
     {
     case PID_HEARTBEAT:
     {
     payload->getHeader(tmp, server::PAYLOAD_END);
     HeartbeatHeader* header = dynamic_cast<HeartbeatHeader*>(tmp);
     header->Print(strs);
     break;
     }
     case PID_GROUP_SETUP:
     {
     payload->getHeader(tmp, server::PAYLOAD_END);
     GroupSetupHeader* header = dynamic_cast<GroupSetupHeader*>(tmp);
     header->Print(strs);
     break;
     }
     case PID_GROUP_MANAGEMENT:
     {
     payload->getHeader(tmp, server::PAYLOAD_END);
     GroupManagementHeader* header = dynamic_cast<GroupManagementHeader*>(tmp);
     header->Print(strs);
     break;
     }
     }
     strs << ")";
     }*/
    return strs.str();
}

void OutputHelper::SectorReset() {
    for (std::vector<Sector>::iterator it = m_sectors.begin(); it != m_sectors.end(); ++it) {
        it->nodeCount = 0;
        it->avgSpeed = 0;
        it->packetCount = 0;
    }
}

NodeMeta* OutputHelper::GetMeta(const Node* node, bool create) {
    std::map<int, NodeMeta*>::const_iterator it = m_nodeMeta.find(node->getId());
    if (it != m_nodeMeta.end()) {
        return it->second;
    }
    if (create) {
        NodeMeta* meta = new NodeMeta();
        meta->type = node->getNodeType();
        m_nodeMeta.insert(std::make_pair(node->getId(), meta));
        return meta;
    }
    return NULL;
}

} /* namespace application */
} /* namespace protocol */
