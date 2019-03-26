//
// Created by alex on 23/03/19.
//

#ifndef MESSAGE_SCHEDULER_HELPER_H
#define MESSAGE_SCHEDULER_HELPER_H


#include <map>
#include <queue>
#include "foreign/tcpip/storage.h"
#include "structs.h"
#include "vector.h"
#include "random-variable.h"
#include "common.h"
#include "application/model/headers.h"
#include <application/helper/scheduler.h>
#include "ics-interface.h"

using namespace baseapp;
using namespace baseapp::application;

namespace baseapp {

    namespace application {

        class Node;

        class MessageScheduler{

        public:

            MessageScheduler(iCSInterface*);

            ~MessageScheduler();

            void Start();

            void V2XmessageScheduler();

            // Transmitt messages
            void SendCAM();
            void SendCPM();

            void SendMCMvehicle();
            void SendMCMvehicle(TransaidHeader::McmVehicleInfo *);

            void SendMCMrsu(TransaidHeader::McmRsuInfo *);

            // Check the objects to be included in the CPM
            void CPM_Sensing();
            void ForwardSensing(int sendernode, int sensorno);
            void ReverseSensing(int sendernode, int sensorno);


        private:
            iCSInterface * m_node_interface;


            // @brief used to refer to abort event scheduled at start
            event_id m_eventBroadcast;

            int m_broadcastCheckInterval;

            //Variables to detect the transmission of CPM objects
            typedef std::map<int, baseapp::application::Node*> NodeMap;
            NodeMap m_NodeMap;

#define totalsensors 1
            //For forwarding vehicles
            double Fsensorpositiveangle [totalsensors]= {90.0};
            double Fsensornegativeangle [totalsensors]= {-90.0};
            double Fsensordistance [totalsensors]= { 150 };

            //For reverse vehicles
            double Rsensorpositiveangle [totalsensors]= {90.0};
            double Rsensornegativeangle [totalsensors]= {-90.0};
            double Rsensordistance [totalsensors]= { 150 };

#define PI 3.14159265 //pi value

            double carlength= 5; //SUMO default value
            double carwidth= 2;

            uint32_t tvcount = 0;
            double Totaldetectedvehicles[100];
            uint32_t Globalflag = 0;

            double sensorcontainer_size = 0;
            int CPM_number_of_objects = 0;

            int	pktSize= 121;

            struct data1{
                double posX;
                double posY;
                double node_velocity;
                //double xVel;
                //double yVel;
                double genT;
                double receptionTime;
                uint32_t senderID;

                std::map<int, data1> Cars;
            };

            typedef std::map<int, data1> DataMap;
            DataMap ETSIlist;

            struct ETSItimer  {
                double genT;
                double msgcount;

                std::map<int, ETSItimer> Timer;
            };
            typedef std::map<int, ETSItimer> DataMap1;
            DataMap1 ETSIlist1;


        protected:

            // @brief used for checking the dynamic transmission of messages
            TransaidHeader::CamInfo   m_lastCAMsent ;
            //TransaidHeader::DenmInfo * m_lastDENMsent;
            TransaidHeader::McmVehicleInfo  m_lastMCMsentVehicle;
            //TransaidHeader::MapInfo * m_lastMAPsent;
            //TransaidHeader::IviInfo * m_lastIVIsent;



        };

    } /* namespace application */
} /* namespace baseapp */


#endif //TRANSAID_MESSAGE_SCHEDULER_HELPER_H
