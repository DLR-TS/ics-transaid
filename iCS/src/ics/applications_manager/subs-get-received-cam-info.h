/****************************************************************************/
/// @file    subs-get-received-cam-info.h
/// @author  Pasquale Cataldi
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef SUBS_GET_RECEIVED_CAM_INFO_H_
#define SUBS_GET_RECEIVED_CAM_INFO_H_

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
* @class SubsGetReceivedCamInfo
* @brief Manages the subscriptions to receive the information about the received CAM messages.
*/
class SubsGetReceivedCamInfo: public ics::Subscription
{
public:

    /**
    * @brief Constructor
    * @param[in] appId ID of the instantiated subscription.
    * @param[in] stationId Station that owns the subscription.
    */
    SubsGetReceivedCamInfo(int subId, ics_types::stationID_t stationId);

    /**
    * @brief Destructor
    */
    virtual ~SubsGetReceivedCamInfo();

    /**
    * @brief Get the information of the CAM messages received by the subscribed station in the last timeStep.
    * @return vector of structure with the information contained in the CAM.
    */
    std::vector<ics_types::TCamInformation>* getInformationFromLastReceivedCAMs();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
    */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

};

}

#endif /* SUBS_GET_RECEIVED_CAM_INFO_H_ */
