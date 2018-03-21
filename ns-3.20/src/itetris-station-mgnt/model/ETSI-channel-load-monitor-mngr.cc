/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012, Mobile Communication Dept., http://www.eurecom.fr,
 *                     EURECOM, ETSI Specialist Task Force 447 STF447
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
 * Author: Jerome Haerri <haerri@eurecom.fr>
 * Author: Laura De Martini <demartin@eurecom.fr>
 */

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/location-table.h"
#include "ETSI-channel-load-monitor-mngr.h"
#include "ns3/itetris-types.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/channel-tag.h"



NS_LOG_COMPONENT_DEFINE ("ETSIChannelLoadMonitorMngr");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ETSIChannelLoadMonitorMngr);
TypeId ETSIChannelLoadMonitorMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ETSIChannelLoadMonitorMngr")
    .SetParent<ChannelLoadMonitorMngr> ()
    .AddConstructor<ETSIChannelLoadMonitorMngr> ()
    .AddAttribute ("MonitorWindow", "The length of the monitoring window",
                       UintegerValue (200),
                       MakeUintegerAccessor (&ETSIChannelLoadMonitorMngr::m_monitorWindow),
                       MakeUintegerChecker<int64_t> (1))
    .AddAttribute ("MonitorInterval", "The length of the monitoring interval",
                       UintegerValue (10),
                       MakeUintegerAccessor (&ETSIChannelLoadMonitorMngr::m_monitorInterval),
                       MakeUintegerChecker<int64_t> (1))
    ;
  return tid;
}


ETSIChannelLoadMonitorMngr::~ETSIChannelLoadMonitorMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

/**
 * @brief ETSIChannelLoadMonitorMngr::Monitor the actual algorithm to measure the channel load
 * @todo check and improve its performance and precision
 */
void
ETSIChannelLoadMonitorMngr::Monitor() {

    // FIR first order 50% filter
	if(Simulator::Now().GetMilliSeconds() < m_lastUpdate+m_monitorWindow) {

		m_probe++;
		Ptr<WifiPhy> netDevicePhy = DynamicCast<WifiNetDevice>(m_netDevice)->GetPhy();

		if((netDevicePhy !=NULL) && netDevicePhy->IsStateBusy()){
			m_total_busy++;
		}
	}
	else {
		m_lastUpdate = Simulator::Now().GetMilliSeconds();

		//  Base monitoring channel load
			m_channelLoad = (float) m_total_busy/m_probe;

		// ETSI filter computation
			m_channelLoad = m_weight*m_previous_channelLoad+(1-m_weight)*((float) m_total_busy/m_probe);
			m_previous_channelLoad =m_channelLoad;
		// end of ETSI filter computation

		if (m_channelLoad>=0){
			m_past_samples.push_back(m_channelLoad);
		}else{
			m_past_samples.push_back(0);
		}

		// ETSI filter channel load update
		if(m_channelLoad>=0){
			m_previous_channelLoad=m_channelLoad;
		}
		//end ESTI filter channel load update

        //end of the part of the first filter implementation

        // the ETSI stage classification
        // TODO (JHNOTE): make it modular, not hard coded. Add table lookup
		if (m_channelLoad > 0.4){
			m_channelState = state3;
		}
		else if (m_channelLoad > 0.1){
			m_channelState= state2;
		}
		else{
			m_channelState= state1;
		}

		m_probe = 0;
		m_total_busy = 0;

		if(m_stopMonitoring)  // manager has scheduled a stop
			return;
	}

	Time nextTime(MilliSeconds(m_monitorInterval)); // Time till next packet
	NS_LOG_LOGIC ("nextProbe = " << nextTime);
	Simulator::Schedule(nextTime, &ChannelLoadMonitorMngr::Monitor, this);

}

void
ETSIChannelLoadMonitorMngr::startMonitoring() {
	m_lastUpdate = Simulator::Now().GetMilliSeconds();
	m_probe=0;
	m_total_busy=0;
	m_previous_channelLoad=0;
	m_weight=0.5;
	m_tmp_probe=0;
	m_tmp_busy=0;
    m_overlapped_probe=0;
	m_overlapped_busy=0;
	m_prev_overlapped_probe = 0;
	m_prev_overlapped_busy = 0;
	m_taps_number = 7;

	for (unsigned i=0; i<7; ++i){
				m_past_samples.push_back(0);
			}

	Time nextTime(MilliSeconds (m_monitorInterval)); // Time till next probe
	NS_LOG_LOGIC ("nextProbe = " << nextTime);
	Simulator::Schedule(nextTime, &ETSIChannelLoadMonitorMngr::Monitor, this);
}

void
ETSIChannelLoadMonitorMngr::stopMonitoring() {
	m_stopMonitoring = true;
}

} //namespace ns3
