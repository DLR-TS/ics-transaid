/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 
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
 * Author:  Sendoa Vaz
 */

#ifndef LTE_VEHICLE_INSTALLER_H
#define LTE_VEHICLE_INSTALLER_H

#include "lte-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/lte-net-device.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/lte-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"

namespace ns3
{

class LteVehicleInstaller : public LteInstaller
{
  public:
    static TypeId GetTypeId (void);
 
  private:
     void DoInstall (NodeContainer container, NetDeviceContainer createdDevices);

};

}

#endif

