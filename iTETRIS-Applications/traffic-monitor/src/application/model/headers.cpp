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
#include "headers.h"
#include <sstream>
#include "behaviour-rsu.h"

namespace protocol
{
	namespace application
	{

		std::string PrintHeader(Header *header)
		{
			return PrintHeader(*header);
		}

		std::string PrintHeader(Header &header)
		{
			std::ostringstream outstr;
			header.Print(outstr);
			return outstr.str();
		}

///CommHeader

		CommHeader::CommHeader()
		{
			m_sourceType = m_destinationType = NT_ALL;
			m_sourceId = m_destinationId = ID_ALL;
			m_sourcePosition = Vector2D();
			m_protocolId = PID_SPEED;
		}

		uint32_t CommHeader::GetSerializedSize(void) const
		{
			return SERIALIZED_SIZE;
		}

		void CommHeader::Print(std::ostream &os) const
		{
			os << "Pid=" << ToString(m_protocolId);
			os << " SRC:";
			if (m_sourceType == NT_RSU)
				os << " Rsu";
			else if (IsVehicle(m_sourceType))
				os << " Vehicle";
			else
				os << " ???";
			os << " Id=" << m_sourceId << " Pos=(" << m_sourcePosition << ")";
			os << " DEST:";
			if (m_destinationType == NT_RSU)
				os << " Rsu";
			else if (m_destinationType == NT_ALL)
				os << " All";
			else if (IsVehicle(m_destinationType))
				os << " Vehicle";
			else
				os << " ???";
			if (m_destinationId == ID_ALL)
				os << " Id=All";
			else
				os << " Id=" << m_destinationId;
		}

///BeaconHeader
		BeaconHeader::BeaconHeader()
		{
			m_maxResponseTime = BehaviourRsu::TimeBeaconMin;
			m_direction = DIR_INVALID;
		}

		uint32_t BeaconHeader::GetSerializedSize(void) const
		{
			return SERIALIZED_SIZE;
		}

		void BeaconHeader::Print(std::ostream &os) const
		{
			os << "Beacon dir=" << m_direction << " maxResp=" << m_maxResponseTime;
		}

///BeaconHeader
		BeaconResponseHeader::BeaconResponseHeader()
		{
			m_sourceDirection = m_conformantDirection = DIR_INVALID;
			m_currentSpeed = m_avgSpeedLow = m_avgSpeedHigh = SPD_INVALID;
			m_lastMessage = false;
		}

		uint32_t BeaconResponseHeader::GetSerializedSize(void) const
		{
			return SERIALIZED_SIZE;
		}

		void BeaconResponseHeader::Print(std::ostream &os) const
		{
			os << "dir=" << m_sourceDirection << " curSPD=" << m_currentSpeed << " avgSPDS=" << m_avgSpeedLow << " avgSPDH="
					<< m_avgSpeedHigh << (m_lastMessage ? " last message." : "");
		}

///BeaconHeader
		NoLongerConformantHeader::NoLongerConformantHeader()
		{
			m_sourceDirection = m_conformantDirection = DIR_INVALID;
		}

		uint32_t NoLongerConformantHeader::GetSerializedSize(void) const
		{
			return SERIALIZED_SIZE;
		}

		void NoLongerConformantHeader::Print(std::ostream &os) const
		{
			os << "dir=" << m_sourceDirection;
		}

	} /* namespace application */
} /* namespace protocol */
