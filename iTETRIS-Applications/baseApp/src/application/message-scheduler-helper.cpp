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
//
// Created by alex on 23/03/19.
//

#include "message-scheduler-helper.h"
#include "node.h"
#include <app-commands-subscriptions-constants.h>
#include "ics-interface.h"
#include <stdio.h>
#include <string.h>
#include <algorithm> // min


// Maximal number of objects to be stored in a CPM message
#define MAX_NUM_CPM_OBJECTS 50

namespace baseapp {
namespace application {
using namespace std;

std::map<MessageType, bool> MessageScheduler::m_defaultHighlightSwitch = {
    {TRANSAID_CAM, false},
    {TRANSAID_CPM, false},
    {TRANSAID_MCM_VEHICLE, false},
    {TRANSAID_DENM, false},
    {TRANSAID_IVI, false},
    {TRANSAID_MCM_RSU, false},
    {TRANSAID_MAP, false}
};
std::map<MessageType, double> MessageScheduler::m_defaultHighlightSizeRSU = {
    {TRANSAID_CAM, 5},
    {TRANSAID_CPM, 6},
    {TRANSAID_DENM, 7},
    {TRANSAID_MCM_RSU, 8},
    {TRANSAID_MCM_VEHICLE, 9},
    {TRANSAID_IVI, 10},
    {TRANSAID_MAP, 11}
};
std::map<MessageType, double> MessageScheduler::m_defaultHighlightSizeVeh = {
    {TRANSAID_CAM, 3},
    {TRANSAID_CPM, 4},
    {TRANSAID_DENM, 5},
    {TRANSAID_MCM_RSU, 6},
    {TRANSAID_MCM_VEHICLE, 7},
    {TRANSAID_IVI, 8},
    {TRANSAID_MAP, 9}
};
std::map<MessageType, double> MessageScheduler::m_defaultHighlightDuration = {
    {TRANSAID_CAM, 2},
    {TRANSAID_CPM, 2},
    {TRANSAID_DENM, 3},
    {TRANSAID_MCM_RSU, 4},
    {TRANSAID_MCM_VEHICLE, 4},
    {TRANSAID_IVI, 4},
    {TRANSAID_MAP, 4}
};
std::map<MessageType, std::string> MessageScheduler::m_defaultHighlightColor =  {
    {TRANSAID_CAM, "blue"},
    {TRANSAID_CPM, "cyan"},
    {TRANSAID_MCM_VEHICLE, "orange"},
    {TRANSAID_DENM, "yellow"},
    {TRANSAID_IVI, "brown"},
    {TRANSAID_MCM_RSU, "red"},
    {TRANSAID_MAP, "green"}
};

MessageScheduler::MessageScheduler(iCSInterface*  controller, const std::string& sumoPOI) :
    m_node_interface(controller), m_eventBroadcast(0),  m_broadcastCheckInterval(100),
    m_highlightDuration(m_defaultHighlightDuration),
    m_highlightSwitch(m_defaultHighlightSwitch),
    m_highlightColor(m_defaultHighlightColor),
    m_sumoPOI(sumoPOI) {
    if (controller->GetNode()->isFixed()) {
        m_highlightSize = m_defaultHighlightSizeRSU;
        m_sumoID = sumoPOI;
    } else {
        m_highlightSize = m_defaultHighlightSizeVeh;
        m_sumoID = controller->GetNode()->getId();
        if (sumoPOI != "") {
            std::cerr << "Warning: MessageScheduler(): Ignoring given argument sumoPOI because this is a vehicle ('" << m_sumoID << "')" << std::endl;

        }
    }

    // std::cout << "Starting  Message Scheduler"  << std::endl;

    Start();

}

MessageScheduler::~MessageScheduler() {
    Scheduler::Cancel(m_eventBroadcast);

}

void MessageScheduler::Start() {

    // std::cout << "Starting  timer of Message Scheduler"  << std::endl;

    TransaidHeader::CamInfo*   messageCAM = new TransaidHeader::CamInfo() ;
    m_lastCAMsent = *messageCAM;

    TransaidHeader::McmVehicleInfo*   messageMCMvehicle = new TransaidHeader::McmVehicleInfo() ;
    m_lastMCMsentVehicle = *messageMCMvehicle;

//           if (!m_node_interface->GetNode()->isFixed()) {
    /// Start transmitting CAMs, CPMs, and MCMs
    m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &MessageScheduler::V2XmessageScheduler, this);
//           }
}

void MessageScheduler::ReceiveMessage(int receiverID, server::Payload* payload, double snr, bool mobileNode) {
    MessageType mt = payload->getHeader(server::Position::PAYLOAD_END)->getMessageType();
    highlightTransmission(mt);
}

void MessageScheduler::V2XmessageScheduler() {

    /*  std::cout << "Message Scheduler checks Tx of cams at node " << m_node_interface->GetId()
                << " time diff "
                << (CurrentTime::Now() - m_lastCAMsent.generationTime) << " last tx "
                << m_lastCAMsent.generationTime << std::endl;
                */

    // Check CAM Tx

    if (m_node_interface->GetNodeType() != NT_RSU) {

        double distance = GetDistance(m_node_interface->GetNode()->getPosition(), m_lastCAMsent.position);
        double speedDiference = fabs(m_lastCAMsent.speed - m_node_interface->GetNode()->getSpeed());

        if ((CurrentTime::Now() - m_lastCAMsent.generationTime) > 1000  || distance > 4 || speedDiference > 0.5) {
            SendCAM();
        }

    }
    /* else {
        if (  (CurrentTime::Now() - m_lastCAMsent.generationTime)>1000  )
        {
            SendCAM();
        }
    } */

    // Check CPM tx
    m_NodeMap  = m_node_interface->GetAllNodes() ;
    CPM_Sensing();



    // Check MCM tx
    if (m_node_interface->GetNodeType() != NT_RSU) {
        double distance = GetDistance(m_node_interface->GetNode()->getPosition(), m_lastMCMsentVehicle.position);
        double speedDifference = fabs(m_lastMCMsentVehicle.speed - m_node_interface->GetNode()->getSpeed());

        if ((CurrentTime::Now() - m_lastMCMsentVehicle.generationTime) > 1000  || distance > 4 || speedDifference > 0.5) {
            SendMCMvehicle();
        }
    }


    m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &MessageScheduler::V2XmessageScheduler, this);
}

void MessageScheduler::SendCAM() {
    // std::cout << "Message Scheduler sendCAM function"  << std::endl;


    TransaidHeader::CamInfo*   message = new TransaidHeader::CamInfo() ;
    message->generationTime = CurrentTime::Now();
    message->senderID = m_node_interface->GetId();
    message->position = m_node_interface->GetNode()->getPosition();
    message->laneIndex = m_node_interface->GetNode()->getLaneIndex();
    message->speed = m_node_interface->GetNode()->getSpeed();
    message->acceleration = m_node_interface->GetNode()->getAcceleration();

    m_lastCAMsent = *message;

    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_CAM, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send CAM at node " << m_node_interface->GetId() << " time " << m_lastCAMsent.generationTime << std::endl;
    highlightTransmission(TRANSAID_CAM);
}

void MessageScheduler::SendCPM() {

    // std::cout << "Message Scheduler sendCPM function"  << std::endl;

    TransaidHeader::CpmInfo*   message = new TransaidHeader::CpmInfo() ;
    message->generationTime = CurrentTime::Now();
    message->senderID = m_node_interface->GetId();
    message->numObstacles = CPM_number_of_objects;
    message->CPM_message_size = pktSize;

    if (CPM_number_of_objects > 0) {
        int i = 0;
        message->CPM_detected_objectID.reserve(std::min<size_t>(MAX_NUM_CPM_OBJECTS, ETSIlist.size()));
        for (std::map<int, data1> ::iterator it = ETSIlist.begin() ; it != ETSIlist.end(); ++it) {
            message->CPM_detected_objectID.push_back(it->first);
            i++;
            if (i == 50) {
                break;
            }
        }
    }



    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_CPM, message, pktSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send CPM at node " << m_node_interface->GetId() << " time " << message->generationTime << std::endl;



    // for (NodeMap::const_iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it)
    //  std::cout <<  " CPM Vehicle node " << it->second->getId()  << " of type " <<   it->second->getNodeType()<< " speed " << it->second->getController()->GetNode()->getSpeed()
    //          << " direction " << it->second->getController()->GetNode()->getDirection()  << " in position "<<it->second->getController()->GetNode()->getPosition() << std::endl;


    highlightTransmission(TRANSAID_CPM);
}


void MessageScheduler::SendMCMvehicle() {


    TransaidHeader::McmVehicleInfo*   message = new TransaidHeader::McmVehicleInfo() ;
    message->generationTime = CurrentTime::Now();
    message->senderID = m_node_interface->GetId();
    message->position = m_node_interface->GetNode()->getPosition();
    message->speed = m_node_interface->GetNode()->getSpeed();

    m_lastMCMsentVehicle = *message;


    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_MCM_VEHICLE, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    //  std::cout << "Send MCM at node " << m_node_interface->GetId() << " time " << m_lastMCMsentVehicle.generationTime << std::endl;
    highlightTransmission(TRANSAID_MCM_VEHICLE);
}

void MessageScheduler::SendMCMvehicle(TransaidHeader::McmVehicleInfo*   message) {

    m_lastMCMsentVehicle = *message;


    int messageSize = 190;
    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_MCM_VEHICLE, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send Vehicle-MCM ordered from behaviour at node " << m_node_interface->GetId() << " time " << m_lastMCMsentVehicle.generationTime << std::endl;

    highlightTransmission(TRANSAID_MCM_VEHICLE);
}

void MessageScheduler::SendMCMrsu(TransaidHeader::McmRsuInfo*   message) {

    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_MCM_RSU, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    //std::cout << "Send RSU-MCM ordered from behaviour at node " << m_node_interface->GetId() << " time " << CurrentTime::Now() << std::endl;

    highlightTransmission(TRANSAID_MCM_RSU);
}

void MessageScheduler::SendMAP(TransaidHeader::MapInfo*   message) {


    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_MAP, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send MAP ordered from app at node " << m_node_interface->GetId() << " time " << CurrentTime::Now() << std::endl;

    highlightTransmission(TRANSAID_MAP);
}

void MessageScheduler::SendIVI(TransaidHeader::IviInfo*   message) {

    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_IVI, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send IVI ordered from app at node " << m_node_interface->GetId() << " time " << CurrentTime::Now() << std::endl;

    highlightTransmission(TRANSAID_IVI);
}

void MessageScheduler::SendDENM(TransaidHeader::DenmInfo*   message) {

    int messageSize = 190;

    TransaidHeader* header = new TransaidHeader(PID_TRANSAID, TRANSAID_DENM, message, messageSize);
    m_node_interface->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);
    // std::cout << "Send DENM ordered from app at node " << m_node_interface->GetId() << " time " << CurrentTime::Now() << std::endl;

    highlightTransmission(TRANSAID_DENM);
}


void MessageScheduler::ForwardSensing(int sendernode, int sensorno) {
    //  std::cout << "FORWARD SENSING START :  "<< std::endl;

    uint32_t ticket = 0;
    uint32_t ticket1 = 0;
    uint32_t detectedvehicles = 0;
    uint32_t k = 0;
    uint32_t mask_counter = 0;


    double maskangle [4];
    double target_sideangles[5];
    double minangle ;
    double maxangle ;

    double detectedvehiclesID [100];
    double maskvehiclesID [100];
    double nonmaskvehiclesID [100];

    //Sendernodeid details
    //	      double t = (Simulator::Now ()).GetSeconds ();

    int sendernodeId = m_node_interface->GetId() ;
    const Vector2D senderposition = m_node_interface->GetNode()->getPosition();
    double t = CurrentTime::Now();

    //--------------SENSOR  START------------------------
    //This for loop is to find vehicles within the sensor1 FOV
    for (NodeMap::const_iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it) {

        if (it->second->getNodeType() != 1) { //To avoid the RSU in detection
            int checknodeId = it->second->getId() ;
            const Vector2D detectedposition = it->second->getController()->GetNode()->getPosition();

            if (sendernodeId != checknodeId) {
                //  if (sendernodeId == 0){
                // std::cout << "DETECTED Vehicle ID : " << checknodeId <<" Distance X :" << detectedposition.x << " Y :"  << detectedposition.y << " Vehicle Type " << it->second->getNodeType()<< std::endl;//}

                double X = detectedposition.x - senderposition.x ;
                double Y = detectedposition.y - senderposition.y ;

                //						  std::cout << "X2-X1 :  "<< X << "Y2-Y1 :  "<< Y << std::endl;



                // compute node distance
                double distance = GetDistance(senderposition, detectedposition);
                //				  if (sendernodeId == 0){
                // std::cout << "Distance from sender node : " << sendernodeId << " to detected node "  << checknodeId << " is : " << distance << std::endl;
                //				  }

                double angle = atan2(Y, X) * 180 / PI;
                // if ((sendernodeId == 11) ){
                // std::cout << "DETECTED Node :  "<< checknodeId << "Distance" << distance << "   DETECTED ANGLE :  "<< angle << std::endl;//}

                if (angle >= 90) {
                    angle = floor(angle);
                    /* if ((sendernodeId == 0)  ){
                         std::cout << "check point "   << std::endl; }*/
                }

                if (angle <= -90) {
                    angle = ceil(angle);
                    //std::cout << "check point "   << std::endl;
                }

                double Fsensing_distance_temp = Fsensordistance[sensorno] + carlength / 2;
                if ((distance < Fsensing_distance_temp)  && (((Fsensornegativeangle[sensorno] <= angle) && (angle <= 0)) || ((0 <= angle) && (angle <= Fsensorpositiveangle[sensorno]))))
                    //if ( (distance < Fsensordistance[sensorno])+ (carlength/2) )
                {
                    detectedvehiclesID [ticket] = checknodeId;
                    ticket++;
                    detectedvehicles += 1;


                    //if(sendernodeId == 6){
                    //std::cout << "DETECTED nodes at FORWARD SENSING :  "<< checknodeId << std::endl;//}

                }

            }
        }

    }
    //std::cout << "TOTAL VEHICLES DETECTED before MASKING SENSOR : "<<sensorno << "  Detected-Vehicles: " << detectedvehicles  << std::endl;

    if (detectedvehicles > 1) {
        for (uint32_t i = 0; i < ticket; ++i) {

            int checki = m_NodeMap.operator [](detectedvehiclesID [i])->getId();
            const Vector2D detectedi = m_NodeMap.operator [](detectedvehiclesID [i])->getPosition();
            double distance1 = GetDistance(senderposition, detectedi);



            /*Ptr<Node> Node_ticket = Nodes.Get (detectedvehiclesID [i]);
            Ptr<MobilityModel> model3 = Node_ticket ->GetObject<MobilityModel>();
            int checki = model3->GetObject<Node> ()->GetId ();
            Vector detectedi = model3->GetPosition ();
            double distance1 = modelx->GetDistanceFrom (model3);*/
            //  std::cout << "DETECTED Vehicle ID :" << checki << "DISTANCE $1$ :  "<< distance1 << std::endl;
            //  std::cout << "DETECTED Vehicle X position :" << detectedi.x << std::endl;
            //  std::cout << "DETECTED Vehicle Y position :" << detectedi.y << std::endl;

            //I assumed car length as 4m and width as 2m
            double leftpointbackx = detectedi.x - (carlength / 2) ;
            double leftpointbacky = detectedi.y + (carwidth / 2);

            //  std::cout << "DETECTED Vehicle leftpointbackx position :" << leftpointbackx << std::endl;
            //  std::cout << "DETECTED Vehicle leftpointbacky position :" << leftpointbacky << std::endl;


            double rightpointbackx = detectedi.x - (carlength / 2) ;
            double rightpointbacky = detectedi.y - (carwidth / 2);

            double leftpointfrontx = detectedi.x + (carlength / 2) ;
            double leftpointfronty = detectedi.y + (carwidth / 2);

            double rightpointfrontx = detectedi.x + (carlength / 2) ;
            double rightpointfronty = detectedi.y - (carwidth / 2);

            double medianX = detectedi.x - senderposition.x ;
            double medianY = detectedi.y - senderposition.y ;

            double leftbackX = leftpointbackx - senderposition.x;
            double leftbackY = leftpointbacky - senderposition.y;

            double rightbackX = rightpointbackx - senderposition.x;
            double rightbackY = rightpointbacky - senderposition.y;

            double leftfrontX = leftpointfrontx - senderposition.x;
            double leftfrontY = leftpointfronty - senderposition.y;

            double rightfrontX = rightpointfrontx - senderposition.x;
            double rightfrontY = rightpointfronty - senderposition.y;

            double maskanglemedian = atan2(medianY, medianX) * 180 / PI;
            //maskangle [0] = maskanglemedian;
            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "Median MASKING angle : " << maskanglemedian  << std::endl;

            //						  std::cout << "VEHICLE BACK ANGLE "  << std::endl;

            double maskangleleftback = atan2(leftbackY, leftbackX) * 180 / PI;
            maskangle [0] = maskangleleftback;
            //***
            //if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "LEFT MASKING angle : " << maskangleleftback  << std::endl;



            double maskanglerightback = atan2(rightbackY, rightbackX) * 180 / PI;
            maskangle [1] = maskanglerightback;
            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //	 std::cout << "RIGHT MASKING angle : " << maskanglerightback  << std::endl;

            //					  std::cout << "VEHICLE FRONT ANGLE "  << std::endl;

            double maskangleleftfront = atan2(leftfrontY, leftfrontX) * 180 / PI;
            maskangle [2] = maskangleleftfront;
            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //	  std::cout << "LEFT MASKING angle : " << maskangleleftback  << std::endl;

            double maskanglerightfront = atan2(rightfrontY, rightfrontX) * 180 / PI;
            maskangle [3] = maskanglerightfront;
            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "RIGHT MASKING angle : " << maskanglerightback  << std::endl;


            //Find the min and max angle
            minangle = maskangle [0];
            maxangle = maskangle [0];

            for (uint32_t x = 0; x <= 3; x++) {
                if (maskangle[x] > maxangle) {
                    maxangle = maskangle[x];
                } else if (maskangle[x] < minangle) {
                    minangle = maskangle[x];
                }
            }
            // if(sendernodeId==6 && checki == ){
            //  std::cout << "CHECKING VEHICLE : " << checki << " MINIMUM MASKING angle : " << minangle  << " MAXIMUM MASKING angle : " << maxangle << std::endl;//}

            for (uint32_t j = 0; j < ticket; ++j) {
                uint32_t alreadyexist = 0;

                int checkj = m_NodeMap.operator [](detectedvehiclesID [j])->getId();
                const Vector2D detectedj = m_NodeMap.operator [](detectedvehiclesID [j])->getPosition();
                double distance2 = GetDistance(senderposition, detectedj);



                /* Ptr<Node> Node_ticket = Nodes.Get (detectedvehiclesID [j]);
                 Ptr<MobilityModel> model4 = Node_ticket ->GetObject<MobilityModel>();
                 int checkj = model4->GetObject<Node> ()->GetId ();
                 Vector detectedj = model4->GetPosition ();
                 double distance2 = modelx->GetDistanceFrom (model4);*/

                for (uint32_t ck = 0; ck < ticket1 ; ++ck) {
                    if (maskvehiclesID [ck] == checkj) {
                        alreadyexist = 1;
                        break;
                    }
                }


                if ((distance1 < distance2) && (checki != checkj) && (alreadyexist == 0)) {

                    //								  std::cout << "DETECTED Vehicle ID :" << checki << " DISTANCE $1$ :  "<< distance1 << " TARGET Vehicle ID :" << checkj << " DISTANCE $2$ :  "<< distance2 << std::endl;

                    double targetX = detectedj.x - senderposition.x ;
                    double targetY = detectedj.y - senderposition.y ;
                    double targetmedianangle = atan2(targetY, targetX) * 180 / PI;

                    target_sideangles[0] = targetmedianangle;


                    double leftpointbackx = detectedj.x - (carlength / 2) ;
                    double leftpointbacky = detectedj.y + (carwidth / 2);

                    double rightpointbackx = detectedj.x - (carlength / 2) ;
                    double rightpointbacky = detectedj.y - (carwidth / 2);

                    double leftpointfrontx = detectedj.x + (carlength / 2) ;
                    double leftpointfronty = detectedj.y + (carwidth / 2);

                    double rightpointfrontx = detectedj.x + (carlength / 2) ;
                    double rightpointfronty = detectedj.y - (carwidth / 2);

                    double leftbackX = leftpointbackx - senderposition.x;
                    double leftbackY = leftpointbacky - senderposition.y;

                    double rightbackX = rightpointbackx - senderposition.x;
                    double rightbackY = rightpointbacky - senderposition.y;

                    double leftfrontX = leftpointfrontx - senderposition.x;
                    double leftfrontY = leftpointfronty - senderposition.y;

                    double rightfrontX = rightpointfrontx - senderposition.x;
                    double rightfrontY = rightpointfronty - senderposition.y;

                    double targetleftback = atan2(leftbackY, leftbackX) * 180 / PI;
                    target_sideangles[1] = targetleftback;
                    double targetrightback = atan2(rightbackY, rightbackX) * 180 / PI;
                    target_sideangles[2] = targetrightback;
                    double targetleftfront = atan2(leftfrontY, leftfrontX) * 180 / PI;
                    target_sideangles[3] = targetleftfront;
                    double targetrightfront = atan2(rightfrontY, rightfrontX) * 180 / PI;
                    target_sideangles[4] = targetrightfront;

                    uint32_t mask_counter = 0;
                    for (int i = 0 ; i < 4; i++) {
                        if ((target_sideangles[i] <= maxangle) && (target_sideangles[i] >= minangle)) {
                            mask_counter++;
                        }
                    }
                    /*	std::cout << "TARGET VEHICLE : " << checkj  << std::endl;
                          //if (sendernodeId==11 && checkj == 14  && (checki == 10)){
                          std::cout << "TARGET MEDIAN angle : " << targetmedianangle  << std::endl;
                          std::cout << "TARGET LEFT BACK angle : " << targetleftback  << std::endl;
                          std::cout << "TARGET LEFT FRONT angle : " << targetleftfront  << std::endl;
                          std::cout << "TARGET RIGHT BACK angle : " << targetrightback  << std::endl;
                          std::cout << "TARGET RIGHT FRONT angle : " << targetrightfront  << std::endl;*/
                    // }



                    //	if ( ((targetangle <= maxangle) && (targetangle >= minangle)) )
                    if (((targetmedianangle <= maxangle) && (targetmedianangle >= minangle))   ||
                            ((((targetleftback <= maxangle) && (targetleftfront <= maxangle)) && ((targetleftback  >= minangle) && (targetleftfront  >= minangle))) ||
                             (((targetrightback <= maxangle) && (targetrightfront <= maxangle)) && ((targetrightback  >= minangle) && (targetrightfront >= minangle))))  || (targetmedianangle == maskanglemedian)) {
                        //|| (mask_counter >= 2)

                        maskvehiclesID [ticket1] = checkj;
                        ticket1++;
                    }
                }

            }

        }
    }

    //Now we have two list  detectedvehiclesID[] list and maskvehiclesID[] list.
    //Compare both list and find the vehicles that are not masked

    detectedvehicles = 0;
    for (uint32_t i = 0; i < ticket; ++i) {
        int find = 0;
        for (uint32_t j = 0; j < ticket1; ++j) {
            if (detectedvehiclesID[i] == maskvehiclesID[j]) {
                find += 1;
                break;
            }
        }
        if (find == 0) {
            nonmaskvehiclesID[k] = detectedvehiclesID[i];
            k += 1;
            detectedvehicles += 1;

        }
    }

    Globalflag =  Globalflag + k;

    //std::cout << "TOTAL VEHICLES DETECTED *AFTER* SENSOR MASKING : " << detectedvehicles << std::endl;


    for (uint32_t i = 0; tvcount < Globalflag ; ++tvcount, ++i) {
        Totaldetectedvehicles[tvcount] = nonmaskvehiclesID[i];
        // std::cout << "FORWARD Non MASKING Vehicle ID : " << nonmaskvehiclesID[i] << std::endl;
    }

    //  std::cout << "FORWARD SENSING ENDS :  "<< std::endl;
    //-------------- SENSOR ENDS---------------

}

void MessageScheduler:: ReverseSensing(int sendernode, int sensorno) {
    // std::cout << "REVERSE SENSING STARTS :  "<< std::endl;
    uint32_t ticket = 0;
    uint32_t ticket1 = 0;
    uint32_t detectedvehicles = 0;
    uint32_t k = 0;
    uint32_t mask_counter = 0;

    double maskangle [4];
    double target_sideangles[5];
    double minangle ;
    double maxangle ;

    double detectedvehiclesID [100];
    double maskvehiclesID [100];
    double nonmaskvehiclesID [100];

    //Sendernodeid details
//	      double t = (Simulator::Now ()).GetSeconds ();
    int sendernodeId = m_node_interface->GetId() ;
    const Vector2D senderposition = m_node_interface->GetNode()->getPosition();
    double t = CurrentTime::Now();

//--------------SENSOR  START------------------------
    //This for loop is to find vehicles within the sensor1 FOV
    for (NodeMap::const_iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it) {

        if (it->second->getNodeType() != 1) { //To avoid the RSU in detection
            int checknodeId = it->second->getId() ;
            const Vector2D detectedposition = it->second->getController()->GetNode()->getPosition();


            if (sendernodeId != checknodeId) {
//				  if (sendernodeId == 0){
//			  std::cout << "DETECTED Vehicle ID : " << checknodeId <<" Distance X :" << detectedposition.x << " Y :"  << detectedposition.y << std::endl;}

                double X = detectedposition.x - senderposition.x ;
                double Y = detectedposition.y - senderposition.y ;

//						  std::cout << "X2-X1 :  "<< X << "Y2-Y1 :  "<< Y << std::endl;

                // compute node distance
                double distance = GetDistance(senderposition, detectedposition);
                /*  if (sendernodeId == 0){
                std::cout << "Distance from sender node : " << sendernodeId << " to detected node "  << checknodeId << " is : " << distance << std::endl;
                }*/

                double angle = atan2(Y, X) * 180 / PI;
                /* if ((sendernodeId == 6)  ){
                 std::cout << "DETECTED Node :  "<< checknodeId << "   DETECTED ANGLE :  "<< angle << std::endl; }*/

                if (angle >= 180) {
                    angle = floor(angle);
                    /* if ((sendernodeId == 0)  ){
                         std::cout << "check point "   << std::endl; }*/
                }

                if (angle <= -180) {
                    angle = ceil(angle);
                    //std::cout << "check point "   << std::endl;
                }

                double Rsensing_distance_temp = Rsensordistance[sensorno] + carlength / 2;
                if ((distance < Rsensing_distance_temp) && (((angle <= Rsensornegativeangle[sensorno]) && (angle >= -180)) || ((angle >= Rsensorpositiveangle[sensorno]) && (angle <= 180)))) {
                    detectedvehiclesID [ticket] = checknodeId;
                    ticket++;
                    detectedvehicles += 1;


                    /*	if((sendernodeId == 6) ){
                            std::cout << "DETECTED node REVERSE sensing :  "<< checknodeId << std::endl;}*/

                }

            }
        }
    }

    //std::cout << "TOTAL VEHICLES DETECTED before MASKING SENSOR : "<<sensorno << "  Detected-Vehicles: " << detectedvehicles  << std::endl;

    //std::cout << "DETECTED vehicle count  :  "<< detectedvehicles  << std::endl;

    //This for loop is to detect the masked vehicles
    if (detectedvehicles > 1) {
        for (uint32_t i = 0; i < ticket; ++i) {

            int checki = m_NodeMap.operator [](detectedvehiclesID [i])->getId();
            const Vector2D detectedi = m_NodeMap.operator [](detectedvehiclesID [i])->getPosition();
            double distance1 = GetDistance(senderposition, detectedi);
            //  std::cout << "DETECTED Vehicle ID :" << checki << "DISTANCE $1$ :  "<< distance1 << std::endl;
            //  std::cout << "DETECTED Vehicle X position :" << detectedi.x << std::endl;
            //  std::cout << "DETECTED Vehicle Y position :" << detectedi.y << std::endl;

            //I assumed car length as 4m and width as 2m
            double leftpointbackx = detectedi.x - (carlength / 2) ;
            double leftpointbacky = detectedi.y + (carwidth / 2);

            //  std::cout << "DETECTED Vehicle leftpointbackx position :" << leftpointbackx << std::endl;
            //  std::cout << "DETECTED Vehicle leftpointbacky position :" << leftpointbacky << std::endl;


            double rightpointbackx = detectedi.x - (carlength / 2) ;
            double rightpointbacky = detectedi.y - (carwidth / 2);

            double leftpointfrontx = detectedi.x + (carlength / 2) ;
            double leftpointfronty = detectedi.y + (carwidth / 2);

            double rightpointfrontx = detectedi.x + (carlength / 2) ;
            double rightpointfronty = detectedi.y - (carwidth / 2);

            double medianX = detectedi.x - senderposition.x ;
            double medianY = detectedi.y - senderposition.y ;

            double leftbackX = leftpointbackx - senderposition.x;
            double leftbackY = leftpointbacky - senderposition.y;

            double rightbackX = rightpointbackx - senderposition.x;
            double rightbackY = rightpointbacky - senderposition.y;

            double leftfrontX = leftpointfrontx - senderposition.x;
            double leftfrontY = leftpointfronty - senderposition.y;

            double rightfrontX = rightpointfrontx - senderposition.x;
            double rightfrontY = rightpointfronty - senderposition.y;

            double maskanglemedian = atan2(medianY, medianX) * 180 / PI;
            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "Median MASKING angle : " << maskanglemedian  << std::endl;

//						  std::cout << "VEHICLE BACK ANGLE "  << std::endl;

            double maskangleleftback = atan2(leftbackY, leftbackX) * 180 / PI;
            maskangle [0] = maskangleleftback;
            if (maskangleleftback < 0) {
                maskangle [0] = maskangleleftback + 360;
            }

            //***
            //if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "LEFT MASKING angle : " << maskangleleftback  << std::endl;



            double maskanglerightback = atan2(rightbackY, rightbackX) * 180 / PI;
            maskangle [1] = maskanglerightback;
            if (maskanglerightback < 0) {
                maskangle [1] = maskanglerightback + 360;
            }

            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //	 std::cout << "RIGHT MASKING angle : " << maskanglerightback  << std::endl;

//					  std::cout << "VEHICLE FRONT ANGLE "  << std::endl;

            double maskangleleftfront = atan2(leftfrontY, leftfrontX) * 180 / PI;
            maskangle [2] = maskangleleftfront;
            if (maskangleleftfront < 0) {
                maskangle [2] = maskangleleftfront + 360;
            }

            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //	  std::cout << "LEFT MASKING angle : " << maskangleleftback  << std::endl;

            double maskanglerightfront = atan2(rightfrontY, rightfrontX) * 180 / PI;
            maskangle [3] = maskanglerightfront;
            if (maskanglerightfront < 0) {
                maskangle [3] = maskanglerightfront + 360;
            }

            // if((sendernodeId == 1) || (sendernodeId == 0)){
            //  std::cout << "RIGHT MASKING angle : " << maskanglerightback  << std::endl;


            //Find the min and max angle
            minangle = maskangle [0];
            maxangle = maskangle [0];

            for (uint32_t x = 0; x <= 3; x++) {
                if (maskangle[x] > maxangle) {
                    maxangle = maskangle[x];
                } else if (maskangle[x] < minangle) {
                    minangle = maskangle[x];
                }
            }

            //  if(sendernodeId == 11 && checki == 10 ){
            // std::cout << "CHECKING VEHICLE : " << checki << " MINIMUM MASKING angle : " << minangle  << " MAXIMUM MASKING angle : " << maxangle << std::endl;}

            for (uint32_t j = 0; j < ticket; ++j) {
                uint32_t alreadyexist = 0;

                int checkj = m_NodeMap.operator [](detectedvehiclesID [j])->getId();
                const Vector2D detectedj = m_NodeMap.operator [](detectedvehiclesID [j])->getPosition();
                double distance2 = GetDistance(senderposition, detectedj);

                for (uint32_t ck = 0; ck < ticket1 ; ++ck) {
                    if (maskvehiclesID [ck] == checkj) {
                        alreadyexist = 1;
                        break;
                    }
                }


                if ((distance1 < distance2) && (checki != checkj) && (alreadyexist == 0)) {
                    // if(sendernodeId == 11 && checki == 10){
                    // std::cout << "DETECTED Vehicle ID :" << checki << " DISTANCE $1$ :  "<< distance1 << " TARGET Vehicle ID :" << checkj << " DISTANCE $2$ :  "<< distance2 << std::endl; }

                    double targetX = detectedj.x - senderposition.x ;
                    double targetY = detectedj.y - senderposition.y ;
                    double targetmedianangle = atan2(targetY, targetX) * 180 / PI;
                    if (targetmedianangle < 0) {
                        targetmedianangle = targetmedianangle + 360;
                    }
                    target_sideangles[0] = targetmedianangle;

                    double leftpointbackx = detectedj.x - (carlength / 2) ;
                    double leftpointbacky = detectedj.y + (carwidth / 2);

                    double rightpointbackx = detectedj.x - (carlength / 2) ;
                    double rightpointbacky = detectedj.y - (carwidth / 2);

                    double leftpointfrontx = detectedj.x + (carlength / 2) ;
                    double leftpointfronty = detectedj.y + (carwidth / 2);

                    double rightpointfrontx = detectedj.x + (carlength / 2) ;
                    double rightpointfronty = detectedj.y - (carwidth / 2);

                    double leftbackX = leftpointbackx - senderposition.x;
                    double leftbackY = leftpointbacky - senderposition.y;

                    double rightbackX = rightpointbackx - senderposition.x;
                    double rightbackY = rightpointbacky - senderposition.y;

                    double leftfrontX = leftpointfrontx - senderposition.x;
                    double leftfrontY = leftpointfronty - senderposition.y;

                    double rightfrontX = rightpointfrontx - senderposition.x;
                    double rightfrontY = rightpointfronty - senderposition.y;

                    double targetleftback = atan2(leftbackY, leftbackX) * 180 / PI;
                    double targetrightback = atan2(rightbackY, rightbackX) * 180 / PI;
                    double targetleftfront = atan2(leftfrontY, leftfrontX) * 180 / PI;
                    double targetrightfront = atan2(rightfrontY, rightfrontX) * 180 / PI;

                    if (targetleftback < 0) {
                        targetleftback = targetleftback + 360;
                    }
                    target_sideangles[1] = targetleftback;

                    if (targetrightback < 0) {
                        targetrightback = targetrightback + 360;
                    }
                    target_sideangles[2] = targetrightback;

                    if (targetleftfront < 0) {
                        targetleftfront = targetleftfront + 360;
                    }
                    target_sideangles[3] = targetleftfront;

                    if (targetrightfront < 0) {
                        targetrightfront = targetrightfront + 360;
                    }
                    target_sideangles[4] = targetrightfront;

                    uint32_t mask_counter = 0;
                    for (int i = 0 ; i < 4; i++) {
                        if ((target_sideangles[i] <= maxangle) && (target_sideangles[i] >= minangle)) {
                            mask_counter++;
                        }
                    }

                    /* if(sendernodeId == 11 && checki == 10 && checkj == 14){
                    std::cout << "TARGET MEDIAN angle : " << targetmedianangle  << std::endl;
                    std::cout << "TARGET LEFT BACK angle : " << targetleftback  << std::endl;
                    std::cout << "TARGET LEFT FRONT angle : " << targetleftfront  << std::endl;
                    std::cout << "TARGET RIGHT BACK angle : " << targetrightback  << std::endl;
                    std::cout << "TARGET RIGHT FRONT angle : " << targetrightfront  << std::endl;}*/


                    //	if ( ((targetangle <= maxangle) && (targetangle >= minangle)) )
                    if (((targetmedianangle <= maxangle) && (targetmedianangle >= minangle))  ||
                            ((((targetleftback <= maxangle) && (targetleftfront <= maxangle)) && ((targetleftback  >= minangle) && (targetleftfront  >= minangle))) ||
                             (((targetrightback <= maxangle) && (targetrightfront <= maxangle)) && ((targetrightback  >= minangle) && (targetrightfront >= minangle))))  || (targetmedianangle == maskanglemedian)) {
                        // || (mask_counter >= 2)
                        maskvehiclesID [ticket1] = checkj;
                        ticket1++;
                    }
                }

            }

        }
    }

    //Now we have two list  detectedvehiclesID[] list and maskvehiclesID[] list.
    //Compare both list and find the vehicles that are not masked

    detectedvehicles = 0;
    for (uint32_t i = 0; i < ticket; ++i) {
        int find = 0;
        for (uint32_t j = 0; j < ticket1; ++j) {
            if (detectedvehiclesID[i] == maskvehiclesID[j]) {
                find += 1;
                break;
            }
        }
        if (find == 0) {
            nonmaskvehiclesID[k] = detectedvehiclesID[i];
            k += 1;
            detectedvehicles += 1;

        }
    }

    Globalflag =  Globalflag + k;

    // std::cout << "TOTAL VEHICLES DETECTED *AFTER* SENSOR MASKING : " << detectedvehicles << std::endl;


    for (uint32_t i = 0; tvcount < Globalflag ; ++tvcount, ++i) {
        Totaldetectedvehicles[tvcount] = nonmaskvehiclesID[i];
        //std::cout << "REVERSE Non MASKING Vehicle ID : " << nonmaskvehiclesID[i] << std::endl;
    }

    // std::cout << "REVERSE SENSING ENDS :  "<< std::endl;
//-------------- SENSOR ENDS---------------

}


void MessageScheduler::CPM_Sensing() {
    Globalflag = 0;
    tvcount = 0;

    uint32_t count = 7;
    uint32_t flag = 7;

    uint32_t ETSIcount = 0;
    double detectedvehiclesETSI[100];
    double tempprevioustime = 0;
    double firstmsg = 0;
    double timedifference = 1;

    uint32_t sensorcontainer = 35 ; //When more than one sensor is configured and it is assumed to be sensor fused, it requires only single sensor container size.
    uint32_t objectcontainer = 35;


    double t = CurrentTime::Now();

    int sendernodeId = m_node_interface->GetId() ;
    const Vector2D senderposition = m_node_interface->GetNode()->getPosition();


    //  std::cout << "----------------------******* SENDER vehicle ID :  "<< sendernodeId  <<"  Direction  "  << m_node_interface->GetNode()->getDirection()<< " Vehicle Type " << m_node_interface->GetNode()->getNodeType() <<"  POSITION_X  "<< senderposition.x << "  POSITION_Y  "<< senderposition.y <<"  TIME  " << t << std::endl;


    for (uint32_t sensorno = 0; sensorno < totalsensors; ++sensorno) {
        ForwardSensing(sendernodeId, sensorno);
        ReverseSensing(sendernodeId,  sensorno);
    }
    //Fusion (removing duplicates from the 	Totaldetectedvehicles array

    for (uint32_t i = 0; i < Globalflag; ++i)
        for (uint32_t j = i + 1; j < Globalflag;) {
            if (Totaldetectedvehicles[i] == Totaldetectedvehicles[j]) {
                for (uint32_t k = j; k < Globalflag - 1; ++k) {
                    Totaldetectedvehicles[k] = Totaldetectedvehicles[k + 1];
                }

                --Globalflag;
            } else {
                ++j;
            }
        }

    //***********ETSI MEssage Generation rules*******************

    for (uint32_t i = 0; i < Globalflag; ++i) {
        int ETvehid1 = m_NodeMap.operator [](Totaldetectedvehicles [i])->getId();
        const Vector2D ETposition = m_NodeMap.operator [](Totaldetectedvehicles [i])->getPosition();
        double ETvelocity = m_NodeMap.operator [](Totaldetectedvehicles [i])->getSpeed();

        //std::cout << "DETECTED OBJECT ID after fusion :  "<< ETvehid1  << "  TIME  " << t << std::endl;
        std::map<int, data1> ::iterator ETSIit;
        data1 current;

        current.posX = ETposition.x;
        current.posY = ETposition.y;
        current.node_velocity = ETvelocity;
        //current.xVel = ETvelocity.x;
        //current.yVel = ETvelocity.y;
        current.genT = t;

        ETSIit = ETSIlist.find(ETvehid1);

        if (ETSIit != ETSIlist.end()) {

            double vehiclemoved = sqrt(pow(ETposition.x - ETSIit->second.posX, 2) + pow(ETposition.y - ETSIit->second.posY, 2));
            // std::cout << "Vehicle moved :  "<< vehiclemoved  << "  TIME  " << t << std::endl;

            double previousspeed = ETSIit->second.node_velocity;
            double currentspeed = ETvelocity;
            double vehiclespeeddifference = currentspeed - previousspeed;
            //std::cout << "Vehicle Speed difference :  "<< vehiclespeeddifference  << "  TIME  " << t << std::endl;

            double timedifer = t - ETSIit->second.genT;
            // std::cout << "Vehicle TIME difference :  "<< timedifer  <<  std::endl;

            if (((vehiclemoved > 4) || (vehiclespeeddifference >  0.5)) || (timedifer >= 1000)) {
                detectedvehiclesETSI [ETSIcount] = ETvehid1;
                ETSIcount ++;

                ETSIlist.erase(ETSIit);
                ETSIlist.insert(std::pair<int, data1>(ETvehid1, current));


                // std::cout << "*******Vehicle Included in the CPM (if):  "<< ETvehid1  << "  TIME  " << t << std::endl;

            }

        } else {
            ETSIlist.insert(std::pair<int, data1>(ETvehid1, current));

            detectedvehiclesETSI [ETSIcount] = ETvehid1;
            ETSIcount ++;

            //    std::cout << "*******Vehicle Included in the CPM (else) :  "<< ETvehid1  << "  TIME  " << t << std::endl;


        }

    }
//**********ETSI RULES ENDS*************************

    std::map<int, ETSItimer> ::iterator timerit;



    for (std::map<int, ETSItimer> ::iterator timerit = ETSIlist1.begin(); timerit != ETSIlist1.end(); ++timerit) {
        tempprevioustime = timerit->second.genT;
        firstmsg = timerit->second.msgcount;
    }

    timedifference = t - tempprevioustime;


    if ((ETSIcount != 0) || (timedifference >= 1000) || (firstmsg == 0)) {
        ETSItimer objtime;
        objtime.genT = t;
        objtime.msgcount = 1;

        timerit = ETSIlist1.find(sendernodeId);

        if (timerit != ETSIlist1.end()) {
            ETSIlist1.erase(timerit);
            ETSIlist1.insert(std::pair<int, ETSItimer>(sendernodeId, objtime));
        } else {
            ETSIlist1.insert(std::pair<int, ETSItimer>(sendernodeId, objtime));
        }


        //Goku: Computing the packet size
        objectcontainer = ETSIcount * 35;

        //Goku: Storing the object count included in the CPM
        CPM_number_of_objects = ETSIcount;


        double last_time = sensorcontainer_size;
        // printf("SenderNodeid is %d. Current time is %f and last time is %f\n", sendernodeId, t, last_time);

        if ((sensorcontainer_size == 0) || (t - last_time >= 1000)) {
            //sensorcontainer_size[0] = 1;
            sensorcontainer_size = t;

            sensorcontainer = 35 ;

            // std::cout << "*******************************The SENSOR CONTAINER SIZE IS 35: ////////////////////// "<< std::endl;

        } else {
            sensorcontainer = 0;

            // std::cout << "*******************************The SENSOR CONTAINER SIZE IS 0: ////////////////////// "<< std::endl;

        }


        pktSize = 121 + sensorcontainer + objectcontainer ;


        //     std::cout << "*******The CPM Size is :  "<< pktSize  << "  TIME  " << t << std::endl;

        //   std::cout << "//////////////////////////**The Sender sent the message: ////////////////////// "<< std::endl;


        SendCPM();

    }

}


void MessageScheduler::switchOnHighlight(MessageType mt) {
    if (mt != MT_ALL) {
        auto i = m_highlightSwitch.find(mt);
        if (i != m_highlightSwitch.end()) {
            i->second = true;
        }
    } else {
        for (auto& p : m_highlightSwitch) {
            p.second = true;
        }
    }
}


void MessageScheduler::switchOffHighlight(MessageType mt) {
    if (mt != MT_ALL) {
        auto i = m_highlightSwitch.find(mt);
        if (i != m_highlightSwitch.end()) {
            i->second = false;
        }
    } else {
        for (auto& p : m_highlightSwitch) {
            p.second = false;
        }
    }
}


void
MessageScheduler::highlightTransmission(MessageType mt) {
    if (m_highlightSwitch[mt]) {
        m_node_interface->Highlight(m_highlightColor[mt], m_highlightSize[mt], mt, m_highlightDuration[mt], m_sumoPOI);
    }
}
} /* namespace application */
} /* namespace baseapp */
