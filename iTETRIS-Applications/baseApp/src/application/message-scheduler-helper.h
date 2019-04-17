//
// Created by alex on 23/03/19.
//

#ifndef MESSAGE_SCHEDULER_HELPER_H
#define MESSAGE_SCHEDULER_HELPER_H


#include <map>
#include <queue>
#include "foreign/tcpip/storage.h"
#include "utils/common/RGBColor.h"
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

        class MessageScheduler : public server::NodeHandler::MessageReceptionListener {

        public:
        	/// @brief Constructor
        	/// @param controller iCSInterface of the corresponding node
        	/// @param sumoPOI Optional parameter to specify a sumo POI to be highlighted with messaging
        	///  	   (only effective for fixed nodes)
            MessageScheduler(iCSInterface* controller, const std::string& sumoPOI="");

            virtual ~MessageScheduler();

            void Start();

	        /// @brief To be called, when a message is received
	        /// @note  If this should be enabled, the MessageScheduler has to be added as a MessageReceptionListener
            ///        at the application's NodeHandler. E.g. from the associated Behaviour:
            ///             m_msgScheduler = std::make_shared<MessageScheduler>(GetController());
            ///        		GetController()->registerMessageReceptionListener(m_msgScheduler);
	        /// @note  The payload pointer will be deleted externally after this call.
            /// TODO: Add into Wiki, add test
	        void ReceiveMessage(int receiverID, server::Payload * payload, double snr, bool mobileNode = false) override;

            void V2XmessageScheduler();

            // Transmitt messages
            void SendCAM();
            void SendCPM();

            void SendMCMvehicle();
            void SendMCMvehicle(TransaidHeader::McmVehicleInfo *);

            void SendMCMrsu(TransaidHeader::McmRsuInfo *);


            void SendMAP(TransaidHeader::MapInfo *);
            void SendIVI(TransaidHeader::IviInfo *);
            void SendDENM(TransaidHeader::DenmInfo *);

            // Check the objects to be included in the CPM
            void CPM_Sensing();
            void ForwardSensing(int sendernode, int sensorno);
            void ReverseSensing(int sendernode, int sensorno);

            /// @brief Globally switch on/off highlighting for a specific message type or all (in case mt==0)
            void switchOnHighlight(MessageType mt = MT_ALL);
            void switchOffHighlight(MessageType mt = MT_ALL);

        private:
            /// @brief highlight event of sending a message
            void highlightTransmission(MessageType);

        private:
            iCSInterface * m_node_interface;

            /// @brief SUMO ID of the object
            /// @note This is automatically retrieved for mobile nodes, for fixed,
            ///       an associated POI ID may be specified.
            std::string m_sumoID;
            /// @brief associated poi ID of the object
            /// @note Not effective for mobile nodes
            std::string m_sumoPOI;

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

            /// @name Visualization of message reception and emission
            /// @{
            /// @brief controls for which message types the highlighting will be displayed
            std::map<MessageType, bool> m_highlightSwitch;
            /// @brief controls the message type specific size of highlighting (in m.)
            std::map<MessageType, double> m_highlightSize;
            /// @brief controls the message type specific duration of highlighting (in s.)
            std::map<MessageType, double> m_highlightDuration;
            /// @brief controls the message type specific color of highlighting
            std::map<MessageType, std::string> m_highlightColor;

            /// @brief defaults for above maps
            /// @{
            static std::map<MessageType, bool> m_defaultHighlightSwitch;
            static std::map<MessageType, double> m_defaultHighlightSizeRSU;
            static std::map<MessageType, double> m_defaultHighlightSizeVeh;
            static std::map<MessageType, double> m_defaultHighlightDuration;
            static std::map<MessageType, std::string> m_defaultHighlightColor;
            /// @}
            /// @}

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
