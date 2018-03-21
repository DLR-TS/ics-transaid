/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Michele Rondinone <mrondinone@umh.es>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/****************************************************************************************
 * Edited by Jin Yan (Renault) <jin.yan@renault.com>
 * RENAULT 2017
 * Added LTE support
***************************************************************************************/


#ifndef LOCAL_COMM_CH_SELECTOR_H
#define LOCAL_COMM_CH_SELECTOR_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"

#include "ns3/itetris-types.h"
#include "ns3/vehicle-sta-mgnt.h"

namespace ns3 {

class LocalCOMMchSelector : public Object
{

public:

  static TypeId GetTypeId (void);
  LocalCOMMchSelector ();
  ~LocalCOMMchSelector ();
  void DoDispose (void);

/**
*  retrieve a structure containing the communication stack and the technology that has to be used to transmit to it 
*/
  stacktodestination* GetCommunicationCh(uint32_t commProfile, TechnologyList technologies);

/**
*  retrieve a structure containing the Communication Infrastructure Unit Id and the communication stack that has to be used to transmit to it (in case where the destination is TMC)
*/  
  stacktodestination* GetCommunicationChTMC(uint32_t commProfile, TechnologyList technologies);

/**
*  retrieve the transmission mode to be used to perform a C2C message dissemination
*/
  TransmissionMode GetC2CDisseminationMode(std::string serviceId, CircularGeoAddress destination);

/**
*  retrieve the number of hops needed to disseminate a c2c message using a topobroadcast transmission mode
*/
  uint32_t getNTopoHops(Ptr<Node> disseminator, CircularGeoAddress destination);

  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  Ptr<Node> m_node;
};

}; //namespace ns3

#endif /* LOCAL_COMM_CH_SELECTOR_H */
