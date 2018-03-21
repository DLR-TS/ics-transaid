/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
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
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#include "subs-sumo-traci-command.h"
#include "app-message-manager.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics
{
	SubsSumoTraciCommand::SubsSumoTraciCommand(int appId, ics_types::stationID_t stationId, unsigned char* msg,
			int msgSize) :
			Subscription(stationId)
	{
		m_id = ++m_subscriptionCounter;
		m_name = "EXECUTES A TRACI COMMAND IN SUMO";
		m_appId = appId;
		tcpip::Storage message(msg, msgSize);
		m_executionId = message.readInt();
		ostringstream oss;
		oss << "SubsSumoTraciCommand id " << m_id << " executionId " << m_executionId;
		IcsLog::LogLevel(oss.str().c_str(), kLogLevelInfo);
		while (message.valid_pos())
			m_request.writeChar(message.readChar());
//		One shot subscription
		ExecuteCommand();
	}

	SubsSumoTraciCommand::~SubsSumoTraciCommand()
	{
	}

	int SubsSumoTraciCommand::InformApp(AppMessageManager* messageManager)
	{
//		Move it here for recurring subscription.
//		ExecuteCommand();
		return messageManager->CommandSendSubscriptionSumoTraciCommand(m_nodeId, m_id, m_executionId, m_result);
	}

	bool SubsSumoTraciCommand::ExecuteCommand()
	{
//		Uncomment if used as recurring subscription.
//		m_result.reset();
		IcsLog::LogLevel("SubsSumoTraciCommand::ExecuteCommand", kLogLevelInfo);
		return SyncManager::m_trafficSimCommunicator->TraciCommand(m_request, m_result) == EXIT_SUCCESS;
	}

} /* namespace ics */
