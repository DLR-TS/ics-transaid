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
 * Author Michael Behrisch
 * University of Bologna
 ***************************************************************************************/

#ifndef BEHAVIOUR_SPEED_FACTORY_H_
#define BEHAVIOUR_SPEED_FACTORY_H_

#include "behaviour-factory.h"

using namespace baseapp;
using namespace baseapp::application;

namespace protocolspeedapp {
namespace application {
/**
 * Factory for the behaviour test instances
 */
class BehaviourSpeedFactory : public BehaviourFactory {
public:
    /**
     * @brief Create one or several new RSU behaviour(s) and add them to the interface
     */
    virtual void createRSUBehaviour(iCSInterface* interface, Node* node);
    /**
     * @brief Create one or several new node behaviour(s) and add them to the interface
     */
    virtual void createNodeBehaviour(iCSInterface* interface, Node* node);
};

} /* namespace application */
} /* namespace testapp */

#endif /* BEHAVIOUR_SPEED_FACTORY_H_ */
