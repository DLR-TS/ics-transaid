/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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
 * Author: Ramon Bauza <rbauza@umh.es>, David Valiente <dvaliente@umh.es>
 */

#include "ns3/assert.h" 
#include "ns3/log.h"
#include "bs-command-manager.h"
#include "ss-command-manager.h"
#include "bs-net-device.h" 
#include "service-flow-manager.h"
#include "connection-manager.h"
#include "bs-uplink-scheduler.h"
#include "ns3/pointer.h"
#include "ns3/enum.h"
#include "ns3/simulator.h"
#include "ns3/event-id.h"
#include "wimax-phy.h"
#include "simple-ofdm-wimax-phy.h"
#include "wimax-channel.h"
#include "simple-ofdm-wimax-channel.h"
#include "ns3/tcp-l4-protocol.h"
#include "ss-record.h"
#include "ss-manager.h"
#include "ns3/itetris-mobility-model.h"
#include "ns3/ip-interface-list.h"

NS_LOG_COMPONENT_DEFINE ("BsCommandManager");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BsCommandManager);


TypeId 
BsCommandManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BsCommandManager")
    .SetParent<Object> ()
    .AddConstructor<BsCommandManager> ()
    ;
  return tid;
}

BsCommandManager::BsCommandManager ()
  : m_coverageRange (5000)
{}

BsCommandManager::~BsCommandManager ()
{
  NS_LOG_FUNCTION (this);
}

void
BsCommandManager::SetNetDevice (Ptr<NetDevice> device)
{
  Ptr<BaseStationNetDevice> bsDevice = DynamicCast<BaseStationNetDevice> (device);
  m_bsDevice = bsDevice;
  m_bsDevice->Add(this);
}

void
BsCommandManager::SetCoverageRange (double coverageRange)
{
  m_coverageRange = coverageRange;
}

Ipv4Address* 
BsCommandManager::GetVehicleIpAddress (uint32_t nodeId) 
{
  Ipv4Address* address = NULL;
  Ptr<SsCommandManager> ssMgnr = GetSsManagerByNodeId (nodeId);
  if (!ssMgnr)
    {
      NS_LOG_INFO ("[BsCommandManager::GetIpAddress] Node " << nodeId << " not registered with the BS");
    }
  else
    {
      address = ssMgnr->GetVehicleIpAddress ();
    }
  return address;
}

Ipv6Address* 
BsCommandManager::GetVehicleIpv6Address (uint32_t nodeId) 
{
  Ipv6Address* address = NULL;
  Ptr<SsCommandManager> ssMgnr = GetSsManagerByNodeId (nodeId);
  if (!ssMgnr)
    {
      NS_LOG_INFO ("[BsCommandManager::GetIpAddress] Node " << nodeId << " not registered with the BS");
    }
  else
    {
      address = ssMgnr->GetVehicleIpv6Address ();
    }
  return address;
}

Ipv4Address* 
BsCommandManager::GetBsIpAddress (void)
{
  Ptr<IpInterfaceList> interfaceList = m_bsDevice->GetNode ()->GetObject<IpInterfaceList> ();
  Ipv4Address* address = new Ipv4Address ();
  *address = interfaceList->GetIpAddress ("Wimax");
  return address;
}

uint32_t 
BsCommandManager::GetNumberOfRegisteredSs (void)
{
  return (m_ssManagers.size());
}

void
BsCommandManager::SetBsId (uint32_t bsid)
{
  m_bsId=bsid;
}

uint32_t
BsCommandManager::GetBsId(void)
{
  return m_bsId;
}

Ptr<BaseStationNetDevice>
BsCommandManager::GetBs(void)
{
  return m_bsDevice;
}

Ptr<MobilityModel>
BsCommandManager::GetMobility(void)
{
  Ptr<MobilityModel> bsMobility = m_bsDevice->GetNode()->GetObject<MobilityModel> ();
  return bsMobility;
}

void
BsCommandManager::InitSfidIndex(void)
{
  m_sfidIndex=150;
}

uint32_t 
BsCommandManager::GetNodeId (void)
{
  return m_bsDevice->GetNode ()->GetId ();
}

void
BsCommandManager::ClearSsCommandManager (Ptr<SsCommandManager> ssCommandManager)
{
  for(std::list<Ptr<SsCommandManager> >:: iterator iter= m_ssManagers.begin(); iter!=m_ssManagers.end(); iter++)
      {
	if(ssCommandManager==(*iter))
	  {
	    m_ssManagers.erase(iter);
	    break;
	  }
      }
}

void 
BsCommandManager::AddSsManager (Ptr<SsCommandManager> ssManager)
{    
  m_ssManagers.push_back(ssManager);
}

Ptr<SsCommandManager> 
BsCommandManager::GetSsManager (uint32_t index)
{
  uint32_t j = 0;
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
      if (j == index)
        {
          return (*iter);
        }
      j++;
    }
  return 0;
}

Ptr<SsCommandManager> 
BsCommandManager::GetSsManager (Mac48Address ssAddress)
{
  uint32_t j = 0;
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
      Mac48Address iterAddress = (*iter)->GetMacAddress();
      if (iterAddress == ssAddress)
        {
          return (*iter);
        }
      j++;
    }
  return 0;
}

Ptr<SsCommandManager> 
BsCommandManager::GetSsManagerByNodeId (uint32_t nodeId)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
      uint32_t iterNodeId = (*iter)->GetNodeId ();
      if (iterNodeId == nodeId)
        {
          return (*iter);
        }
    }
  return NULL;
}

Ptr<SsCommandManager> 
BsCommandManager::GetSsManagerByIpAddress (Ipv4Address address)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
      Ipv4Address* addressIter = (*iter)->GetVehicleIpAddress ();
      if (address == *addressIter)
        {
          return (*iter);
        }
    }
  return NULL;
}

Ptr<SsCommandManager> 
BsCommandManager::GetNonRegisteredSsManager (Ipv4Address address)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_nonRegisteredSsManagers.begin(); iter != m_nonRegisteredSsManagers.end(); ++iter)
    {
      Ipv4Address* addressIter = (*iter)->GetVehicleIpAddress ();
      if (address == *addressIter)
        {
          return (*iter);
        }
    }
  return NULL;
}

void
BsCommandManager::ClearNonRegisteredSsCommandManager (Ptr<SsCommandManager> ssCommandManager)
{
   for(std::list<Ptr<SsCommandManager> >:: iterator iter= m_nonRegisteredSsManagers.begin(); iter!=m_nonRegisteredSsManagers.end(); iter++)
        {
          if(ssCommandManager==(*iter))
           {
            m_nonRegisteredSsManagers.erase(iter);
            break;
           }
        }
}

void 
BsCommandManager::AddNonRegisteredSsManager (Ptr<SsCommandManager> ssManager)
{    
  m_nonRegisteredSsManagers.push_back(ssManager);
}

Ptr<SsCommandManager> 
BsCommandManager::GetNonRegisteredSsManager (uint32_t index)
{
  uint32_t j = 0;
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_nonRegisteredSsManagers.begin(); iter != m_nonRegisteredSsManagers.end(); ++iter)
    {
      if (j == index)
        {
          return (*iter);
        }
      j++;
    }
  return 0;
}

Ptr<SsCommandManager> 
BsCommandManager::GetNonRegisteredSsManager (Mac48Address ssAddress)
{
  uint32_t j = 0;
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_nonRegisteredSsManagers.begin(); iter != m_nonRegisteredSsManagers.end(); ++iter)
    {
      Mac48Address iterAddress = (*iter)->GetMacAddress();
      if (iterAddress == ssAddress)
        {
          return (*iter);
        }
      j++;
    }
  return 0;
}


void 
BsCommandManager::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_ssManagers.clear();
  m_nonRegisteredSsManagers.clear();
  m_bsDevice = 0;
  Object::DoDispose ();
}

void
BsCommandManager::RegisterSS (Ptr<SsCommandManager> ssCommandManager, Mac48Address ssAddress, WimaxPhy::ModulationType modulationType)
{ 
  AddSsManager (ssCommandManager);
  Ptr<SSManager> ssManager = m_bsDevice->GetSSManager();
  SSRecord* ssRecord = ssManager->CreateSSRecord(ssAddress);
  ssRecord->SetRangingStatus(WimaxNetDevice::RANGING_STATUS_SUCCESS);
  ssRecord->SetModulationType(modulationType);
  return;
}

Mac48Address
BsCommandManager::GetMacAddress (void)
{
  return m_bsDevice->GetMacAddress();
}

ServiceFlow*
BsCommandManager::ConnectSS (Mac48Address ssAddress, ServiceFlow* SSserviceFlow)
{
  ServiceFlow* BSserviceFlow;
  SSRecord* ssRecord = m_bsDevice->GetSSManager()->GetSSRecord(ssAddress);
  Ptr<WimaxConnection> transportConnection = m_bsDevice->GetConnectionManager()->CreateConnection  (Cid::TRANSPORT);
  BSserviceFlow = new ServiceFlow (m_sfidIndex++, ServiceFlow::SF_DIRECTION_DOWN, transportConnection);
  BSserviceFlow->SetServiceSchedulingType (SSserviceFlow->GetSchedulingType ()); 
  BSserviceFlow->SetConnection(transportConnection);
  m_bsDevice->GetServiceFlowManager()->AddServiceFlow (BSserviceFlow);
  BSserviceFlow->SetIsEnabled(true);
  BSserviceFlow->SetType(ServiceFlow::SF_TYPE_ACTIVE);
  ssRecord->AddServiceFlow(BSserviceFlow);
  m_bsDevice->GetUplinkScheduler()->SetupServiceFlow (ssRecord, BSserviceFlow);
  Ptr<WimaxConnection> basicConnection = m_bsDevice->GetConnectionManager()->CreateConnection (Cid::BASIC);
  ssRecord->SetBasicCid (basicConnection->GetCid ());
  Ptr<WimaxConnection> primaryConnection =m_bsDevice->GetConnectionManager()-> CreateConnection (Cid::PRIMARY);
  ssRecord->SetPrimaryCid (primaryConnection->GetCid ());
  ssRecord->SetAreServiceFlowsAllocated(true);
  Ptr<SsCommandManager> sscm= GetSsManager(ssAddress);
  sscm->Bconnection(basicConnection->GetCid());
  sscm->Pconnection(primaryConnection->GetCid());
  return BSserviceFlow;
}

void
BsCommandManager::ResetOldRecords (Mac48Address ssAddress)
{
  SSRecord* ssRecord = m_bsDevice->GetSSManager()->GetSSRecord(ssAddress);
  std::vector<ServiceFlow*> servFlows = ssRecord->GetServiceFlows (ServiceFlow::SF_TYPE_NRTPS);
  for (std::vector<ServiceFlow*>::iterator iter = servFlows.begin (); iter != servFlows.end (); iter++)
    {
      if ((*iter)->GetDirection () == ServiceFlow::SF_DIRECTION_UP)
	{
          ServiceFlowRecord *record = (*iter)->GetRecord ();
	  record->Reset ();
	}
    }
}

ServiceFlow*
BsCommandManager::ConnectSSIp (Mac48Address ssAddress, ServiceFlow* SSserviceFlow, Ipv4Address ssIpAddress, ServiceFlow::Direction direction)
{
  SSRecord* ssRecord = m_bsDevice->GetSSManager()->GetSSRecord(ssAddress);
  Ptr<WimaxConnection> transportConnection = m_bsDevice->GetConnectionManager()->CreateConnection  (Cid::TRANSPORT);
  ServiceFlow* BSserviceFlow = CreateServiceFlowUnicast (transportConnection, ssIpAddress, direction);
  transportConnection->SetServiceFlow (BSserviceFlow);
  ssRecord->AddServiceFlow(BSserviceFlow);
  m_bsDevice->GetUplinkScheduler()->SetupServiceFlow (ssRecord, BSserviceFlow);
  Ptr<WimaxConnection> basicConnection = m_bsDevice->GetConnectionManager()->CreateConnection (Cid::BASIC);
  ssRecord->SetBasicCid (basicConnection->GetCid ());
  Ptr<WimaxConnection> primaryConnection =m_bsDevice->GetConnectionManager()-> CreateConnection (Cid::PRIMARY);
  ssRecord->SetPrimaryCid (primaryConnection->GetCid ());
  ssRecord->SetAreServiceFlowsAllocated(true);
  Ptr<SsCommandManager> sscm= GetSsManager(ssAddress);
  sscm->Bconnection(basicConnection->GetCid());
  sscm->Pconnection(primaryConnection->GetCid());
  return BSserviceFlow;
}

void BsCommandManager::ProcessBwRequest(uint32_t bytesToRequest, uint32_t sfid)
{
  ServiceFlow * BSserviceFlow = m_bsDevice->GetServiceFlowManager ()->GetServiceFlow(sfid);
  if (bytesToRequest>0)
    {
      BSserviceFlow->GetRecord ()->SetRequestedBandwidth (bytesToRequest);
    }
}

void 
BsCommandManager::TriggerVehiclesScanning (void)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_nonRegisteredSsManagers.begin(); iter != m_nonRegisteredSsManagers.end(); ++iter)
    {
      (*iter)->PeriodicScanning ();
    }
}

uint32_t 
BsCommandManager::GetLatitude (void)
{
  Ptr<ItetrisMobilityModel> mobility = m_bsDevice->GetNode ()->GetObject<ItetrisMobilityModel> ();
  if (mobility)
    {
      return mobility->GetLatitude ();
    }
 //std::cout << "[BsCommandManager::GetLatitude] ItetrisMobilityModel not found" << std::endl;
  abort ();
}

uint32_t 
BsCommandManager::GetLongitude (void)
{
  Ptr<ItetrisMobilityModel> mobility = m_bsDevice->GetNode ()->GetObject<ItetrisMobilityModel> ();
  if (mobility)
    {
      return mobility->GetLongitude ();
    }
 //std::cout << "[BsCommandManager::GetLongitude] ItetrisMobilityModel not found" << std::endl;
  abort ();
}

void
BsCommandManager::SendBroadcastPacketIp (void)
{
  Ptr<Packet> packet = Create<Packet> (50); 
  Ipv4Header ipHdr;
  ipHdr.SetProtocol (TcpL4Protocol::PROT_NUMBER);
  ipHdr.SetSource (Ipv4Address ("10.1.1.6"));
  ipHdr.SetDestination (Ipv4Address ("10.1.1.255"));
  packet->AddHeader (ipHdr);
  TriggerVehiclesScanning ();
  if (!m_bsDevice->Send (packet,  Mac48Address ("ff:ff:ff:ff:ff:ff") , 2048)) 
    {
      NS_LOG_INFO ("[BsCommandManager::SendBroadcastPacketIp] Txon not possible");
    }

  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
      (*iter)->SetOnOff(true);
    }
  BsON();
}


bool
BsCommandManager::SendOnePacket (Mac48Address ssAddress)
{
  Ptr<SsCommandManager> ssCommMngr = GetNonRegisteredSsManager (ssAddress);
  ssCommMngr->PeriodicScanning (); 
  // variabile inutile
  //uint32_t sfid;
  Ptr<SSManager> ssManager = m_bsDevice->GetSSManager();
  bool q = ssManager-> IsInRecord(ssAddress);
  if (!q)
    {
      NS_LOG_INFO ("SS not registered in BS");
      return false;
    }
  Ptr<SsCommandManager> ssCommandManager = GetSsManager(ssAddress);
  SSRecord * ssRecord = ssManager->GetSSRecord(ssAddress);
  bool w =  ssRecord->GetAreServiceFlowsAllocated();
  if(!w)
  {
	NS_LOG_INFO ("SS's connection not created");
	ssCommandManager->InitiateConnection();
  }
  // non serve
  //sfid= ssCommandManager->GetSfid();
  Ptr<Packet> packet = Create<Packet> (50);  
  if (!m_bsDevice->Send (packet, ssAddress , 1)) 
    {
      NS_LOG_INFO ("[BsCommandManager::SendOnePacket] Txon not possible");
      return false;
    }

  ssCommandManager->SetOnOff(true);
  BsON();      
  return true;
}

bool 
BsCommandManager::PrepareConnection (Ipv4Address dest)
{
  Ptr<SsCommandManager> ssCommMngr = GetNonRegisteredSsManager (dest);
  ssCommMngr->PeriodicScanning (); 
  Ptr<SSManager> ssManager = m_bsDevice->GetSSManager();
  Mac48Address macAddress = ssCommMngr->GetMacAddress ();

  if (!ssManager-> IsInRecord(macAddress))
    {
      NS_LOG_INFO ("[BsCommandManager::PrepareConnection] SS not registered in BS");
      return false;
    }

  SSRecord * ssRecord = ssManager->GetSSRecord(macAddress);
  std::vector<ServiceFlow*> servFlows = ssRecord->GetServiceFlows (ServiceFlow::SF_TYPE_NRTPS);
  if(!GetAreServiceFlowsAllocatedDown (servFlows))
    {
      NS_LOG_INFO ("[BsCommandManager::PrepareConnection] SS's connection not created");
      ssCommMngr->InitiateConnectionIp (dest, ServiceFlow::SF_DIRECTION_DOWN);
    }
  return true;
}

bool
BsCommandManager::GetAreServiceFlowsAllocatedDown (std::vector<ServiceFlow*> servFlows)
{
  for (std::vector<ServiceFlow*>::iterator iter = servFlows.begin (); iter != servFlows.end (); iter++)
    {
      if ((*iter)->GetDirection () == ServiceFlow::SF_DIRECTION_DOWN)
	{
	  return true;
	}
    }
  return false;
}

void
BsCommandManager::ActivateNodes (Ipv4Address dest)
{
  Ptr<SsCommandManager> ssCommMngr = GetNonRegisteredSsManager (dest);
  ssCommMngr->SetOnOff(true);
  BsON();
}


bool
BsCommandManager::SendOnePacketIp (Mac48Address ssAddress)
{
  Ptr<SsCommandManager> ssCommMngr = GetNonRegisteredSsManager (ssAddress);
  ssCommMngr->PeriodicScanning (); 
  Ptr<SSManager> ssManager = m_bsDevice->GetSSManager();
  bool q = ssManager-> IsInRecord(ssAddress);
  if (!q)
    {
      NS_LOG_INFO ("SS not registered in BS");
      return false;
    }

  Ptr<SsCommandManager> ssCommandManager = GetSsManager(ssAddress);
  SSRecord * ssRecord = ssManager->GetSSRecord(ssAddress);
  bool w =  ssRecord->GetAreServiceFlowsAllocated();
  if(!w)
    {
      NS_LOG_INFO ("SS's connection not created");
      ssCommandManager->InitiateConnection();
    } 
  Ptr<Packet> packet = Create<Packet> (50);  
  Ipv4Header ipHdr;
  ipHdr.SetProtocol (TcpL4Protocol::PROT_NUMBER);
  ipHdr.SetSource (Ipv4Address ("10.1.1.5"));
  ipHdr.SetDestination (Ipv4Address ("10.1.1.1"));
  packet->AddHeader (ipHdr);

  if (!m_bsDevice->Send (packet, ssAddress , 2048)) 
    {
      NS_LOG_INFO ("[BsCommandManager::SendOnePacketIp] Txon not possible");
      return false;
    }

   ssCommandManager->SetOnOff(true);
   BsON();
      
   return true;
}

void 
BsCommandManager::BsON(void)
{ 
  SetOnOff (true);
}

bool 
BsCommandManager::GetOnOff(void)
{
  return m_bsPhyOn;
}

void
BsCommandManager::SetOnOff(bool valor)
{
  if (GetOnOff())
    {
      if (valor)
	{
	  NS_LOG_INFO ("BS already ON");
	}
      else
	{
	  m_bsPhyOn=valor;
	  NS_LOG_INFO ("BS OFF");
	  return;
	}
    }
  else 
    {
      if (valor)
	{
	  m_bsPhyOn=valor;
	  NS_LOG_INFO ("BS ON");
	  return;
	}
      else
	{
	  NS_LOG_INFO ("BS already OFF");       
	}
    }
}

void
BsCommandManager::SetSsPhyOn(void)
{
 for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
     if ((*iter)->GetOnOff() == true )
      {
        (*iter)->m_ssDevice->GetPhy()->SetState(WimaxPhy::PHY_STATE_IDLE);
      }
    }
}

void
BsCommandManager::SetSsOff(void)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
     if ((*iter)->GetOnOff() == true)
      {
       (*iter)->SetOnOff (false);
      }
    }
}
void
BsCommandManager::SetSsPhyOff(void)
{
 for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
     if ( (*iter)->m_ssDevice->GetPhy()->GetState() == WimaxPhy::PHY_STATE_IDLE )
      {
        (*iter)->m_ssDevice->GetPhy()->SetState(WimaxPhy::PHY_STATE_OFF);
      }
    }
}
void
BsCommandManager::DeleteRegister (Mac48Address ssAddress)
{
  Ptr<SSManager> ssManager = m_bsDevice->GetSSManager();
  SSRecord *ssRecord = ssManager->GetSSRecord(ssAddress);
  ssManager->DeleteSSRecord(ssRecord->GetBasicCid());
}

void
BsCommandManager::DeleteConnection(Cid cid, uint32_t sfid)
{
  SSRecord* ssRecord = m_bsDevice->GetSSManager()->GetSSRecord(cid);  
  ServiceFlow* serviceFlow = m_bsDevice->GetServiceFlowManager ()->GetServiceFlow (sfid);
  m_bsDevice->GetServiceFlowManager()->ClearServiceFlow (serviceFlow);
  ssRecord->ClearServiceFlow(serviceFlow);
  ssRecord->SetAreServiceFlowsAllocated(false);
  serviceFlow-> ~ServiceFlow();
  m_bsDevice->GetConnectionManager()->ClearConnection (cid);
}

void
BsCommandManager::InitiateBroadcastConnection(void)
{
  Ptr<WimaxConnection> conn = m_bsDevice->GetConnectionManager()->CreateConnection (Cid::MULTICAST);
  NS_LOG_INFO ("Broadcast data connection established");
  conn->SetServiceFlow (CreateServiceFlowBroadcast(conn));
}

ServiceFlow*
BsCommandManager::CreateServiceFlowUnicast (Ptr<WimaxConnection> conn, Ipv4Address ssIpAddress, ServiceFlow::Direction direction)
{
  ServiceFlow* serviceFlow = new ServiceFlow (direction);
  if (direction == ServiceFlow::SF_DIRECTION_DOWN)
    {
      IpcsClassifierRecord DlClassifierUgs (Ipv4Address ("0.0.0.0"),
					    Ipv4Mask ("0.0.0.0"),
					    ssIpAddress,
					    Ipv4Mask ("255.255.255.255"),
					    0,
					    65000,
					    0,
					    100,
					    17, // UDP
					    1);
      CsParameters csParam (CsParameters::ADD, DlClassifierUgs);
      serviceFlow->SetConvergenceSublayerParam (csParam);
    }
  else
    {
      IpcsClassifierRecord UlClassifierUgs (ssIpAddress,
					    Ipv4Mask ("255.255.255.255"),
					    Ipv4Address ("0.0.0.0"),
					    Ipv4Mask ("0.0.0.0"),
					    0,
					    65000,
					    0,
					    100,
					    17, // UDP
					    1);
      CsParameters csParam (CsParameters::ADD, UlClassifierUgs);
      serviceFlow->SetConvergenceSublayerParam (csParam);
    }
  serviceFlow->SetCsSpecification (ServiceFlow::IPV4);
  serviceFlow->SetServiceSchedulingType (ServiceFlow::SF_TYPE_NRTPS);
  serviceFlow->SetMaxSustainedTrafficRate (100);
  serviceFlow->SetMinReservedTrafficRate (1000000);
  serviceFlow->SetMinTolerableTrafficRate (1000000);
  serviceFlow->SetMaximumLatency (100);
  serviceFlow->SetMaxTrafficBurst (2000);
  serviceFlow->SetTrafficPriority (1);
  serviceFlow->SetUnsolicitedGrantInterval (1);
  serviceFlow->SetMaxSustainedTrafficRate (70);
  serviceFlow->SetToleratedJitter (10);
  serviceFlow->SetRequestTransmissionPolicy (0);
  serviceFlow->SetModulation (WimaxPhy::MODULATION_TYPE_BPSK_12);
  serviceFlow->SetConnection (conn);
  serviceFlow->SetIsMulticast (false);
  serviceFlow->SetIsEnabled(true);
  serviceFlow->SetType(ServiceFlow::SF_TYPE_ACTIVE);
  serviceFlow->SetSfid (m_sfidIndex++);
  m_bsDevice->GetServiceFlowManager ()->AddServiceFlow (serviceFlow);
  return serviceFlow;
}

ServiceFlow*
BsCommandManager::CreateServiceFlowBroadcast (Ptr<WimaxConnection> conn)
{
  ServiceFlow* serviceFlow = new ServiceFlow (ServiceFlow::SF_DIRECTION_DOWN);
  IpcsClassifierRecord DlClassifierUgs (Ipv4Address ("0.0.0.0"),
                                        Ipv4Mask ("0.0.0.0"),
                                        Ipv4Address ("10.4.255.255"),
                                        Ipv4Mask ("255.255.0.0"),
                                        0,
                                        65000,
                                        0,
                                        100,
                                        17, // UDP,
//                                         6, // TCP
                                        1);
  CsParameters csParam (CsParameters::ADD, DlClassifierUgs);
  serviceFlow->SetConvergenceSublayerParam (csParam);
  serviceFlow->SetCsSpecification (ServiceFlow::IPV4);
  serviceFlow->SetServiceSchedulingType (ServiceFlow::SF_TYPE_NRTPS);
  serviceFlow->SetMaxSustainedTrafficRate (100);
  serviceFlow->SetMinReservedTrafficRate (1000000);
  serviceFlow->SetMinTolerableTrafficRate (1000000);
  serviceFlow->SetMaximumLatency (100);
  serviceFlow->SetMaxTrafficBurst (2000);
  serviceFlow->SetTrafficPriority (1);
  serviceFlow->SetUnsolicitedGrantInterval (1);
  serviceFlow->SetMaxSustainedTrafficRate (70);
  serviceFlow->SetToleratedJitter (10);
  serviceFlow->SetRequestTransmissionPolicy (0);
  serviceFlow->SetModulation (WimaxPhy::MODULATION_TYPE_BPSK_12);
  serviceFlow->SetConnection (conn);
  serviceFlow->SetIsMulticast (true);
  serviceFlow->SetIsEnabled(true);
  serviceFlow->SetType(ServiceFlow::SF_TYPE_ACTIVE);
  m_bsDevice->GetServiceFlowManager ()->AddServiceFlow (serviceFlow);
  return serviceFlow;
}

void
BsCommandManager::SendBroadcastPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (50);  
  std::vector<Ptr<WimaxConnection> > connections;
  connections = m_bsDevice-> GetConnectionManager()->GetConnections(Cid::MULTICAST) ;
  Ptr<WimaxConnection> connection = connections.at((0));
  if (!(m_bsDevice->Enqueue (packet, MacHeaderType (),connection)))  
    {
      NS_LOG_INFO ("Enqueue Error!!");
      return;
    }  

  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_nonRegisteredSsManagers.begin(); iter != m_nonRegisteredSsManagers.end(); ++iter)
    {
     (*iter)->PeriodicScanning ();
    }

  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
     (*iter)->SetOnOff(true);
    }
    this->BsON();
}

void 
BsCommandManager::PrepareBroadcastTxon (void)
{
  for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
    {
     (*iter)->SetOnOff(true);
    }
  BsON();
}



void
BsCommandManager::InitiateMulticastConnections(uint8_t numGroups)
{
  for (uint8_t i=0; i<numGroups; i++)
    {
      m_bsDevice->GetConnectionManager()-> CreateConnection (Cid::MULTICAST);
      m_nSSgroup[i]=0;
    }
  m_noGroups=numGroups;
}

bool
BsCommandManager::JoinMulticastGroup (uint8_t groupId)
{
  Ptr<WimaxConnection> connection;
  if(m_nSSgroup[(groupId-1)]<4)
    {
      m_nSSgroup[(groupId-1)]++;
      return true;
    }
  return false;
}

void
BsCommandManager::LeaveMulticastGroup(uint8_t groupId)
{
  m_nSSgroup[(groupId-1)]--;
}

void
BsCommandManager::SendMulticastPacket(uint8_t groupId)
{
  std::vector<Ptr<WimaxConnection> > connections;
  connections = m_bsDevice-> GetConnectionManager()->GetConnections(Cid::MULTICAST) ;
  Ptr<WimaxConnection> connection = connections.at((groupId));
  if (groupId>m_noGroups || groupId==0)
    {
      return;
    }
  else if (m_nSSgroup[(groupId-1)]>0)
    {
      Ptr<Packet> packet = Create<Packet> (50); 
      if (!(m_bsDevice->Enqueue (packet, MacHeaderType (), connection) ))  
	{
	  return ;
	}  
      for (std::list<Ptr<SsCommandManager> >::const_iterator iter = m_ssManagers.begin(); iter != m_ssManagers.end(); ++iter)
	{
	  if( (*iter)->GetMulticastGroupId()==groupId)
	    {
	      (*iter)->SetOnOff(true);
	    }  
	}
	BsON();
    }
}

double
BsCommandManager::GetCoverageRange (void)
{
  return m_coverageRange;
}

Ptr<WimaxChannel>
BsCommandManager::GetChannel(void)
{
  return m_bsDevice->GetPhy()->GetChannel();
}

void
BsCommandManager::RefreshModulation(WimaxPhy::ModulationType modulationType,  Mac48Address ssAddress)
{
  SSRecord* ssRecord = m_bsDevice->GetSSManager()->GetSSRecord(ssAddress);
  ssRecord->SetModulationType(modulationType);
}

void 
BsCommandManager::AddPacketTags (Ipv4Address address, PacketTags* packetTags)
{
  RemoveOldPacketTags ();
  PacketTagList::iterator iter = m_packetTagList.begin ();
  iter = m_packetTagList.find (address);
  if( iter != m_packetTagList.end() ) 
    {
      // In the WiMAX implementation the packets tags are not successfully used since these are eliminated at the PHY layer. 
     //std::cout << "[BsCommandManager::AddPacketTags] ReceiverIp="<< address <<" already present in m_packetTagList. Limitation of current implementation. Transmissions for a pair of sender and receiver with frequency higher than 0.5Hz is not possible." << std::endl;
      abort ();
      return;
    }
  m_packetTagList.insert (std::make_pair (address, packetTags));    
}

BsCommandManager::PacketTags* 
BsCommandManager::GetPacketTags (Ipv4Address address)
{
  RemoveOldPacketTags ();
  PacketTagList::iterator iter = m_packetTagList.begin ();
  iter = m_packetTagList.find (address);
  if( iter != m_packetTagList.end() ) 
    {
      return iter->second;
    }
  return NULL;
}

void
BsCommandManager::RemoveOldPacketTags (void)
{
  uint32_t simTime = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  for (PacketTagList::iterator it = m_packetTagList.begin(); it != m_packetTagList.end(); it++)
    {
      uint32_t generationTime = it->second->timeStepTag.Get ();
      if (generationTime + 2 <= simTime)
	{
          delete it->second;
          it->second = NULL;
	  m_packetTagList.erase (it);
	}
    }
}


} // namespace ns3
