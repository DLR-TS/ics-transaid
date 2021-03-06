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
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez,
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alejandro Correa <acorrea@umh.es>, Gokulnath Thandavarayan <gthandavarayan@umh.es>
 */


#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // -> min
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/application.h"
#include "iTETRIS-Results.h"
#include "ns3/config.h"
#include <stdio.h>
#include "ns3/ns3-server.h"
#include <math.h>
#include "ns3/time-step-tag.h"
#include "ns3/node-id-tag.h"
#include "ns3/v2x-message-type-tag.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-80211p-helper.h"

using namespace std;

namespace ns3 {


iTETRISResults::iTETRISResults(int initial_x, int initial_y, int end_x, int end_y) {
    m_interval = 1;
    m_initial_x = initial_x;
    m_initial_y = initial_y;
    m_end_x = end_x;
    m_end_y = end_y;

    Simulator::ScheduleNow(&iTETRISResults::writeResults, this);
}

iTETRISResults::~iTETRISResults() {

}

void iTETRISResults::LogPacketsTx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId) {


    std::size_t posInit = context.find("/NodeList/");
    std::size_t posEnd = context.find("/DeviceList/");
    std::string strRx = context.substr(posInit + 10, (posEnd - posInit - 10));
    int nodeRx = std::stoul(strRx);


    if (m_TransAIDNodes->GetById(nodeRx) != NULL) {

        Ptr<MobilityModel> modelrx = m_TransAIDNodes->GetById(nodeRx)->GetObject<MobilityModel>();
        Ptr<MobilityModel> modeltx = m_TransAIDNodes->GetById(sendernodeId)->GetObject<MobilityModel>();

        if ((modelrx->GetPosition().x > m_initial_x && modelrx->GetPosition().x < m_end_x && modelrx->GetPosition().y > m_initial_y && modelrx->GetPosition().y < m_end_y)
                && (modeltx->GetPosition().x > m_initial_x && modeltx->GetPosition().x < m_end_x && modeltx->GetPosition().y > m_initial_y && modeltx->GetPosition().y < m_end_y)) {


            V2XmessageTypeTag v2x_tag;
            packet->PeekPacketTag(v2x_tag);
            uint32_t v2x_type = v2x_tag.Get();
            int indexAux = 0;
            switch (v2x_type) {
                case 6  : // CAM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdataCAM.countTx[indexAux];
                    break; //optional
                case 7 : // CPM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdataCPM.countTx[indexAux];
                    break; //optional
                case 8 : // MCM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdataMCM.countTx[indexAux];
                    break; //optional

                // you can have any number of case statements.
                default : //Optional

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdata.countTx[indexAux];

            }
        }
    }

}

void iTETRISResults::LogPacketsRx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId) {


    std::size_t posInit = context.find("/NodeList/");
    std::size_t posEnd = context.find("/DeviceList/");
    std::string strRx = context.substr(posInit + 10, (posEnd - posInit - 10));
    int nodeRx = std::stoul(strRx);


    if (m_TransAIDNodes->GetById(nodeRx) != NULL) {

        Ptr<MobilityModel> modelrx = m_TransAIDNodes->GetById(nodeRx)->GetObject<MobilityModel>();
        Ptr<MobilityModel> modeltx = m_TransAIDNodes->GetById(sendernodeId)->GetObject<MobilityModel>();

        if ((modelrx->GetPosition().x > m_initial_x && modelrx->GetPosition().x < m_end_x && modelrx->GetPosition().y > m_initial_y && modelrx->GetPosition().y < m_end_y)
                && (modeltx->GetPosition().x > m_initial_x && modeltx->GetPosition().x < m_end_x && modeltx->GetPosition().y > m_initial_y && modeltx->GetPosition().y < m_end_y)) {

            V2XmessageTypeTag v2x_tag;
            packet->PeekPacketTag(v2x_tag);
            uint32_t v2x_type = v2x_tag.Get();
            int indexAux = 0;

            std::map<int, IPRTdata>::iterator itPRT;
            std::map<int, double>::iterator itIPRTrx;

            std::map<int, NARdata>::iterator itNAR;
            std::map<int, double>::iterator itNARrx;

            switch (v2x_type) {

                case 6  : // CAM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdataCAM.countRx[indexAux];

                    // IPRT


                    itPRT = m_IPRTdataMapCAM.find(nodeRx);
                    if (itPRT != m_IPRTdataMapCAM.end()) {

                        itIPRTrx = (*itPRT).second.initial_time.find(sendernodeId);
                        if (itIPRTrx != (*itPRT).second.initial_time.end()) {

                            double IPTR_value = Simulator::Now().GetMilliSeconds() - (*itIPRTrx).second;
                            (*itIPRTrx).second = Simulator::Now().GetMilliSeconds();

                            indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                            ++m_IPRT_CAM.countRx[indexAux];
                            m_IPRT_CAM.IPRT[indexAux] =  m_IPRT_CAM.IPRT[indexAux] + IPTR_value;

                        } else {
                            (*itPRT).second.initial_time.insert((*itPRT).second.initial_time.end(), std::pair<int, double>(sendernodeId, Simulator::Now().GetMilliSeconds()));
                        }
                    } else {
                        IPRTdata IPRTdataAux = {};
                        itPRT = m_IPRTdataMapCAM.insert(m_IPRTdataMapCAM.end(), std::pair<int, IPRTdata>(nodeRx, IPRTdataAux));

                        std::map<int, double>::iterator itIPRTrx;
                        itIPRTrx = (*itPRT).second.initial_time.find(sendernodeId);
                        if (itIPRTrx == (*itPRT).second.initial_time.end()) {
                            (*itPRT).second.initial_time.insert((*itPRT).second.initial_time.end(), std::pair<int, double>(sendernodeId, Simulator::Now().GetMilliSeconds()));
                        }
                    }


                    // NAR

                    itNAR = m_NARdataMap.find(nodeRx);
                    if (itNAR == m_NARdataMap.end()) {
                        NARdata NARdataAux = {};
                        itNAR = m_NARdataMap.insert(m_NARdataMap.end(), std::pair<int, NARdata>(nodeRx, NARdataAux));
                    }

                    itNARrx = (*itNAR).second.detectedVehicles.find(sendernodeId);
                    if (itNARrx != (*itNAR).second.detectedVehicles.end()) {
                        if ((*itNARrx).second > distanceTxRx) {
                            (*itNARrx).second = distanceTxRx;
                        }
                    } else {
                        (*itNAR).second.detectedVehicles.insert((*itNAR).second.detectedVehicles.end(), std::pair<int, double>(sendernodeId, distanceTxRx));
                    }



                    break; //optional

                case 7 : // CPM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdataCPM.countRx[indexAux];
                    break; //optional
                case 8 : // MCM

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));

                    ++m_PDRdataMCM.countRx[indexAux];
                    break; //optional

                // you can have any number of case statements.
                default : //Optional

                    indexAux = std::min(N_LAST_STEP, (int) floor(distanceTxRx / 10));
                    ++m_PDRdata.countRx[indexAux];

            }



            std::map<int, NIRdata>::iterator itNIR;

            itNIR = m_NIRdataMap.find(nodeRx);
            if (itNIR != m_NIRdataMap.end()) {

                std::map<int, double>::iterator itNIRrx;
                itNIRrx = (*itNIR).second.detectedVehicles.find(sendernodeId);
                if (itNIRrx != (*itNIR).second.detectedVehicles.end()) {
                    if ((*itNIRrx).second < distanceTxRx) {
                        (*itNIRrx).second = distanceTxRx;
                    }
                } else {
                    (*itNIR).second.detectedVehicles.insert((*itNIR).second.detectedVehicles.end(), std::pair<int, double>(sendernodeId, distanceTxRx));
                }
            } else {
                NIRdata NIRdataAux = {};
                m_NIRdataMap.insert(m_NIRdataMap.end(), std::pair<int, NIRdata>(nodeRx, NIRdataAux));

            }


            // Latency

            TimeStepTag timestepTag;
            packet->PeekPacketTag(timestepTag);
            uint32_t timeStep = timestepTag.Get();

            double latency = Simulator::Now().GetMilliSeconds() - timeStep;

            indexAux = std::min(199, (int) floor(latency));

            ++m_LatencyData.latency[indexAux];

            ++m_LatencyData.countTotal;

            // Messages Rx per vehicle

            std::map<int, int>::iterator itMRV;

            itMRV = m_MessagesRxMap.find(nodeRx);
            if (itMRV != m_MessagesRxMap.end()) {

                (*itMRV).second = (*itMRV).second + 1;

            } else {
                m_MessagesRxMap.insert(m_MessagesRxMap.end(), std::pair<int, int>(nodeRx, 1));
            }



        }
    }
}

void iTETRISResults::LogAwarenessRatio(const NodeContainer& m_NodeContainer) {


    m_TransAIDNodes = &m_NodeContainer;

    double distance;
    int indexAux;
    bool NodeActive = false;
    bool NodeActiveRx = false;

    NodeContainer::Iterator it;
    for (it = m_NodeContainer.Begin(); it != m_NodeContainer.End(); ++it) {
        uint32_t nodeId = (*it)->GetId();

        if ((*it)->IsMobileNode()) {

            Ptr<VehicleStaMgnt> StaMgnt = (*it)->GetObject<VehicleStaMgnt>();
            NodeActive = StaMgnt->IsNodeActive();
        } else {
            NodeActive = true;
        }

        if (NodeActive) {

            Ptr<MobilityModel> mobModel = (*it)->GetObject<MobilityModel>();

            if (mobModel->GetPosition().x > m_initial_x && mobModel->GetPosition().x < m_end_x && mobModel->GetPosition().y > m_initial_y && mobModel->GetPosition().y < m_end_y) {
                std::map<int, NARdata>::iterator itNAR;

                itNAR = m_NARdataMap.find(nodeId);

                if (itNAR == m_NARdataMap.end()) {
                    NARdata NARdataAux = {};
                    itNAR = m_NARdataMap.insert(m_NARdataMap.end(), std::pair<int, NARdata>(nodeId, NARdataAux));
                }

                NodeContainer::Iterator it1;
                for (it1 = m_NodeContainer.Begin(); it1 != m_NodeContainer.End(); ++it1) {
                    if ((*it1)->IsMobileNode()) {

                        Ptr<VehicleStaMgnt> StaMgntRx = (*it1)->GetObject<VehicleStaMgnt>();
                        NodeActiveRx = StaMgntRx->IsNodeActive();
                    } else {
                        NodeActiveRx = true;
                    }

                    if ((nodeId != (*it1)->GetId()) && NodeActiveRx) {

                        Ptr<MobilityModel> mobModel1 = (*it1)->GetObject<MobilityModel>();

                        if (mobModel1->GetPosition().x > m_initial_x && mobModel1->GetPosition().x < m_end_x && mobModel1->GetPosition().y > m_initial_y && mobModel1->GetPosition().y < m_end_y) { // TODO update the conditions of the border of the scenario

                            mobModel->GetPosition();

                            distance = sqrt((mobModel->GetPosition().x - mobModel1->GetPosition().x) *
                                            (mobModel->GetPosition().x - mobModel1->GetPosition().x) +
                                            (mobModel->GetPosition().y - mobModel1->GetPosition().y) *
                                            (mobModel->GetPosition().y - mobModel1->GetPosition().y));


                            std::map<int, double>::iterator itNARtot;
                            itNARtot = (*itNAR).second.totalVehicles.find((*it1)->GetId());
                            if (itNARtot != (*itNAR).second.totalVehicles.end()) {
                                if ((*itNARtot).second > distance) {
                                    (*itNARtot).second = distance;
                                }
                            } else {
                                (*itNAR).second.totalVehicles.insert((*itNAR).second.totalVehicles.end(),
                                                                     std::pair<int, double>((*it1)->GetId(),
                                                                             distance));
                            }

                        }

                    }
                }
            }
        }
    }

}


void iTETRISResults::PhyStateTracer(std::string context, Time start, Time duration, enum WifiPhy::State state) {
//	std::cout << "Context text  : " << context << std::endl;


    std::size_t posInit = context.find("/NodeList/");
    std::size_t posEnd = context.find("/DeviceList/");
    std::string strRx = context.substr(posInit + 10, (posEnd - posInit - 10));
    int nodeID = std::stoul(strRx);

    Ptr<MobilityModel> mobModel = m_TransAIDNodes->Get(nodeID)->GetObject<MobilityModel>();

    if (mobModel->GetPosition().x > m_initial_x && mobModel->GetPosition().x < m_end_x && mobModel->GetPosition().y > m_initial_y && mobModel->GetPosition().y < m_end_y) {
        if (state != 0) { // Compute the time that the receiver is not idle

            std::map<int, double>::iterator itCBR;

            itCBR = m_CBRdataMap.find(nodeID);

            if (itCBR != m_CBRdataMap.end()) {
                (*itCBR).second = (*itCBR).second + duration.GetSeconds() ;

            } else {
                m_CBRdataMap.insert(m_CBRdataMap.end(), std::pair<int, double>(nodeID, duration.GetSeconds()));
            }


        }

    }

}



void iTETRISResults::writeResults() {


    // Transmitted Packets PDR

    std::string data, data1, data2, data3;

    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());
    data1 = "Time," + to_string(Simulator::Now().GetMilliSeconds());
    data2 = "Time," + to_string(Simulator::Now().GetMilliSeconds());
    data3 = "Time," + to_string(Simulator::Now().GetMilliSeconds());

//        std::cout << data << std::endl;

    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(m_PDRdata.countRx[i]);
        data1 += "," + to_string(m_PDRdataCAM.countRx[i]);
        data2 += "," + to_string(m_PDRdataCPM.countRx[i]);
        data3 += "," + to_string(m_PDRdataMCM.countRx[i]);

        m_PDRdata.countRx[i] = 0;
        m_PDRdataCAM.countRx[i] = 0;
        m_PDRdataCPM.countRx[i] = 0;
        m_PDRdataMCM.countRx[i] = 0;

    }

    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(m_PDRdata.countTx[i]);
        data1 += "," + to_string(m_PDRdataCAM.countTx[i]);
        data2 += "," + to_string(m_PDRdataCPM.countTx[i]);
        data3 += "," + to_string(m_PDRdataMCM.countTx[i]);

        m_PDRdata.countTx[i] = 0;
        m_PDRdataCAM.countTx[i] = 0;
        m_PDRdataCPM.countTx[i] = 0;
        m_PDRdataMCM.countTx[i] = 0;
    }

    Ns3Server::outfileLogPacketsPDR << data << std::endl;
    Ns3Server::outfileLogPacketsPDRCAM << data1 << std::endl;
    Ns3Server::outfileLogPacketsPDRCPM << data2 << std::endl;
    Ns3Server::outfileLogPacketsPDRMCM << data3 << std::endl;

    // NAR

    int sum_NAR_detected [N_STEPS_METRIC] = {0};
    int sum_NAR_total [N_STEPS_METRIC] = {0};

    double average_NAR_total [N_STEPS_METRIC] = {0};
    int average_NAR_total_vehicles [N_STEPS_METRIC] = {0};

    std::map<int, NARdata>::iterator itNAR;


    for (itNAR = m_NARdataMap.begin(); itNAR != m_NARdataMap.end(); ++itNAR) {
        std::map <int, double>::iterator detectedIterator;

        for (detectedIterator = (*itNAR).second.detectedVehicles.begin(); detectedIterator != (*itNAR).second.detectedVehicles.end(); ++detectedIterator) {
            int indexAux = std::min(N_LAST_STEP, (int) floor((*detectedIterator).second / 10));
            for (int i = indexAux; i < N_STEPS_METRIC; i++) {
                ++sum_NAR_detected[i];
            }
            // std::cout << "Node " << (*itNAR).first << " Detected Vehicle " <<  (*detectedIterator).first << " of " << (*itNAR).second.detectedVehicles.size() <<  " at distance " << (*detectedIterator).second << std::endl;
        }

        std::map <int, double>::iterator totalIterator;

        for (totalIterator = (*itNAR).second.totalVehicles.begin(); totalIterator != (*itNAR).second.totalVehicles.end(); ++totalIterator) {
            int indexAux = std::min(N_LAST_STEP, (int) floor((*totalIterator).second / 10));
            for (int i = indexAux; i < N_STEPS_METRIC; i++) {
                ++sum_NAR_total[i];
            }
            //  std::cout << "Node " << (*itNAR).first << " Total Vehicle " <<  (*totalIterator).first << " of " << (*itNAR).second.totalVehicles.size() << " at distance " << (*totalIterator).second << std::endl;
        }

        /*            if ( (*itNAR).first==1){

                        data = "Time," + to_string(Simulator::Now().GetMilliSeconds());

                        for (int i=0; i< N_STEPS_METRIC; i++){

                            data += "," + to_string(sum_NAR_detected[i]);
                        }

                        std::cout << data << std::endl;
                        data = "Time," + to_string(Simulator::Now().GetMilliSeconds());


                        for (int i=0; i< N_STEPS_METRIC; i++){

                            data += "," + to_string(sum_NAR_total[i]);
                        }

                        std::cout << data << std::endl;
                    }*/

        for (int i = 0; i < N_STEPS_METRIC; i++) {

            if (sum_NAR_total[i] > 0) {
                average_NAR_total[i] = average_NAR_total[i]  + ((double)  sum_NAR_detected[i] / (double)  sum_NAR_total[i]);
                ++average_NAR_total_vehicles[i];
            }

            sum_NAR_detected[i] = 0;
            sum_NAR_total[i] = 0;

        }



    }


    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());


    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(average_NAR_total[i]);
    }

    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(average_NAR_total_vehicles[i]);
    }

    Ns3Server::outfileLogNAR << data << std::endl;


    // NIR

    std::map<int, NIRdata>::iterator itNIR;

    double sum_NIR_detected [N_STEPS_METRIC] = {0};

    for (itNIR = m_NIRdataMap.begin(); itNIR != m_NIRdataMap.end(); ++itNIR) {
        std::map <int, double>::iterator detectedIterator;

        for (detectedIterator = (*itNIR).second.detectedVehicles.begin(); detectedIterator != (*itNIR).second.detectedVehicles.end(); ++detectedIterator) {
            int indexAux = std::min(N_STEPS_METRIC, (int) floor((*detectedIterator).second / 10));
            for (int i = 0; i < indexAux; i++) {
                sum_NIR_detected[i] = sum_NIR_detected[i] + ((double) 1 / (double)(*itNIR).second.detectedVehicles.size()) ;
            }
        }
    }

    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());


    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(sum_NIR_detected[i]);

    }

    data += "," + to_string(m_NIRdataMap.size());

    Ns3Server::outfileLogNIR << data << std::endl;


    // CBR

    int total_cbr[100] = {0};
    std::map<int, double>::iterator itCBR;

    for (itCBR = m_CBRdataMap.begin(); itCBR != m_CBRdataMap.end(); ++itCBR) {
        int indexAux = std::min(N_LAST_STEP, (int) floor((*itCBR).second * 100));
        ++total_cbr[indexAux];
    }

    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());


    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(total_cbr[i]);

    }

    data += "," + to_string(m_CBRdataMap.size());

    Ns3Server::outfileLogPacketsCBR << data << std::endl;

    // latency

    data = "Time," + to_string(Simulator::Now().GetSeconds());

    for (int i = 0; i < 200; i++) {

        data += "," + to_string(m_LatencyData.latency[i]);

    }

    data += "," + to_string(m_LatencyData.countTotal);

    Ns3Server::outfileLogLatency << data << std::endl;


    // IPRT

    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());


    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(m_IPRT_CAM.IPRT[i]);

    }

    for (int i = 0; i < N_STEPS_METRIC; i++) {

        data += "," + to_string(m_IPRT_CAM.countRx[i]);

    }

    Ns3Server::outfileLogPacketsIPRT << data << std::endl;


    // Messages rx per vehicle

    int total_messages = 0;
    std::map<int, int>::iterator itMPV;

    for (itMPV = m_MessagesRxMap.begin(); itMPV != m_MessagesRxMap.end(); ++itMPV) {
        total_messages += (*itMPV).second;
    }

    data = "Time," + to_string(Simulator::Now().GetMilliSeconds());
    data += "," + to_string(total_messages);
    data += "," + to_string(m_MessagesRxMap.size());

    Ns3Server::outfileLogMessages << data << std::endl;


    Simulator::Schedule(Seconds(m_interval), &iTETRISResults::writeResults, this);
    ResetCounters();
}

void iTETRISResults::ResetCounters() {

    m_PDRdata = {};
    m_PDRdataCAM = {};
    m_PDRdataCPM = {};
    m_PDRdataMCM = {};

    m_LatencyData = {};

    m_NARdataMap.clear();
    m_NIRdataMap.clear();
    m_CBRdataMap.clear();
    m_IPRT_CAM = {};
    m_IPRTdataMapCAM.clear();
    m_MessagesRxMap.clear();

}



} // Namespace ns3
