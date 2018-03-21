/****************************************************************************/
/// @file    subs-app-message-send.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @author  Jerome Haerri (EURECOM)
/// @date    December 3rd, 2010, revised May 2012
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

#include "subs-app-message-send.h"
#include "subscriptions-type-constants.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../facilities/mapFacilities/road/Edge.h"
#include "../facilities/mapFacilities/road/Lane.h"
#include "../facilities/mapFacilities/road/Junction.h"
#include "../../utils/ics/geometric/Shapes.h"

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int SubsAppMessageSend::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
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
    if (typeinfo == typeid(SubsAppMessageSend*))
    {
      SubsAppMessageSend* subsAppMessageSend = static_cast<SubsAppMessageSend*>(sub);
      if (subsAppMessageSend->m_nodeId == stationID)
      {
        delete subsAppMessageSend;
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

SubsAppMessageSend::SubsAppMessageSend(int appId, ics_types::stationID_t stationId, SyncManager* syncManager,
    unsigned char* msg, int msgSize) :
    Subscription(stationId), m_msg(msg, msgSize)
{
  // Read parameters
  m_id = ++m_subscriptionCounter;

  m_name = "SEND AN APPLICATION MESSAGE";

  m_appId = appId;

  m_appMsgType = 0x01;        // def value
  m_prefTechs = 0xFF;         // def value - means no preferred techno
  m_commProfile = 0x00;       // def value - means no communication profile provided
  m_senderId = 0;             // def value
  m_msgLifetime = 1;          // def value
  m_appMsgLength = 500;         // def value
  m_msgSeqNo = 0;             // def value
  m_frequency = 1;            // def value
  m_msgRegenerationTime = 1;  // def value

  // Send the message
  m_schedulingStatus = push(syncManager);
}

SubsAppMessageSend::~SubsAppMessageSend()
{
}

bool SubsAppMessageSend::push(SyncManager* syncManager)
{
  unsigned int index = 0;

  m_appMsgType = m_msg.readChar();                   // HEADER__APP_MSG_TYPE

  unsigned char headerFlag = m_msg.readChar(); // HEADER__MSG_FLAG (necessary to know which optional fields of the header are present)

  if ((headerFlag & 0x01) == 0x01)
    m_prefTechs = m_msg.readChar();                // HEADER__PREF_TECHS

  if ((headerFlag & 0x02) == 0x02)
    m_commProfile = m_msg.readChar();              // HEADER__GEN_PROF

  if ((headerFlag & 0x04) == 0x04)
    m_senderId = m_msg.readInt();                   // HEADER__SOURCE_ID
  else
    m_senderId = m_nodeId;

  if ((headerFlag & 0x08) == 0x08)
    m_msgLifetime = (int) m_msg.readChar();                  // HEADER__LIFE_TIME

  if((headerFlag & 0x10) == 0x10)             //To specify a precise time at which send the message
    m_time = m_msg.readDouble();
  else
    m_time = SyncManager::m_simStep;

  m_appMsgLength = m_msg.readShort();                   // HEADER__MSG_PAYLOAD_LENGTH

  m_msgSeqNo = m_msg.readInt();                       // HEADER__MSG_SEQ_No

  unsigned char commMode = m_msg.readChar(); // Defines the communication mode and thus the way the extended header will be read.
#ifdef LOG_ON
  stringstream tmp;
  tmp << "SubsAppMessageSend number " << m_id;
  IcsLog::LogLevel((tmp.str()).c_str(), kLogLevelInfo);
#endif

  switch (commMode)
  {
  case EXT_HEADER_TYPE_TOPOBROADCAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: TOPOBROADCAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    short numHops = 0;
    unsigned char flagHops = m_msg.readChar();
    if (flagHops == EXT_HEADER__VALUE_BLOCK_HOPS_No)
      numHops = m_msg.readShort();
    else
    {
      stringstream log;
      log << "To send a TOPOBROADCAST message the number of hops must be specified.";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
      return false;
    }

    string extra;
    std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

    if (syncManager->ScheduleV2xTopobroadcastMessages(m_senderId, m_appId, m_msgSeqNo, m_frequency, m_appMsgLength,
        m_msgRegenerationTime, m_appMsgType, m_commProfile, m_prefTechs, numHops, m_msgLifetime, extra,
        genericContainer) == EXIT_SUCCESS)
    {
      m_schedulingStatus = true;
    }
    else
    {
      stringstream log;
      log << "TOPOBROADCAST message could not be sent";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
      return false;
    }

    break;
  }
  case EXT_HEADER_TYPE_UNICAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: UNICAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    int destId = 0;
    unsigned char flagDest = m_msg.readChar();
    unsigned int numDests = (unsigned int) m_msg.readChar();

    if (flagDest == EXT_HEADER__VALUE_BLOCK_IDs)
    {
      int i = 0;
      unsigned short container_l;

      for (i = 0; i < numDests; i++)
      {
        destId = m_msg.readInt();

        string extra;
        std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

        ITetrisNode* destinationNode = syncManager->GetNodeByIcsId(destId);

        if (destinationNode == NULL)
        {  // check the possibility that the vehicle left the simulation
#ifdef LOG_ON
          stringstream log;
          log << "SubsAppMessageSend::sendMessage() The iCS ID node does not longer exist: " << m_commProfile;
          IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
          return false; // we return false, even without checking if the other destinations could find the ID..as it would be untracable otherwise
        }
        if (syncManager->ScheduleV2xUnicastMessages(m_senderId, m_appId, destinationNode, m_msgSeqNo, m_appMsgType,
            m_frequency, m_appMsgLength, m_msgRegenerationTime, m_commProfile, m_prefTechs, m_msgLifetime, extra, m_time,
            genericContainer) == EXIT_SUCCESS)
        {
            stringstream log;
            log << "UNICAST message sent";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }
        else
        {
          stringstream log;
          log << "UNICAST message could not be sent";
          IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
          return false;
        }
      }
    }
    else
    {
      stringstream log;
      log
          << "SubsAppMessageSend::sendMessage(): To send a UNICAST message a list of destination IDs (each with a single UNICAST message) must be specified";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
      return false;
    }
    m_schedulingStatus = true;
    break;

  }
  case EXT_HEADER_TYPE_MULTICAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: MULTICAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    std::vector<int> destIds;
    unsigned char flagDest = m_msg.readChar();
    unsigned int numDests = (unsigned int) m_msg.readChar();
    if (flagDest == EXT_HEADER__VALUE_BLOCK_IDs)
      for (unsigned int i = 0; i < numDests; i++)
        destIds.push_back(m_msg.readInt());
    else
    {
      stringstream log;
      log << "To send a MULTICAST message the destination IDs must be specified.";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

      return false;
    }
    string extra;
    std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

    std::cerr << "MULTICAST transmission not implemented." << std::endl; // TODO: Implement the multicast transmission.

    break;
  }
  case EXT_HEADER_TYPE_GEOBROADCAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: GEOBROADCAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    unsigned char flagAreas = m_msg.readChar();
    if (flagAreas == EXT_HEADER__VALUE_BLOCK_AREAs)
      m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
    else
    {
      stringstream log;
      log << "To send a GEOBROADCAST message the dissemination area must be specified.";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
      return false;
    }

    string extra;
    std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

    std::vector<Area2D*> areas = syncManager->m_facilitiesManager->convertAreas(m_areas);
#ifdef LOG_ON
    log.str("");
    log << "Area center=" << ((Circle*) m_areas[0].shape)->getCenter() << " radius="
        << ((Circle*) m_areas[0].shape)->getRadius() << " area=" << ((Circle*) m_areas[0].shape)->getArea();
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    if (syncManager->ScheduleV2xGeobroadcastMessages(m_senderId, m_appId, m_msgSeqNo, m_frequency, m_appMsgLength,
        m_msgRegenerationTime, m_appMsgType, m_commProfile, m_prefTechs, areas, m_msgLifetime, extra, m_time,
        genericContainer) == EXIT_SUCCESS)
    {
      m_schedulingStatus = true;
    }
    else
    {
      stringstream log;
      log << "GEOBROADCAST message could not be sent";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
      return false;
    }
    break;
  }
  case EXT_HEADER_TYPE_GEOUNICAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: GEOUNICAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    bool readArea = false;
    bool readDest = false;
    int destId = 0;

    while (readArea && readDest)
    {
      unsigned char flag = m_msg.readChar();
      switch (flag)
      {
      case EXT_HEADER__VALUE_BLOCK_AREAs:
      {
        readArea = true;
        m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
        break;
      }
      case EXT_HEADER__VALUE_BLOCK_IDs:
      {
        readDest = true;

        unsigned int numDests = (unsigned int) m_msg.readChar();
        if (numDests == 1)
          destId = m_msg.readInt();
        else
        {
          stringstream log;
          log << "The number of destinations must be one for GEOUNICAST transmission";
          IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
          return false;
        }
        break;
      }
      default:
      {
        stringstream log;
        log << "Block not recognized!";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
        return false;
      }
      }

      string extra;
      std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

    }

    std::cerr << "GEOUNICAST transmission not implemented." << std::endl; // TODO: Implement the geounicast transmission.

    break;
  }
  case EXT_HEADER_TYPE_GEOANYCAST:
  {
#ifdef LOG_ON
    stringstream log;
    log << "Communication mode: GEOANYCAST";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    unsigned char flagAreas = m_msg.readChar();
    if (flagAreas == EXT_HEADER__VALUE_BLOCK_AREAs)
      m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
    else
    {
      stringstream log;
      log << "To send a GEOANYCAST message the dissemination area must be specified.";
      IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
      return false;
    }

    string extra;
    std::vector<unsigned char> *genericContainer = readGenericContainerAndExtrea(extra);

    std::cerr << "GEOANYCAST transmission not implemented." << std::endl; // TODO: Implement the geoanycast transmission.

    break;
  }
  default:
  {
    stringstream log;
    log << "Impossible to send the message. Communication mode not recognized.";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
    return false;
  }
  }
#ifdef LOG_ON
  stringstream log;
  log << "Sub-app-message-send() - The message was correctly sent!!!";
  IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

  return m_schedulingStatus;
}

std::vector<unsigned char> * SubsAppMessageSend::readGenericContainerAndExtrea(string & extra)
{
  unsigned short container_l = m_msg.readShort();
  std::vector<unsigned char> *genericContainer = new std::vector<unsigned char>();

  for (unsigned int k = 0; k < container_l; k++)
  {
    unsigned char type = m_msg.readChar();
    if (k == 0 && type == EXT_HEADER__EXTRA)
    {
      unsigned start = m_msg.position();
      extra = m_msg.readString();
      k += m_msg.position() - start;
    }
    else
    {
      genericContainer->push_back(type);
    }
  }
  return genericContainer;
}

//===================================================================

bool SubsAppMessageSend::returnStatus()
{
  return m_schedulingStatus;
}

//===================================================================

Area2D* getWholeArea(vector<Area2D*> areas);

/**
 * @brief Given a geometric shape or a road element, returns a circle object that includes the area.
 * @param[in] area Geometric shape or road element.
 * @return Returns a circle object that includes the area given in input.
 */
Circle getCircleFromArea(Area2D* area);

//===================================================================

unsigned char SubsAppMessageSend::getAppMsgType() const
{
  return m_appMsgType;
}

unsigned char SubsAppMessageSend::getPrefTechs() const
{
  return m_prefTechs;
}

unsigned char SubsAppMessageSend::getCommProfile() const
{
  return m_commProfile;
}

ics_types::stationID_t SubsAppMessageSend::getSenderId() const
{
  return m_senderId;
}

ics_types::icstime_t SubsAppMessageSend::getMsgLifetime() const
{
  return m_msgLifetime;
}

short SubsAppMessageSend::getAppMsgLength() const
{
  return m_appMsgLength;
}

int SubsAppMessageSend::getMsgSeqNo() const
{
  return m_msgSeqNo;
}

int SubsAppMessageSend::getFrequency() const
{
  return m_frequency;
}

float SubsAppMessageSend::getMsgRegenerationTime() const
{
  return m_msgRegenerationTime;
}

Circle SubsAppMessageSend::getCircleFromAreas()
{
  return SyncManager::m_facilitiesManager->getCircleFromAreas(m_areas);
}

}
