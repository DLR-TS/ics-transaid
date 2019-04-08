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


#include "ns3/channel-tag.h"
#include "ns3/callback.h"
#include "ns3/storage.h"
#include "ns3/config.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

namespace ns3
{


	class iTETRISResults
	{
		public:

			iTETRISResults();
			virtual ~iTETRISResults();

        static void LogPacketsTx(std::string context, Ptr<const Packet> packet , double distanceTxRx, uint32_t sendernodeId);
        static void LogPacketsRx(std::string context, Ptr<const Packet> packet , double distanceTxRx, uint32_t sendernodeId);


        void writeResultsLogPacketsTx(std::string file, std::string data );
        void writeResultsLogPacketsRx(std::string file, std::string data );

		private:





	};

} // namespace ns3

#endif   /* ITETRIS_RESULTS_H  */
