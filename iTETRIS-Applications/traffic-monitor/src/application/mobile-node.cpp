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

#include "subscription-helper.h"
#include "mobile-node.h"
#include "node-sampler.h"
#include <sstream>
#include "model/ics-interface.h"

namespace protocol
{

	namespace application
	{

		MobileNode::MobileNode(int id) :
				Node(id)
		{
			selectNodeType();
			m_position = new MobilityInfo(id);
			init();
		}

		MobileNode::MobileNode(MobilityInfo* info) :
				Node(info->id)
		{
			selectNodeType();
			m_position = NULL;
			init();
			updateMobilityInformation(info);
		}

		MobileNode::MobileNode(const int nodeId, const int ns3NodeId, const std::string & sumoNodeId,
				const std::string & sumoType, const std::string & sumoClass) :
				Node(nodeId)
		{
			m_ns3Id = ns3NodeId;
			m_sumoId = sumoNodeId;
			m_sumoType = sumoType;
			m_sumoClass = sumoClass;
			selectNodeType();
			m_position = new MobilityInfo(nodeId);
			init();
		}

		MobileNode::~MobileNode()
		{
			delete m_position;
		}

		void MobileNode::updateMobilityInformation(MobilityInfo * info)
		{
			if (m_controller != NULL)
				m_controller->GetNodeSampler()->UpdatePosition(info);
			delete m_position;
			m_position = info;
		}

		Vector2D MobileNode::getPosition()
		{
			return m_position->position;
		}

		Vector2D MobileNode::getVelocity()
		{
			double speed_x = (double) m_position->speed * cos((double) m_position->direction * M_PI / 180.0);
			double speed_y = (double) m_position->speed * sin((double) m_position->direction * M_PI / 180.0);
			return Vector2D(speed_x, speed_y);
		}

		double MobileNode::getDirection()
		{
			return m_position->direction;
		}

		void MobileNode::selectNodeType()
		{
//			TODO Add appropriate type according the type from sumo. Eg
//			if(m_type == "special")
//				m_type = NT_SPECIAL;
//			else if (mt_type == "police")
//				m_type = NT_POLICE;
//			else
			m_type = GetRandomNodeType();
			std::ostringstream oss;
			oss << "Selected class for node " << m_id << " is " << (int) m_type;
			Log::WriteLog(oss);
		}

	} /* namespace application */
} /* namespace protocol */
