/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
 */

#ifndef MESSAGE_MANAGEMENT_H
#define MESSAGE_MANAGEMENT_H


#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/ipv4-address.h"
#include "ns3/iTETRIS-Application.h"


namespace ns3 {

class C2CAddress;

class MessageManagement : public Object
{

public:

  static TypeId GetTypeId (void);
  MessageManagement ();
  ~MessageManagement ();
  void DoDispose (void);

/**
* start an IP transmission with all the parameters set by the other facilities 
*/
  void StartTransmission(Ptr<iTETRISApplication> service, Ipv4Address address);

/**
* start an IP transmission with all the parameters set by the other facilities 
*/
  void StartTransmission(Ptr<iTETRISApplication> service, Ipv6Address address);
  
/**
* start an IP transmission with all the parameters set by the other facilities 
*/ 
  void StartTransmission(Ptr<iTETRISApplication> service, Ipv4Address address,uint64_t applicationId);

/**
* start an IP transmission with all the parameters set by the other facilities 
*/ 
  void StartTransmission(Ptr<iTETRISApplication> service, Ipv6Address address,uint64_t applicationId);

/**
* start a C2C transmission with all the parameters set by the other facilities 
*/
  void StartTransmission(Ptr<iTETRISApplication> service, Ptr<c2cAddress> address);


  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  Ptr<Node> m_node;

};

}; //namespace ns3

#endif /* MESSAGE_MANAGEMENT_H */
