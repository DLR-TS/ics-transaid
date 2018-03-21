/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, CBT EU FP7 iTETRIS project
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
 * Author:  Sendoa Vaz <svaz@cbt.es>
 */


#ifndef UMTS_APP_H
#define UMTS_APP_H

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
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
#include "ns3/iTETRIS-Application.h"  


namespace ns3 {

class UmtsApp : public iTETRISApplication
{
public:
  static TypeId GetTypeId (void);

  UmtsApp ();
  virtual ~UmtsApp();

  void SetSockets(void);      
  void SetFrequency(double frequency);
  void SetMessRegenerationTime (double MessRegenerationTime);
  void SetMsgLifeTime (uint8_t MsgLifeTime);   
  void SendPacketThroughSocket(Ptr<Socket> socket,Ptr<Packet> packet);
  void StartTransmitting(Ipv4Address address);
  
  void StopTransmitting(void);
  void SetPacketSize (uint32_t packetSize); 
  void UninstallFlowInNode();
  void ConfigureNode(Ipv4Address address);
  void FindNodeIdentifier(Ipv4Address address);

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);  
  void SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);    
  void ReceiveIP (Ptr<Socket> socketip);  
  void ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
    
  Ptr<Socket>     m_IPv4socket;
  Ptr<Socket>     m_IPv6socket;       
  uint16_t        m_portIP;
  uint16_t        m_portIPv6;      
  uint32_t        m_packetSize;
  DataRate        m_dataRate;  
  EventId         m_sendEventIP;       
  bool            m_runningIP;      
  uint32_t        m_packetsSentIP;   
  TypeId          m_firstSocketfactory;       
  uint16_t        m_sendCountIP;
  uint64_t        m_recvCountIP;
  double          m_frequency;
  double          m_MessRegenerationTime;
  uint8_t         m_MsgLifeTime;
  double          m_StartTime;
  Ipv4Address     m_IPAddress;
  Address         m_destinationaddress;  
  RandomVariable  m_rndOffset;
  uint32_t 	  m_destinationId;
  std::string	  m_applicationType;
  Ipv4Address     m_serviceIp;
  Ipv4Address 	  m_broadcastIp;
  Ipv6Address     m_serviceIpv6;
  Ipv6Address 	  m_broadcastIpv6;

};

} // namespace ns3

#endif   /* UMTS_APP_H  */
