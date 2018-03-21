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
#include "ss-command-manager.h"
#include "bs-command-manager.h"
#include "ss-net-device.h"
#include "connection-manager.h"
#include "ns3/pointer.h"
#include "ns3/enum.h"
#include "bandwidth-manager.h"
#include "ns3/mobility-model.h"
#include "ns3/cost231-propagation-loss-model.h"
#include "ns3/event-id.h"
#include "ns3/simulator.h"
#include "wimax-phy.h"
#include "simple-ofdm-wimax-phy.h"
#include "wimax-channel.h"
#include "simple-ofdm-wimax-channel.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/ip-interface-list.h"

NS_LOG_COMPONENT_DEFINE ("SsCommandManager");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SsCommandManager);

TypeId 
SsCommandManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SsCommandManager")
    .SetParent<Object> ()
    .AddConstructor<SsCommandManager> ()
    ;
  return tid;
}

SsCommandManager::SsCommandManager ()
  : m_schedulingType (ServiceFlow::SF_TYPE_NRTPS),
    m_registeredBsManager (NULL),
    m_idRegisteredBd (0),
    m_nodeActive (true)
{}

SsCommandManager::~SsCommandManager ()
{
  NS_LOG_FUNCTION (this);
}

void
SsCommandManager::SetNetDevice (Ptr<NetDevice> device)
{
  /*Ptr<SubscriberStationNetDevice> ssDevice = DynamicCast<SubscriberStationNetDevice> (device);
  m_ssDevice = ssDevice;*/
	m_ssDevice = DynamicCast<SubscriberStationNetDevice> (device);
}

Ptr<SubscriberStationNetDevice>
SsCommandManager::GetSs(void)
{
  return m_ssDevice;
}

void 
SsCommandManager::ActivateNode (void)
{
  m_nodeActive = true;
}

void 
SsCommandManager::DeactivateNode (void)
{
  m_nodeActive = false;
  if (!m_idRegisteredBd)
    {
      DeleteRegister ();
    }
}

bool SsCommandManager::IsNodeActive (void)
{
  return (m_nodeActive);
}

void
SsCommandManager::SetIdRegisteredBs(uint32_t bsid)
{
  m_idRegisteredBd = bsid;
}

Ptr<BsCommandManager> 
SsCommandManager::GetRegisteredBsManager (void)
{
  return m_registeredBsManager;
}

void 
SsCommandManager::AddBsManager (Ptr<BsCommandManager> bsManager)
{
  m_bsManagers.push_back(bsManager);
}

Ptr<BsCommandManager> 
SsCommandManager::GetBsManager (uint32_t index)
{
  uint32_t j = 1;
  for (std::list<Ptr<BsCommandManager> >::const_iterator iter = m_bsManagers.begin(); iter != m_bsManagers.end(); ++iter)
    {
      if (j == index)
        {
          return (*iter);
        }
      j++;
    }

  return 0;
}

Ipv4Address* 
SsCommandManager::GetVehicleIpAddress (void)
{
  Ptr<IpInterfaceList> interfaceList = m_ssDevice->GetNode ()->GetObject<IpInterfaceList> ();
  Ipv4Address* address = new Ipv4Address ();
  *address = interfaceList->GetIpAddress ("Wimax");
  return address;
}


Ipv6Address* 
SsCommandManager::GetVehicleIpv6Address (void)
{
  Ptr<IpInterfaceList> interfaceList = m_ssDevice->GetNode ()->GetObject<IpInterfaceList> ();
  Ipv6Address* address = new Ipv6Address ();
  *address = interfaceList->GetIpv6Address ("Wimax");
  return address;
}

void 
SsCommandManager::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_bsManagers.clear();
  m_ssDevice = 0;
  Object::DoDispose ();
}

void 
SsCommandManager::RegisterInBS (uint32_t bsId)
{
  Mac48Address ssAddress = GetMacAddress ();
  Ptr<BsCommandManager> bsManager = GetBsManager(bsId);
  m_registeredBsManager = bsManager;
  WimaxPhy::ModulationType modulationType = m_ssDevice->GetModulationType();
  bsManager->RegisterSS(this, ssAddress, modulationType);
  m_ssDevice->SetState(SubscriberStationNetDevice::SS_STATE_REGISTERED);
  SetIdRegisteredBs(bsId);
  Ptr<WimaxChannel> channel = bsManager->GetChannel();
  m_ssDevice->GetPhy()->Attach(channel);
}

Mac48Address
SsCommandManager::GetMacAddress (void)
{
  return m_ssDevice->GetMacAddress();
}

uint32_t 
SsCommandManager::GetNodeId (void)
{
  return m_ssDevice->GetNode ()->GetId ();
}

void
SsCommandManager::InitiateConnection (void)
{
  Mac48Address ssAddress = GetMacAddress ();
  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
  ServiceFlow * serviceFlow = m_ssDevice->GetServiceFlowManager()->GetServiceFlows (m_schedulingType).at(0);
  ServiceFlow * BSserviceFlow = bsManager->ConnectSS(ssAddress, serviceFlow);
  serviceFlow->SetSfid(BSserviceFlow->GetSfid());     
  Ptr<WimaxConnection> connection= BSserviceFlow->GetConnection();
  SetMyCid(connection->GetCid());
  Ptr<WimaxConnection> transportConnection = CreateObject<WimaxConnection> (m_cid,Cid::TRANSPORT);
  m_ssDevice->GetConnectionManager()->AddConnection(transportConnection,Cid::TRANSPORT);
  serviceFlow->SetConnection(transportConnection);
  serviceFlow->SetIsEnabled(true);
  m_ssDevice->SetAreServiceFlowsAllocated(true);
  m_ssDevice->SetAreManagementConnectionsAllocated (true);
  m_sfid=serviceFlow->GetSfid();      
  return ;
}

void
SsCommandManager::InitiateConnectionIp (Ipv4Address ssIpAddress, ServiceFlow::Direction direction)
{
  Mac48Address ssAddress = GetMacAddress ();
  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
  ServiceFlow * serviceFlow = m_ssDevice->GetServiceFlowManager()->GetServiceFlows (m_schedulingType).at(0);
  ServiceFlow * BSserviceFlow = bsManager->ConnectSSIp (ssAddress, serviceFlow, ssIpAddress, direction);
  serviceFlow->SetSfid (BSserviceFlow->GetSfid()); 
  Ptr<WimaxConnection> connection= BSserviceFlow->GetConnection();
  SetMyCid(connection->GetCid());
  Ptr<WimaxConnection> transportConnection = CreateObject<WimaxConnection> (m_cid,Cid::TRANSPORT);
  m_ssDevice->GetConnectionManager()->AddConnection(transportConnection,Cid::TRANSPORT);
  serviceFlow->SetConnection(transportConnection);
  serviceFlow->SetIsEnabled(true);
  m_ssDevice->SetAreServiceFlowsAllocated(true);
  m_ssDevice->SetAreManagementConnectionsAllocated (true);
  m_sfid=serviceFlow->GetSfid(); 
  return ;
}

void
SsCommandManager::Bconnection (Cid c)
{
  m_ssDevice->SetBasicConnection (CreateObject<WimaxConnection> (c,Cid::BASIC));
}

void
SsCommandManager::Pconnection(Cid c)
{
  m_ssDevice->SetPrimaryConnection (CreateObject<WimaxConnection> (c,Cid::PRIMARY));
}

void
SsCommandManager::BandwidthRequest (Ptr<BsCommandManager> bsManager)
{
  ServiceFlow * serviceFlow;
  std::vector<ServiceFlow*> serviceFlows =
  m_ssDevice->GetServiceFlowManager ()->GetServiceFlows (ServiceFlow::SF_TYPE_ALL);

  for (std::vector<ServiceFlow*>::iterator iter = serviceFlows.begin (); iter != serviceFlows.end (); ++iter)
    {
      serviceFlow = *iter;     
      if (serviceFlow->GetSchedulingType () == ServiceFlow::SF_TYPE_RTPS || serviceFlow->GetSchedulingType()
	  == ServiceFlow::SF_TYPE_NRTPS || serviceFlow->GetSchedulingType()== ServiceFlow::SF_TYPE_BE)
           {          
             uint32_t bytesToRequest = serviceFlow->GetQueue ()->GetNBytes ();   
             bsManager-> ProcessBwRequest (bytesToRequest, serviceFlow->GetSfid());
           }                    
    }
}

void
SsCommandManager::ResetNBytes (void)
{
  ServiceFlow * serviceFlow;
  std::vector<ServiceFlow*> serviceFlows =
  m_ssDevice->GetServiceFlowManager ()->GetServiceFlows (ServiceFlow::SF_TYPE_ALL);
  for (std::vector<ServiceFlow*>::iterator iter = serviceFlows.begin (); iter != serviceFlows.end (); ++iter)
    {
      serviceFlow = *iter;     
      if (serviceFlow->GetSchedulingType () == ServiceFlow::SF_TYPE_RTPS || serviceFlow->GetSchedulingType()
	  == ServiceFlow::SF_TYPE_NRTPS || serviceFlow->GetSchedulingType()== ServiceFlow::SF_TYPE_BE)
           {          
             serviceFlow->GetQueue ()->ResetNBytes ();
           }                    
    }
}

void
SsCommandManager::SendUnicastPacket (uint32_t size)
{ 
  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);  
  if (PeriodicScanning())
    {
      if(!(m_ssDevice->GetAreServiceFlowsAllocated()))
	{
	  NS_LOG_INFO ("SS: I've got no connection");
	  InitiateConnection();
	}
      Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
      Mac48Address bsAddress = bsManager->GetMacAddress();
      Ptr<Packet> p = Create<Packet> (size);  
      m_ssDevice->Send (p, bsAddress , 1);
      BandwidthRequest(bsManager);
      SsON();
      bsManager->BsON();
    }
  else
    {
      return;
    }
}

void
SsCommandManager::SendUnicastPacketIp (uint32_t size, Ipv4Address dest)
{ 
  if (this->PeriodicScanning())
    {
      Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
      NS_LOG_INFO ("[SsCommandManager::SendUnicastPacket] Allo=" << m_ssDevice->GetAreServiceFlowsAllocated());
      if(!(m_ssDevice->GetAreServiceFlowsAllocated()))
	{
	  NS_LOG_INFO ("SS: I've got no connection");
	  this->InitiateConnectionIp (dest, ServiceFlow::SF_DIRECTION_DOWN);
	}
      else
	{
	  NS_LOG_INFO ("SS: I've already got a connection");
	  ServiceFlow * serviceFlow = m_ssDevice->GetServiceFlowManager()->GetServiceFlows (m_schedulingType).at(0);
	  ServiceFlowRecord *record = serviceFlow->GetRecord ();
	  record->Reset ();
	  bsManager->ResetOldRecords (GetMacAddress());
	}

      Mac48Address bsAddress = bsManager->GetMacAddress();
      Ptr<Packet> p = Create<Packet> (size);  
      Ipv4Header ipHdr;
      ipHdr.SetProtocol (TcpL4Protocol::PROT_NUMBER);
      ipHdr.SetSource (Ipv4Address ("10.1.1.1"));
      ipHdr.SetDestination (dest);
      p->AddHeader (ipHdr);
      m_ssDevice->Send (p, bsAddress , 2048);
      BandwidthRequest(bsManager);
      SsON();
      bsManager->BsON();
    }
  else
    {    
      return;
    }
 }

bool
SsCommandManager::PrepareConnection (Ipv4Address dest)
{
  if (PeriodicScanning())
    {
      Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
      if(!(m_ssDevice->GetAreServiceFlowsAllocated()))
        {
	  InitiateConnectionIp (*GetVehicleIpAddress (), ServiceFlow::SF_DIRECTION_UP);
	  return true;
	}
      else
        {
	  NS_LOG_INFO ("[SsCommandManager::PrepareConnection] SS: I've already got a connection");
	  ServiceFlow * serviceFlow = m_ssDevice->GetServiceFlowManager()->GetServiceFlows (m_schedulingType).at(0);
	  ServiceFlowRecord *record = serviceFlow->GetRecord ();
	  record->Reset ();
	  bsManager->ResetOldRecords (GetMacAddress());
	}
    }
  return false;
}

void
SsCommandManager::InitiateBandwidthRequest (void) 
{
  NS_LOG_INFO ("[SsCommandManager::InitiateBandwidthRequest]");
  if (m_idRegisteredBd)
    {
      Ptr<BsCommandManager> bsManager = GetBsManager (m_idRegisteredBd);
      BandwidthRequest (bsManager);
      SsON();
      bsManager->BsON();
    }
}

bool 
SsCommandManager::GetOnOff(void)
{
 return m_ssPhyOn;
}

void
SsCommandManager::SetOnOff(bool valor)
{
  if(GetOnOff())
    {
      if(valor)
	{
	  NS_LOG_INFO ( "SS already ON");
	}
      else
	{
	  m_ssPhyOn=valor;
	  NS_LOG_INFO ( "SS OFF" );;
	  return;
	}
    }
  else 
    {
      if(valor)
	{
	  m_ssPhyOn=valor;
	  NS_LOG_INFO ( "SS ON" );;
	  return;
	}
      else
	{
	  NS_LOG_INFO ( "SS already OFF" );;       
	}
    }
}

void
SsCommandManager::SsON (void)
{
  SetOnOff (true);
}

uint32_t
SsCommandManager::GetSfid(void)
{
  return m_sfid;
}

void
SsCommandManager::DeleteRegister (void)
{
  if(m_ssDevice->GetState()== SubscriberStationNetDevice::SS_STATE_REGISTERED)
    {
      LeaveMulticastGroup();
      Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
      m_registeredBsManager = NULL;
      bsManager->DeleteRegister(this->GetMacAddress());
      m_ssDevice->GetPhy()->DisAttach();
      bsManager->ClearSsCommandManager(this);
      m_ssDevice->SetState(WimaxPhy::PHY_STATE_OFF);   
      NS_LOG_INFO ("SS: "<<GetMacAddress()<<" DeRegistered from BS:"<<m_idRegisteredBd);
      SetIdRegisteredBs (0);
    }
}

void
SsCommandManager::DeleteConnection(void/*Ptr<ConnectionIdentifier> m_cid*/)
{
  if (m_ssDevice->GetAreServiceFlowsAllocated())
    {
      ServiceFlow * serviceFlow = NULL;
      serviceFlow = m_ssDevice->GetServiceFlowManager()->GetServiceFlows (m_schedulingType).at(0);
      NS_LOG_INFO ( "[SsCommandManager::DeleteConnection] m_schedulingType=" << m_schedulingType << " serviceFlow="<<serviceFlow); 
      if (serviceFlow)
	{
          serviceFlow->SetIsEnabled (false);
	  NS_LOG_INFO ( "[SsCommandManager::DeleteConnection] Clear Connection ... "); 
	  m_ssDevice->GetConnectionManager()->ClearConnection (m_cid);
	  m_ssDevice->SetAreServiceFlowsAllocated(false);
	  m_ssDevice->SetAreManagementConnectionsAllocated (false);
	  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
	  bsManager->DeleteConnection (m_cid, m_sfid);
	}    
    }
  return;
}

bool
SsCommandManager::PeriodicScanning (void)
{ 
  if (!IsNodeActive ())
    {
      NS_LOG_INFO ( "[SsCommandManager::PeriodicScanning] SS is OFF" );
      return false;
    }

  uint32_t newBsid = ScanningBss ();
  if (newBsid == m_idRegisteredBd) 
   {
    if (newBsid==0) 
     {
       NS_LOG_INFO ("SS: "<<GetMacAddress()<<" No coverage" );
       return false;
     }
    NS_LOG_INFO ("SS: "<<GetMacAddress()<<" Same cell" );
    return true;
   }
  else
    {
      if (newBsid==0) 
	{
	  NS_LOG_INFO ("SS: "<<GetMacAddress()<<" No coverage" );
	  DeleteRegister ();
	  return false;
	}

      DeleteRegister ();
      RegisterInBS (newBsid);
      return true;
    }
}

uint32_t
SsCommandManager::ScanningBss(void)
{ 
  if(m_idRegisteredBd != 0)
    {
      Ptr<BsCommandManager> bsManager = this->GetBsManager(m_idRegisteredBd);
      if (IsInCoverageArea (bsManager))
        {
  	  return m_idRegisteredBd;
        }
      else
	{
	  DeleteConnection ();
	}
    }
  for (std::list<Ptr<BsCommandManager> >::const_iterator iter = m_bsManagers.begin(); iter != m_bsManagers.end(); ++iter)
    {
      if(IsInCoverageArea((*iter)))
	{	
	  return (*iter)->GetBsId(); 
	} 
    }    
  return 0 ;
}

Ptr<MobilityModel>
SsCommandManager::GetMobility(void)
{
  Ptr<MobilityModel> ssMobility = m_ssDevice->GetNode()->GetObject<MobilityModel> ();
  return ssMobility;
}

bool
SsCommandManager::IsInCoverageArea (Ptr<BsCommandManager> bsManager)
{ 
  Ptr<MobilityModel> ssMobility =0;
  Ptr<MobilityModel> bsMobility =0;
  ssMobility = GetMobility ();
  bsMobility = bsManager->GetMobility ();
  double bsRange = bsManager->GetCoverageRange();
  double distanceToBs = ssMobility->GetDistanceFrom (bsMobility);
  if (distanceToBs<bsRange)
    {
      return true;
    }
  return false;
}

void
SsCommandManager::SetScheduling (enum ServiceFlow::SchedulingType schedulingtype)
{
  m_schedulingType = schedulingtype;
}

void 
SsCommandManager::SetMyCid(Cid Cid)
{
  m_cid=Cid;
}

void
SsCommandManager::JoinMulticastGroup(uint8_t groupId)
{
  if (m_ssDevice->GetState() == SubscriberStationNetDevice::SS_STATE_REGISTERED)
    {
      if (m_groupId==0)
	{
	  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
	  if ( bsManager->JoinMulticastGroup(groupId)==true)
	    {
	      SetMulticastGroupId(groupId);
	    }
	  else
	    {
	      NS_LOG_INFO ( "Cannot join, group is full");
	    }
	}
    }
  NS_LOG_INFO ( "Cannot join Multicast Group. Not Registered" );
}

void
SsCommandManager::LeaveMulticastGroup(void)
{
  if (m_ssDevice->GetState() == SubscriberStationNetDevice::SS_STATE_REGISTERED)
    {
      if(m_groupId!=0)
	{
	  Ptr<BsCommandManager> bsManager = GetBsManager(m_idRegisteredBd);
	  bsManager->LeaveMulticastGroup(GetMulticastGroupId());      
	  SetMulticastGroupId(0);
	}
      else
	{
	  NS_LOG_INFO ( "Not joined in any group" );   
	}
    }
  else
    {
      NS_LOG_INFO ( "Not registered" );  
    }
}

void
SsCommandManager::SetMulticastGroupId(uint8_t groupId)
{
  m_groupId=groupId;
}

uint8_t
SsCommandManager::GetMulticastGroupId(void)
{
  return m_groupId;
}

void 
SsCommandManager::AddPacketTags (Ipv4Address address, PacketTags* packetTags)
{
  RemoveOldPacketTags ();
  PacketTagList::iterator iter = m_packetTagList.begin ();
  iter = m_packetTagList.find (address);
  if( iter != m_packetTagList.end() ) 
    {
      // In the WiMAX implementation the packets tags are not successfully used since these are eliminated at the PHY layer.
     //std::cout << "[SsCommandManager::AddPacketTags] ReceiverIp="<< address <<" already present in m_packetTagList.Limitation of current implementation. Transmissions for a pair of sender and receiver with frequency higher than 0.5Hz is not possible." << std::endl;
      abort ();
      return;
    }
  m_packetTagList.insert (std::make_pair (address, packetTags));    
}

SsCommandManager::PacketTags* 
SsCommandManager::GetPacketTags (Ipv4Address address)
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
SsCommandManager::RemoveOldPacketTags (void)
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
