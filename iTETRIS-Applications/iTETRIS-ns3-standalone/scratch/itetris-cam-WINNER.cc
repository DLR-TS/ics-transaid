/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, UMH, EU FP7 iTETRIS project
 *            2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *            University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * Author: Ramon Bauza <rbauza@umh.es>
 * Author: Michele Rondinone <mrondinone@umh.es>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#include "ns3/core-module.h"
#include "ns3/config-store-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/c2c-stack-module.h"
#include "ns3/CAMmanagement.h"
#include "ns3/inci-utils-module.h"
#include "ns3/itetris-station-mgnt-module.h"
#include "ns3/channel-tag.h"

#include "stdio.h"
#include <iostream>
#include <fstream>

using namespace ns3;
using namespace std;

uint32_t numNodes = 3;
double sim_time = 100.0;
iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();

void
PhyTxTrace (std::string context, Ptr<const Packet> packet, WifiMode mode, WifiPreamble preamble, uint8_t txPower)
{
  WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  std::cout <<  Simulator::Now () << " " << context << " Packet Uid " << packet->GetUid() 
  << " PHYTX mode=" << mode << " to=" << hdr.GetAddr1 () << " from=" << hdr.GetAddr2 () 
  << std::endl;
}

void
PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble)
{
  WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  std::cout <<  Simulator::Now () << " " << context << " Packet Uid " << packet->GetUid() 
  << " PHYRXOK mode=" << mode << " to=" << hdr.GetAddr1 () << " from=" << hdr.GetAddr2 () 
  << std::endl;
}

void
moveright (Ptr<Node> node)
{
  Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
  Vector position = model->GetPosition ();
  
 // std::cout << "at "  << Simulator::Now () << " node " << node->GetId() << " in x= " << position.x << " y= " << position.y << std::endl; 

  model->SetPosition(Vector ((position.x + 2.5), position.y, 0)); 
  
  Simulator::Schedule(Seconds(0.5),&moveright, node);

}

void
moveup (Ptr<Node> node)
{
  Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
  Vector position = model->GetPosition ();
  
 // std::cout << "at "  << Simulator::Now () << " node " << node->GetId() << " in x= " << position.x << " y= " << position.y << std::endl; 

  model->SetPosition(Vector ((position.x), (position.y +2.5), 0)); 
  
  Simulator::Schedule(Seconds(0.5),&moveup, node);


}

void
ReceivePackets (PacketManager* packetManager, uint32_t nodeId)
{
  struct InciPacket::ReceivedInciPacket inciPacket;
  std::cout << "*** iCS collecting received CAMs ***" << std::endl;
  while ( packetManager->GetReceivedPacket(nodeId,inciPacket) )
    {
      std::cout << "Node=" << nodeId << " at time=" << inciPacket.ts << " received a msgType="<< inciPacket.msgType 
      << " fromNode="<< inciPacket.senderId << std::endl;
    }
}

NS_LOG_COMPONENT_DEFINE ("ns3_iTETRIS");

int 
main (int argc, char *argv[])
{
  /**
   * Two different configuration files can be used: a file (fileGeneralParameters) with the default attribute values to be used 
   * in the simulation and a file (ConfigTechnologies) with the configuration of the iTETRIS scenario. The file paths can be passed 
   * from the command line.
   *
   * In this example, the configTechnologiesWINNER.xml is used. This configuration file calls the confWaveVehicleWINNER.xml configuration
   * file to install on vehicles ITS G5A Netdevices that communicate over a propagation channel modeled with the WINNER model for urban scenarios. the 
   * This model is able to emulate the propagation effects deriving by pathloss, correlated shadowing and multipath fading in LOS and NLOS conditions
   * according to the building map provided in the file asymmetricbuildings.txt. 
   *
   * In this script, two vehicles are created to exchange beacons at 10Hz transmission frequency. Starting from second 30, the distance between the two 
   * vehicles increase as a consequence of vehicle 1's movement. As a result, less CAMs are received. Then, starting from second 60, vehicle 0 
   * moves to the right and creates NLOS conditions with vehicle 1 (a wall between the line linking the two nodes is defined in asymmetricbuildings.txt). 
   * As a result, no CAMs are received any longer.
   */
  std::string fileGeneralParameters = "";
  std::string fileConfTechnologies = "";
  CommandLine cmd;
  cmd.AddValue("fileGeneralParameters", "Path to the configuration file", fileGeneralParameters);
  cmd.AddValue("fileConfTechnologies", "Path to the configuration file", fileConfTechnologies);
  cmd.Parse (argc, argv);
  
   LogComponentEnable ("ConfigurationManagerXml",LOG_LEVEL_DEBUG);
   LogComponentEnable ("WaveInstaller",LOG_LEVEL_DEBUG);
  

  if (fileGeneralParameters != "")
    {
      Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (fileGeneralParameters));
      Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
      ConfigStore config;
      config.ConfigureDefaults ();
    }

  if (fileConfTechnologies == "")
    {
      fileConfTechnologies = "scratch/configTechnologiesWINNER.xml"; // change this
    }

  ConfigurationManagerXml confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  confManager.ReadFile(nodeManager);

  nodeManager->CreateItetrisNode (Vector(200,200,0));
  nodeManager->InstallCommunicationModule ("WaveVehicle");
  
  nodeManager->CreateItetrisNode (Vector(200,250,0));
  nodeManager->InstallCommunicationModule ("WaveVehicle");
 

  PacketManager* packetManager = new PacketManager ();
  packetManager->SetNodeManager(nodeManager);
  nodeManager->ActivateNode(0);
  nodeManager->ActivateNode(1);
  
  
   NodeContainer mobileNodes = nodeManager->GetItetrisNodes ();
  
      
  Simulator::Schedule (Seconds (1.0), &PacketManager::ActivateCamTxon,packetManager,0,10,300,1);
  Simulator::Schedule (Seconds (1.0), &PacketManager::ActivateCamTxon,packetManager,1,10,300,1);


  Simulator::Schedule(Seconds(30.0),&moveup, mobileNodes.Get (1));
  Simulator::Schedule(Seconds(60.0),&moveright, mobileNodes.Get (0));

   Simulator::Schedule (Seconds (80.0), &ReceivePackets,packetManager,0);


  Simulator::Stop (Seconds (sim_time));
 // Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxOk", MakeCallback (&PhyRxOkTrace));
 // Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/Tx", MakeCallback (&PhyTxTrace));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
