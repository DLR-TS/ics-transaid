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
/*
 * TMCBehaviour.h
 *
 *  Created on: Mar 25, 2019
 *      Author: Leonhard Luecken
 */

#ifndef SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_
#define SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_

#include <map>
#include "node-handler.h"

namespace baseapp {

namespace server {
class Payload;
}

namespace application {

class TMCBehaviour : public server::NodeHandler::MessageReceptionListener {
public:
    TMCBehaviour();
    virtual ~TMCBehaviour();

    /// @brief Add a new RSU to be controlled by this TMC, pointer deletion responsibility lies at caller side
    /// @note  On the first RSU registration, some provisions are taken - the first added RSU's controller will serve
    ///        as interface to the generic application functionalities.
    /// @todo: If it is deleted before the TMCBehaviour, the RSU should be removed
    void addRSU(iCSInterface* rsu);

    /// @brief To be called, when a message is received at an RSU
    /// @note  The payload pointer will be deleted externally after this call.
    virtual void ReceiveMessage(int rsuID, server::Payload* payload, double snr, bool mobileNode = false) = 0;

    /// @brief Add a new RSU to be controlled by this TMC
    /// @brief Execute() is called once per simulation step, when the last RSU has been executed.
    ///        The TMC is allowed to use the rsu's interface for issuing of general subscriptions.
    virtual void Execute() = 0;

    /// @brief OnAddSubscriptions() is called once per simulation step, when the first RSU gets
    ///        its turn to issue iCS subscriptions. The TMC is allowed to use the rsu's interface for general
    ///        subscriptions.
    virtual void OnAddSubscriptions() = 0;

protected:

    /// @brief RSU interfaces controllable by the TMC
    std::map<int, iCSInterface*> m_RSUController;

    /// @brief Controller (iCSInterface) of an arbitrary RSU (currently the first RSU added to the TMC).
    /// @note Should be used for generic interactions with the application/iCS
    /// @todo Handle rsu-deletion
    iCSInterface* iface;
};

} /* namespace application */
} /* namespace baseapp */

#endif /* SRC_APPLICATION_MODEL_TMCBEHAVIOUR_H_ */
