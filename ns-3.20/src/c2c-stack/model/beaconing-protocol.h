/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#ifndef BEACONING_PROTOCOL_H
#define BEACONING_PROTOCOL_H

#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/event-id.h"


namespace ns3 {

/**
 * \ingroup c2cStack
 * \defgroup BeaconingProtocol BeaconingProtocol
 *
 */

/**
 * \ingroup BeaconingProtocol
 * \brief  iTETRIS [WP600] - Object to create c2c socket instances
 * \internal
 *
 * This class implements the network beaconing protocol.
 *
 *
 */
class BeaconingProtocol : public Object
{
public:

  static TypeId GetTypeId (void);
  static const float BEACON_INTERVAL;
  static const uint8_t TRAFFIC_CLASS;

  BeaconingProtocol ();
  ~BeaconingProtocol ();

  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

  /**
  * Sending periodically beacons
  */
  void SendBeacon ();

protected:

  Ptr<Node> m_node;

private:

  EventId m_sendBEvent;

  /**
  * Schedules the transmission of beacons
  */
  void ScheduleBeaconSending ();
};

}; // namespace ns3

#endif /* BEACONING_PROTOCOL_H */
 
