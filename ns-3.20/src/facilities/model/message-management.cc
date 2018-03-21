/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/ipv4-address.h"
#include "ns3/c2c-address.h"
#include "ns3/CAMmanagement.h"
#include "ns3/C2C-IP-app.h"
#include "ns3/Umts-app.h"
#include "ns3/service-management.h"

#include "message-management.h"

NS_LOG_COMPONENT_DEFINE ("MessageManagement");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MessageManagement);

TypeId 
MessageManagement::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MessageManagement")
    .SetParent<Object> ()
    .AddConstructor<MessageManagement> ()
    ;
  return tid;
}

MessageManagement::MessageManagement ()
{
}

MessageManagement::~MessageManagement ()
{
}

void
MessageManagement::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
MessageManagement::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
MessageManagement::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}


void
MessageManagement::StartTransmission(Ptr<iTETRISApplication> service, Ipv4Address address)
{  
  service->StartTransmitting(address);
}

void
MessageManagement::StartTransmission(Ptr<iTETRISApplication> service, Ipv6Address address)
{  
  service->StartTransmitting(address);
}

void
MessageManagement::StartTransmission(Ptr<iTETRISApplication> service, Ipv4Address address,uint64_t applicationId)
{  
  service->SetApplicationId(applicationId);
  service->StartTransmitting(address);
}

void
MessageManagement::StartTransmission(Ptr<iTETRISApplication> service, Ipv6Address address,uint64_t applicationId)
{  
  service->SetApplicationId(applicationId);
  service->StartTransmitting(address);
}

void
MessageManagement::StartTransmission(Ptr<iTETRISApplication> service, Ptr<c2cAddress> address)
{  
  service->StartTransmitting(address);
}


} //namespace ns3
