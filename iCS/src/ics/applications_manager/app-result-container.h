/****************************************************************************/
/// @file    app-result-container.h
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
#ifndef APP_RESULT_CONTAINER_H
#define APP_RESULT_CONTAINER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "foreign/tcpip/storage.h"
#include "../../utils/ics/iCStypes.h"
#include "subscriptions-type-constants.h"
#include <map>
#include <vector>

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
	class SyncManager;
	class ApplicationHandler;

// ===========================================================================
// class definitions
// ===========================================================================
	/**
	 * @class ResultContainer
	 * @brief Manages the results obtained from the applications' executions.
	 */
	class ResultContainer
	{

		public:
			/// @brief True if the container of results is empty.
			bool m_containerIsEmpty;

			/**
			 * @brief Reads the information relative to the execution of the application.
			 * @param[in,out] &storage Data from the application to be processed.
			 * @return True: If the operation finishes successfully.
			 * @return False: If an error occurs.
			 */
			virtual bool ProcessResult(tcpip::Storage& storage) = 0;

			/**
			 * @brief Triggers the actions necesary to apply the result of the application.
			 * @param[in] syncManager To get necessary info from the environment.
			 * @param[in] appHandler Needed to get information of the application.
			 * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE.
			 */
			virtual int ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler) = 0;

			/**
			 * @brief Determines if the container of results is empty or not.
			 * @return True: If the container is empty.
			 * @return False: If the container isn't empty.
			 */
			bool ResultContainerIsEmpty()
			{
				return m_containerIsEmpty;
			}
			;

			///@brief The station the result belong to. Does not mean applys to.
			ics_types::stationID_t m_ownerStation;

			///@brief The ID of the Application Handler the result belongs to.
			int m_applicationHandlerId;

			///@brief typedef for maps of types <unsigned char, map> and <string, vector<unsigned char>>
			typedef std::map<std::string, std::vector<unsigned char> > inMap;
			typedef std::map<unsigned char, inMap> containerMap;

			///@brief a map containing a set of <CMD, MAP<TAG,result>> provided by an application
			containerMap m_resultMap;

			///@brief generic method accessing the result map and returning the result value
			inline bool push(unsigned char CMD_TYPE, std::string TAG, std::vector<unsigned char> result)
			{
				containerMap::iterator it = m_resultMap.find(CMD_TYPE);
				if (it != m_resultMap.end())
				{
					inMap resultsMap = it->second;

					resultsMap.insert(make_pair(TAG, result));
				} else
				{
					inMap resultsMap;
					resultsMap.insert(make_pair(TAG, result));
					m_resultMap.insert(make_pair(CMD_TYPE, resultsMap));
				}
				return true;
			}

			///@brief generic method accessing the result map and returning the result value
			inline std::vector<unsigned char> pull(unsigned char CMD_TYPE, std::string TAG)
			{
				containerMap::iterator it = m_resultMap.find(CMD_TYPE);
				inMap resultsMap = it->second;

				inMap::iterator it2 = resultsMap.find(TAG);
				return it2->second;
			}

			///@brief Factory method
			static ResultContainer* CreateResultContainer(int type, ics_types::stationID_t nodeId, int handlerId);

			///@brief Correct use if polymorphism...
			int CheckMessage(int appMessageId);
			///@brief Correct use if polymorphism...
			int CheckMessage(int appMessageId, ics_types::stationID_t receiverId, SyncManager* syncManager);
			///@brief Correct use if polymorphism...
			void GetReceivedMessages(std::vector<std::pair<int, ics_types::stationID_t> >&);
			///@brief Correct use if polymorphism...
			bool AskSendMessageStatus();
	};

}
#endif
