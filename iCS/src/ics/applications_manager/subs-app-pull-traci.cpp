/****************************************************************************/
/// @file    subs-app-pull-traci.cpp
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <cstring>

#include "subs-app-pull-traci.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../itetris-node.h"

namespace ics
{

// ===========================================================================
// Constants
// ===========================================================================


// ===========================================================================
// static member definitions
// ===========================================================================
int SubsAppPullTraci::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
{
  if (subscriptions == NULL)
  {
    return EXIT_FAILURE;
  }

  vector<Subscription*>::iterator it;
  for (it = subscriptions->begin(); it < subscriptions->end(); it++)
  {
    Subscription* sub = *it;
    const type_info& typeinfo = typeid(sub);
    if (typeinfo == typeid(SubsAppPullTraci*))
    {
      SubsAppPullTraci* subsAppPullTraci = static_cast<SubsAppPullTraci*>(sub);
      if (subsAppPullTraci->m_nodeId == stationID)
      {
        delete SubsAppPullTraci;
        delete sub;
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_SUCCESS;
}

// ===========================================================================
// member method definitions
// ===========================================================================

SubsAppPullTraci::SubsAppPullTraci(int appId, ics_types::stationID_t stationId, unsigned char* msg,
    int msgSize) :
    Subscription(stationId), out_msg(msg, msgSize)
{
  // Read parameters
  m_id = ++m_subscriptionCounter;

  m_name = "SEND A Command To the Traffic Simulator";

  m_appId = appId;

  pull(synchManager);
}

SubsAppPullTraci::~SubsAppPullTraci()
{
  //delete[] m_msg;
}

void SubsAppPullTraci::pull(SyncManager* syncManager)
{

  syncManager->m_trafficSimCommunicator->controlTraCI(in_msg, out_msg);

}

tcpip::Storage& SubsAppPullTraci::returnValues()
{
  return in_msg;
}

} // end namespace ics
