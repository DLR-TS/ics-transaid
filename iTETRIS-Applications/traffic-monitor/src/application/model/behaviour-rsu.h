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
#ifndef BEHAVIOUR_RSU_H_
#define BEHAVIOUR_RSU_H_

#include "behaviour.h"
#include "scheduler.h"
#include "program-configuration.h"
#include <map>

namespace protocol {
namespace application {
struct VehicleDirectionOrdering {
    bool operator()(const VehicleDirection& left, const VehicleDirection& right);
};

static const double BIG_VALUE = 1000 * 1000; //Big arbitrary value
struct FlowStatus {
    FlowStatus(VehicleDirection direction) :
        dir(direction) {
        Reset();
    }
    VehicleDirection dir;
    unsigned quantity;
    double maxDistance;
    double minDistance;
    double avgSpeed;
    double avgTime;
    unsigned numLastMessage;
    void Reset() {
        numLastMessage = avgSpeed = avgTime = quantity = maxDistance = 0;
        minDistance = BIG_VALUE;
    }
};

class CommHeader;
class BeaconResponseHeader;

/**
 * Message exchange behavior installed on a rsu node.
 * It contains the message logic only.
 * The storage of the messages is delegated to the data-manager class
 */
class BehaviourRsu: public Behaviour {
public:
    static bool Enabled;
    static uint16_t TimeBeaconMin;
    static uint16_t TimeBeacon;
    static uint16_t TimeCheck;
    static uint16_t Timeout;

    BehaviourRsu(iCSInterface* controller);
    virtual ~BehaviourRsu();

    /**
     * @brief Called by the ics interface to add the relevant direction
     * @brief that have to be polled to get informations
     */
    void AddDirections(std::vector<Direction>);
    const std::vector<VehicleDirection>& GetDirections() const;
    void Start();
    void Stop();

    virtual bool IsSubscribedTo(ProtocolId pid) const;
    virtual void Receive(server::Payload* payload, double snr);
    virtual bool Execute(DirectionValueMap& data);

    TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR_RSU;
    }

private:
    std::vector<VehicleDirection> m_directions;
    double m_beaconInterval;

    typedef std::map<const VehicleDirection, int, VehicleDirectionOrdering> DirMap;
    typedef std::map<const int, DirMap> TimeoutMap;
    TimeoutMap m_nodeLastSeen;
    /**
     * @brief Called to update the last seen time of a node the a message is received
     */
    void UpdateLastSeen(NodeInfo*);
    /**
     * @brief Called to remove the node if it has send a message with the last message flag set
     * @brief or a no longer conformant message
     */
    void RemoveLastSeen(NodeInfo*);
    /**
     * @brief Periodically check if a node has timed out
     */
    void CheckTimeout();

    //Configuration
    uint16_t m_timeBeaconMin;
    uint16_t m_timeBeacon;
    uint16_t m_timeCheck;
    uint16_t m_timeOut;

    //Events
    event_id m_eventBeacon;
    event_id m_eventCheck;

    void EventBeacon(int position);
    bool m_executeAtThisStep;
    void EventCheck();

    /**
     * @brief Called when a beacon response is received
     */
    void OnBeaconResponse(CommHeader*, BeaconResponseHeader*);
    /**
     * @brief Trace invoked when the rsu has received a beacon response from a node
     */
    TracedCallback<NodeInfo*> m_traceBeaconResponse;
    /**
     * @brief Called when a no longer conformant is received
     */
    void OnNoLongerConformant(CommHeader*, NoLongerConformantHeader*);
    /**
     * @brief Trace invoked when the rsu has received a nolongerconformat message
     */
    TracedCallback<NodeInfo*> m_traceNoLongerConforman;

    //More events
    /**
     * @brief Called when a noded times out
     */
    TracedCallback<NodeInfo*> m_traceTimeOutNode;
    /**
     * @brief Called when the message received from the node is its last one for the current direction
     * @brief Only used in the case IcsInterface::UseSink is true
     */
    TracedCallback<NodeInfo*> m_traceLastMessageNode;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_RSU_H_ */
