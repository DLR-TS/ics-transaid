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
 * Copyright (c) 205 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-20-8, grant agreement no. 38622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * . Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Michael Behrisch
 ***************************************************************************************/

#ifndef BEHAVIOUR_UC_FACTORY_H_
#define BEHAVIOUR_UC_FACTORY_H_

#include "application/model/behaviour-factory.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {
/**
* Factory for the behaviour uc instances
*/
class BehaviourUCFactory : public BehaviourFactory {
public:
    BehaviourUCFactory(const bool _use_ns3);

    virtual ~BehaviourUCFactory();
    /**
    		 * @brief Create one or several new RSU behaviour(s) and add them to the interface
    		 */
    virtual void createRSUBehaviour(iCSInterface* interface, Node* node);
    /**
    		 * @brief Create one or several new node behaviour(s) and add them to the interface
    		 */
    virtual void createNodeBehaviour(iCSInterface* interface, Node* node);

    /**
    		 * @brief Create a new TMC behaviour, the caller is responsible for deletion
    		 */
    virtual TMCBehaviour* createTMCBehaviour();
private:
    bool use_ns3;

};

} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_FACTORY_H_ */
