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

namespace baseapp
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

// TransaidHeader

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const CamInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_camInfo(message)
				{};


		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const DenmInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_denmInfo(message)
				{};

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const CpmInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_cpmInfo(message)
				{};

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const MapInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_mapInfo(message)
				{};

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const McmVehicleInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_mcmVehicleInfo(message)
				{};

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const McmRsuInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_mcmRsuInfo(message)
				{};

		TransaidHeader::TransaidHeader (ProtocolId pid, MessageType msgType, const IviInfo& message):
				m_protocolId(pid), m_messageType(msgType), m_iviInfo(message)
				{};

        uint32_t TransaidHeader::GetSerializedSize(void) const {
        	if (m_messageType == TRANSAID_CAM)  {
        		return 2 + sizeof(m_camInfo);
        	}
        	else if (m_messageType == TRANSAID_DENM)
        	{
        		return 2 + sizeof(m_denmInfo);
        	}
        	else if (m_messageType == TRANSAID_CPM)
        	{
        		return 2 + sizeof(m_cpmInfo);
        	}
        	else if (m_messageType == TRANSAID_MAP)
        	{
        		return 2 + sizeof(m_mapInfo);
        	}
        	else if (m_messageType == TRANSAID_MCM_VEHICLE)
        	{
        		return 2 + sizeof(m_mcmVehicleInfo);
        	}
        	else if (m_messageType == TRANSAID_MCM_RSU)
        	{
        		return 2 + sizeof(m_mcmRsuInfo);
        	}
        	else if (m_messageType == TRANSAID_IVI)
        	{
        		return 2 + sizeof(m_iviInfo);
        	}
        	else {
        		return 2;
        	}
        };

        void TransaidHeader::Print(std::ostream &os) const {
            os << " [TransaidHeader] PId:" << m_protocolId << ", MsgTypeId:" << m_messageType << ", size:" << GetSerializedSize();
        };

        std::string TransaidHeader::Name() const
        {
            return "TransaidHeader";
        };

        MessageType TransaidHeader::getMessageType() const {
            return m_messageType;
        }

        TransaidHeader::CamInfo TransaidHeader::getCamInfo() const {
            return m_camInfo;
        }

        TransaidHeader::DenmInfo TransaidHeader::getDenmInfo() const {
            return m_denmInfo;
        }

        TransaidHeader::CpmInfo TransaidHeader::getCpmInfo() const {
            return m_cpmInfo;
        }

        TransaidHeader::McmRsuInfo TransaidHeader::getMcmRsuInfo() const {
            return m_mcmRsuInfo;
        }

        TransaidHeader::McmVehicleInfo TransaidHeader::getMcmVehicleInfo() const {
            return m_mcmVehicleInfo;
        }

        TransaidHeader::MapInfo TransaidHeader::getMapInfo() const {
             return m_mapInfo;
         }

        TransaidHeader::IviInfo TransaidHeader::getIviInfo() const {
             return m_iviInfo;
         }



// TestHeader
		uint16_t TestHeader::maxResponseTime = 100;

        TestHeader::TestHeader(ProtocolId pid, MessageType msgType, const ResponseInfo& response) :
                        m_protocolId(pid), m_messageType(msgType), m_message(response.message),
                        m_stopEdge(response.stopEdge), m_stopPosition(response.stopPosition)
                    {};

        TestHeader::TestHeader(ProtocolId pid, MessageType msgType, const std::string& message) :
                        m_protocolId(pid), m_messageType(msgType), m_message(message),
                        m_stopEdge(""), m_stopPosition(0)
                    {};

        uint32_t
        TestHeader::GetSerializedSize(void) const {
            return 2 + m_message.size();
        };
        void
        TestHeader::Print(std::ostream &os) const {
            os << " [TestHeader] PId:" << m_protocolId << ", MsgTypeId:" << m_messageType << ", message:" << m_message << ", size:" << GetSerializedSize();
        };
        std::string
        TestHeader::Name() const
        {
            return "TestHeader";
        };
        MessageType
        TestHeader::getMessageType() const {
            return m_messageType;
        }
        std::string
        TestHeader::getMessage() const {
            return m_message;
        }

        std::string
        TestHeader::getStopEdge() const {
            return m_stopEdge;
        }

        double
        TestHeader::getStopPosition() const {
            return m_stopPosition;
        }

        uint16_t
        TestHeader::getMaxResponseTime() const
        {
            return maxResponseTime;
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
