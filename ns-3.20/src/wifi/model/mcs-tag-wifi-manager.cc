/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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

#include "mcs-tag-wifi-manager.h"
#include "ns3/mcs-tag.h"
#include "ns3/string.h"
#include "ns3/assert.h"

namespace ns3 {

McsTagWifiRemoteStation::McsTagWifiRemoteStation (Ptr<McsTagWifiManager> manager)
  : m_manager (manager)
{}

McsTagWifiRemoteStation::~McsTagWifiRemoteStation ()
{}

void 
McsTagWifiRemoteStation::DoReportRxOk (WifiRemoteStation *station, double rxSnr, WifiMode txMode)
{}
void 
McsTagWifiRemoteStation::DoReportRtsFailed (WifiRemoteStation *station)
{}
void 
McsTagWifiRemoteStation::DoReportDataFailed (WifiRemoteStation *station)
{}
void 
McsTagWifiRemoteStation::DoReportRtsOk (WifiRemoteStation *station, double ctsSnr, WifiMode ctsMode, double rtsSnr)
{}
void 
McsTagWifiRemoteStation::DoReportDataOk (WifiRemoteStation *station, double ackSnr, WifiMode ackMode, double dataSnr)
{
  m_packet = 0; 
}
void 
McsTagWifiRemoteStation::DoReportFinalRtsFailed (WifiRemoteStation *station)
{
  m_packet = 0; 
}
void 
McsTagWifiRemoteStation::DoReportFinalDataFailed (WifiRemoteStation *station)
{
  m_packet = 0; 
}

Ptr<WifiRemoteStationManager>
McsTagWifiRemoteStation::GetManager (void) const
{
  return m_manager;
}

WifiMode 
McsTagWifiRemoteStation::DoGetDataMode (uint32_t size)
{
  //NS_ASSERT(GetManager ()->IsLowLatency ());
  NS_ASSERT(IsLowLatency ());
  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = m_manager->GetNonUnicastMode ();
    }
  return mode;
}

WifiMode 
McsTagWifiRemoteStation::DoGetRtsMode (void)
{
  //NS_ASSERT(GetManager ()->IsLowLatency ());
  NS_ASSERT(IsLowLatency ());
  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = m_manager->GetNonUnicastMode ();
    }
  return mode;
}

WifiMode 
McsTagWifiRemoteStation::GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize)
{
  //NS_ASSERT(GetManager ()->IsLowLatency ());
  NS_ASSERT(IsLowLatency ());
  m_packet = packet;
  return DoGetDataMode (fullPacketSize);
}

WifiMode 
McsTagWifiRemoteStation::GetRtsMode (Ptr<const Packet> packet)
{
  //NS_ASSERT(GetManager ()->IsLowLatency ());
  NS_ASSERT(IsLowLatency ());
  m_packet = packet;
  return DoGetRtsMode ();
}

WifiMode 
McsTagWifiRemoteStation::GetWifiMode (uint8_t mcs)
{
  switch (mcs) {
  case 0 :
    {
    WifiMode mode ("wifi-3mbs-10Mhz");
    return mode;
    break;
    }
  case 1 :
    {
    WifiMode mode ("wifi-4.5mbs-10Mhz");
    return mode;
    break;
    }
  case 2 :
    {
    WifiMode mode ("wifi-6mbs-10Mhz");
    return mode;
    break;
    }
  case 3 :
    {
    WifiMode mode ("wifi-9mbs-10Mhz");
    return mode;
    break;
    }
  case 4 :
    {
    WifiMode mode ("wifi-12mbs-10Mhz");
    return mode;
    break;
    }
  case 5 :
    {
    WifiMode mode ("wifi-18mbs-10Mhz");
    return mode;
    break;
    }
  case 6 :
    {
    WifiMode mode ("wifi-24mbs-10Mhz");
    return mode;
    break;
    }
  case 7 : 
    {
    WifiMode mode ("wifi-27mbs-10Mhz");
    return mode;
    break;
    }
  }
  WifiMode mode ("wifi-3mbs-10Mhz");
  return mode;
}

bool
McsTagWifiRemoteStation::IsLowLatency(void) const
{
	return true;
}


NS_OBJECT_ENSURE_REGISTERED (McsTagWifiManager);

void
McsTagWifiManager::DoReportRxOk (WifiRemoteStation *station, double rxSnr, WifiMode txMode)
{}
void
McsTagWifiManager::DoReportRtsFailed (WifiRemoteStation *station)
{}
void
McsTagWifiManager::DoReportDataFailed (WifiRemoteStation *station)
{}
void
McsTagWifiManager::DoReportRtsOk (WifiRemoteStation *station, double ctsSnr, WifiMode ctsMode, double rtsSnr)
{}
void
McsTagWifiManager::DoReportDataOk (WifiRemoteStation *station, double ackSnr, WifiMode ackMode, double dataSnr)
{}
void
McsTagWifiManager::DoReportFinalRtsFailed (WifiRemoteStation *station)
{}
void
McsTagWifiManager::DoReportFinalDataFailed (WifiRemoteStation *station)
{}

bool
McsTagWifiManager::IsLowLatency(void) const
{
	return true;
}

bool
McsTagWifiManager::DoNeedRts (WifiRemoteStation *st,
                            Ptr<const Packet> packet, bool normally)
{
  return true;
}

WifiTxVector
McsTagWifiManager::DoGetDataTxVector (WifiRemoteStation *st,
                                    uint32_t size)
{
	return WifiTxVector();
}
WifiTxVector
McsTagWifiManager::DoGetRtsTxVector (WifiRemoteStation *st)
{
	return WifiTxVector();
}

TypeId 
McsTagWifiManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McsTagWifiManager")
    .SetParent<WifiRemoteStationManager> ()
    .AddConstructor<McsTagWifiManager> ();
  return tid;
}

McsTagWifiManager::McsTagWifiManager (): WifiRemoteStationManager()
{
}

McsTagWifiManager::~McsTagWifiManager ()
{}

WifiRemoteStation *
McsTagWifiManager::CreateStation (void)
{
  return new McsTagWifiRemoteStation (this);
}

WifiRemoteStation *
McsTagWifiManager::DoCreateStation (void) const
{
	return NULL;
}

} // namespace ns3
