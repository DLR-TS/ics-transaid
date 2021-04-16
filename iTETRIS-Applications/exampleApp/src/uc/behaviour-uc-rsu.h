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

#ifndef BEHAVIOUR_UC_RSU_H_
#define BEHAVIOUR_UC_RSU_H_

#include "application/model/behaviour-rsu.h"
#include "application/model/headers.h"
#include "application/model/common.h"
#include "application/helper/scheduler.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {
class BehaviourUCRSU : public BehaviourRsu {
    struct denm_message_t {
        double frequency;
        double startingPoint; 	//downward ToC
        double endPoint;		//upward ToC
        event_id eventId;
        DenmType type;

        bool initialize;
        bool highlight;
        bool enable;
    };

    struct rsu_request_t {
        bool unicast;
        bool geobroadcast;
        rsu_request_t() : unicast(false), geobroadcast(false) {
        }
    };

public:
    BehaviourUCRSU(iCSInterface* controller, const bool use_ns3);
    ~BehaviourUCRSU();

    void Start();
    void OnAddSubscriptions();
    bool IsSubscribedTo(ProtocolId pid) const;
    void Receive(server::Payload* payload, double snr);
    bool Execute(DirectionValueMap& data);
    void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata>& receivedCAMmessages);

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        // in transaid/iTETRIS-Applications/baseApp/src/application/model/common.h
        return TYPE_BEHAVIOUR_UC4_RSU;
    }

private:
    void handle_CAM_msg(TransaidHeader* receivedHeader);

    void sendDENMInfoRepeated();
    void sendDENMInfo();

    denm_message_t denm_message;
    rsu_request_t rsu_request;

    bool m_use_ns3;

    bool m_optimizeForSpeed;
    bool m_debugCamMessages;
    bool m_debugDenmMessages;

    std::string rsuSumoName;
    std::string rsuWithId;
};

} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_RSU_H_ */
