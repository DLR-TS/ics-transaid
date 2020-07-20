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

#include "program-configuration.h"
#include "application/node.h"
#include "application/model/ics-interface.h"
#include "behaviour-uc-node.h"
#include "behaviour-uc-rsu.h"
#include "behaviour-uc-tmc.h"
#include "behaviour-uc-factory.h"
#include "vehicleManager.h"

using namespace baseapp;
using namespace baseapp::application;

#define USE_NS3

namespace ucapp
{
namespace application
{

//-------------------------------------------------------------------------------------------------------
//start ns3 simulation via application_config_file-scenario.xml (add '--ns3' in executable params)
//-------------------------------------------------------------------------------------------------------
BehaviourUCFactory::BehaviourUCFactory(const bool _use_ns3) : 
use_ns3(_use_ns3) 
{

//or start ns3 simulation via compile
#ifdef USE_NS3
	use_ns3 = true;
#endif

	if (use_ns3)
	{
		std::cout << "Running with ns3 interface" << std::endl;
	}

	VehicleManager::getInstance().create(use_ns3);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
BehaviourUCFactory::~BehaviourUCFactory()
{

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCFactory::createRSUBehaviour(iCSInterface *interface, Node *node)
{
	interface->SubscribeBehaviour(new BehaviourUCRSU(interface, use_ns3));
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void BehaviourUCFactory::createNodeBehaviour(iCSInterface *interface, Node *node)
{
	interface->SubscribeBehaviour(new BehaviourUCNode(interface, use_ns3));
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
TMCBehaviour *BehaviourUCFactory::createTMCBehaviour()
{
	return new BehaviourUCTMC(use_ns3);
}

} /* namespace application */
} /* namespace ucapp */
