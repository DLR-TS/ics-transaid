/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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

#ifndef CONFIGURATION_MANAGER_XML_H
#define CONFIGURATION_MANAGER_XML_H

#include <string>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

namespace ns3 {

class iTETRISNodeManager;

/**
 * @class ConfigurationManagerXml 
 * @brief This class is used to read the configuration file where the simulation scenario is described. Based on the technologies and communication modules specified in the configuration file, the ConfigurationManagerXml attaches the corresponding communication module installers to the iTETRISNodeManager.
 */
class ConfigurationManagerXml 
{
public:
  /**
   * The configuration file contains a line per communication module installer. 
   * The following tags can be employed:
   *  type -> ns-3 communication module installer
   *  name -> name used in iTETRIS (ns-3 and iCS) to reference the installer
   *  file -> path to the file which configures a given communication module 
   *          (i.e.see wave-installer.{h.cc})
   *  relatedInstaller -> indicate a related installer that can be used within the installer 
   *                      being defined, e.g. this tag is used in the RSU WaveInstaller to 
   *			  specify that the wireless channels employed for Wave vehicles must be
   *                      also used for RSUs. 
   *  default -> tag indicating that the communication module must be installed in
   *         all the nodes.
   */
  ConfigurationManagerXml (std::string fileName);
  virtual ~ConfigurationManagerXml ();
 
  void ReadFile (iTETRISNodeManager* nodeManager);
  int GetSeed (void);
  int GetRunNumber (void);


private:
  std::string m_filename;
  void SetSeed (int seed);
  void SetRunNumber (int runNumber);
  int m_seed;
  int m_runNumber;
};

} // namespace ns3

#endif /* CONFIGURATION_MANAGER_XML_H */
