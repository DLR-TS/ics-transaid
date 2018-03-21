/****************************************************************************/
/// @file    app-result-void.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef APP_RESULT_VOID_H
#define APP_RESULT_VOID_H

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
// class definitions
// ===========================================================================
/**
* @class ResultVoid
* @brief To fill the result logic for the apps that do not return values
*/
class ResultVoid : public ResultContainer
{
public:

    /**
     * @brief Constructor
     * @param[in] owner The station the result belongs to.
     */
    ResultVoid(ics_types::stationID_t owner, int appHandlerId);

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
    int ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler);

    /// @todo To be Commented
    std::vector<std::pair<int,ics_types::stationID_t> > GetReceivedMessages();

};

}
#endif
