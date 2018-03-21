/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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

/**
 * \ingroup applications
 * \defgroup denm-app
 *
 * iTETRIS application to send DENM on C2C stack
 *
*/

/**
 * \ingroup denm-app
 *
 * \brief This is the iTETRIS C-ITS DEN Service sending and receiving DENM on the C2C stack
 *
 *  @author Michele Rondinone <mrondinone@umh.es>
 *
 */


#ifndef DENM_APP_H
#define DENM_APP_H

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/application.h"
#include "ns3/random-variable.h" 
#include "ns3/iTETRIS-Application.h" 
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/callback.h" 
#include "ns3/address-utils.h"
#include "ns3/socket.h"
#include "ns3/socket-factory.h"


namespace ns3 {

class Socketc2c;

class DENMApp : public iTETRISApplication
{
public:
  static TypeId GetTypeId (void);

  DENMApp ();
  virtual ~DENMApp();

  void SetSockets(void);      
  void SetFrequency(double frequency);
  void SetMessRegenerationTime (double MessRegenerationTime);
  void SetPacketSize (uint32_t packetSize);
  void SetMsgLifeTime (uint8_t MsgLifeTime);  

  void SetC2CAddress(Ptr<c2cAddress> address);

  void StartTransmitting(Ptr<c2cAddress> address);
  void StopTransmitting(void);
  void SetChTag(ChannelTag ch_tag);



protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void ReceiveC2C (Ptr<Socketc2c> socketc2c);
  void ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);

  Ptr<Socketc2c>  m_C2Csocket;  
  uint16_t        m_portC2C;     
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEventC2C;   
  bool            m_runningC2C;   
  uint32_t        m_packetsSentC2C;  
  TypeId          m_C2CSocketfactory;   

  uint16_t        m_sendCountC2C;
  uint64_t        m_recvCountC2C;

  double          m_frequency;
  double          m_MessRegenerationTime;
  double          m_StartTime;
  uint8_t         m_MsgLifeTime;

  Ptr<c2cAddress> m_c2cAddress;

  RandomVariable  m_rndOffset;

};

} // namespace ns3

#endif   /* DENM_APP_H */
