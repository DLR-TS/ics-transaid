/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          CBT
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author: Julen Maneros <jmaneros@cbt.es>, Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <Fatma.hrizi@eurecom.fr>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#include "ns3/server-socket.h"
#include "ns3/storage.h"
#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"
#include "ns3/nstime.h"
#include "ns3/iTETRISNodeManager.h"
#include "ns3/packet-manager.h" 
#include <fstream>
#include "ns3/iTETRIS-Results.h"

namespace ns3
{

class Ns3Commands;
class MessageScheduleUnicast;
class MessageScheduleGeoBroadcast;

class Ns3Server
{
  friend class MessageScheduleUnicast;
  friend class MessageScheduleGeoBroadcast;
  public:
    Ns3Server();
    Ns3Server(int port, iTETRISNodeManager *node_manager, PacketManager *packetManager);

    /** 
     * @brief Singleton instance of the server
     */
    static Ns3Server* instance_;

    /**
     * @brief Socket on which server is listening on
     */
    tcpip::ServerSocket* socket_;

    static bool closeConnection_;
    tcpip::Storage myInputStorage;
    tcpip::Storage myOutputStorage;
    bool myDoingSimStep;  
    Ns3Commands* commands;
    static void processCommandsUntilSimStep();    
    static void processCommandsUntilSimStep(int port, std::string logfile, iTETRISNodeManager *node_manager, PacketManager *packetManager);    
    static std::ofstream myfile;
    static int targetTime_;
    static bool Log(const char* message);
    static bool logActive_;

    // Log Results TransAID
    iTETRISResults * my_resultsManager;
    static std::ofstream outfileLogPacketsRx;
    static std::ofstream outfileLogPacketsTx;

  private:
    static std::string CAM_TYPE; 
    static std::string DNEM_TYPE;
    int dispatchCommand();
    bool RunSimStep(int time);

    /** 
     * @brief Update node's position
     */
    bool UpdateNodePosition();

    /** 
     * @brief Update node's position, speed, heading and laneId
     */
    bool UpdateNodePosition2 (); 

    /** 
     * @brief Start sending CAM in the nodes indicated in a list of nodes
     */
    bool StartSendingCam();

    /** 
     * @brief Stop sending CAM in the nodes indicated in a list of nodes
     */
    bool StopSendingCam();

    /** 
     * @brief Retrieve the messages that have been received by the simulated nodes
     */
    bool GetReceivedMessages();

    /**
		 * @brief Retrieve all the messages that have been received by the all the simulated nodes
		 */
    bool GetAllReceivedMessages();

    /** 
     * @brief Create a new node (vehicle or CIU) specifying its initial position
     */
    bool CreateNode(); 

    /** 
     * @brief Create a new node (vehicle or CIU) specifying its initial position, speed, heading and laneId
     */
    bool CreateNode2 (); 
    bool Close();
    void writeStatusCmd(int commandId, int status, std::string description);  
    int CalculateStringListByteSize(std::vector<std::string> list);    

    /** 
     * @brief Activate a topobroadcast transmision using WAVE and the C2C stack
     */
    bool StartTopoTxon (void);

    /** 
     * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast or broadcast transmision in a vehicle or a RSU. It works with different radio access technologies (if a vehicle is the sender, e.g. WAVE, UMTS, etc.) and the C2C and IP stacks.
     */
    bool StartIdBasedTxon (void);

    /**
     * @brief Activate a transmision from the TMC based on a geographic area. This txon mode can be used to activate any kind of transmission supported by the Technology Selector according the ServiceID rules.
    */
    bool StartMWTxon (void);

    /** 
     * @brief Activate a transmision based on the destination node ID. This txon mode can be used to active a unicast, broadcast or multicast transmision in a IP-based base station or CIU (e.g. UMTS base stations, WiMAX base station, etc.)
     */
    bool StartIpCiuTxon (void);

    /** 
     * @brief Activate a geobroadcast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
     */
    bool StartGeobroadcastTxon (void);

    /** 
     * @brief Activate a geoanycast txon in a WAVE vehicle or RSU. The geodestination area is a circle defined by its radius (Lat and Lon coordinates) and center point (meters).
     */
    bool StartGeoanycastTxon (void);

    /** 
     * @brief Deactivate the txon of a service running in a vehicle or a RSU.
     */
    bool StopServiceTxon (void);

    /** 
     * @brief Deactivate the txon of a service running in IP-based base station or CIU.
     */
    bool StopIpCiuServiceTxon (void);

    /**
     * @brief Deactivate the MWtxon of a MW service running on a TMC.
     */
    bool StopMWServiceTxon (void);

    /** 
     * @brief Activate a node and all its communication modules, e.g. PHY layer 
     */
    bool ActivateNode (void);

    /** 
     * @brief Deactivate a node and all its communication modules, e.g. PHY layer 
     */
    bool DeactivateNode (void);

    /** 
     * @brief Pointer to iTETRISNodeManager object which is responsible for the creation, the initial placement, and the position updates of the nodes in ns-3
     */
    iTETRISNodeManager *my_nodeManagerPtr; 

    /** 
     * @brief Pointer to PacketManager object which is responsible for the the activation and deactivation of packet transmissions in ns-3
     */
    PacketManager *my_packetManagerPtr; 

    std::string Int2String (int n);

    std::string Double2String (double n);


};

}
