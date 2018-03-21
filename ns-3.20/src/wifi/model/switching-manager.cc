/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/assert.h" 
#include "switching-manager.h"
#include "ns3/wifi-net-device.h"
#include "ns3/log.h"
#include "ns3/qadhoc-wifi-mac.h"
#include "edca-txop-n.h"
#include "wifi-mac-queue.h"
#include "wifi-mac-header.h"

NS_LOG_COMPONENT_DEFINE ("SwitchingManager");

namespace ns3 {

AcIndex AcVector[] = {AC_VO,AC_VI,AC_BE,AC_BK};

NS_OBJECT_ENSURE_REGISTERED (SwitchingManager);

TypeId 
SwitchingManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SwitchingManager")
    .SetParent<Object> ()
    .AddConstructor<SwitchingManager> ()
    ;
  return tid;
}

SwitchingManager::SwitchingManager ()
{}

SwitchingManager::~SwitchingManager ()
{
  NS_LOG_FUNCTION (this);
}

void
SwitchingManager::SetNetDevice (Ptr<NetDevice> device)
{
  m_device = device;
  SetMacQueues();
}

void 
SwitchingManager::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_device = 0;
  Object::DoDispose ();
}

void 
SwitchingManager::FlushEnqueuedPackets (void)
{
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++)
    {
      (*i)->FlushQueue();
    }
}

void 
SwitchingManager::SetMacQueues (void)
{
  Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(m_device);
  Ptr<QadhocWifiMac> mac = DynamicCast<QadhocWifiMac>(device->GetMac());
  mac->SetQueuesForSwitchingMngr(this);
}

void 
SwitchingManager::SetMacQueue (Ptr< EdcaTxopN > queue)
{
  m_queues.push_back(queue);
}

void 
SwitchingManager::SwitchChannel (uint16_t channelNumber)
{
  Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(m_device);
  Ptr<WifiPhy> phy = device->GetPhy();
  phy->SetChannelNumber(channelNumber);
  NS_LOG_DEBUG ("Switching to channel " << channelNumber);
}

void 
SwitchingManager::SwitchChannelAfterMacTxon (uint16_t channelNumber)
{
  NS_ASSERT(m_channelToSwitch == 0);
  m_channelToSwitch = channelNumber;
  FlushEnqueuedPackets();
  SetEndTransmissionCallback ();
}

void 
SwitchingManager::SwitchChannelAfterMacQueuesServed (uint16_t channelNumber)
{
  NS_ASSERT(m_channelToSwitch == 0);
  m_channelToSwitch = channelNumber;
  SetEndTransmissionCallback ();
}

void 
SwitchingManager::SetEndTransmissionCallback (void)
{
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++)
    {
    if ((*i)->PendingPacket()||(*i)->GetSizeQueue()>0) 
        {
          (*i)->SetTxFailedCallback (MakeCallback (&SwitchingManager::EndTransmissionCallback, this));
          (*i)->SetTxOkCallback (MakeCallback (&SwitchingManager::EndTransmissionCallback, this));
        }
    }

  if (!PendingPackets())
    {
      EndTransmission ();
    }
}

void 
SwitchingManager::EndTransmission (void)
{
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++)
    {
      (*i)->UnsetTxFailedCallback ();
      (*i)->UnsetTxOkCallback ();   
    }

  SwitchChannel (m_channelToSwitch);
  m_channelToSwitch = 0;

}

void 
SwitchingManager::EndTransmissionCallback (WifiMacHeader const& hdr)
{
  NS_LOG_DEBUG ("End of Transmission - QosTid "<< QosUtilsMapTidToAc(hdr.GetQosTid()));
  if (!PendingPackets())
    {
      NS_LOG_DEBUG ("All pending packets transmitted");
      EndTransmission();
    }
}

bool 
SwitchingManager::PendingPackets (void)
{
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++)
    {
      if ((*i)->PendingPacket()||(*i)->GetSizeQueue()>0)    
        {
          return true;
        }
    }
  return false;
}

bool
SwitchingManager::LookupQueue(uint16_t channel)
{
  BufferedPackets::iterator iter = m_bufferedPackets.begin();
  iter = m_bufferedPackets.find(channel);
  if( iter == m_bufferedPackets.end() ) 
    {
      return false;
    }
  return true;
}

bool
SwitchingManager::DeleteQueue(uint16_t channel)
{
  bool foundQueue = LookupQueue (channel);
  if ( foundQueue )
    {
      WifiMacQueues* queues;
      queues = m_bufferedPackets[channel];
      WifiMacQueues::iterator iterBegin = queues->begin();
      WifiMacQueues::iterator iterEnd = queues->end();
      queues->erase(iterBegin,iterEnd);
      delete queues;
      BufferedPackets::iterator iter = m_bufferedPackets.begin();
      iter = m_bufferedPackets.find(channel);
      m_bufferedPackets.erase(iter);
    }
  return foundQueue;
}

std::vector<Ptr< WifiMacQueue > >*
SwitchingManager::CreateQueue(uint16_t channel)
{
  WifiMacQueues* macQueues = new WifiMacQueues;
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++)
    {
      Ptr< WifiMacQueue> queue = CreateObject<WifiMacQueue> ();
      macQueues->push_back (queue);
    }
  m_bufferedPackets.insert (std::make_pair(channel, macQueues));
  return m_bufferedPackets[channel];
}

uint16_t
SwitchingManager::StorePacketsForLaterTxon (void)
{  
  Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(m_device);
  Ptr<WifiPhy> phy = device->GetPhy();
  uint16_t channel = phy->GetChannelNumber();
  WifiMacQueues* queues;
  if ( LookupQueue(channel) )
    {
      queues = m_bufferedPackets[channel];
    }
  else
    {
      queues = CreateQueue (channel);
    }

  int j = 0;
  for (Queues::const_iterator i = m_queues.begin (); i != m_queues.end (); i++, j++)
    {
      Ptr< WifiMacQueue > queue = (*queues).at(j);
      (*i)->CopyToQueue(queue);
    }

  return channel;
}

bool 
SwitchingManager::InsertPacketsFront (uint16_t fromChannel)
{
  NS_LOG_DEBUG ("InsertPacketsFront");
  bool foundQueue = LookupQueue (fromChannel);
  if ( foundQueue )
    {
      WifiMacQueues* queues;
      queues = m_bufferedPackets[fromChannel];
      int j=0;
      for (Queues::const_iterator k = m_queues.begin (); k != m_queues.end (); k++, j++)
        {
          Ptr< WifiMacQueue > queue = (*queues).at(j);
          (*k)->CopyFrontFromQueue (queue);
          (*k)->StartAccessIfNeeded ();
        }      
      DeleteQueue(fromChannel);
    }
  return foundQueue;
}

bool 
SwitchingManager::InsertPacketsBack(uint16_t fromChannel)
{
  bool foundQueue = LookupQueue (fromChannel);
  if ( foundQueue )
    {
      WifiMacQueues* queues;
      queues = m_bufferedPackets[fromChannel];
      int j=0;
      for (Queues::const_iterator k = m_queues.begin (); k != m_queues.end (); k++, j++)
        {
          Ptr< WifiMacQueue > queue = (*queues).at(j);
          (*k)->CopyBackFromQueue (queue);
          (*k)->StartAccessIfNeeded ();
        }
      DeleteQueue(fromChannel);
    }
  return foundQueue;
}

} // namespace ns3
