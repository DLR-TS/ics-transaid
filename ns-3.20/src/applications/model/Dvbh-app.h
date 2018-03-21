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

class DvbhApp : public iTETRISApplication
{
public:
  static TypeId GetTypeId (void);

  DvbhApp ();
  virtual ~DvbhApp();

  void SetSockets(void);      
  void SetFrequency(double frequency);
  void SetMessRegenerationTime (double MessRegenerationTime);
  void SetMsgLifeTime (uint8_t MsgLifeTime);   
  
  void StartTransmitting(Ipv4Address address);
  
  void StopTransmitting(void);
  void SetPacketSize (uint32_t packetSize); 
  void UninstallFlowInNode();
  void ConfigureBaseStation(Ipv4Address address);
  void ConfigureVehicle();
//   void FindNodeIdentifier(Ipv4Address address);  
  void AddStreams(std::list<std::pair<std::string,uint32_t > > streams);
  void Send(Ptr<Packet> packet);
  void SetServiceId(uint32_t serviceId);
  
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
  uint16_t        m_portIP;      
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
  double 	  m_deltaT;
  uint32_t 	  m_burstSize;
  uint32_t	  m_serviceId;
  
  typedef std::list<std::pair<std::string,uint32_t > > StreamIdList;
  typedef std::list<std::pair<std::string,uint32_t > >::iterator StreamIdListI;
  StreamIdList streamList;
  std::vector< Ptr<Packet> > m_fragmentList;
  
  
//   Ptr<Node> m_node;

};

} // namespace ns3

#endif   /* UMTS_APP_H  */