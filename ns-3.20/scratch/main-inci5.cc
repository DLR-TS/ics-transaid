/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/

/*************************************************************************************
 *                        main-inci5.cc.
 *
 * this file is used to start ns-3 through iTETRIS instead of ./waf
 * its main functionality is to start and configure the INCI, the
 * iTETRIS Network interface to connect ns-3 to the iCS
 *
 *
 * ***********************************************************************************/

#include "ns3/core-module.h"
#include "ns3/config-store-module.h"
#include "ns3/internet-module.h"
#include "ns3/inci-module.h"
#include "ns3/mobility-module.h"
#include "ns3/inci-utils-module.h"
#include <exception>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("main-inci5");

uint32_t stringToUInt(std::string value)
{
  uint32_t ret;
  std::stringstream str;
  str << value;
  str >> ret;
  return ret;
}

void loadOptionFile()
{
  using namespace std;
  string file = "option.txt";
  ifstream inFile;
  inFile.open(file.c_str());
  if (!inFile.good())
  {
    cout << "ns3 -> No option file found. Must be named option.txt" << endl;
  }
  else
  {
    string key, value;
    while (inFile >> key >> value)
    {
      if (key == "RngRun")
      {
        SeedManager::SetRun(stringToUInt(value));
        cout << "ns3 -> RngRun set to " << value << endl;
      }
      else if (key == "RngSeed")
      {
        SeedManager::SetSeed(stringToUInt(value));
        cout << "ns3 -> RngSeed set to " << value << endl;
      }
    }
  }
  inFile.close();
}

int main (int argc, char *argv[])
{
  std::cout << "main-inci v 3.20" << std::endl;
  /**
   * Two different configuration files can be used: a file (fileGeneralParameters) with the default attribute values to be used 
   * in the simulation and a file (fileConfTechnologies) with the configuration of the iTETRIS scenario. The file paths can be passed 
   * from the command line.
   */
  std::string fileGeneralParameters = "";
  std::string fileConfTechnologies = "";
  std::string inciPort = "";
  std::string logFile = "";

  CommandLine cmd;
  cmd.AddValue("fileGeneralParameters", "Path to the configuration file", fileGeneralParameters);
  cmd.AddValue("fileConfTechnologies", "Path to the configuration file", fileConfTechnologies);
  cmd.AddValue("inciPort", "iNCI listening socket port number", inciPort);


  if (argc > 4)
	  cmd.AddValue("logFile", "Log file path", logFile);

  cmd.Parse (argc, argv);


  if (fileGeneralParameters != "")
    {
      Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (fileGeneralParameters));
      Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
      ConfigStore config;
      config.ConfigureDefaults ();
    }

  if (fileConfTechnologies == "")
    {
      fileConfTechnologies = "scratch/configTechnologies-ics.xml";
    }


  int port = atoi(inciPort.c_str());   

  ConfigurationManagerXml confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  PacketManager* packetManager = new PacketManager (); 
  packetManager->SetNodeManager (nodeManager);
  confManager.ReadFile(nodeManager);
  SeedManager::SetSeed (confManager.GetSeed());  
  SeedManager::SetRun (confManager.GetRunNumber());


  loadOptionFile();



  try {
    ns3::Ns3Server::processCommandsUntilSimStep(port, logFile, nodeManager, packetManager); 
  } catch (int e)
  {
    std::cout << "Exiting... " << std::endl;
    return 0;
  }


  Simulator::Destroy ();
  
  return 0;
}
