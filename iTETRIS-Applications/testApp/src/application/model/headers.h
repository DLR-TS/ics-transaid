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
#ifndef HEADER_H_
#define HEADER_H_

#include "vector.h"
#include "common.h"
#include <stdint.h>

namespace testapp
{
	namespace application
	{
		/**
		 * Abstract header class. Extended to create the messages payloads
		 */
		class Header
		{
			public:
				virtual ~Header()
				{
				}
				virtual void Print(std::ostream &os) const=0;
				virtual uint32_t GetSerializedSize(void) const=0;
				virtual std::string Name() const = 0;
				virtual MessageType getMessageType() const = 0;
		};

		std::string PrintHeader(Header &header);
		std::string PrintHeader(Header *header);


        /**
         * Minimal header. Used by test application
         */
        class TestHeader: public Header {
        public:
            TestHeader(ProtocolId pid, MessageType msgType, std::string testMsg);

            uint32_t GetSerializedSize(void) const;
            void Print(std::ostream &os) const;
            std::string Name() const;
            MessageType getMessageType() const;
            std::string getMessage() const;
            uint16_t getMaxResponseTime() const;

        private:
            ProtocolId m_protocolId;
            MessageType m_messageType;
            std::string m_message;

            /// @brief Maximal delay at which a response to a received message is scheduled (in ms)
            /// (The specific values are sampled uniformly in [0, maxResponseTime]), @see BehaviourTestNode::Receive()
            static uint16_t maxResponseTime;
		};


		/**
		 * Common header always sent in every message.
		 * It contains some information about the source node.
		 * It's added by the class ics-interface to every message sent.
		 */
		class CommHeader: public Header
		{
			public:
				CommHeader();

				uint32_t GetSerializedSize(void) const;
				void Print(std::ostream &os) const ;
				std::string Name() const
				{
					return "CommHeader";
				}

				int getDestinationId() const
				{
					return m_destinationId;
				}

				void setDestinationId(int destinationId)
				{
					m_destinationId = destinationId;
				}

				NodeType getDestinationType() const
				{
					return m_destinationType;
				}

				void setDestinationType(NodeType destinationType)
				{
					m_destinationType = destinationType;
				}

				ProtocolId getProtocolId() const
				{
					return m_protocolId;
				}

				void setProtocolId(ProtocolId messageType)
				{
					m_protocolId = messageType;
				}

				int getSourceId() const
				{
					return m_sourceId;
				}

				void setSourceId(int sourceId)
				{
					m_sourceId = sourceId;
				}

				const Vector2D& getSourcePosition() const
				{
					return m_sourcePosition;
				}

				void setSourcePosition(const Vector2D& sourcePosition)
				{
					m_sourcePosition = sourcePosition;
				}

				NodeType getSourceType() const
				{
					return m_sourceType;
				}

				void setSourceType(NodeType sourceType)
				{
					m_sourceType = sourceType;
				}

				MessageType getMessageType() const
				{
					return m_messageType;
				}

				void setMessageType(MessageType messageType)
				{
					m_messageType = messageType;
				}

			private:
				static const int SERIALIZED_SIZE = 4 + 2 * sizeof(int) + sizeof(Vector2D);

				int m_sourceId, m_destinationId;
				Vector2D m_sourcePosition;
				NodeType m_sourceType, m_destinationType;
				ProtocolId m_protocolId;
				MessageType m_messageType;
		};

		/**
		 * Message periodically sent by the RSU node to query the nodes in a particular direction
		 */
		class BeaconHeader: public Header
		{
			public:

				BeaconHeader();

				uint32_t GetSerializedSize(void) const;
				void Print(std::ostream &os) const;
				std::string Name() const
				{
					return "BeaconHeader";
				}

				double getDirection() const
				{
					return m_direction;
				}

				void setDirection(double direction)
				{
					m_direction = direction;
				}

				uint16_t getMaxResponseTime() const
				{
					return m_maxResponseTime;
				}

				void setMaxResponseTime(uint16_t maxResponseTime)
				{
					m_maxResponseTime = maxResponseTime;
				}

				VehicleMovement getVehicleMovement() const
				{
					return m_vehicleMovement;
				}

				void setVehicleMovement(VehicleMovement vehicleMovement)
				{
					m_vehicleMovement = vehicleMovement;
				}

				MessageType getMessageType() const
				{
					return MT_RSU_BEACON;
				}

			private:
				static const int SERIALIZED_SIZE = 2 + 1 + sizeof(double);

				double m_direction;
				VehicleMovement m_vehicleMovement;
				uint16_t m_maxResponseTime;
		};

		/**
		 * Reply to a beacon message. Sent by a node, it contains some further information
		 * complementing the CommHeader data.
		 */
		class BeaconResponseHeader: public Header
		{
			public:
				BeaconResponseHeader();
				uint32_t GetSerializedSize(void) const;
				void Print(std::ostream &os) const;
				std::string Name() const
				{
					return "BeaconResponseHeader";
				}

				double getAvgSpeedHigh() const
				{
					return m_avgSpeedHigh;
				}

				void setAvgSpeedHigh(double avgSpeedHigh)
				{
					m_avgSpeedHigh = avgSpeedHigh;
				}

				double getAvgSpeedLow() const
				{
					return m_avgSpeedLow;
				}

				void setAvgSpeedLow(double avgSpeedLow)
				{
					m_avgSpeedLow = avgSpeedLow;
				}

				double getCurrentSpeed() const
				{
					return m_currentSpeed;
				}

				void setCurrentSpeed(double currentSpeed)
				{
					m_currentSpeed = currentSpeed;
				}

				double getSourceDirection() const
				{
					return m_sourceDirection;
				}

				void setSourceDirection(double sourceDirection)
				{
					m_sourceDirection = sourceDirection;
				}

				double getConformantDirection() const
				{
					return m_conformantDirection;
				}

				void setConformantDirection(double conformantDirection)
				{
					m_conformantDirection = conformantDirection;
				}

				bool getLastMessage() const
				{
					return m_lastMessage;
				}

				void setLastMessage(bool lastMessage)
				{
					m_lastMessage = lastMessage;
				}

				VehicleMovement getVehicleMovement() const
				{
					return m_vehicleMovement;
				}

				void setVehicleMovement(VehicleMovement vehicleMovement)
				{
					m_vehicleMovement = vehicleMovement;
				}

				MessageType getMessageType() const
				{
					return MT_BEACON_RESPONSE;
				}

			private:
				static const int SERIALIZED_SIZE = 2 + 5 * sizeof(double);

				double m_sourceDirection;
				double m_conformantDirection;
				double m_currentSpeed;
				double m_avgSpeedLow;
				double m_avgSpeedHigh;
				VehicleMovement m_vehicleMovement;
				bool m_lastMessage;
		};

		/**
		 * Message sent by a node when it is no longer traveling in the previous direction.
		 * Used only if the flag UseSink in ics-interface is set to false
		 */
		class NoLongerConformantHeader: public Header
		{
			public:
				NoLongerConformantHeader();
				uint32_t GetSerializedSize(void) const;
				void Print(std::ostream &os) const;
				std::string Name() const
				{
					return "NoLongerConformantHeader";
				}
				double getSourceDirection() const
				{
					return m_sourceDirection;
				}

				void setSourceDirection(double sourceDirection)
				{
					m_sourceDirection = sourceDirection;
				}

				double getConformantDirection() const
				{
					return m_conformantDirection;
				}

				void setConformantDirection(double conformantDirection)
				{
					m_conformantDirection = conformantDirection;
				}
				VehicleMovement getVehicleMovement() const
				{
					return m_vehicleMovement;
				}
				void setVehicleMovement(VehicleMovement vehicleMovement)
				{
					m_vehicleMovement = vehicleMovement;
				}
				MessageType getMessageType() const
				{
					return MT_NO_LONGHER_CONFORMANT;
				}
			private:
				static const int SERIALIZED_SIZE = 1 + 2 * sizeof(double);

				double m_conformantDirection;
				double m_sourceDirection;
				VehicleMovement m_vehicleMovement;
		};
	} /* namespace application */
} /* namespace protocol */

#endif /* HEADER_H_ */
