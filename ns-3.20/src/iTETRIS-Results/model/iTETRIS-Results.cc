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


using namespace std;

namespace ns3
{


    iTETRISResults::iTETRISResults()
    {
        m_interval = 1;
        Simulator::ScheduleNow (&iTETRISResults::writeResults, this);
    }

    iTETRISResults::~iTETRISResults()
    {

    }

    void iTETRISResults::LogPacketsTx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId){

        V2XmessageTypeTag v2x_tag;
        packet->PeekPacketTag(v2x_tag);
        uint32_t v2x_type = v2x_tag.Get();

        switch(v2x_type) {
            case 6  : // CAM
                if (distanceTxRx < 10){
                    ++m_PDRdataCAM.countTx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataCAM.countTx[49];
                } else {
                    int indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataCAM.countTx[indexAux];
                }
                break; //optional
            case 7 : // CPM
                if (distanceTxRx < 10){
                    ++m_PDRdataCPM.countTx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataCPM.countTx[49];
                } else {
                    int indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataCPM.countTx[indexAux];
                }
                break; //optional
            case 8 : // MCM
                if (distanceTxRx < 10){
                    ++m_PDRdataMCM.countTx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataMCM.countTx[49];
                } else {
                    int indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataMCM.countTx[indexAux];
                }
                break; //optional

                // you can have any number of case statements.
            default : //Optional
                if (distanceTxRx < 10){
                    ++m_PDRdata.countTx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdata.countTx[49];
                } else {
                    int indexAux = floor(distanceTxRx/10);
                    ++m_PDRdata.countTx[indexAux];
                }
        }

    }

    void iTETRISResults::LogPacketsRx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId){

        int indexAux;

        V2XmessageTypeTag v2x_tag;
        packet->PeekPacketTag(v2x_tag);
        uint32_t v2x_type = v2x_tag.Get();

        switch(v2x_type) {
            case 6  : // CAM
                if (distanceTxRx < 10){
                    ++m_PDRdataCAM.countRx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataCAM.countRx[49];
                } else {
                    indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataCAM.countRx[indexAux];
                }
                break; //optional
            case 7 : // CPM
                if (distanceTxRx < 10){
                    ++m_PDRdataCPM.countRx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataCPM.countRx[49];
                } else {
                    indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataCPM.countRx[indexAux];
                }
                break; //optional
            case 8 : // MCM
                if (distanceTxRx < 10){
                    ++m_PDRdataMCM.countRx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdataMCM.countRx[49];
                } else {
                    indexAux = floor(distanceTxRx/10);
                    ++m_PDRdataMCM.countRx[indexAux];
                }
                break; //optional

                // you can have any number of case statements.
            default : //Optional
                if (distanceTxRx < 10){
                    ++m_PDRdata.countRx[0];

                } else if (distanceTxRx > 490){
                    ++m_PDRdata.countRx[49];
                } else {
                    indexAux = floor(distanceTxRx/10);
                    ++m_PDRdata.countRx[indexAux];
                }
        }



        std::size_t posInit = context.find("/NodeList/");
        std::size_t posEnd = context.find("/DeviceList/");
        std::string strRx = context.substr (posInit+10, (posEnd-posInit-10));

        int nodeRx = std::stoi(strRx);

        std::map<int, NARdata> ::iterator itNAR;

        itNAR = m_NARdataMap.find(nodeRx);
        if (itNAR != m_NARdataMap.end()){

            if (distanceTxRx < 10){
                ++(*itNAR).second.countRx[0];

            } else if (distanceTxRx > 490){
                ++(*itNAR).second.countRx[49];
            } else {
                indexAux = floor(distanceTxRx/10);
                for (int i=0; i<=indexAux;i++) {
                    ++(*itNAR).second.countRx[i];
                }
            }

        }

        std::map<int, NIRdata> ::iterator itNIR;

        itNIR = m_NIRdataMap.find(nodeRx);
        if (itNIR == m_NIRdataMap.end()) {
            NIRdata auxNIRdata = {};
            itNIR = m_NIRdataMap.insert(m_NIRdataMap.end(),std::pair<int,NIRdata>(nodeRx,auxNIRdata));
        }

        if (distanceTxRx < 10) {

        } else if (distanceTxRx >= 500) {
            ++(*itNIR).second.countRx[49];
        } else {
            indexAux = floor(distanceTxRx / 10) - 1;
            for (int i = 0; i <= indexAux; i++) {
                ++(*itNIR).second.countRx[i];
            }
        }
        ++(*itNIR).second.countTotal;

        // Latency

        TimeStepTag timestepTag;
        packet->PeekPacketTag(timestepTag);
        uint32_t timeStep = timestepTag.Get();

       m_LatencyData.latency += Simulator::Now().GetMilliSeconds() - timeStep;
       ++m_LatencyData.countTotal;

    }

    void iTETRISResults::LogAwarenessRatio(NodeContainer m_NodeContainer){

        double distance;
        int indexAux;

        for (NodeContainer::Iterator it = m_NodeContainer.Begin(); it != m_NodeContainer.End(); ++it)
        {
            uint32_t nodeId = (*it)->GetId();
            NARdata m_auxNAR = {};

            Ptr<MobilityModel> mobModel = (*it)->GetObject<MobilityModel> ();

            for (NodeContainer::Iterator it1 = m_NodeContainer.Begin(); it1 != m_NodeContainer.End(); ++it1)
            {
                if (nodeId != (*it1)->GetId()){

                    Ptr<MobilityModel> mobModel1 = (*it1)->GetObject<MobilityModel> ();
                    mobModel->GetPosition();

                    distance =  sqrt( (mobModel->GetPosition().x - mobModel1->GetPosition().x) * (mobModel->GetPosition().x - mobModel1->GetPosition().x)  +
                                      (mobModel->GetPosition().y - mobModel1->GetPosition().y)*(mobModel->GetPosition().y - mobModel1->GetPosition().y) );


                    if (distance < 10){
                        ++m_auxNAR.countTotal[0];
                    } else if (distance > 490){
                        ++m_auxNAR.countTotal[49];
                    } else {
                        indexAux = floor(distance/10);
                        for (int i=0; i<=indexAux;i++) {
                            ++m_auxNAR.countTotal[i];
                        }
                    }

                }
            }
            m_NARdataMap.operator[](nodeId) = m_auxNAR;
        }

    }


    void iTETRISResults::writeResults (){


        // Transmitted Packets PDR


        std::string data, data1, data2, data3;

        data = "Time," + to_string(Simulator::Now().GetMilliSeconds());
        data1 = "Time," + to_string(Simulator::Now().GetMilliSeconds());
        data2 = "Time," + to_string(Simulator::Now().GetMilliSeconds());
        data3 = "Time," + to_string(Simulator::Now().GetMilliSeconds());

        for (int i=0; i< 50; i++){

            data += "," + to_string(m_PDRdata.countRx[i]);
            data1 += "," + to_string(m_PDRdataCAM.countRx[i]);
            data2 += "," + to_string(m_PDRdataCPM.countRx[i]);
            data3 += "," + to_string(m_PDRdataMCM.countRx[i]);
        }

        for (int i=0; i< 50; i++){

            data += "," + to_string(m_PDRdata.countTx[i]);
            data1 += "," + to_string(m_PDRdataCAM.countTx[i]);
            data2 += "," + to_string(m_PDRdataCPM.countTx[i]);
            data3 += "," + to_string(m_PDRdataMCM.countTx[i]);
        }
        Ns3Server::outfileLogPacketsPDR << data << std::endl;
        Ns3Server::outfileLogPacketsPDRCAM << data1 << std::endl;
        Ns3Server::outfileLogPacketsPDRCPM << data2 << std::endl;
        Ns3Server::outfileLogPacketsPDRMCM << data3 << std::endl;

        // NAR


        double average_NAR [50] = {0};

        for (std::map<int, NARdata>::iterator it = m_NARdataMap.begin(); it != m_NARdataMap.end(); ++it){
            for (int i = 0; i < 50; ++i) {
                if ((*it).second.countTotal[i] != 0){
                    average_NAR[i] += (double) ((double) (*it).second.countRx[i] / (*it).second.countTotal[i]) / m_NARdataMap.size();
                }
            }
        }


        data = "Time," + to_string(Simulator::Now().GetSeconds());

        for(int i=0; i< 50; i++){
            data += "," + to_string(average_NAR[i]);
        }

        Ns3Server::outfileLogNAR << data << std::endl;

        // NIR


        double average_NIR [50] = {0};


        for (std::map<int, NIRdata>::iterator it = m_NIRdataMap.begin(); it != m_NIRdataMap.end(); ++it)
        {

            if ((*it).second.countTotal !=0 ) {
                for (int i = 0; i < 50; ++i) {
                    average_NIR[i] += ((*it).second.countRx[i] / (*it).second.countTotal) / m_NIRdataMap.size();
                }
            }
        }


        data = "Time," + to_string(Simulator::Now().GetSeconds());

        for (int i=0; i< 50; i++){
            data += "," + to_string(average_NIR[i]);
        }

        Ns3Server::outfileLogNIR << data << std::endl;

        // latency

        data = "Time," + to_string(Simulator::Now().GetSeconds());
        if (m_LatencyData.countTotal!=0){
            m_LatencyData.latency /= m_LatencyData.countTotal;
            data += "," + to_string(m_LatencyData.latency);
            Ns3Server::outfileLogLatency << data << std::endl;
        }


        Simulator::Schedule(Seconds(m_interval),&iTETRISResults::writeResults,this);
        ResetCounters();
    }

    void iTETRISResults::ResetCounters ()
    {
        m_PDRdata = {};
        m_NARdataMap.clear();
        m_NIRdataMap.clear();
    }



} // Namespace ns3
