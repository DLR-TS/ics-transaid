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
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace ns3
{
    class NodeContainer;

    struct PDRdata{
        uint32_t countTx[50] = { 0 };
        uint32_t countRx[50] = { 0 };
    };
    struct NARdata{
        uint32_t countTotal[50] = { 0 };
        uint32_t countRx[50] = { 0 };
    };
    struct NIRdata{
        double countRx[50] = { 0 };
        uint32_t countTotal = 0;
    };

    struct LatencyData{
        double latency =  0 ;
        uint32_t countTotal = 0;
    };

	class iTETRISResults : public Object
	{
		public:

			iTETRISResults();
			virtual ~iTETRISResults();

         void LogPacketsTx(std::string context, Ptr<const Packet> packet , double distanceTxRx, uint32_t sendernodeId);
         void LogPacketsRx(std::string context, Ptr<const Packet> packet , double distanceTxRx, uint32_t sendernodeId);

         void LogAwarenessRatio(NodeContainer m_NodeContainer);

         void writeResults();


		private:

        void ResetCounters ();

        PDRdata m_PDRdata;
        LatencyData m_LatencyData;

        std::map<int, NARdata> m_NARdataMap;
        std::map<int, NIRdata> m_NIRdataMap;

        int m_interval;

	};

} // namespace ns3

#endif   /* ITETRIS_RESULTS_H  */
