/****************************************************************************/
/// @file    subs-app-result-traff-sim.h
/// @author  Jerome Haerri (EURECOM)
/// @date    March 31st, 2015
/// @version $Id:
///

/****************************************************************************************
 * Added functionalities for SINETIC
 * Author: Tien-Thinh Nguyen (tien-thinh.nguyen@eurecom.fr)
 * EURECOM 2016
 * SINETIC project
 *
 ***************************************************************************************/

#ifndef SUBS_APP_PULL_TRACI_H_
#define SUBS_APP_PULL_TRACI_H_

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
//class SubscriptionsHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SubsAppResultTraffSim
 * @brief Manages the subscription to information from Traffic Simulator at the Application Level
 */
class SubsAppPullTraci: public ics::Subscription
{
public:

  /**
   * @brief Constructor
   * @param[in] appId ID of the instantiated subscription.
   * @param[in] stationId Station that owns the subscription.
   * @param[in] msg Command fields to subscribe to the traffic simulator.
   * @param[in] msgSize Length of the command fields Command fields to subscribe to the traffic simulator.
   */
	SubsAppPullTraci(int subId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize);

  /**
   * @brief Destructor
   */
  virtual ~SubsAppPullTraci();

  /**
   * @brief Send a set of commands to retrieve data from TraCI.
   *
   * The required command and related informations are coded according to the Type-Length-Value method.
   *
   */
  void pull(SyncManager* syncManager);

  /**
   * @brief retrieve the return data from TraCI, without interpreting it.
   *
   * The required command and related informations are coded according to the Type-Length-Value method.
   *
   * @return data in a generic tcpip::Storage, without interpreting them
   */
  tcpip::Storage& returnValues();


  /**
   * @brief Deletes the subscription according to the input parameters.
   * @param[in] subscriptions Collection of subscriptions to delete
   * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
   */
  static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

private:

  tcpip::Storage in_msg;
  tcpip::Storage out_msg;
};

}

#endif /* SUBS_APP_PULL_TRACI_H_ */
