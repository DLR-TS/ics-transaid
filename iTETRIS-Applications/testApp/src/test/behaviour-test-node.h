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

#ifndef BEHAVIOUR_TEST_NODE_H_
#define BEHAVIOUR_TEST_NODE_H_

#include "behaviour-node.h"
#include "scheduler.h"
#include "random-variable.h"
#include <map>
#include "structs.h"
#include "application/model/headers.h"
#include "message-scheduler-helper.h"


using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {
/**
 * Behaviour for mobile nodes in test cases.
 */
class BehaviourTestNode: public BehaviourNode {
public:
    BehaviourTestNode(iCSInterface* controller);
    ~BehaviourTestNode();

    void Start();
    void OnAddSubscriptions();
    virtual bool IsSubscribedTo(ProtocolId pid) const;
    virtual void Receive(server::Payload* payload, double snr);
    virtual bool Execute(DirectionValueMap& data);
    virtual void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages);

    /**
     * @brief Called after a random timeout when a test message is received, @see Receive()
     * @input[in] sendingRSU The source of the received message
     */
    void EventSendResponse(TestHeader::ResponseInfo response);

    void sendRepeatedBroadcast();

    void abortWaitingForRSUResponse();

    void VehicleBroadcastTestV2XmsgSet();

    void SendCAM();

    void SendCPM();

    void SendMCM();

    void abortBroadcast();

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR_TEST_NODE;
    }

private:
    /// @name Flags to be used by test cases
    /// @{
    /// @brief Vehicle check this if the RSU responded to their message.
    bool m_waitForRSUAcknowledgement;
    bool m_vehicleStopScheduled;
    bool m_firstBroadcast;
    int m_broadcastInterval;
    bool m_setCAMareaSubscription;
    bool m_subReceiveMessage;
    bool m_broadcastActive;
    /// @}


    /// @name Events
    /// @{
    /// @brief used to refer to abort event scheduled at start
    event_id m_eventAbortWaitingForRSU;
    event_id m_eventBroadcast;
    event_id m_eventAbortBroadcast;
    /// @}
    event_id m_eventBroadcastCAM;
    event_id m_eventBroadcastMCM;
    event_id m_eventBroadcastCPM;
    /// @}

    /// @brief Multipurpose counter (used in TMCBehaviour test to alternate between different RSUs)
    int m_counter;

    MessageScheduler* m_MessageScheduler;


    int m_broadcastCheckInterval;
    TransaidHeader::CamInfo m_lastCAMsent;
    TransaidHeader::DenmInfo m_lastDENMsent;
    TransaidHeader::CpmInfo m_lastCPMsent;
    TransaidHeader::McmVehicleInfo m_lastMCMsent;

};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_TEST_NODE_H_ */
