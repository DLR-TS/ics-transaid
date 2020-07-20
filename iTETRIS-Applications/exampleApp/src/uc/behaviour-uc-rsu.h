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
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#ifndef BEHAVIOUR_UC_RSU_H_
#define BEHAVIOUR_UC_RSU_H_

#include "application/model/behaviour-rsu.h"
#include "application/model/headers.h"
#include "application/model/common.h"
#include "application/helper/scheduler.h"
#include "structs.h"

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp
{
namespace application
{
class BehaviourUCRSU : public BehaviourRsu
{
	struct denm_message_t
	{
		double frequency;
		double startingPoint; 	//downward ToC
		double endPoint;		//upward ToC
		event_id eventId;
		DenmType type;

		bool initialize;
		bool highlight;
		bool enable;
	};

	struct rsu_request_t
    {
        bool unicast;
		bool geobroadcast;
		rsu_request_t() : unicast(false), geobroadcast(false)
        {
		}
    };

public:
	BehaviourUCRSU(iCSInterface *controller, const bool use_ns3);
	~BehaviourUCRSU();

	void Start();
	void OnAddSubscriptions();
	bool IsSubscribedTo(ProtocolId pid) const;
	void Receive(server::Payload *payload, double snr);
	bool Execute(DirectionValueMap &data);
	void processCAMmessagesReceived(const int nodeID, const std::vector<CAMdata> &receivedCAMmessages);

	TypeBehaviour GetType() const
	{
		return Type();
	}

	static TypeBehaviour Type()
	{
		// in transaid/iTETRIS-Applications/baseApp/src/application/model/common.h
		return TYPE_BEHAVIOUR_UC4_RSU;
	}

private:
	void handle_CAM_msg(TransaidHeader* receivedHeader);

	void sendDENMInfoRepeated();
	void sendDENMInfo();

	denm_message_t denm_message;
	rsu_request_t rsu_request;

	bool m_use_ns3;

	bool m_optimizeForSpeed;
	bool m_debugCamMessages;
    bool m_debugDenmMessages;

	std::string rsuSumoName;
	std::string rsuWithId;
};

} /* namespace application */
} /* namespace ucapp */

#endif /* BEHAVIOUR_UC_RSU_H_ */
