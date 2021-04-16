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
 * TMCBehaviourTest.h
 *
 *  Created on: Mar 26, 2019
 *      Author: Leonhard Luecken
 */


#ifndef SRC_TEST_TMCBEHAVIOURTEST_H_
#define SRC_TEST_TMCBEHAVIOURTEST_H_

#include "application/model/TMCBehaviour.h"

namespace baseapp {
namespace application {
class iCSInterface;
}
}

using namespace baseapp;
using namespace baseapp::application;

namespace testapp {
namespace application {

class TMCBehaviourTest : public TMCBehaviour {
public:
    TMCBehaviourTest();

    virtual ~TMCBehaviourTest();

    /// @brief To be called, when a message is received at an RSU
    /// @note  The payload pointer will be deleted externally after this call.
    void ReceiveMessage(int rsuID, server::Payload* payload, double snr, bool mobileNode = false);

    /// @brief Add a new RSU to be controlled by this TMC
    void Execute();

    /// @brief Add a new RSU to be controlled by this TMC
    void OnAddSubscriptions();

private:

    bool isActive();

};

} /* namespace application */
} /* namespace testapp */

#endif /* SRC_TEST_TMCBEHAVIOURTEST_H_ */
