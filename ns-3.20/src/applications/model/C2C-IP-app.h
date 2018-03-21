/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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
 * Author:  Michele Rondinone <mrondinone@umh.es>
 */

/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/**
 * \ingroup applications
 * \defgroup c2c-ip-app
 *
 * This is a generic iTETRIS C-ITS service capable of sending
 * both IPv6/IPv4 or C2C packets.
 * This method is used by any C-ITS applications not having a dedicated service
 * such as CAM or DENM. The ITS applications must be specified externally,
 * either in the ns-3 scratch or in the iTETRIS application module.
 *
*/

/**
 * \ingroup c2c-ip-app
 *
 * \brief Receive and consume C-ITS traffic generated either using an IPv4/IPv6 or C2C stack
 *
 * This is a generic iTETRIS C-ITS service capable of sending
 * both IPv6/IPv4 or C2C packets.
 * This method is used by any C-ITS applications not having a dedicated service
 * such as CAM or DENM. The ITS applications must be specified externally,
 * either in the ns-3 scratch or in the iTETRIS application module.
 *
 *  @author Michele Rondinone <mrondinone@umh.es>
 *
 */

#ifndef C2C_IP_APP_H
#define C2C_IP_APP_H

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable.h" 
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/channel-tag.h"
#include "ns3/iTETRIS-Application.h"  


namespace ns3 {

class Socketc2c;

class C2CIPApp : public iTETRISApplication
{
public:
  static TypeId GetTypeId (void);

  C2CIPApp ();
  virtual ~C2CIPApp();

  void SetSockets(void);      
  void SetFrequency(double frequency);
  void SetMessRegenerationTime (double MessRegenerationTime);
  void SetMsgLifeTime (uint8_t MsgLifeTime); 

  void SetC2CAddress(Ptr<c2cAddress> address);
  void SetIPAddress(Ipv4Address address);
  void SetIPAddress(Ipv6Address address);

  void StartTransmitting(Ipv4Address address);
  void StartTransmitting(Ipv6Address address);
  void StartTransmitting(Ptr<c2cAddress> address);

  void StopTransmitting(void);
  void SetPacketSize (uint32_t packetSize);
  
  void SetChTag(ChannelTag ch_tag); 
  
  void ConfigureNode(Ipv6Address address);
  void FindNodeIdentifier(Ipv6Address address);

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  /**
   * \brief Send UP packets based on IPv4 addresses
   * \param address IPv4 address
   * \param destinationAddress destination address
   * \param messRegenerationTime time between two periodic transmission of the iTETRIS message
   * \param startTime starting time of the first packet transmitted
   */
  void DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void DoSendIP(Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime);

  void SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void SendPacketIPv6 (Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);

  void ReceiveC2C (Ptr<Socketc2c> socketc2c);
  void ReceiveIP (Ptr<Socket> socketip);
  void ReceiveIPv6 (Ptr<Socket> socketip);

  void ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void ScheduleTxIP (Ipv6Address address,Address destinationAddress,double messRegenerationTime,double startTime);

  Ptr<Socketc2c>  m_C2Csocket;  
  Ptr<Socket>     m_IPv4socket; 
  Ptr<Socket>     m_IPv6socket; 
  uint16_t        m_portC2C;    
  uint16_t        m_portIP;
  uint16_t        m_portIPv6;      
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEventC2C;    
  EventId         m_sendEventIP; 
  EventId         m_sendEventIPv6;    
  bool            m_runningC2C;   
  bool            m_runningIP;    
  bool            m_runningIPv6;
  uint32_t        m_packetsSentC2C;  
  uint32_t        m_packetsSentIP;
  uint32_t        m_packetsSentIPv6;   
  TypeId          m_firstSocketfactory;   
  TypeId          m_secondSocketfactory;
  uint16_t        m_sendCountC2C;
  uint64_t        m_recvCountC2C;
  uint16_t        m_sendCountIP;
  uint16_t        m_sendCountIPv6;
  uint64_t        m_recvCountIP;
  uint64_t        m_recvCountIPv6;
  double          m_frequency;
  double          m_MessRegenerationTime;
  uint8_t         m_MsgLifeTime;
  double          m_StartTime;
  Ipv4Address     m_IPAddress;
  Ipv6Address     m_IPv6Address;
  Address         m_destinationaddress;

  uint32_t        m_destinationId;
  std::string	  m_applicationType;
  Ptr<c2cAddress> m_c2cAddress;
  RandomVariable  m_rndOffset;

};

} // namespace ns3

#endif   /* C2C_IP_APP_H  */
