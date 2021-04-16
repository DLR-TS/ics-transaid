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

#ifndef BEHAVIOUR_UC_TMC_H_
#define BEHAVIOUR_UC_TMC_H_

#include "application/model/TMCBehaviour.h"
#include "application/helper/scheduler.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {
/**
 * Behaviour for rsu in uc cases. Inherits from BehaviourNode to have the random response offset variables at hand.
*/
class BehaviourUCTMC : public TMCBehaviour {
public:
    BehaviourUCTMC(const bool use_ns3);
    virtual ~BehaviourUCTMC();

    /// @brief To be called when a message is received by an RSU
    /// @note The payload pointer will be deleted externally after this call.
    void ReceiveMessage(int rsuID, server::Payload* payload, double snr, bool mobileNode = false) override;

    void Execute();

    /// @brief OnAddSubscriptions() is called once per simulation step, when the first RSU gets
    ///        its turn to issue iCS subscriptions. The TMC is allowed to use the rsu's interface for general
    ///        subscriptions.
    virtual void OnAddSubscriptions();

private:
    bool m_initialize;
    bool m_use_ns3;
    bool m_useMobilityInfo;
};

} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_TMC_H_ */
