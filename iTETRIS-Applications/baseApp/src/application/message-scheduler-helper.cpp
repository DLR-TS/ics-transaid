//
// Created by alex on 23/03/19.
//

#include "message-scheduler-helper.h"
#include "node.h"
#include "../../app-commands-subscriptions-constants.h"
#include "ics-interface.h"




namespace baseapp {
    namespace application {
        using namespace std;

        MessageScheduler::MessageScheduler(int id) :
                m_eventBroadcast(0), m_eventAbortBroadcast(0), m_broadcastCheckInterval(100) {
            std::cout << "Starting  Message Scheduler"  << std::endl;
            Start();
        }

      /*  MessageScheduler::MessageScheduler(Node* node) :
                m_node(node), m_eventBroadcast(0), m_eventAbortBroadcast(0), m_broadcastCheckInterval(100) {


                std::cout << "Starting  Message Scheduler"  << std::endl;
                Start();

        }*/

        MessageScheduler::~MessageScheduler(){
            //Scheduler::Cancel(m_eventBroadcast);
            //Scheduler::Cancel(m_eventAbortBroadcast);
        }


        void MessageScheduler::Start(){

            m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &MessageScheduler::V2XmessageScheduler, this);

        }


        void MessageScheduler::V2XmessageScheduler(){


            std::cout << "Message Scheduler checks Tx of cams"  << std::endl;
            // Check CAM Tx

            if (m_lastCAMsent == nullptr || (CurrentTime::Now() - m_lastCAMsent->generationTime)>1000)
            {
                SendCAM();
            }

            m_eventBroadcast = Scheduler::Schedule(m_broadcastCheckInterval, &MessageScheduler::V2XmessageScheduler, this);
        }

        void MessageScheduler::SendCAM()
        {
            std::cout << "Message Scheduler sendCAM function"  << std::endl;



/*          TransaidHeader::CamInfo * message = new TransaidHeader::CamInfo();
            message->generationTime = CurrentTime::Now();
            message->senderID = m_node->getController()->GetId();
            message->position =m_node->getController()->GetPosition();
            message->speed = m_node->getController()->GetSpeed() ;

            m_lastCAMsent = message;

            TransaidHeader * header = new TransaidHeader(PID_TRANSAID, TRANSAID_CAM, message);
            m_node->getController()->Send(NT_ALL,  header, PID_TRANSAID, MSGCAT_TRANSAID);*/
            std::cout << "Send CAM at node " << m_node->getController()->GetId() <<  std::endl;
        }




    } /* namespace application */
} /* namespace baseapp */