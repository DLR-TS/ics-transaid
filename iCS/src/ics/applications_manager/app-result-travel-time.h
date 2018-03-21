/****************************************************************************/
/// @file    app-result-travel-time.h
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
#ifndef APP_RESULT_TRAVEL_TIME_H
#define APP_RESULT_TRAVEL_TIME_H


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

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class TrafficSimulatorCommunicator;
class ITetrisNode;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
* @struct TravelTimeResults
* @brief Structure to store the station and the speed to apply.
*/
typedef struct TravelTimeResults {
    ///@brief Travel time in seconds.
    float m_travelTime;

    ///@brief Rsu to send travel time
    ics_types::stationID_t m_destinationStationId;

    /**
    * @brief True if the value can be applyed in SUMO, false if
    * its simulation in ns-3 is pending.
    */
    ics_types::TrafficApplicationResultMessageState m_messageStatus;

    /// @brief The identifier of the message.
    int m_messageId;

    /// @brief Frequency
    int m_frequency;

    /// @brief Payload length
    int m_payloadLength;

    /// @todo To be commented.
    float m_msgRegenerationTime;
} TTravelTimeResult;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class ResultTravelTime
* @brief Manages the results of the applications relatives to the establishment of the maximum speed.
*/
class ResultTravelTime : public ResultContainer
{
public:

    /**
     * @brief Constructor.
     * @param[in] owner The station the result belongs to.
     * @param[in] appHandlerId The Application Handler the result is attached to.
     */
    ResultTravelTime(ics_types::stationID_t owner, int appHandlerId);

    /**
    * @brief Reads the information relative to the execution of the application.
    * @param[in,out] &storage Data from the application to be processed.
    * @return True: If the operation finishes successfully.
    * @return False: If an error occurs.
    */
    bool ProcessResult(tcpip::Storage& storage);

    /**
     * @brief
     * @param[in] syncManager Object to get access to utility functions.
     * @param[in] appHandler Needed to get information of the application
     * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE
     */
    int ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler);

    /**
    * @brief Provides the travel time estimation of the station.
    * @param[in] nodeId The identifier of the station.
    * @return The travel time estimation.
    * @return -1, if the node identifier is not found.
    */
    float GetTravelTimeEstimation(int nodeId);

    /// @todo TO BE COMMENTED
    int CheckMessage(int appMessageId);

    /// @todo TO BE COMMENTED
    void GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >&);
		///@brief Correct use if polymorphism...
		bool AskSendMessageStatus();

private:

    ///@brief Set of speed and stations to apply.
    TTravelTimeResult m_travelTime;

    ///@brief Average/Global TTE result
    static int m_globalTravelTime;
};

}
#endif
