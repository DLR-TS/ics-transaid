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

#ifndef BEHAVIOUR_SPEED_RSU_H_
#define BEHAVIOUR_SPEED_RSU_H_

#include "behaviour-rsu.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {

/**
 * Installed on the nodes if IcsInterface::UseSink is set to true
 * It uses a threshold to communicate to the rsu that it has reached the center of the
 * intersection and that the current message will be its last one the current direction
 */
class BehaviourSpeedRSU: public BehaviourRsu {
public:
    BehaviourSpeedRSU(iCSInterface* controller);
    virtual ~BehaviourSpeedRSU();
    void Start();

};
} /* namespace application */
} /* namespace protocol */

#endif /* BEHAVIOUR_SPEED_RSU_H_ */
