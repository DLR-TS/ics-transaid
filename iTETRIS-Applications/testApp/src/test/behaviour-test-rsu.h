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

#ifndef BEHAVIOUR_TEST_RSU_H_
#define BEHAVIOUR_TEST_RSU_H_

#include <map>
#include "application/model/behaviour-rsu.h"
#include "application/helper/scheduler.h"
#include "application/helper/random-variable.h"
#include "structs.h"
#include "application/model/headers.h"
#include "message-scheduler-helper.h"

using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {
/**
 * Behaviour for rsu in test cases. Inherits from BehaviourNode to have the random response offset variables at hand.
 */
class BehaviourTestRSU: public BehaviourRsu {
public:
    BehaviourTestRSU(iCSInterface* controller);
    ~BehaviourTestRSU();

    void Start();

    bool IsSubscribedTo(ProtocolId pid) const;
    void Receive(server::Payload* payload, double snr);
    bool Execute(DirectionValueMap& data);
    void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages);
    void processTraCIResult(const int result, const Command& command);

    /**
     * @brief Called after a random timeout when a test message is received, @see Receive()
     * @input[in] sender The source of the received message
     */
    void EventSendResponse(TestHeader::ResponseInfo response);

    void RSUBroadcastCommSimple2();

    void RSUBroadcastTestV2XmsgSet();

    void SendCAM();

    void SendDENM();

    void SendMCM();

    void SendMCMTo(const std::string& vehID);

    void SendCPM();

    void SendMAP();

    void SendIVI();

    void abortBroadcast();

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR_TEST_RSU;
    }

private:

    /// @name Flags to be used by test cases
    /// @{
    bool m_firstBroadcast;
    bool m_broadcastActive;
    int m_broadcastInterval;
    bool m_mobilitySubscription;
    bool m_trafficLightSubscription;
    bool m_setCAMareaSubscription;
    bool m_subReceiveMessage;
    // see test "setVType"
    std::string m_lastVType;
    /// @}


    /// @name Events
    /// @{
    /// @brief used to refer to abort event scheduled at start
    event_id m_eventBroadcast;
    event_id m_eventAbortBroadcast;
    event_id m_eventBroadcastCAM;
    event_id m_eventBroadcastDENM;
    event_id m_eventBroadcastMAP;
    event_id m_eventBroadcastMCM;
    event_id m_eventBroadcastIVI;
    event_id m_eventBroadcastCPM;
    /// @}
    MessageScheduler* m_MessageScheduler;

    int m_broadcastCheckInterval;

    TransaidHeader::CamInfo* m_lastCAMsent;
    TransaidHeader::DenmInfo* m_lastDENMsent;
    TransaidHeader::CpmInfo* m_lastCPMsent;
    TransaidHeader::McmRsuInfo* m_lastMCMsent;
    TransaidHeader::MapInfo* m_lastMAPsent;
    TransaidHeader::IviInfo* m_lastIVIsent;

    //CPM object detection
    typedef std::map<int, baseapp::application::Node*> NodeMap;
    NodeMap m_NodeMap;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_TEST_RSU_H_ */
