/****************************************************************************/
/// @file    app-result-traffic-jam-detection.h
/// @author  Ramon Bauza
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
#ifndef APP_RESULT_TRAFFIC_JAM_DETECTION_H
#define APP_RESULT_TRAFFIC_JAM_DETECTION_H


// ===========================================================================
// included modules
// ===========================================================================
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
* @struct TCteMessage
* @brief Structure to store the CTE messages
*/
typedef struct SetCteMessage {

    /// @brief ID of the node sending the CTE message
    ics_types::stationID_t m_nodeId;

    /// @todo To be commented
    ics_types::TrafficApplicationResultMessageState m_messageStatus;

    /// @brief The identifier of the message.
    int m_messageId;

    /// @brief Frequency
    int m_frequency;

    /// @brief Payload length
    int m_payloadLength;

    /// @todo To be commented
    float m_msgRegenerationTime;

    /// @todo To be commented
    int m_schedulingTime;

    /// @todo To be commented
    std::vector<int> m_listOfReceiverIds;
} TCteMessage;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class ResultTrafficJamDetection
* @brief Manages the results of the applications relatives to the establishment of the maximum speed
*/
class ResultTrafficJamDetection : public ResultContainer
{
public:

    /**
     * @brief Constructor
     * @param[in] owner The station the result belongs to.
     */
    ResultTrafficJamDetection(ics_types::stationID_t owner, int appHandlerId);

    /**
    * @brief Reads the information relative to the execution of the application
    * @param[in,out] &storage Data from the application to be processed
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool ProcessResult(tcpip::Storage& storage);

    /**
     * @brief Sends a message to SUMO to apply the result in the node.
     * @param[in] syncManager Object to get access to utility functions.
     * @param[in] appHandler Needed to get information of the application
     * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE
     */
    int ApplyResult(SyncManager* syncManager);

    /// @todo TO BE COMMENTED
    int CheckMessage(int appMessageId, ics_types::stationID_t receiverId);
    int CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager);

    /// @todo TO BE COMMENTED
    void GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >&);
		///@brief Correct use if polymorphism...
		bool AskSendMessageStatus();


private:

    ///@brief Sent CTE messages
    std::vector<TCteMessage> m_cteMessages;
};

}
#endif
