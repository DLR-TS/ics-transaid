/****************************************************************************/
/// @file    subs-x-application-data.h
/// @author  Thales
/// @author  Jerome Haerri (EURECOM)
/// @date    December 3rd, 2010, revised June 17th 2012
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef SUBS_X_APPLICATION_DATA_H_
#define SUBS_X_APPLICATION_DATA_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "subscription.h"

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class SubsXApplicationData
* @brief Manages the subscriptions to receive the information from the facilities.
*/
class SubsXApplicationData: public ics::Subscription
{
public:

    /**
    * @brief Constructor
    * @param[in] appId ID of the instantiated subscription.
    * @param[in] stationId Station that owns the subscription.
    * @param[in] fields Requested fields to return to the application.
    */
    SubsXApplicationData(int subId, ics_types::stationID_t stationId, SyncManager* syncManager, unsigned char* msg, int msgSize);

    /**
    * @brief Destructor
    */
    virtual ~SubsXApplicationData();

    /**
    * @brief Send a set of commands to get cross-application information at the current timeStep.
    *
    * The required command and related informations are coded according to the Type-Length-Value method.
    *
    * @return a vector of requested data coded according to the TYPE-LENGTH-VALUE. NULL if it fails.
    */
    vector<unsigned char> pull(SyncManager* syncManager);

    /**
    * @brief Returns if the command completed successfully.
    * @return TRUE if the command completed, FALSE otherwise.
    */
    std::vector<unsigned char> returnStatus();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
    */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);


private:

    bool m_resultStatus;
    tcpip::Storage  m_msg;
    std::vector<unsigned char> m_data;

};

}

#endif /* SUBS_X_APPLICATION_DATA_H_ */
