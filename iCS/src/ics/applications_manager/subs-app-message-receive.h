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
/****************************************************************************/
/// @file    subs-app-message-send.h
/// @author  Jerome Haerri (EURECOM)
/// @date    December 30rd, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

#ifndef SUBS_APP_MSG_RECEIVE_H_
#define SUBS_APP_MSG_RECEIVE_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "subscription.h"
#include "../../utils/ics/iCStypes.h"
#include "foreign/tcpip/storage.h"
#include "app-message-manager.h"

namespace ics {

// ===========================================================================
// class definitions
// ===========================================================================

class ITetrisNode;
/**
 * @class SubsAppMessageReceive
 * @brief Manages the subscriptions to send application messages from a station.
 */
class SubsAppMessageReceive: public ics::Subscription {
public:

    /**
     * @brief Constructor
     * @param[in] appId ID of the instantiated subscription.
     * @param[in] stationId Station that owns the subscription.
     * @param[in] msg Message reception details (destination, technologies, type, etc...).
     */
    SubsAppMessageReceive(int subId, /* Subscription id */
                          ics_types::stationID_t stationId, /* Station that subscribed */
                          SyncManager* m_syncManager, /* Pointer to the SyncManager */
                          unsigned char* msg, int msgSize /* Message transmission details */
                         );

    /**
     * @brief Destructor
     */
    virtual ~SubsAppMessageReceive();

    /**
     * @brief Deletes the subscription according to the input parameters.
     * @param[in] subscriptions Collection of subscriptions to delete.
     * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE.
     */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

    /**
     * @brief Returns if the message was scheduled successfully.
     * @return TRUE if the message was scheduled, FALSE otherwise.
     */
    bool returnStatus();

    /**
     * @brief Returns the application message type.
     */
    unsigned char getAppMsgType() const;

    /**
     * @brief Returns the preferred technologies.
     */
    unsigned char getPrefTechs() const;

    /**
     * @brief Returns the communication profile.
     */
    unsigned char getCommProfile() const;

    /**
     * @brief Returns the sender id.
     */
    ics_types::stationID_t getSenderId() const;

    /**
     * @brief Returns the message lifetime.
     */
    ics_types::icstime_t getMsgLifetime() const;

    /**
     * @brief Returns the message length.
     */
    short getAppMsgLength() const;

    /**
     * @brief Returns the message sequence number.
     */
    int getMsgSeqNo() const;

    /**
     * @brief Returns message frequency.
     */
    int getFrequency() const;

    /**
     * @brief Returns the message regeneration time.
     */
    float getMsgRegenerationTime() const;

    /**
     * @brief Returns a circle that contains the geocast areas.
     */
    Circle getCircleFromAreas();

    /// @brief Areas for the geocast protocols.
    std::vector<TArea> m_areas;

    /**
     * @brief Called by the Application Handler to know if a subscription has anything to transmit to an Application
     * @return True if an message is waiting. False otherwise
     */
    int InformApp(AppMessageManager* messageManager);

    /**
     * @brief Process Application Messages = cross-check if an application asked for the message
     * @return A pair<int,stationID_t> representing the Application_ID and the Node_ID
     */
    int ProcessReceivedAppMessage(Message& message, SyncManager* syncManager);

    bool getAppMsgReceived() {
        return m_appMsgReceived;
    }

//  After a ProcessReceivedAppMessage this method will tell if the message was added to the received collection.
//  getAppMsgReceived is general, and will be true if the application received at least one message.
    bool getLastMessageAddedToReceived() {
        return m_lastMessageAddedToReceived;
    }

    /// @brief Type of the message according to the application logic.
    unsigned char m_appMsgType;
private:

    /// @brief Message transmission details (destination, technologies, etc...) passed by the application.
    tcpip::Storage m_msg;
    /// @brief Status of the message scheduling process (TRUE if the message was scheduled, FALSE otherwise).
    bool m_schedulingStatus;



    /// @brief Preferred technologies to send the message (each bit represents a technology, according to enum RATID)
    unsigned char m_prefTechs;

    /// @brief Communication profile for the technology selector module.
    unsigned char m_commProfile;

    /// @brief Identifier of the target node(s) of the transmission.
    std::vector<ics_types::stationID_t> m_destId;

    /// @brief Identifier of the source node(s) of the transmission.
    std::vector<ics_types::stationID_t> m_sourceId;

    /// @brief Message lifetime.
    ics_types::icstime_t m_msgLifetime;

    /// @brief Message length in bytes.
    short m_appMsgLength;

    /// @brief Message sequence number according to the application logic.
    int m_msgSeqNo;

    /// @brief Number of messages to be scheduled every time step.
    int m_frequency;

    /// @brief Number of time steps when the messages are scheduled.
    float m_msgRegenerationTime;

    /// @brief communication mode
    messageType_t m_commMode;

    /// @brief communication mode
    short m_numHops;

    bool m_appMsgReceived;

    bool m_lastMessageAddedToReceived;

    std::vector<std::pair<Message, stationID_t> > receivedData;

    /**
     * @brief Reads the transmission information and schedules the message to be sent.
     * @return True if the message was correctly scheduled. False otherwise.
     */
    int pull(SyncManager* syncManager);

};

}

#endif /* SUBS_APP_MSG_RECEIVE_H_ */
