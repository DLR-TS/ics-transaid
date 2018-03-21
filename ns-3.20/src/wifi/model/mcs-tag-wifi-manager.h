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
#ifndef MCS_TAG_WIFI_MANAGER_H
#define MCS_TAG_WIFI_MANAGER_H

#include <stdint.h>
#include "wifi-remote-station-manager.h"

namespace ns3 {

class McsTagWifiManager : public WifiRemoteStationManager
{
public:
  static TypeId GetTypeId (void);
  McsTagWifiManager ();
  virtual ~McsTagWifiManager ();

private:
  virtual class WifiRemoteStation *CreateStation (void);
  // overriden from base class
  virtual class WifiRemoteStation *DoCreateStation (void) const;
  virtual void DoReportRxOk (WifiRemoteStation *station,
					  double rxSnr, WifiMode txMode);
  virtual void DoReportRtsFailed (WifiRemoteStation *station);
  virtual void DoReportDataFailed (WifiRemoteStation *station);
  virtual void DoReportRtsOk (WifiRemoteStation *station,
					   double ctsSnr, WifiMode ctsMode, double rtsSnr);
  virtual void DoReportDataOk (WifiRemoteStation *station,
						double ackSnr, WifiMode ackMode, double dataSnr);
  virtual void DoReportFinalRtsFailed (WifiRemoteStation *station);
  virtual void DoReportFinalDataFailed (WifiRemoteStation *station);
  virtual WifiTxVector DoGetDataTxVector (WifiRemoteStation *station, uint32_t size);
  virtual WifiTxVector DoGetRtsTxVector (WifiRemoteStation *station);
  virtual bool DoNeedRts (WifiRemoteStation *station,
				   Ptr<const Packet> packet, bool normally);
  virtual bool IsLowLatency (void) const;
};

class McsTagWifiRemoteStation : public WifiRemoteStation
{
public:
  McsTagWifiRemoteStation (Ptr<McsTagWifiManager> stations);
  virtual ~McsTagWifiRemoteStation ();
  WifiMode GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize);
  WifiMode GetRtsMode (Ptr<const Packet> packet);

protected:

private:
  // overriden from base class
  virtual void DoReportRxOk (WifiRemoteStation *station,
						   double rxSnr, WifiMode txMode);
  virtual void DoReportRtsFailed (WifiRemoteStation *station);
  virtual void DoReportDataFailed (WifiRemoteStation *station);
  virtual void DoReportRtsOk (WifiRemoteStation *station,
							double ctsSnr, WifiMode ctsMode, double rtsSnr);
  virtual void DoReportDataOk (WifiRemoteStation *station,
							 double ackSnr, WifiMode ackMode, double dataSnr);
  virtual void DoReportFinalRtsFailed (WifiRemoteStation *station);
  virtual void DoReportFinalDataFailed (WifiRemoteStation *station);
  virtual bool IsLowLatency (void) const;
  virtual Ptr<WifiRemoteStationManager> GetManager (void) const;
  virtual WifiMode DoGetDataMode (uint32_t size);
  virtual WifiMode DoGetRtsMode (void);
  virtual WifiMode GetWifiMode (uint8_t mcs);
  Ptr<McsTagWifiManager> m_manager;
  Ptr<const Packet> m_packet;
};

} // namespace ns3

#endif /* MCS_TAG_WIFI_MANAGER_H */
