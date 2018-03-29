/****************************************************************************/
/// @file    subs-app-result-traff-sim.h
/// @author  Jerome Haerri (EURECOM)
/// @date    December 15th, 2010
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

#ifndef SUBS_APP_RESULT_TRAFF_SIM_H_
#define SUBS_APP_RESULT_TRAFF_SIM_H_

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
	class SubsAppResultTraffSim: public ics::Subscription
	{
		public:

			/**
			 * @brief Constructor
			 * @param[in] appId ID of the instantiated subscription.
			 * @param[in] stationId Station that owns the subscription.
			 * @param[in] msg Command fields to subscribe to the traffic simulator.
			 * @param[in] msgSize Length of the command fields Command fields to subscribe to the traffic simulator.
			 */
			SubsAppResultTraffSim(int subId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize);

			/**
			 * @brief Destructor
			 */
			virtual ~SubsAppResultTraffSim();

			/**
			 * @brief Send a set of commands to subscribe to the traffic simulator at the current timeStep.
			 *
			 * The required command and related informations are coded according to the Type-Length-Value method.
			 *
			 * @return a vector of requested data coded according to the TYPE-LENGTH-VALUE. NULL if it fails.
			 */
			std::vector<unsigned char> pull(SyncManager* syncManager);

			/**
			 * @brief Deletes the subscription according to the input parameters.
			 * @param[in] subscriptions Collection of subscriptions to delete
			 * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
			 */
			static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

		private:
        #define VALUE_SET_EDGE_TRAVELTIME       0x01
        #define VALUE_GET_EDGE_TRAVELTIME       0x02
        #define VALUE_RE_ROUTE   	    		0x03
        #define VALUE_GET_ROUTE_VARIABLE	 	0x04
        tcpip::Storage            m_msg;
        tcpip::Storage            m_message;
	};

}

#endif /* SUBS_APP_RESULT_TRAFF_SIM_H_ */
