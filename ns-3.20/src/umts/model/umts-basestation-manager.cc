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
#include "umts-basestation-manager.h"
#include "ns3/log.h"
#include "controlpacket.h"
#include "ns3/random-variable.h"
#include "umts-userequipment-manager.h"
#include "umts-channel.h"
#include "umts-phy-layer-bs.h"
#include "umts-rrc-layer-bs.h"
#include "ns3/mobility-model.h"
#include "cost231_model.h"
#include "ns3/simulator.h"
#include "ns3/itetris-types.h"

NS_LOG_COMPONENT_DEFINE ("UmtsBaseStationManager");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsBaseStationManager);


TypeId
UmtsBaseStationManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsBaseStationManager")
    .SetParent<Object> ()
    .AddConstructor<UmtsBaseStationManager> ()
  ;
  return tid;
}

static int uplinkFreq = 1;

UmtsBaseStationManager::UmtsBaseStationManager ()
{
  m_coverageRange=0;
  m_nodeIdentifier = (uint32_t)-1;
  m_myIp.Set ("0");

  m_costPropagationModel = Create<Cost231Propagation> ();
  ObtainFrequency ();
        
}

void
UmtsBaseStationManager::SetPhy (Ptr<UmtsPhyLayerBS> phy_)
{
  m_phy = phy_;  
  m_phy->SetDownlinkFrequency (m_downlinkFrequency);
  m_phy->SetInterferenceCallback (MakeCallback (&UmtsBaseStationManager::ReturnExternalUplinkInterference, this));
  m_coverageRange=m_phy->GetCoverageRange();  
}

void
UmtsBaseStationManager::ObtainFrequency ()
{
  m_uplinkFrequency = uplinkFreq;
  m_downlinkFrequency = m_uplinkFrequency + 12;
  uplinkFreq++;

  if (uplinkFreq == 12)
    {
      uplinkFreq = 1;
    }
}

void
UmtsBaseStationManager::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
  m_rrc->SetNodeIdentifier(identifier);
  
}

uint32_t
UmtsBaseStationManager::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

uint32_t 
UmtsBaseStationManager::GetNodeUEIdentifier(Ipv4Address address)
{
  for (NodeUEListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetIpAddress () == address)
        {
          return (*i)->GetNodeIdentifier();

        }
    }
  return (uint32_t)-1;
  
}

uint32_t 
UmtsBaseStationManager::GetNodeUEIdentifier(Ipv6Address address)
{
  for (NodeUEListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetIpv6Address () == address)
        {
          return (*i)->GetNodeIdentifier();

        }
    }
  return (uint32_t)-1;
  
}


void
UmtsBaseStationManager::AddNodeUE (Ptr<UmtsUserEquipmentManager> manager_)
{  
  m_nodeUEList.push_back (manager_);
}

void
UmtsBaseStationManager::RegisterNewUser(Ptr<UmtsUserEquipmentManager> manager_)
{
  NS_LOG_DEBUG ("Node B "<<m_nodeIdentifier<<" Manager // Registering Node UE "<<manager_->GetNodeIdentifier());
  m_registeredUsers.push_back (manager_);
}

void
UmtsBaseStationManager::RemoveNodeUE (Ptr<UmtsUserEquipmentManager> manager_)
{
  if (!m_registeredUsers.empty ())
    {
      for (NodeUEListI i = m_registeredUsers.begin (); i != m_registeredUsers.end (); i++)
        {

          if ((*i) == manager_)
            {

              m_registeredUsers.erase (i);            
              break;
            }
        }
    }

}

Ptr<UmtsUserEquipmentManager>
UmtsBaseStationManager::FindNodeUE (uint32_t identifier,NodeUEList nodeUEList_)
{
  for (NodeUEListI i = nodeUEList_.begin (); i != nodeUEList_.end (); i++)
    {

      if ((*i)->GetNodeIdentifier () == identifier)
        {
          return (*i);
        }
    }
  return 0;

}

Ptr<UmtsUserEquipmentManager>
UmtsBaseStationManager::FindNodeUE (Ipv4Address destinationIp,NodeUEList nodeUEList_)
{
 for (NodeUEListI i = nodeUEList_.begin (); i != nodeUEList_.end (); i++)
    {

      if ((*i)->GetIpAddress() == destinationIp)
        {
          return (*i);

        }
    }
  return 0;
}

Ipv4Address*
UmtsBaseStationManager::GetNodeUEIpAddress(uint32_t nodeIdentifier)
{
  Ipv4Address* address=new Ipv4Address();
  
  if(nodeIdentifier==(uint32_t)-1)
  {	
      address->Set("0");
      return address ;
  }
  
  for (NodeUEListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetNodeIdentifier() == nodeIdentifier)
        {
	  
	  address->Set(((*i)->GetIpAddress()).Get());
          return address;

        }
    }
    return NULL;
}



Ipv6Address*
UmtsBaseStationManager::GetNodeUEIpv6Address(uint32_t nodeIdentifier)
{
  Ipv6Address* address;
  
  if(nodeIdentifier==(uint32_t)-1)
  {	
	  address=new Ipv6Address("0");
      return address ;
  }
  
  for (NodeUEListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetNodeIdentifier() == nodeIdentifier)
        {
    	  address=new Ipv6Address((*i)->GetIpv6Address());

    	  //modificato, mi sembrava stupido fatto com'era prima...
    	  /*uint8_t* addr_buf = (uint8_t*) malloc (16*sizeof (uint8_t));
	  	  address->Set(((*i)->GetIpv6Address()).GetBytes(addr_buf));
	  	  m_rrc->AddUEIpv6Address(nodeIdentifier, *address);*/
          return address;

        }
    }
    return NULL;
}


int
UmtsBaseStationManager::GetDownlinkFrequency ()
{
  return m_downlinkFrequency;
}

void
UmtsBaseStationManager::SetIpAddress(Ipv4Address address)
{
  m_myIp=address;  
  m_rrc->SetIpAddress(m_myIp);
}

void
UmtsBaseStationManager::SetIpv6Address(Ipv6Address address)
{
  m_myIp6=address;  
 //std::cout<<"Check 9:"<<m_myIp6<<std::endl;
  m_rrc->SetIpv6Address(m_myIp6);
}

Ipv4Address
UmtsBaseStationManager::GetIpAddress ()
{
  return m_myIp;
}

Ipv6Address
UmtsBaseStationManager::GetIpv6Address ()
{
  return m_myIp6;
}


double
UmtsBaseStationManager::GetCoverageRange (void)
{
  return m_coverageRange;
}

void
UmtsBaseStationManager::SetCoverageRange (double range)
{
  m_coverageRange = range;
}

Ptr<MobilityModel>
UmtsBaseStationManager::GetMobility ()
{

  return m_phy->GetMobility ()->GetObject<MobilityModel> ();
}

Ptr<UmtsPhyLayerBS>
UmtsBaseStationManager::GetPhy ()
{
  return m_phy;

}

void
UmtsBaseStationManager::AddUmtsBaseStationManager (Ptr<UmtsBaseStationManager> manager_)
{
  m_nodeBList.push_back (manager_);
}

void
UmtsBaseStationManager::SetRRC (Ptr<UmtsRrcLayerBS> rrc_)
{
  m_rrc = rrc_;  
  m_rrc->SetDownlinkFrequency (m_downlinkFrequency);  
}

void
UmtsBaseStationManager::RemoveApplication(uint32_t applicationId,std::string type,uint32_t destinationId)
{
  Ptr<UmtsUserEquipmentManager> manager_ = FindNodeUE (destinationId,m_registeredUsers);
  
  if (manager_ != 0)
    {
      manager_->RemoveDownLinkFlow (applicationId);
    }    
}

void
UmtsBaseStationManager::TriggerNodeUEScanning()
{
  for(NodeUEListI iterator=m_nodeUEList.begin();iterator!=m_nodeUEList.end();iterator++)
  {    
    (*iterator)->PeriodicScanning(IPv6);
  }
}

void
UmtsBaseStationManager::TriggerNodeUEScanning(uint32_t nodeIdentifier)
{
	std::cout<<"UmtsBaseStationManager::TriggerNodeUEScanning"<<std::endl;
  for(NodeUEListI iterator=m_nodeUEList.begin();iterator!=m_nodeUEList.end();iterator++)
  {
    if((*iterator)->GetNodeIdentifier()==nodeIdentifier)
    {
      (*iterator)->PeriodicScanning(IPv6);
      break;
    }        
  }
}

/**-------------------------------------MBMS MANAGEMENT-----------------------------------------------------**/

void
UmtsBaseStationManager::AddNewSubscriber(std::string serviceName,uint32_t identifier)
{
   for (std::list<struct UmtsService >::iterator i = m_serviceList.begin (); i != m_serviceList.end (); i++)
    {
      if ((*i).name== serviceName)
        {
          for (std::list<uint32_t>::iterator j = (*i).subscriberList.begin (); j != (*i).subscriberList.end (); j++)
            {
              if ((*j)== identifier)
                {
                  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " Manager// User with ID "<<identifier<<" already in service " << (*i).name << ".Time " << Simulator::Now ());
                  return;
                }
            }
            (*i).subscriberList.push_back(identifier);
	    m_phy->AddNewUserToService((*i).id,identifier);
        }
    }
}

void
UmtsBaseStationManager::DeleteSubscriber(std::string serviceName,uint32_t identifier)
{
  for (std::list<struct UmtsService >::iterator i = m_serviceList.begin (); i != m_serviceList.end (); i++)
    {
      if ((*i).name== serviceName)
        {
          for (std::list<uint32_t>::iterator j = (*i).subscriberList.begin (); j != (*i).subscriberList.end (); j++)
            {
              if ((*j)== identifier)
                {
                  (*i).subscriberList.erase(j);
                  break;
                }
            }
            break;
            
        }
    }
}

void
UmtsBaseStationManager::RemoveNodeUEFromServices (uint32_t identifier)
{
  for (std::list<struct UmtsService >::iterator i = m_serviceList.begin (); i != m_serviceList.end (); i++)
    {
      for (std::list<uint32_t>::iterator k = (*i).subscriberList.begin (); k != (*i).subscriberList.end (); k++)
        {
          if ((*k) == identifier)
            {
              NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " Manager// Removing subscriber " << identifier<< " from service " << (*i).name << ".Time " << Simulator::Now () << " \n");
              (*i).subscriberList.erase (k);
              break;
            }

        }
    }
}


/**-----------------END OF MBMS MANAGEMENT-----------------------------------------------------**/


/** The following function configures the new application, establishing if it is going to be implemented through the common or the dedicated
**  channel.
*/

uint64_t
UmtsBaseStationManager::NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t destinationId,Ipv4Address address)
{

  if(destinationId==ID_BROADCAST||destinationId==ID_MULTICAST)
  {
    m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,address);
    return applicationId;
  }
  else
  {
    Ptr<UmtsUserEquipmentManager> manager_ = FindNodeUE(destinationId,m_registeredUsers);
  
    if(manager_==0)
    {
      NS_LOG_DEBUG ("Node B "<<m_nodeIdentifier<<" Manager// It has been imposible to configure an application for the Node "<<destinationId<<
      ".It is not already registered.");
	
      Time time = Seconds (RandomVariable (UniformVariable (2,4)).GetValue ());
      //Simulator::Schedule (time,&UmtsBaseStationManager::NewApplicationConfiguration,this,type,applicationId,rate,destinationId,address);
      return (uint64_t)-1;
    }
    
    if(type!="AM")
    {
     //std::cout<<"UmtsBaseStationManager::NewApplicationConfiguration 23"<<std::endl;
      if(m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,manager_->GetIpAddress()))
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

      if(m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,manager_->GetIpAddress()))
      {
	PagingRequest(manager_,type,rate,applicationId);
	return applicationId;
      }
      return (uint64_t)-1;
	    
    }
  }
  
  	         
}


uint64_t
UmtsBaseStationManager::NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t destinationId,Ipv6Address address)
{
	//std::cout<<"UmtsBaseStationManager::NewApplicationConfiguration 11"<<std::endl;
  if(destinationId==ID_BROADCAST||destinationId==ID_MULTICAST)
  {
    m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,address);
    return applicationId;
  }
  else
  {
    Ptr<UmtsUserEquipmentManager> manager_ = FindNodeUE(destinationId,m_registeredUsers);
  
    if(manager_==0)
    {
      NS_LOG_DEBUG ("Node B "<<m_nodeIdentifier<<" Manager// It has been imposible to configure an application for the Node "<<destinationId<<
      ".It is not already registered.");
	
      Time time = Seconds (RandomVariable (UniformVariable (2,4)).GetValue ());
      //Simulator::Schedule (time,&UmtsBaseStationManager::NewApplicationConfiguration,this,type,applicationId,rate,destinationId,address);
      return (uint64_t)-1;
    }
    
    if(type!="AM")
    {
    	std::cout<<"UmtsBaseStationManager::NewApplicationConfiguration 2"<<std::endl;
      if(m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,manager_->GetIpv6Address()))
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
    	std::cout<<"UmtsBaseStationManager::NewApplicationConfiguration 3"<<std::endl;
      if(m_rrc->ConfigureNewFlow (type,rate,destinationId,applicationId,manager_->GetIpv6Address()))
      {
	PagingRequest(manager_,type,rate,applicationId);
	return applicationId;
      }
      return (uint64_t)-1;
	    
    }
  }
  
  	         
}

void
UmtsBaseStationManager::PagingRequest(Ptr<UmtsUserEquipmentManager> manager_,std::string type,double rate,uint32_t applicationId)
{    
  manager_->PagingReceived (m_nodeIdentifier,type,applicationId);
}


/** ---------------------------------------------INTERFERENCE MEASUREMENT---------------------------------------------*/

/**This function is used to calculate the path Loss (substitute it with the Cost 231 Method)
*/

double
UmtsBaseStationManager::CalculatePropagationLoss (Ptr<UmtsPhyLayerUE> phyNodeUE,double power)
{

  double txpower = power + m_phy->GetAntennaGain () + phyNodeUE->GetAntennaGain ();
  double loss = ReturnCost231Loss (phyNodeUE->GetMobility (),m_phy->GetMobility (),300000000 / phyNodeUE->GetTxFrequency (),m_phy->GetShadowing (),phyNodeUE->GetMinDistance (),10,10);
  loss = loss + txpower;

  return loss;
}

double
UmtsBaseStationManager::ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
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

/**This function is called to calculate the external interference in the UL. This task is fulfilled taking into account the interference
***provoked by other users locating in different cells which share the same UL frequency than the users in the NodeB's cell.
*/

void
UmtsBaseStationManager::ReturnExternalUplinkInterference ()
{
  double externalInterference = 0;

  if (!m_nodeBList.empty ())
    {
      for (NodeBListI i = m_nodeBList.begin (); i != m_nodeBList.end (); i++)
        {

          if ((*i)->GetDownlinkFrequency () == m_downlinkFrequency)
            {
              externalInterference += (*i)->CalculateExternalUplinkInterference (m_phy);

            }
        }
    }
  m_phy->CalculateExternalInterference (externalInterference);
}

/**This function is used to return the external Interference provoked by the Node UE attached to the NodeB which are causing troubles to the
***transmission of other NodeUE located in a different cell
*/

double
UmtsBaseStationManager::CalculateExternalUplinkInterference (Ptr<UmtsPhyLayerBS> phyNodeB_)
{
  double externalInterference = 0;

  if (!m_nodeUEList.empty ())
    {
      for (NodeUEListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
        {
          if ((*i)->GetPhy ()->GetTransmitting ())
            {
              externalInterference += pow (10.0,(*i)->CalculateExternalInterference (phyNodeB_) / 10);             
            }


        }
    }
  return externalInterference;
}

/**This function is used to calculate the downlink interference. In order to calculat the interference level it is checked wether the NodeB is
***transmitting to any NodeUE while measuring the interference
*/

double
UmtsBaseStationManager::CalculateDownlinkInterference (Ptr<UmtsPhyLayerUE> phyNodeUE)
{
  double externalInterference = 0;

  for (int i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_phy->m_nodeUEInformation[i].transmitting == 1&&m_phy->LookForUE (phyNodeUE->GetNodeIdentifier ()) != i)
        {
          externalInterference += CalculatePropagationLoss (phyNodeUE,m_phy->m_nodeUEInformation[i].dedicatedTxPower);
        }

    }
  return externalInterference;
}

/**-------------------------------END OF INTERFERENCE CALCULATION----------------------------------*/

/**------------------------------RRC SIGNALLING FUNCTIONS------------------------------------------*/

/**The following function is used to set a new connection with a NodeUE
**/


int
UmtsBaseStationManager::NewConnection ( Ptr<ControlPacket> controlpacket)
{
  if (m_phy->IsPacketForMe (controlpacket))
    {
      controlpacket->SetSourceNodeIdentifier (m_phy->m_nodeUEIdRegistry[controlpacket->GetSourceNodeIdentifier ()]);           // change the source address
      
      if (m_rrc->RegisterNewUE (controlpacket))
        {
          // UE register in RRC layer
          m_rrc->UpdateNodeRegistry (controlpacket->GetSourceNodeIdentifier ());                // update the static location matrix

          controlpacket->SetLLType (LL_SETUP_REPLY);
          controlpacket->SetDestination (controlpacket->GetSource ());
          controlpacket->SetDestinationNodeIdentifier (controlpacket->GetSourceNodeIdentifier ());
          controlpacket->SetSource (m_myIp);
          controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);

          m_phy->RegisterUE (controlpacket);
          controlpacket->SetFrequency (m_uplinkFrequency);
          int i = m_phy->LookForUE (controlpacket->GetDestinationNodeIdentifier ());              // looks for the UE in the addresses registry
          m_phy->m_nodeUEInformation[i].ul_freq = controlpacket->GetFrequency ();
          Ptr<UmtsUserEquipmentManager> manager_ = controlpacket->GetUmtsUserEquipmentManager ();
          RegisterNewUser (manager_);
          controlpacket->SetPrimaryScramblingCode (m_phy->GetPrimaryScramblingCode ());

          controlpacket->SetUmtsPhyLayerBSPointer (m_phy);
          manager_->ConnectionReply (controlpacket);
          return 1;

        }
      else
        {
          controlpacket->SetLLType (LL_FAILURE);
          m_phy->RegisterUE (controlpacket);
          return 0;

        }
    }
  return 0;
}


//!!! NEW FUNCTION !!!
int
UmtsBaseStationManager::NewConnection ( Ptr<ControlPacket> controlpacket, STACK stack)
{
  if (m_phy->IsPacketForMe (controlpacket))
    {
      controlpacket->SetSourceNodeIdentifier (m_phy->m_nodeUEIdRegistry[controlpacket->GetSourceNodeIdentifier ()]);           // change the source address
      
      if (m_rrc->RegisterNewUE (controlpacket))
        {
          // UE register in RRC layer
          m_rrc->UpdateNodeRegistry (controlpacket->GetSourceNodeIdentifier ());                // update the static location matrix

          controlpacket->SetLLType (LL_SETUP_REPLY);
          controlpacket->SetDestination (controlpacket->GetSource ());
          controlpacket->SetDestinationNodeIdentifier (controlpacket->GetSourceNodeIdentifier ());
          if(stack == IPv4)
          {
                controlpacket->SetSource (m_myIp);
          }
          else
          {
                controlpacket->SetSourcev6 (m_myIp6);
          }

          controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);

          m_phy->RegisterUE (controlpacket);
          controlpacket->SetFrequency (m_uplinkFrequency);
          int i = m_phy->LookForUE (controlpacket->GetDestinationNodeIdentifier ());              // looks for the UE in the addresses registry
          m_phy->m_nodeUEInformation[i].ul_freq = controlpacket->GetFrequency ();
          Ptr<UmtsUserEquipmentManager> manager_ = controlpacket->GetUmtsUserEquipmentManager ();
          RegisterNewUser (manager_);
          controlpacket->SetPrimaryScramblingCode (m_phy->GetPrimaryScramblingCode ());

          controlpacket->SetUmtsPhyLayerBSPointer (m_phy);
          manager_->ConnectionReply (controlpacket);
          return 1;

        }
      else
        {
          controlpacket->SetLLType (LL_FAILURE);
          m_phy->RegisterUE (controlpacket);
          return 0;

        }
    }
  return 0;
}


int
UmtsBaseStationManager::PagingResponse (uint32_t uePhyAddress,Ipv4Address address,uint32_t applicationId)
{
  if(applicationId!=(uint32_t)-1)
  {
    int i = m_phy->LookForUE (uePhyAddress);
    m_phy->m_nodeUEInformation[i].channel->SetUmtsPhyLayerBS (m_phy);  
    m_phy->m_nodeUEInformation[i].channel->SetDedicatedChannelPeer ((FindNodeUE(uePhyAddress,m_registeredUsers))->GetPhy());
    
    if(m_rrc->PagingOkReceived (uePhyAddress,applicationId))
    {
      NS_LOG_DEBUG ("NodeB" << m_nodeIdentifier << "Manager // New Resources for Application " <<applicationId<<
      " correctly allocated");
      return 1; 	//The resources have been allocated successfully
    }
    else
    {
      RemoveApplication(applicationId,"AM",uePhyAddress);
      NS_LOG_DEBUG ("NodeB" << m_nodeIdentifier << "Manager // New Resources for Application " <<applicationId<<
      " not allocated");
      return 0; 	//The Base Station is not capable of allocating the desired resources
    }
  }
  else //The Node UE is not capable of receiving any dedicated data. Deleting Allocated Resources
  {
    NS_LOG_DEBUG ("NodeB" << m_nodeIdentifier << "Manager // The Node UE "<<uePhyAddress<<
    " is not capable of receiving any dedicated data");
    return 0;
  }
}

void
UmtsBaseStationManager::ConnectionRelease (Ptr<UmtsUserEquipmentManager> manager_)
{  
  NodeUEConnectionLoss (manager_);
  m_rrc->NodeUEConnectionLoss(manager_->GetNodeIdentifier(),manager_->GetIpAddress());
  
}

void
UmtsBaseStationManager::ULApplicationReleaseRequest (uint32_t flowId,uint32_t source)
{
  int i = m_rrc->LookForUE (source);

  m_rrc->RemoveUpLinkFlow (flowId,i);          // remove the uplink resources allocated for that flow
  
  FindNodeUE(source,m_registeredUsers)->ReleaseReply (flowId);

}

void
UmtsBaseStationManager::ReleaseReplyReceived (uint32_t destinationId,uint32_t applicationId)
{
   NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " Manager. Removing Downlink Flow with ID "<<applicationId);
   m_rrc->RemoveDownLinkFlow (destinationId,applicationId);

}

int
UmtsBaseStationManager::ResourceRequestReceived (Ptr<UmtsUserEquipmentManager> manager_,std::string type,double rate,uint32_t applicationId,uint32_t size)
{
      
      if(m_rrc->ResourceRequestReceived(type,applicationId,manager_->GetNodeIdentifier()))
      {	    
	    int i = m_phy->LookForUE (manager_->GetNodeIdentifier());
	    
	    if(i!=-1)
	    {
	      m_phy->m_nodeUEInformation[i].ul_freq = m_uplinkFrequency;
	      m_phy->m_nodeUEInformation[i].channel->SetUmtsPhyLayerBS (m_phy);
	      m_phy->m_nodeUEInformation[i].channel->SetDedicatedChannelPeer (manager_->GetPhy());
	      manager_->ResourceReplyReceived (applicationId,size);
	    }
	    
	return 1;    
      }
      else
      {
	return 0;
      }
	
}

void
UmtsBaseStationManager::NodeUEConnectionLoss (Ptr<UmtsUserEquipmentManager> manager_)
{
  RemoveNodeUE (manager_);
  RemoveNodeUEFromServices (manager_->GetNodeIdentifier());
}

uint32_t
UmtsBaseStationManager::getRegistedUsers() {
	return (uint32_t) m_registeredUsers.size();
}

/*NodeUEList
UmtsBaseStationManager::getUsersList(){
	return m_registeredUsers;
}*/

}

