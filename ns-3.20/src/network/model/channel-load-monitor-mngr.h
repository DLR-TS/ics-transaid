/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012, Mobile Communication Dept., http://www.eurecom.fr,
 *                     EURECOM, ETSI Specialist Task Force 447 STF447
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
 * Author: Jerome Haerri <haerri@eurecom.fr>
 * Author: Laura De Martini <demartin@eurecom.fr>
 */

#ifndef CHANNEL_LOAD_MONITOR_MNGR_H
#define CHANNEL_LOAD_MONITOR_MNGR_H

#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/itetris-types.h"

namespace ns3
{
static const std::string state1="IDLE";
static const std::string state2="ACTIVE";
static const std::string state3="CONGESTED";
static const std::string state4="NOT FOUND";



/**
 * @class ChannelLoadMonitorMngr
 * @brief The base class ChannelLoadMonitorMngr can be derived to implement an protocol-specific channel load algorithms
 */

class ChannelLoadMonitorMngr : public Object
{
  public:
	static TypeId GetTypeId (void);
	virtual ~ChannelLoadMonitorMngr();
	void SetNetDevice (Ptr<NetDevice> device);
	Ptr<NetDevice> GetNetDevice(void);
	float GetChannelLoad();
	std::string GetChannelState();

     virtual void startMonitoring()=0;
     virtual void stopMonitoring()=0;
     virtual void Monitor()=0;

  protected:
     Ptr<NetDevice> m_netDevice;
     float m_channelLoad;
     std::string  m_channelState;
};

} //namespace ns3



#endif
