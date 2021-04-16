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
 ***************************************************************************************/

#ifndef BEHAVIOUR_FACTORY_H_
#define BEHAVIOUR_FACTORY_H_


/*
 *  DEBUG Defines
 */
//#define DEBUG_TMC

namespace baseapp {
namespace application {

class iCSInterface;
class Node;
class TMCBehaviour;

/**
 * Default factory for the behaviour instances
 */
class BehaviourFactory {
public:
    virtual ~BehaviourFactory() {};
    /**
     * @brief Create one or several new RSU behaviour(s) and add them to the interface
     */
    virtual void createRSUBehaviour(iCSInterface* interface, Node* node) = 0;
    /**
     * @brief Create one or several new node behaviour(s) and add them to the interface
     */
    virtual void createNodeBehaviour(iCSInterface* interface, Node* node) = 0;
    /**
     * @brief Create a new TMC behaviour, the caller is responsible for deletion
     */
    virtual TMCBehaviour* createTMCBehaviour() {
#ifdef DEBUG_TMC
        std::cout << "BehaviourFactory::createTMCBehaviour() is not overriden: No TMC Behaviour will be created." << std::endl;
#endif
        return nullptr;
    }
};

} /* namespace application */
} /* namespace testapp */

#endif /* BEHAVIOUR_FACTORY_H_ */
