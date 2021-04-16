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
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/
#ifndef HEADER_H_
#define HEADER_H_

#include "vector.h"
#include "common.h"
#include <stdint.h>

namespace protocol {
namespace application {
/**
 * Abstract header class. Extended to create the messages payloads
 */
class Header {
public:
    virtual ~Header() {
    }
    virtual void Print(std::ostream& os) const = 0;
    virtual uint32_t GetSerializedSize(void) const = 0;
    virtual std::string Name() const = 0;
    virtual MessageType getMessageType() const = 0;
};

std::string PrintHeader(Header& header);
std::string PrintHeader(Header* header);

/**
 * Common header always sent in every message.
 * It contains some information about the source node.
 * It's added by the class ics-interface to every message sent.
 */
class CommHeader: public Header {
public:
    CommHeader();

    uint32_t GetSerializedSize(void) const;
    void Print(std::ostream& os) const;
    std::string Name() const {
        return "CommHeader";
    }

    int getDestinationId() const {
        return m_destinationId;
    }

    void setDestinationId(int destinationId) {
        m_destinationId = destinationId;
    }

    NodeType getDestinationType() const {
        return m_destinationType;
    }

    void setDestinationType(NodeType destinationType) {
        m_destinationType = destinationType;
    }

    ProtocolId getProtocolId() const {
        return m_protocolId;
    }

    void setProtocolId(ProtocolId messageType) {
        m_protocolId = messageType;
    }

    int getSourceId() const {
        return m_sourceId;
    }

    void setSourceId(int sourceId) {
        m_sourceId = sourceId;
    }

    const Vector2D& getSourcePosition() const {
        return m_sourcePosition;
    }

    void setSourcePosition(const Vector2D& sourcePosition) {
        m_sourcePosition = sourcePosition;
    }

    NodeType getSourceType() const {
        return m_sourceType;
    }

    void setSourceType(NodeType sourceType) {
        m_sourceType = sourceType;
    }

    MessageType getMessageType() const {
        return m_messageType;
    }

    void setMessageType(MessageType messageType) {
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
class BeaconHeader: public Header {
public:

    BeaconHeader();

    uint32_t GetSerializedSize(void) const;
    void Print(std::ostream& os) const;
    std::string Name() const {
        return "BeaconHeader";
    }

    double getDirection() const {
        return m_direction;
    }

    void setDirection(double direction) {
        m_direction = direction;
    }

    uint16_t getMaxResponseTime() const {
        return m_maxResponseTime;
    }

    void setMaxResponseTime(uint16_t maxResponseTime) {
        m_maxResponseTime = maxResponseTime;
    }

    VehicleMovement getVehicleMovement() const {
        return m_vehicleMovement;
    }

    void setVehicleMovement(VehicleMovement vehicleMovement) {
        m_vehicleMovement = vehicleMovement;
    }

    MessageType getMessageType() const {
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
class BeaconResponseHeader: public Header {
public:
    BeaconResponseHeader();
    uint32_t GetSerializedSize(void) const;
    void Print(std::ostream& os) const;
    std::string Name() const {
        return "BeaconResponseHeader";
    }

    double getAvgSpeedHigh() const {
        return m_avgSpeedHigh;
    }

    void setAvgSpeedHigh(double avgSpeedHigh) {
        m_avgSpeedHigh = avgSpeedHigh;
    }

    double getAvgSpeedLow() const {
        return m_avgSpeedLow;
    }

    void setAvgSpeedLow(double avgSpeedLow) {
        m_avgSpeedLow = avgSpeedLow;
    }

    double getCurrentSpeed() const {
        return m_currentSpeed;
    }

    void setCurrentSpeed(double currentSpeed) {
        m_currentSpeed = currentSpeed;
    }

    double getSourceDirection() const {
        return m_sourceDirection;
    }

    void setSourceDirection(double sourceDirection) {
        m_sourceDirection = sourceDirection;
    }

    double getConformantDirection() const {
        return m_conformantDirection;
    }

    void setConformantDirection(double conformantDirection) {
        m_conformantDirection = conformantDirection;
    }

    bool getLastMessage() const {
        return m_lastMessage;
    }

    void setLastMessage(bool lastMessage) {
        m_lastMessage = lastMessage;
    }

    VehicleMovement getVehicleMovement() const {
        return m_vehicleMovement;
    }

    void setVehicleMovement(VehicleMovement vehicleMovement) {
        m_vehicleMovement = vehicleMovement;
    }

    MessageType getMessageType() const {
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
class NoLongerConformantHeader: public Header {
public:
    NoLongerConformantHeader();
    uint32_t GetSerializedSize(void) const;
    void Print(std::ostream& os) const;
    std::string Name() const {
        return "NoLongerConformantHeader";
    }
    double getSourceDirection() const {
        return m_sourceDirection;
    }

    void setSourceDirection(double sourceDirection) {
        m_sourceDirection = sourceDirection;
    }

    double getConformantDirection() const {
        return m_conformantDirection;
    }

    void setConformantDirection(double conformantDirection) {
        m_conformantDirection = conformantDirection;
    }
    VehicleMovement getVehicleMovement() const {
        return m_vehicleMovement;
    }
    void setVehicleMovement(VehicleMovement vehicleMovement) {
        m_vehicleMovement = vehicleMovement;
    }
    MessageType getMessageType() const {
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
