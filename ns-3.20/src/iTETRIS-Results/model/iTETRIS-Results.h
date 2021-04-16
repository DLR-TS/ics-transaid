/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez,
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
 * Author: Alejandro Correa <acorrea@umh.es>, Gokulnath Thandavarayan <gthandavarayan@umh.es>
 */


#ifndef ITETRIS_RESULTS_H
#define ITETRIS_RESULTS_H

#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/object.h"
#include "ns3/channel-tag.h"
#include "ns3/callback.h"
#include "ns3/storage.h"
#include "ns3/config.h"
#include "ns3/node-container.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-80211p-helper.h"

namespace ns3 {
class NodeContainer;

#define N_STEPS_METRIC 100
#define N_LAST_STEP 99
#define N_LAST_DISTANCE_STEP 990


struct PDRdata {
    uint32_t countTx[N_STEPS_METRIC] = {0};
    uint32_t countRx[N_STEPS_METRIC] = {0};
};

struct NARdata {
    std::map <int, double> detectedVehicles;
    std::map <int, double> totalVehicles;
};

struct NIRdata {
    std::map <int, double> detectedVehicles;
};


struct LatencyData {
    int latency [200] =  {0} ;
    uint32_t countTotal = 0;
};

struct IPRTdata {
    std::map <int, double> initial_time;
};

struct IPRTdataDistance {
    double IPRT[N_STEPS_METRIC] = {0};
    uint32_t countRx[N_STEPS_METRIC] = {0};
};


class iTETRISResults : public Object {
public:

    iTETRISResults(int initial_x, int initial_y, int end_x, int end_y);
    virtual ~iTETRISResults();

    void LogPacketsTx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId);
    void LogPacketsRx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId);
    void PhyStateTracer(std::string context, Time start, Time duration, enum WifiPhy::State state);


    void LogAwarenessRatio(const NodeContainer& m_NodeContainer);

    void writeResults();


private:

    void ResetCounters();

    PDRdata m_PDRdataCAM;
    PDRdata m_PDRdataCPM;
    PDRdata m_PDRdataMCM;
    PDRdata m_PDRdata;

    LatencyData m_LatencyData;

    std::map<int, NARdata> m_NARdataMap;

    std::map<int, NIRdata> m_NIRdataMap;

    std::map<int, double> m_CBRdataMap;

    std::map<int, IPRTdata> m_IPRTdataMapCAM;

    IPRTdataDistance m_IPRT_CAM;

    std::map<int, int> m_MessagesRxMap;

    int m_interval;

    int m_initial_x;
    int m_initial_y;
    int m_end_x;
    int m_end_y;

    const NodeContainer* m_TransAIDNodes;
};

} // namespace ns3

#endif   /* ITETRIS_RESULTS_H  */

