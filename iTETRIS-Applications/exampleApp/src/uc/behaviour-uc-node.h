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

#ifndef BEHAVIOUR_UC_NODE_H_
#define BEHAVIOUR_UC_NODE_H_

#include "application/model/behaviour-node.h"
#include "application/helper/scheduler.h"
#include "application/message-scheduler-helper.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {
struct cam_message_t {
    double frequency;
    event_id eventId;

    bool autoSend;
    bool initialize;
    bool highlight;
    bool enable;
};

/**
* Behaviour for mobile nodes in uc cases.
*/
class BehaviourUCNode : public BehaviourNode {
public:
    BehaviourUCNode(iCSInterface* controller, const bool use_ns3);
    ~BehaviourUCNode();

    void Start();

    void OnAddSubscriptions();
    bool IsSubscribedTo(ProtocolId pid) const;
    void Receive(server::Payload* payload, double snr);
    bool Execute(DirectionValueMap& data);

    TypeBehaviour GetType() const {
        return Type();
    }

    // in transaid/iTETRIS-Applications/baseApp/src/application/model/common.h
    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR_UC4_NODE;
    }

private:
    /** @brief Regular broadcast of CAM-like messages (schedules itself)
    *  @note Awaits fix of CAM mechanism
    */
    void sendCAMInfoRepeated();

    /** @brief Sends current vehicle state
    *  @note Awaits fix of CAM mechanism
    */
    void sendCAMInfo();

private:
    MessageScheduler* m_msgScheduler;
    cam_message_t cam_message;

    int m_node_id;

    bool m_use_ns3;
    bool m_info_set;
    bool m_optimizeForSpeed;
    bool m_debugCamMessages;

    std::string m_vehID;
};

} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_NODE_H_ */
