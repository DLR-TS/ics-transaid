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
/// @file    app-result-maximum-speed.h
/// @author  Julen Maneros
/// @date
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
#ifndef APP_RESULT_MAXIMUM_SPEED_H
#define APP_RESULT_MAXIMUM_SPEED_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "app-result-container.h"
#include "../../utils/ics/iCStypes.h"

namespace ics {

// ===========================================================================
// class declarations
// ===========================================================================
class TrafficSimulatorCommunicator;
class ITetrisNode;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
* @struct TSetMaximumSpeed
* @brief Structure to store the station and the speed to apply.
*/
typedef struct SetMaximumSpeedResult {
    ///@brief Speed to apply.
    float m_maximumSpeed;

    ///@brief Node to apply speed.
    ics_types::stationID_t m_nodeId;

    /**
    * @brief True if the value can be applyed in SUMO, false if
    * its simulation in ns-3 is pending.
    */
    ics_types::TrafficApplicationResultMessageState m_messageStatus;

    /// @brief The identifier of the message.
    int m_messageId;

    /// @brief Frequency.
    int m_frequency;

    /// @brief Payload length.
    int m_payloadLength;

    /// @todo To be commented.
    float m_msgRegenerationTime;
} TSetMaximumSpeed;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class ResultSetMaximumSpeed
* @brief Manages the results of the applications relatives to the establishment of the maximum speed.
*/
class ResultSetMaximumSpeed : public ResultContainer {
public:

    /**
    * @brief Constructor.
    * @param[in] owner The station the result belongs to.
    */
    ResultSetMaximumSpeed(ics_types::stationID_t owner, int appHandlerId);

    /**
    * @brief Reads the information relative to the execution of the application.
    * @param[in,out] &storage Data from the application to be processed.
    * @return True: If the operation finishes successfully.
    * @return False: If an error occurs.
    */
    bool ProcessResult(tcpip::Storage& storage);

    /**
    * @brief Sends a message to SUMO to apply the result in the node.
    * @param[in] syncManager Object to get access to utility functions.
    * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE.
    */
    int ApplyResult(SyncManager* syncManager);

    /**
    * @brief Provides the value of the maximum speed of the station.
    * @param[in] nodeId The identifier of the station.
    * @return The speed value.
    * @return -1, if the node identifier is not found.
    */
    float GetMaximumSpeed(int nodeId);

    /// @todo To be commented.
    int CheckMessage(int appMessageId);

    /// @todo To be commented.
    void GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >&);
    ///@brief Correct use if polymorphism...
    bool AskSendMessageStatus();


private:

    ///@brief Set of speed and stations to apply.
    std::vector<TSetMaximumSpeed> m_maximumSpeeds;
};

}
#endif
