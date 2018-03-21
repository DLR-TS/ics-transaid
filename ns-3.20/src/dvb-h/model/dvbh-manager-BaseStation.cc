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


#include "dvbh-link-layer-BaseStation.h"
#include "dvbh-manager-BaseStation.h"
#include "dvbh-manager.h"
#include "dvbh-channel.h"
#include "dvbh-manager-UserEquip.h"
#include "dvbh-ofdm-layer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "dvbh-controlpacket.h"
#include "ns3/random-variable.h"
#include "dvbh-tags.h"
#include "dvbh-link-layer-BaseStation.h"
#include "ns3/mobility-model.h"
#include "ns3/app-index-tag.h"
#include <list>

NS_LOG_COMPONENT_DEFINE ("DVBHManagerBaseStation");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHManagerBaseStation);


static int TransportStreamId = 0;


TypeId
DVBHManagerBaseStation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHManagerBaseStation")
    .SetParent<Object> ()   
  ;
  return tid;
}

DVBHManagerBaseStation::DVBHManagerBaseStation ()
{
  m_myIp.Set ("0");
  m_coverageRange = 30000;
  m_mpeFecEnabled = false;
  m_nodeIdentifier = 0;
}

void
DVBHManagerBaseStation::DoDispose ()
{

}

void
DVBHManagerBaseStation::SetLinkLayer (Ptr<DVBHLinkLayerBaseStation> linkLayer_)
{
  m_linkLayer = linkLayer_;

}

void
DVBHManagerBaseStation::SetNodeIdentifier (uint32_t nodeIdentifier)
{
  m_nodeIdentifier= nodeIdentifier;
  m_linkLayer->SetNodeIdentifier (nodeIdentifier);
}

uint32_t
DVBHManagerBaseStation::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHManagerBaseStation::SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_)
{
  m_ofdm = ofdm_;
}

Ptr<DVBHOfdmLayer>
DVBHManagerBaseStation::GetOfdmLayer ()
{
  return m_ofdm;
}

void
DVBHManagerBaseStation::SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHManagerBaseStation::ForwardUp (Ptr<Packet> packet)
{
  m_forwardUp (packet);
}

void
DVBHManagerBaseStation::SetIpAddress (Ipv4Address address)
{
  m_myIp = address;
}

Ipv4Address
DVBHManagerBaseStation::GetIpAddress ()
{
  return m_myIp;
}

void
DVBHManagerBaseStation::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
  m_ofdm->SetMobility (mobility);

}

Ptr<MobilityModel>
DVBHManagerBaseStation::GetMobility ()
{
  return m_mobility;
}

void
DVBHManagerBaseStation::EnableMpeFec ()
{
  m_mpeFecEnabled = true;
  m_linkLayer->EnableMpeFec ();
}

void
DVBHManagerBaseStation::DisableMpeFec ()
{
  m_mpeFecEnabled = false;
  m_linkLayer->DisableMpeFec ();
}

double
DVBHManagerBaseStation::GetCoverageRange (void)
{
  return m_coverageRange;
}

void
DVBHManagerBaseStation::SetCoverageRange (double range)
{
  m_coverageRange = range;
}

void
DVBHManagerBaseStation::ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket)
{

}

void
DVBHManagerBaseStation::AddNewTransportStream (double transmissionRate,std::string name)
{
  TransportStream stream;
  stream.id = TransportStreamId;
  TransportStreamId++;
  stream.name = name;
  stream.bandwidth = transmissionRate;
  m_transportStreamList.push_back (stream);
  m_linkLayer->AddNewTS (name,transmissionRate);
  m_linkLayer->AddPatSection (stream.id);
  m_linkLayer->AddSdtSection (stream.id);
}


void
DVBHManagerBaseStation::AddNewService (std::string serviceName,uint32_t serviceId,Ipv4Address address,uint32_t burstSize,std::string transmissionType,std::string type,int transportStreamId)
{
  
  DVBHService service;
  service.serviceName=serviceName;
  service.packetSize = burstSize; 
  service.joined = false;
  service.ipAddress = address;
  service.serviceId = serviceId;
  service.serviceType=transmissionType;
  
  m_serviceList.push_back (service);
  NS_LOG_INFO("BaseStation " << m_myIp << " Manager// Adding service " << service.serviceName << " with ID "<<serviceId<<" .Time " << Simulator::Now ());
  m_linkLayer->AddEntryPATTable (service.serviceId,serviceId,transportStreamId);
  m_linkLayer->AddEntrySDTTable (service.serviceId,type,transportStreamId);

}

void
DVBHManagerBaseStation::AddElementaryStreamToService (uint32_t serviceId,uint32_t streamId,std::string streamName,int streamSize)
{
  for (ServiceListI i = m_serviceList.begin (); i != m_serviceList.end (); i++)
    {
      if ((*i).serviceId==serviceId)
        {
          NS_LOG_INFO("BaseStation " << m_myIp << " Manager// Adding Stream " << streamName << " to the service " << serviceId<< " .Time " << Simulator::Now ());
          ElementaryStream stream;

          stream.numberOfPackets = (int)streamSize / 4000;
	  stream.name=streamName;
          stream.totalSize = streamSize;
          stream.id = streamId;
          stream.sizeToSend = streamSize;
          (*i).streamList.push_back (stream);
          m_linkLayer->NewServiceStreamAdded (streamId);
          m_linkLayer->AddEntryPMTTable ((*i).serviceId,streamId,"audio");

          m_ofdm->GetChannel ()->AddNewService (stream.id,(*i).serviceType);
          break;
        }

    }
}


void
DVBHManagerBaseStation::RemoveService (std::string serviceName)
{
  if (!m_serviceList.empty ())
    {
      for (ServiceListI i = m_serviceList.begin (); i != m_serviceList.end (); i++)
        {
          if ((*i).serviceName == serviceName)
            {
              m_ofdm->GetChannel ()->RemoveService ((*i).serviceId);
              i = m_serviceList.erase (i);

            }

        }

    }
}

void
DVBHManagerBaseStation::RemoveServiceStream (int streamId)
{
  if (!m_serviceList.empty ())
    {
      for (ServiceListI i = m_serviceList.begin (); i != m_serviceList.end (); i++)
        {
          if ((*i).streamList.empty ())
            {
              for (std::list<struct ElementaryStream>::iterator streamIterator = (*i).streamList.begin (); streamIterator != (*i).streamList.end (); streamIterator++)
                {
                  if (streamId == (*streamIterator).id)
                    {
                      (*i).streamList.erase (streamIterator);
                    }
                }
            }

        }

    }
}

/**Before each service starts sending data it is necessary to inform to each subscriber
**
*/

void
DVBHManagerBaseStation::InformAboutSessionStart (struct DVBHService service)
{
NS_LOG_FUNCTION(this);
  for (std::list<Ptr<DVBHManagerUserEquip> >::iterator i = service.subscriberList.begin (); i != service.subscriberList.end (); i++)
    {
      (*i)->ServiceStartNotice (service);
    }
  Simulator::Schedule (Seconds (service.duration),&DVBHManagerBaseStation::InformAboutSessionStop,this,service);

}

/**When there is no more data belonged to a service it is necessary to inform the subscriber about the end of the service
**
*/

void
DVBHManagerBaseStation::InformAboutSessionStop (struct DVBHService service)
{

  for (std::list<Ptr<DVBHManagerUserEquip> >::iterator i = service.subscriberList.begin (); i != service.subscriberList.end (); i++)
    {
      (*i)->ServiceStopNotice (service);
    }

}

void
DVBHManagerBaseStation::RemoveUserEquipFromServices (Ptr<DVBHManagerUserEquip> manager_)
{
  for (std::list<struct DVBHService >::iterator i = m_serviceList.begin (); i != m_serviceList.end (); i++)
    {
      for (std::list<Ptr<DVBHManagerUserEquip> >::iterator k = (*i).subscriberList.begin (); k != (*i).subscriberList.end (); k++)
        {

          if ((*k) == manager_)
            {
              m_ofdm->GetChannel ()->RemoveUserFromService ((*i).serviceId,(*k)->GetOfdmLayer ());              
              k = (*i).subscriberList.erase (k);
              break;
            }

        }
    }
}

void
DVBHManagerBaseStation::ReceiveFromAbove(Ptr<Packet> packet)
{
  AppIndexTag idTag;
  packet->PeekPacketTag(idTag);
  
  DvbhDeltaTTag deltaTag;
  packet->PeekPacketTag(deltaTag);
  
  DvbhServiceIdTag serviceTag;
  
 //std::cout<<"Receiving Packet from above "<<packet->GetSize()<<std::endl;
      
  for (ServiceListI iterator = m_serviceList.begin (); iterator != m_serviceList.end (); iterator++)
    {
      if ((*iterator).serviceId == idTag.Get())
        {
         
          for (std::list< struct ElementaryStream >::iterator i = (*iterator).streamList.begin (); i != (*iterator).streamList.end (); i++)
	  {	    	    
	    serviceTag.Set((*i).id);
	    Ptr<Packet> newPacket=packet->Copy();
	    newPacket->AddPacketTag(serviceTag);
	    
	    m_linkLayer->ReceiveFromAbove(newPacket,Seconds(deltaTag.Get()),0);
	    	    
	  }

        }
    }
}


Ptr<DVBHChannel>
DVBHManagerBaseStation::GetSharedChannel (Ptr<DVBHManagerUserEquip> manager_)
{
  m_ofdm->GetChannel ()->AddNewUserEquip (manager_->GetOfdmLayer ());
  return m_ofdm->GetChannel ();
}

void
DVBHManagerBaseStation::AddVehicle(Ptr<DVBHManagerUserEquip> nodeUE)
{
  m_nodeUEList.push_back(nodeUE);
}

int
DVBHManagerBaseStation::NewConnection (Ptr<DVBHManagerUserEquip> nodeUE)
{
  int found = 0;
  if (m_registeredUEList.size () != DVBH_MAX_NUM_UE)
    {
      if (!m_registeredUEList.empty ())
        {
          for (UserEquipListI i = m_registeredUEList.begin (); i != m_registeredUEList.end (); i++)
            {
              if ((*i)->GetIpAddress () == nodeUE->GetIpAddress ())
                {
                  found = 1;
                }
            }

          if (found == 0)
            {
              NS_LOG_INFO("BaseStation " << m_myIp << " Manager// Adding Node UE " << nodeUE->GetIpAddress () << ".Time " << Simulator::Now ());
              m_registeredUEList.push_back (nodeUE);
              return 1;
            }
          else
            {
              return 0;
            }
        }
      else
        {
          NS_LOG_INFO("BaseStation " << m_myIp << " Manager// Adding Node UE " << nodeUE->GetIpAddress () << ".Time " << Simulator::Now ());
          m_registeredUEList.push_back (nodeUE);
          return 1;
        }
    }
  else
    {
      return 0;
    }

}

Ipv4Address*
DVBHManagerBaseStation::GetVehicleIpAddress(uint32_t nodeIdentifier)
{
  Ipv4Address* address=new Ipv4Address();
  
  if(nodeIdentifier==(uint32_t)-1)
  {	
      address->Set("0");
      return address ;
  }
  
  for (UserEquipListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetNodeIdentifier() == nodeIdentifier)
        {
	  
	  address->Set(((*i)->GetIpAddress()).Get());
          return address;

        }
    }
    return NULL;
}


//NEW FUNCTION
Ipv6Address*
DVBHManagerBaseStation::GetVehicleIpv6Address(uint32_t nodeIdentifier)
{
  Ipv6Address* address=new Ipv6Address();
  
  if(nodeIdentifier==(uint32_t)-1)
  {	
      address->Set("0");
      return address ;
  }
  
  for (UserEquipListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
    {

      if ((*i)->GetNodeIdentifier() == nodeIdentifier)
        {
	  uint8_t* addr_buf = (uint8_t*) malloc (16*sizeof (uint8_t));
	  address->Set(((*i)->GetIpv6Address()).GetBytes2(addr_buf));
          return address;

        }
    }
    return NULL;
}



void
DVBHManagerBaseStation::RemoveUserEquip (Ptr<DVBHManagerUserEquip> nodeUE)
{

  if (!m_registeredUEList.empty ())
    {
      for (UserEquipListI i = m_registeredUEList.begin (); i != m_registeredUEList.end (); i++)
        {
          if ((*i)->GetIpAddress () == nodeUE->GetIpAddress ())
            {
              i = m_registeredUEList.erase (i);
              RemoveUserEquipFromServices (nodeUE);
              break;
            }
        }
    }
}

void
DVBHManagerBaseStation::TriggerVehicleScanning()
{
  if (!m_nodeUEList.empty ())
    {
      for (UserEquipListI i = m_nodeUEList.begin (); i != m_nodeUEList.end (); i++)
        {
          (*i)->PeriodicScanning();          
        }
    }
}

void
DVBHManagerBaseStation::AddBaseStationManager (Ptr<DVBHManagerBaseStation> manager_)
{
  m_nodeBList.push_back (manager_);
}

void
DVBHManagerBaseStation::AddNewSubscriber (uint32_t serviceId,Ptr<DVBHManagerUserEquip> manager_)
{
  for (ServiceListI iterator = m_serviceList.begin (); iterator != m_serviceList.end (); iterator++)
    {
      if ((*iterator).serviceId == serviceId)
        {
          NS_LOG_INFO("Node B Manager " << m_nodeIdentifier << ". Adding new subscriber " << manager_->GetNodeIdentifier () << " to service " << serviceId);
          for (std::list<struct ElementaryStream >::iterator i = (*iterator).streamList.begin (); i != (*iterator).streamList.end (); i++)
            {
              m_ofdm->GetChannel ()->AddNewDestination ((*i).id,manager_->GetOfdmLayer ());
            }

          (*iterator).subscriberList.push_back (manager_);
          break;
        }
    }
}

uint32_t
DVBHManagerBaseStation::getRegistedUsers() {
	return (uint32_t) m_registeredUEList.size();
}

}
