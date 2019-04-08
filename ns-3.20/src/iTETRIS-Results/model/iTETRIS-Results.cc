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


#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/application.h"
#include "iTETRIS-Results.h"
#include "ns3/config.h"
#include <stdio.h>
#include "ns3/ns3-server.h"


using namespace std;

namespace ns3
{




    iTETRISResults::iTETRISResults()
	{

	}

    iTETRISResults::~iTETRISResults()
	{

	}

	void iTETRISResults::LogPacketsTx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId){


        std::string file;

        file = "ReceivedPackets.txt";

        std::string data;

        data = "IDPacket ";
        data +=   to_string(packet->GetUid());
        data += " Size ";
        data += to_string(packet->GetSize());
        data += " Time ";
        data += to_string(Simulator::Now().GetSeconds());
        data += " Sender ID ";
        data += to_string( sendernodeId);
        data += " distance ";
        data += to_string(distanceTxRx);

        iTETRISResults m_ObjHandler;

        m_ObjHandler.writeResultsLogPacketsTx(file, data);
    }

    void iTETRISResults::LogPacketsRx(std::string context, Ptr<const Packet> packet, double distanceTxRx, uint32_t sendernodeId){


        std::string file;

        file = "TransmittedPackets.txt";

        std::string data;

        data = "IDPacket ";
        data +=  to_string(packet->GetUid());
        data += " Size ";
        data += to_string(packet->GetSize());
        data += " Time ";
        data += to_string(Simulator::Now().GetSeconds());
        data += " Sender ID ";
        data += to_string( sendernodeId);
        data += " distance ";
        data += to_string(distanceTxRx);



        iTETRISResults m_ObjHandler;

        m_ObjHandler.writeResultsLogPacketsRx(file, data);
    }


    void iTETRISResults::writeResultsLogPacketsTx(std::string file, std::string data ){


        Ns3Server::outfileLogPacketsTx << data << std::endl;


    }

    void iTETRISResults::writeResultsLogPacketsRx(std::string file, std::string data ){

        Ns3Server::outfileLogPacketsRx << data << std::endl;

    }



} // Namespace ns3
