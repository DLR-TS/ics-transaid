/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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

#ifndef WIMAX_BS_INSTALLER_H
#define WIMAX_BS_INSTALLER_H

#include "wimax-installer.h"

namespace ns3
{

class WimaxBsInstaller : public WimaxInstaller
{
  public:
    static TypeId GetTypeId (void);

  private:
    NetDeviceContainer DoInstall (NodeContainer container);
    void DoSetWimaxParameters (NetDeviceContainer devices); 
    void DoProcessApplicationInstall (std::string appName);

};

}

#endif


