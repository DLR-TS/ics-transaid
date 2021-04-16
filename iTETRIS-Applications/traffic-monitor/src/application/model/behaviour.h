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

#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include "trace-manager.h"
#include "headers.h"
#include "fatal-error.h"
#include "structs.h"

namespace protocol {
namespace server {
class Payload;
}

namespace application {

class iCSInterface;

/**
 * Abstract behaviour class
 */
class Behaviour: public TraceManager {
public:
    Behaviour(iCSInterface* controller);
    virtual ~Behaviour();

    virtual bool IsActiveOnStart(void) const;
    bool IsRunning() const;
    /**
     * @brief Contains the actions to be executed when the behavior starts
     */
    virtual void Start();
    /**
     * @brief Contains the actions to be executed when the behavior stops
     */
    virtual void Stop();

    /**
     * @brief If a message of the specified pid should be forwarded to the class
     * @param[in] pid the ProtocolId of the message
     * @return true if the class is interested messages of said pid. False otherwise
     */
    virtual bool IsSubscribedTo(ProtocolId pid) const = 0;
    /**
     * @brief Called by the ics-interface if a message is received by the node
     * @brief and its pid is relevant to the behavior
     * @param[in] payload The received message
     * @param[in] snr The snr of the reception from ns3
     */
    virtual void Receive(server::Payload* payload, double snr) = 0;

    /**
     * @brief Called by ics-interface to get data to send back to iCS.
     * @brief If the class does not returns data it has to return false.
     * @param[out] data Data to send back to iCS. The application has to fill the map
     * @return Whatever the application executed. If true data will be sent to iCS. If false data is discarded
     */
    virtual bool Execute(DirectionValueMap& data) = 0;

    virtual TypeBehaviour GetType() const {
        return Type();
    }

    static TypeBehaviour Type() {
        return TYPE_BEHAVIOUR;
    }

protected:
    virtual std::string Log() const;
    iCSInterface* GetController() const;

    bool m_enabled;
private:

    bool m_running;
    iCSInterface* m_controller;

    // trace sources
    TracedCallback<bool> m_traceStartToggle;
};

} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_H_ */
