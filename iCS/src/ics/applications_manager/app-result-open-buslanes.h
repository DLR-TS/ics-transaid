/****************************************************************************/
/// @file    app-result-open-buslanes.h
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2010 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef APP_RESULT_OPEN_BUSLANES_H
#define APP_RESULT_OPEN_BUSLANES_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <map>
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
// class definitions
// ===========================================================================
/**
* @class ResultOpenBuslanes
* @brief Manages the results of the open buslanes application
*/
class ResultOpenBuslanes : public ResultContainer
{
public:

    /**
     * @brief Constructor.
     * @param[in] owner The station the result belongs to.
     * @param[in] appHandlerId The Application Handler the result is attached to.
     */
    ResultOpenBuslanes(ics_types::stationID_t owner, int appHandlerId);

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
     * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE
     */
    int ApplyResult(SyncManager* syncManager);


    /// @todo TO BE COMMENTED
    int CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager);

    /// @todo TO BE COMMENTED
    std::vector<std::pair<int,ics_types::stationID_t> > GetReceivedMessages();


private:
    struct OpenBuslanesMessage {
        ics_types::icstime_t sendingTime;
        bool opened;
        int messageID;
        std::vector<ics_types::stationID_t> receiver;
    };

    ///@brief Whether buslanes shall be opened
    bool m_areOpen;
    int m_runningMessageID;
    std::vector<OpenBuslanesMessage> m_messages;

    static std::map<ics_types::stationID_t, bool> m_knownInfo;
};

}
#endif
