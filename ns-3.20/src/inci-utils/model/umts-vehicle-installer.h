/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

#ifndef UMTS_VEHICLE_INSTALLER_H
#define UMTS_VEHICLE_INSTALLER_H

#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/umts-net-device.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/umts-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/c2c-facilities-helper.h"
#include "ns3/itetris-types.h"
#include "umts-installer.h"

namespace ns3 {

class UmtsVehicleInstaller : public UmtsInstaller {
public:
    static TypeId GetTypeId(void);
    void AddBaseStations(NetDeviceContainer netDevices);
//     void AddVehicles(NetDeviceContainer netDevices){};

private:
    void DoInstall(NodeContainer container, NetDeviceContainer devices, STACK stack);

};

}

#endif


