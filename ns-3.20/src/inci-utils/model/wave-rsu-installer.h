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

#ifndef WAVE_RSU_INSTALLER_H
#define WAVE_RSU_INSTALLER_H

#include "wave-installer.h"

namespace ns3
{

class WaveRsuInstaller : public WaveInstaller
{
  public:
    static TypeId GetTypeId (void);

  private:
    void DoInstall (NodeContainer container, NetDeviceContainer cchDevices, NetDeviceContainer schDevices);
    //Ipv6Prefix AllocateIpv6Prefix();
    Ipv6Address AllocateIpv6Prefix();

};

}

#endif


