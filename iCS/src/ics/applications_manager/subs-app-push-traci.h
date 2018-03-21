/****************************************************************************/
/// @file    subs-app-push-traci.h
/// @author  Jerome Haerri (EURECOM)
/// @date    March 31th, 2015
/// @version $Id:
///

/****************************************************************************************
 * Added functionalities for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/


#ifndef SUBS_APP_PUSH_TRACI_H_
#define SUBS_APP_PUSH_TRACI_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#include "foreign/tcpip/storage.h"

#include "subscription.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class SubscriptionsHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class SubsAppPushTraci
* @brief Manages the transmission of commands to TraCI - it acts as a proxy, as only forwards the commands to TraCI without interpreting them
*/
class SubsAppPushTraci: public ics::Subscription
{
public:

    /**
    * @brief Constructor
    * @param[in] appId ID of the instantiated subscription.
    * @param[in] synchManager used to get a pointer to the Traffic Simulator.
    * @param[in] stationId Station that owns the subscription.
    * @param[in] fields Command fields to be sent to the traffic simulator.
    */
	SubsAppPushTraci(int subId, ics_types::stationID_t stationId, SyncManager* syncManager, unsigned char* msg, int msgSize);

    /**
    * @brief Destructor
    */
    virtual ~SubsAppPushTraci();

    /**
    * @brief Send a set of commands to TraCI at the current timeStep, without interpreting them.
    *
    * The required command and related informations are coded according to the Type-Length-Value method.
    *
    */
    void push(SyncManager* syncManager);

    /**
    * @brief forwards the return values from TraCI, without interpreting them.
    * @return return data from TraCI.
    */
    tcpip::Storage& returnValues();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
    */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

private:

    tcpip::Storage             in_msg; // generic return data from TraCI; will only be interpreted by the creator, not here
    tcpip::Storage             out_msg; // generic push data to TraCI


};

}

#endif /* SUBS_APP_PUSH_TRACI_H_ */
