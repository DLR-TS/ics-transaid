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

#include "dvbh-channel.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/net-device.h"
#include "dvbh-controlpacket.h"
#include "dvbh-manager.h"
#include "dvbh-ofdm-layer.h"
#include "ns3/cost231_model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"

NS_LOG_COMPONENT_DEFINE ("DVBHChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHChannel);

TypeId
DVBHChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHChannel")
    .SetParent<Channel> ()
    .AddConstructor<DVBHChannel> ()
  ;
  return tid;
}


DVBHChannel::DVBHChannel ()
{  
  m_costPropagationModel = Create<Cost231Propagation> ();
}

DVBHChannel::~DVBHChannel ()
{  
}


void
DVBHChannel::ReceiveFromBaseStation (struct TSPacket tspacket)
{
  for (std::list< Ptr<DVBHOfdmLayer> >::iterator i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {
      (*i)->ReceivePacketFromChannel (tspacket);
    }

}

void
DVBHChannel::SendToUE (struct OFDMBlock block, bool isFirstBlock, uint64_t frequency,Ptr<DVBHQueue> burst,double txPower)
{
  double dbPower = 0;  
  
  for (RegistryListI iter = m_registryList.begin (); iter != m_registryList.end (); iter++)
    {

      if ((uint32_t)(*iter).serviceId == block.serviceId)
        {	  
	  if((*iter).serviceType=="BROADCAST")
	  {	    
	    
	    for (std::list<Ptr<DVBHOfdmLayer> >::iterator i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
            {
              dbPower = CalculateRxPowerFromBaseStation (*i);	      
	      Simulator::ScheduleWithContext((*i)->GetNodeIdentifier(),MicroSeconds(10),&DVBHOfdmLayer::StartReceive,(*i),block,
					     isFirstBlock,frequency,burst->Copy (),dbPower);

            }
	  }
	  else	//Case of Multicast Transmission
	  {
	    for (std::list<Ptr<DVBHOfdmLayer> >::iterator i = (*iter).destinationList.begin (); i != (*iter).destinationList.end (); i++)
            {
              dbPower = CalculateRxPowerFromBaseStation (*i);              
              (*i)->StartReceive (block,isFirstBlock,frequency,burst->Copy (),dbPower);
            }
	  }         
          break;
        }
    }
}

void
DVBHChannel::AddNewService (int serviceId,std::string type)
{
  if (!m_registryList.empty ())
    {
      for (RegistryListI iter = m_registryList.begin (); iter != m_registryList.end (); ++iter)
        {
          if (serviceId == (*iter).serviceId)
            {
              return;
            }
        }
      ServiceRegistry registry;
      registry.serviceId = serviceId;
      registry.serviceType=type;

      m_registryList.push_back (registry);
    }
  else
    {
      ServiceRegistry registry;
      registry.serviceId = serviceId;
      registry.serviceType=type;      
      m_registryList.push_back (registry);
    }
}

void
DVBHChannel::RemoveService (int serviceId)
{
  for (RegistryListI iter = m_registryList.begin (); iter != m_registryList.end (); ++iter)
    {
      if (serviceId == (*iter).serviceId)
        {
          iter = m_registryList.erase (iter);
          break;
        }
    }
}

void
DVBHChannel::RemoveUserFromService (int serviceId,Ptr<DVBHOfdmLayer> ofdm)
{
  for (RegistryListI iter = m_registryList.begin (); iter != m_registryList.end (); ++iter)
    {
      if (serviceId == (*iter).serviceId)
        {

          for (std::list<Ptr<DVBHOfdmLayer> >::iterator i = (*iter).destinationList.begin (); i != (*iter).destinationList.end (); i++)
            {
              if (ofdm == (*i))
                {
                  i = (*iter).destinationList.erase (i);
                  return;
                }
            }
        }
    }
}

void
DVBHChannel::AddNewDestination (int serviceId,Ptr<DVBHOfdmLayer> ofdm)
{
  for (RegistryListI iter = m_registryList.begin (); iter != m_registryList.end (); ++iter)
    {

      if (serviceId == (*iter).serviceId)
        {

          for (std::list<Ptr<DVBHOfdmLayer> >::iterator i = (*iter).destinationList.begin (); i != (*iter).destinationList.end (); i++)
            {

              if (ofdm->GetNodeIdentifier () == (*i)->GetNodeIdentifier ())
                {
                  return;
                }
            }          
          (*iter).destinationList.push_back (ofdm);
          break;
        }
    }
}


void
DVBHChannel::AddNewUserEquip (Ptr<DVBHOfdmLayer> node)
{  
  m_nodeUEList.push_back (node);
}

void
DVBHChannel::RemoveUserEquip (Ptr<DVBHOfdmLayer> node)
{
  if (!m_nodeUEList.empty ())
    {
      for (std::list< Ptr<DVBHOfdmLayer> >::iterator i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
        {
          if ((*i)->GetIdentifier () == node->GetIdentifier ())
            {
              m_nodeUEList.erase (i);
            }
        }
    }
}

double
DVBHChannel::CalculateRxPowerFromBaseStation (Ptr<DVBHOfdmLayer> destinationOfdmLayer)
{
  double txpower = 0;

  txpower = m_ofdmLayer->GetTxPower () + m_ofdmLayer->GetAntennaGain () + destinationOfdmLayer->GetAntennaGain ();
    
  double loss = ReturnCost231Loss (m_ofdmLayer->GetMobility (),destinationOfdmLayer->GetMobility (),m_ofdmLayer->GetLambda (),m_ofdmLayer->GetShadowing (),m_ofdmLayer->GetMinDistance ()/1000,
                                   m_ofdmLayer->GetAntennaHeight (),destinationOfdmLayer->GetAntennaHeight ());
        
  return txpower + loss+m_ofdmLayer->GetAntennaGain()+destinationOfdmLayer->GetAntennaGain();
}

double
DVBHChannel::ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
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

void
DVBHChannel::SetOfdmOfBaseStation (Ptr<DVBHOfdmLayer> ofdmLayer)
{
  m_ofdmLayer = ofdmLayer;
}


Ptr<DVBHChannel>
DVBHChannel::GetChannel ()
{
  return this;
}

void
DVBHChannel::SendToBaseStation (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  /*Not Implemented*/
}


} // namespace ns3
