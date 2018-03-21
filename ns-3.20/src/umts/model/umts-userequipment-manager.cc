/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "umts-manager.h"
#include "umts-userequipment-manager.h"
#include "ns3/log.h"
#include "controlpacket.h"
#include "ns3/random-variable.h"
#include "umts-phy-layer-ue.h"
#include "umts-rrc-layer-ue.h"
#include "ns3/mobility-model.h"
#include "cost231_model.h"
#include "umts-basestation-manager.h"
#include "ns3/propagation-loss-model.h"
#include "umts-channel.h"
#include "ns3/itetris-types.h"
#include "ns3/constant-velocity-mobility-model.h"
#include <math.h>


NS_LOG_COMPONENT_DEFINE ("UmtsUserEquipmentManager");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsUserEquipmentManager);

TypeId
UmtsUserEquipmentManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsUserEquipmentManager")
    .SetParent<Object> ()
    .AddConstructor<UmtsUserEquipmentManager> ()
  ;
  return tid;
}

uint64_t UmtsUserEquipmentManager::m_applicationId=10000000;

UmtsUserEquipmentManager::UmtsUserEquipmentManager ()
{
  m_nodeBIdentifier = (uint32_t)-1;
  m_nodeBIp.Set ("0");
  m_myIp.Set ("0");

  m_nodeBManager = 0;
  m_nodeIdentifier=(uint32_t)-1;    
  m_costPropagationModel = Create<Cost231Propagation> ();  
}

Ipv4Address
UmtsUserEquipmentManager::GetIpAddress()
{
  return m_myIp;
}


Ipv6Address
UmtsUserEquipmentManager::GetIpv6Address()
{
  return m_myIpv6;
}

void
UmtsUserEquipmentManager::SetIpAddress(Ipv4Address address)
{
  m_myIp=address;  
  m_rrc->SetIpAddress(m_myIp);
}


void
UmtsUserEquipmentManager::SetIpv6Address(Ipv6Address address)
{
  m_myIpv6=address;  
  m_rrc->SetIpAddress(m_myIpv6);
}

void
UmtsUserEquipmentManager::SetPhy (Ptr<UmtsPhyLayerUE> phy_)
{
  m_phy = phy_;  
  m_phy->SetInterferenceCallback (MakeCallback (&UmtsUserEquipmentManager::CalculateDownlinkInterference, this));
}

Ptr<UmtsPhyLayerUE>
UmtsUserEquipmentManager::GetPhy ()
{
  return m_phy;
}

Ptr<UmtsRrcLayerUE>
UmtsUserEquipmentManager::GetRRC ()
{
  return m_rrc;
}

void
UmtsUserEquipmentManager::SetRRC (Ptr<UmtsRrcLayerUE> rrc_)
{
  m_rrc = rrc_;  
}

void
UmtsUserEquipmentManager::SetupConnection ()
{
  PeriodicScanning (IPv6);
}

void
UmtsUserEquipmentManager::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
  m_rrc->SetNodeIdentifier(identifier);
}

uint32_t
UmtsUserEquipmentManager::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

uint32_t 
UmtsUserEquipmentManager::GetNodeBIdentifier()
{  
  return m_nodeBIdentifier;  
}


void
UmtsUserEquipmentManager::PeriodicScanning (void)
{

  Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();
  
  controlpacket->SetSource (m_myIp);
  controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);
  controlpacket->SetInitialAccess (1);
  controlpacket->SetUmtsUserEquipmentManager (this);
    
  Ptr<UmtsBaseStationManager> newUmtsBaseStationManager = ScanningNodeB ();
  
  
     
  uint32_t newNodeBIdentifier=(uint32_t)-1;

  if (newUmtsBaseStationManager != 0)
    {      
      newNodeBIdentifier = newUmtsBaseStationManager->GetNodeIdentifier();
    }
  else
    {
      newNodeBIdentifier =(uint32_t)-1;
    }

  if (newNodeBIdentifier==(uint32_t)-1 && m_nodeBIdentifier!= (uint32_t)-1 )
    {
      
      NS_LOG_DEBUG ("UmtsUserEquipmentManager " << m_nodeIdentifier << " Lost Coverage");
        
      ConnectionLoss ();
      m_nodeBManager = 0;
      m_nodeBIdentifier = (uint32_t)-1;
    }
  else if (newNodeBIdentifier == (uint32_t)-1 && m_nodeBIdentifier == (uint32_t)-1)
    {      
      m_nodeBManager = 0;
      m_nodeBIdentifier = (uint32_t)-1;

    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier == (uint32_t)-1)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 2"<<std::endl;
    NS_LOG_DEBUG ("UmtsUserEquipmentManager "<<m_nodeIdentifier<<" We are going to connect to NodeB "<<newNodeBIdentifier);

      m_nodeBManager = newUmtsBaseStationManager;
      if (newUmtsBaseStationManager->NewConnection (controlpacket))
        {
          m_nodeBIdentifier = newNodeBIdentifier;
          m_phy->SetSharedChannel (m_nodeBManager->GetPhy ()->GetSharedChannel ());
        }
      else
        {
          m_nodeBIdentifier = (uint32_t)-1;
          m_nodeBManager = 0;
        }

    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier != newNodeBIdentifier)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 3"<<std::endl;
      NS_LOG_DEBUG ("UmtsUserEquipmentManager " << m_nodeIdentifier << " Handover Procedure . Last Base Station we were connected to was " << m_nodeBIdentifier);
            
      HandoverProcedure ();
      m_rrc->SetHandoverState (1);
      m_phy->SetHandoverState (1);
      if (newUmtsBaseStationManager->NewConnection (controlpacket))
        {
          m_nodeBManager = newUmtsBaseStationManager;
          m_nodeBIdentifier = newNodeBIdentifier;
          m_nodeBIp = m_nodeBManager->GetIpAddress ();
        }
      else
        {
          m_nodeBIdentifier = (uint32_t)-1;
          m_nodeBManager = 0;
          m_nodeBIp.Set ("0");
        }


    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier == newNodeBIdentifier)
    {
	//For testing Purposes. The Vehicle continues connected to the same Base Station

    }
    
}





//!!! NEW FUNCTION !!!
void
UmtsUserEquipmentManager::PeriodicScanning (STACK stack)
{
	std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 11 "<<m_myIpv6<<"at time"<<Simulator::Now ().GetSeconds ()<<std::endl;
  //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning"<<std::endl;
  Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();
  //why controlpacket both here and in umts-basestation-manager???
  if (stack==IPv4)
  {
        controlpacket->SetSource (m_myIp);
  }
  else
  {
       controlpacket->SetSourcev6 (m_myIpv6);
  }
  controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);
  controlpacket->SetInitialAccess (1);
  controlpacket->SetUmtsUserEquipmentManager (this);
    
  Ptr<UmtsBaseStationManager> newUmtsBaseStationManager = ScanningNodeB ();
  /*if(newUmtsBaseStationManager != 0)
  {
       //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning NEW NODE B IDENTIFIER: "<<newUmtsBaseStationManager->GetNodeIdentifier()<<std::endl;
  }*/
  
     
  uint32_t newNodeBIdentifier=(uint32_t)-1;
  //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning, newBaseManager"<<newUmtsBaseStationManager->GetNodeIdentifier()<<std::endl;
  //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning, m_nodeBManager"<<m_nodeBIdentifier<<std::endl;


  if (newUmtsBaseStationManager != 0)
    {      
      newNodeBIdentifier = newUmtsBaseStationManager->GetNodeIdentifier();
    }
  else
    {
      newNodeBIdentifier =(uint32_t)-1;
    }

  if (newNodeBIdentifier==(uint32_t)-1 && m_nodeBIdentifier!= (uint32_t)-1 )
    {
     //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 2"<<std::endl;
      NS_LOG_DEBUG ("UmtsUserEquipmentManager " << m_nodeIdentifier << " Lost Coverage");
        
      ConnectionLoss ();
      m_nodeBManager = 0;
      m_nodeBIdentifier = (uint32_t)-1;
    }
  else if (newNodeBIdentifier == (uint32_t)-1 && m_nodeBIdentifier == (uint32_t)-1)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 3"<<std::endl;
      m_nodeBManager = 0;
      m_nodeBIdentifier = (uint32_t)-1;

    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier == (uint32_t)-1)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 4"<<std::endl;
    NS_LOG_DEBUG ("UmtsUserEquipmentManager "<<m_nodeIdentifier<<" We are going to connect to NodeB "<<newNodeBIdentifier);

      m_nodeBManager = newUmtsBaseStationManager;
      if (newUmtsBaseStationManager->NewConnection (controlpacket, stack))
        {
         //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning, Connection successfully established"<<newNodeBIdentifier<<std::endl;
          m_nodeBIdentifier = newNodeBIdentifier;
          m_phy->SetSharedChannel (m_nodeBManager->GetPhy ()->GetSharedChannel ());
          if (stack==IPv4)
          {
                m_nodeBIp = m_nodeBManager->GetIpAddress ();
          }
          else
          {
                m_nodeBIp6 = m_nodeBManager->GetIpv6Address ();
          }
        }
      else
        {
          m_nodeBIdentifier = (uint32_t)-1;
          m_nodeBManager = 0;
        }

    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier != newNodeBIdentifier)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 5"<<std::endl;
      NS_LOG_DEBUG ("UmtsUserEquipmentManager " << m_nodeIdentifier << " Handover Procedure . Last Base Station we were connected to was " << m_nodeBIdentifier);
            
      HandoverProcedure ();
      m_rrc->SetHandoverState (1);
      m_phy->SetHandoverState (1);
      if (newUmtsBaseStationManager->NewConnection (controlpacket, stack))
        {
          m_nodeBManager = newUmtsBaseStationManager;
         //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning  NEW STATION'S MANAGER IPV6 ADDRESS:"<<m_nodeBManager->GetIpv6Address()<<std::endl;
          m_nodeBIdentifier = newNodeBIdentifier;
          if (stack==IPv4)
          {
                m_nodeBIp = m_nodeBManager->GetIpAddress ();
          }
          else
          {
                m_nodeBIp6 = m_nodeBManager->GetIpv6Address ();
          }
        }
      else
        {
          m_nodeBIdentifier = (uint32_t)-1;
          m_nodeBManager = 0;
          m_nodeBIp.Set ("0");
        }


    }
  else if (newNodeBIdentifier != (uint32_t)-1 && m_nodeBIdentifier == newNodeBIdentifier)
    {
	 //std::cout<<"UmtsUserEquipmentManager::PeriodicScanning 6"<<std::endl;
	//For testing Purposes. The Vehicle continues connected to the same Base Station

    }
    
}



Ptr<UmtsBaseStationManager>
UmtsUserEquipmentManager::GetBestUmtsBaseStationManager(void)
{
  PeriodicScanning();

  return m_nodeBManager;
}


//!!! NEW FUNCTION
Ptr<UmtsBaseStationManager>
UmtsUserEquipmentManager::GetBestUmtsBaseStationManager(STACK stack)
{
  PeriodicScanning(stack);

  return m_nodeBManager;
}
//!!!


void
UmtsUserEquipmentManager::JoinMulticastGroup(std::string serviceName)
{
    m_nodeBManager->AddNewSubscriber(serviceName,m_nodeIdentifier);
}

void
UmtsUserEquipmentManager::LeaveMulticastGroup(std::string serviceName)
{
    m_nodeBManager->DeleteSubscriber(serviceName,m_nodeIdentifier);
}

Ptr<UmtsBaseStationManager>
UmtsUserEquipmentManager::ScanningNodeB (void)
{
  //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB NODE B LIST SIZE:"<<m_nodeBList.size()<<std::endl;
  //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB NODE B IDENTIFIER:"<<m_nodeBIdentifier<<std::endl;
  // Test if the Node UE is connected to a Node B
  if (m_nodeBIdentifier != (uint32_t)-1)
    {
	 //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB 11"<<std::endl;
      // Obtain the Manager of the Node B I am connect to
      Ptr<UmtsBaseStationManager> nodeBManager = GetUmtsBaseStationManager (m_nodeBIdentifier);

      // Test if I am still within the coverage area
      if (IsInCoverageArea (nodeBManager))
        {
          // If I am inside the Coverage Area I return the same Manager
          return nodeBManager;
        }
    }

// If I was not registered with a Node B or if I have lost the connection

  else
    {
	 //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB 22"<<std::endl;
      //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB"<<m_nodeBList.size()<<std::endl;
      for (NodeBListI iter = m_nodeBList.begin (); iter != m_nodeBList.end (); ++iter)
        {
          if (IsInCoverageArea ((*iter)))
            {
              //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB I found a NEW NODE B IN COVERAGE"<< iter->GetIpv6Address() <<std::endl;
              //std::cout<<"UmtsUserEquipmentManager::ScanningNodeB NODE B IDENTIFIER:"<<m_nodeBIdentifier<<std::endl;              
              return (*iter);
            }
        }
    }

  return 0 ;
}

bool
UmtsUserEquipmentManager::IsInCoverageArea (Ptr<UmtsBaseStationManager> manager_)
{
  Ptr<MobilityModel> nodeBMobilityModel = manager_->GetMobility ();
  Ptr<MobilityModel> m_mobility = m_phy->GetMobility ();
  double nodeBRange = manager_->GetCoverageRange ();
  double distanceToBs = m_mobility->GetDistanceFrom (nodeBMobilityModel);
 //std::cout<<"UmtsUserEquipmentManager::IsInCoverageArea 1"<<std::endl;
  if (distanceToBs < nodeBRange )
    {
     //std::cout<<"UmtsUserEquipmentManager::IsInCoverageArea 2"<<std::endl;
      return true;
    }

  return false;
}

void
UmtsUserEquipmentManager::RemoveApplication(uint32_t applicationId,std::string type)
{  
  if(type=="AM"&&m_nodeBManager != 0)
  {
       m_nodeBManager->ULApplicationReleaseRequest(applicationId,m_nodeIdentifier);
  }
  
  m_rrc->RemoveFlow (applicationId);          // remove resources
    
}

void
UmtsUserEquipmentManager::ConnectionLoss ()
{
  NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " Manager// The connection has been Lost. Time " << Simulator::Now () << " \n");  
  m_nodeBManager->ConnectionRelease (this);
  m_rrc->RemoveAllFlows ();

}

void
UmtsUserEquipmentManager::ReleaseReply (uint32_t flowId)
{
  NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " Manager// Release Reply Received.Time " << Simulator::Now () << " \n");
  m_rrc->RemoveFlow (flowId);          // remove resources
}

void
UmtsUserEquipmentManager::RemoveDownLinkFlow(uint32_t applicationId)
{
  m_rrc->RemoveDownLinkFlow(applicationId);  // remove resources in RLC layer
  if (m_nodeBManager != 0)
  {
     m_nodeBManager->ReleaseReplyReceived (m_nodeIdentifier,applicationId);
  }
}

void
UmtsUserEquipmentManager::HandoverProcedure ()
{  
  ConnectionLoss ();
}


//!!! Modify function !!!
void
UmtsUserEquipmentManager::ConnectionReply (Ptr<ControlPacket> controlpacket)
{
  NS_LOG_DEBUG ("UmtsUserEquipmentManager " << m_nodeIdentifier << " Setup Reply Received from Node B " << controlpacket->GetSourceNodeIdentifier () << " Ip " << controlpacket->GetSourcev6 ());

  m_phy->SetupReplyReceived (controlpacket);
  m_rrc->SetupReplyReceived (controlpacket);
  //Additional code for Ipv6 support
  if (Ipv4Address::IsMatchingType(controlpacket->GetSource ()))
  {
        m_nodeBIp = controlpacket->GetSource ();
  }
  else
  {
        m_nodeBIp6 = controlpacket->GetSourcev6 ();
  }

}

/**The next function establishes the kind of application it is going to be configured**/

uint64_t
UmtsUserEquipmentManager::NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t size)
{
 NS_LOG_FUNCTION(this);
  //3,5
  Time time = Seconds (RandomVariable (UniformVariable (3,5)).GetValue ());
       
  if (m_nodeBIdentifier != (uint32_t)-1)
    {
	 //std::cout<<"UmtsUserEquipmentManager::NewApplicationConfiguration 1"<<std::endl;
     
      if(m_rrc->ConfigureNewFlow (type,rate,applicationId,m_nodeBIdentifier))
      {
	if(type=="AM")
	{
	  if(m_nodeBManager->ResourceRequestReceived (this,type,rate,applicationId,size))
	  {	    
	    return applicationId;
	  }
	  else
	  {
	   return (uint64_t)-1; 
	  }
	  
	}
	else
	{	  
	  return applicationId; 
	}
		
      }
         return (uint64_t)-1; 
    }
  else
    {	 
	 Simulator::Schedule (time,&UmtsUserEquipmentManager::NewApplicationConfiguration,this,type,applicationId,rate,size);
	 return (uint64_t)-1;
    }
}

/**---------------------------------------Interference Measurement-----------------------------------------------*/

/** The following function calculates the internal DL interference*/

void
UmtsUserEquipmentManager::CalculateDownlinkInterference ()
{
  double externalInterference = 0;

  if (m_nodeBIdentifier != (uint32_t)-1)
    {
      for (NodeBListI i = m_nodeBList.begin (); i != m_nodeBList.end (); i++)
        {

          if ((*i) != m_nodeBManager && (*i)->GetDownlinkFrequency () == m_nodeBManager->GetDownlinkFrequency ())
            {
              externalInterference += pow (10.0,((*i)->CalculateDownlinkInterference (GetPhy ())) / 10);

            }
        }
    }
  m_phy->SetCalculatedInternalInterferenceDownlink (externalInterference);


}

/**The following function is used to calculate the external Interference*/

double
UmtsUserEquipmentManager::CalculateExternalInterference (Ptr<UmtsPhyLayerBS> phyNodeB_)
{

  double loss = ReturnCost231Loss (phyNodeB_->GetMobility (),m_phy->GetMobility (),300000000 / m_phy->GetTxFrequency (),phyNodeB_->GetShadowing (),m_phy->GetMinDistance (),10,10);

  double txpower = m_phy->GetDedicatedTxPower () + m_phy->GetAntennaGain () + phyNodeB_->GetAntennaGain ();

  loss = loss + txpower;

  return loss;
}

double
UmtsUserEquipmentManager::ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                                  double nodeBHeight,double nodeUEHeight)
{
  m_costPropagationModel->SetLambda (lambda);
  m_costPropagationModel->SetShadowing (shadowing);
  m_costPropagationModel->SetMinDistance (minDistance);
  m_costPropagationModel->SetBSAntennaHeight (nodeBHeight);
  m_costPropagationModel->SetSSAntennaHeight (nodeUEHeight);
  double loss = m_costPropagationModel->GetLoss (a,b);

  return loss;

}

/**-------------------------------End of Interference Measurement----------------------------------------------*/

void
UmtsUserEquipmentManager::AddNodeBStation (Ptr<UmtsBaseStationManager> manager_)
{  
  m_nodeBList.push_back (manager_);
}


void
UmtsUserEquipmentManager::ResourceReplyReceived (uint32_t applicationId,uint32_t size)
{  
  m_rrc->ResourceReplyReceived (applicationId,size);
}

Ptr<UmtsBaseStationManager>
UmtsUserEquipmentManager::GetUmtsBaseStationManager (uint32_t nodeIdentifier)
{
  for (NodeBListI i = m_nodeBList.begin (); i != m_nodeBList.end (); i++)
    {

      if ((*i)->GetNodeIdentifier() == nodeIdentifier)
        {
          return (*i);

        }
    }
  return 0;

}

void
UmtsUserEquipmentManager::PagingReceived (uint32_t address,std::string type,uint32_t applicationId)
{
  int value=0; 
      
  if(m_rrc->PagingReceived(type,applicationId,address)!=-1)
  {      
      value=m_nodeBManager->PagingResponse(m_nodeIdentifier,m_myIp,applicationId);
      
      if(value==0)	//The NodeB cannot allocate more resources
      {
	RemoveDownLinkFlow(applicationId);
      }
      
  }
  else	//The Node UE cannot allocate more resources
  {
      m_nodeBManager->PagingResponse(m_nodeIdentifier,m_myIp,(uint32_t)-1);
  }    
}

}

