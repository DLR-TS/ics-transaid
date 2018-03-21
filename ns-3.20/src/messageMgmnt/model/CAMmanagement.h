/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Michele Rondinone <mrondinone@umh.es>, Ramon Bauza <rbauza@umh.es>
 */

/****************************************************************************************
 *                          C2C-IP.cc
 * This is the iTETRIS C-ITS CA Service sending CAM on the C2C stack
 *
***************************************************************************************/


/**
 * \ingroup applications
 * \defgroup cam-manage
 *
 * iTETRIS application to send CAM on C2C stack
 *
*/

/**
 * \ingroup cam-manage
 *
 * \brief This is the iTETRIS C-ITS CA Service sending and receiving CAM on the C2C stack
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * @author Michele Rondinone <mrondinone@umh.es>
 * @author Ramon Bauza <rbauza@umh.es>
 *
  */



#ifndef __CAM_management_h__
#define __CAM_management_h__

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/application.h"
#include "ns3/iTETRIS-Application.h"         
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable.h" 
#include "ns3/channel-tag.h"
#include "ns3/c2c-address.h"

namespace ns3 {

class Socketc2c;

/**
* \ingroup messageMgmnt
*
* \brief  iTETRIS - Enables CAM Facility beaconing/reception on nodes.
*
*/

class CAMmanage : public iTETRISApplication                            
{
public:
  static TypeId GetTypeId (void);

  CAMmanage ();
  virtual ~CAMmanage();
  void SetPacketSize (uint32_t packetSize); 
  void SetFrequency (double frequency); 
  void StartTransmitting(Ptr<c2cAddress> address);                                             
  void StopTransmitting (void);                                                      
  void SetC2CAddress(Ptr<c2cAddress> address);

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void Receive (Ptr<Socketc2c> socket);

  void ScheduleTx (void);
  void SendPacket (void);
  void SetChTag(ChannelTag ch_tag);

  Ptr<c2cAddress> m_c2cAddress;  
  Ptr<Socketc2c>  m_socket;
  uint16_t        m_port;
  uint32_t        m_packetSize;
  double          m_frequency;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  TypeId          m_tid;
  ChannelType     channel;
  uint16_t        m_sendCount;
  uint16_t        m_recvCount;
  RandomVariable  m_rndOffset; 

};

} // namespace ns3

#endif
