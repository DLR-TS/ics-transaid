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

#ifndef BEHAVIOUR_NODE_H_
#define BEHAVIOUR_NODE_H_

#include "behaviour.h"
#include "scheduler.h"
#include "random-variable.h"
#include <map>
#include "structs.h"

namespace baseapp {
namespace application {

struct RSU {
    RSU() {
        id = 0;
        muted = false;
    }
    RSU(int rsuId, VehicleDirection conformantDirection) {
        id = rsuId;
        dir = conformantDirection;
        muted = false;
    }
    int id;
    bool muted;
    VehicleDirection dir;
};

/**
 * Base class implementing some base functionality for the children classes
 */
class BehaviourNode: public Behaviour {
public:
    static bool Enabled;
    static double SinkThreshold;

    BehaviourNode(iCSInterface* controller);
    virtual ~BehaviourNode();

    virtual void Start();
    void Stop();

    virtual bool IsSubscribedTo(ProtocolId pid) const;
    virtual void Receive(server::Payload* payload, double snr);
    virtual bool Execute(DirectionValueMap& data);

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR_NODE;
    }

protected:
    ns3::UniformVariable m_rnd;

    TracedCallback<NodeInfo&> m_traceSendData;
    //Configuration
    uint16_t m_responseTimeSpacing;

    // Events
    event_id m_eventResponse;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_NODE_H_ */
