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

#include <cstdlib>

#include "app-result-container.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/log/ics-log.h"
#include "app-result-maximum-speed.h"
#include "app-result-travel-time.h"
#include "app-result-open-buslanes.h"
#include "app-result-traffic-jam-detection.h"
#include "app-result-void.h"
#include "app-result-generic.h"

namespace ics
{

	ResultContainer* ResultContainer::CreateResultContainer(int type, ics_types::stationID_t nodeId, int handlerId)
	{
		switch (type)
		{
		case OUTPUT_SET_SPEED_ADVICE_DEMO:
		case OUTPUT_SET_VEHICLE_MAX_SPEED:
			return new ResultSetMaximumSpeed(nodeId, handlerId);
		case OUTPUT_TRAVEL_TIME_ESTIMATION:
			return new ResultTravelTime(nodeId, handlerId);
		case OUTPUT_TRAFFIC_JAM_DETECTION:
			return new ResultTrafficJamDetection(nodeId, handlerId);
		case OUTPUT_OPEN_BUSLANES:
			return new ResultOpenBuslanes(nodeId, handlerId);
		case OUTPUT_VOID:
			return new ResultVoid(nodeId, handlerId);
		case OUTPUT_GENERIC:
			return new ResultGeneric(nodeId, handlerId);
		default:
			IcsLog::LogLevel("iCS --> Result type is not registered. Please contact Application scientist. ", kLogLevelError);
			return NULL;
		}
	}

	int ResultContainer::CheckMessage(int appMessageId)
	{
		return EXIT_FAILURE;
	}
	int ResultContainer::CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager)
	{
		return EXIT_FAILURE;
	}

	void ResultContainer::GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >& v)
	{
	}
	bool ResultContainer::AskSendMessageStatus()
	{
		return false;
	}

}
;
