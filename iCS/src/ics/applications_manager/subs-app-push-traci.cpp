/****************************************************************************/
/// @file    subs-app-cmd-traff-sim.cpp
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

#include "subs-app-cmd-traff-sim.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics
{


// ===========================================================================
// static member definitions
// ===========================================================================
int SubsAppPushTraci::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
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
    if (typeinfo == typeid(SubsAppPushTraci*))
    {
      SubsAppPushTraci* subsAppPushTraci = static_cast<SubsAppPushTraci*>(sub);
      if (subsAppPushTraci->m_nodeId == stationID)
      {
        delete subsAppPushTraci;
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

SubsAppPushTraci::SubsAppPushTraci(int appId, ics_types::stationID_t stationId, SyncManager* syncManager,
    unsigned char* msg, int msgSize) :
    Subscription(stationId), out_msg(msg, msgSize)
{
  // Read parameters
  m_id = ++m_subscriptionCounter;

  m_name = "SEND A Command To TraCI";

  m_appId = appId;

  push(syncManager);

}

SubsAppPushTraci::~SubsAppPushTraci()
{
}

void SubsAppPushTraci::push(SyncManager* syncManager)
{
  unsigned int index = 0;

  syncManager->m_trafficSimCommunicator->controlTraCI(in_msg, out_msg);

 }

//===================================================================

tcpip::Storage& SubsAppPushTraci::returnValues()
{
  return in_msg;
}

} // end namespace ics
