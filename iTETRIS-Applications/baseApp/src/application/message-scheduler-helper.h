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

namespace baseapp {

    namespace application {

        class Node;

        class MessageScheduler{

                public:

                MessageScheduler(int);

                ~MessageScheduler();

                void Start();

                void V2XmessageScheduler();
                void SendCAM();

                private:
                Node * m_node;


                // @brief used to refer to abort event scheduled at start
                event_id m_eventBroadcast;
                event_id m_eventAbortBroadcast;
                int m_broadcastCheckInterval;

                // @brief used for checking the dynamic transmission of messages
                TransaidHeader::CamInfo * m_lastCAMsent;
                TransaidHeader::DenmInfo * m_lastDENMsent;
                TransaidHeader::CpmInfo * m_lastCPMsent;
                TransaidHeader::McmRsuInfo * m_lastMCMsent;
                TransaidHeader::MapInfo * m_lastMAPsent;
                TransaidHeader::IviInfo * m_lastIVIsent;


        };

    } /* namespace application */
} /* namespace baseapp */


#endif //TRANSAID_MESSAGE_SCHEDULER_HELPER_H
