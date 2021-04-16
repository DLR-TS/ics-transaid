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

#include "wimax-command-manager-helper.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/bs-command-manager.h"
#include "ns3/ss-command-manager.h"
#include "ns3/bs-command-manager-container.h"
#include "ns3/ss-command-manager-container.h"

NS_LOG_COMPONENT_DEFINE("WimaxCommandManagerHelper");

namespace ns3 {

uint32_t WimaxCommandManagerHelper::m_bsId = 0;

WimaxCommandManagerHelper::WimaxCommandManagerHelper() {
    NS_LOG_INFO("WimaxCommandManagerHelper");
}

BsCommandManagerContainer
WimaxCommandManagerHelper::InstallBsCommandManagers(NetDeviceContainer bsDevices, double txPower) {
    uint8_t groups = 4;
    BsCommandManagerContainer managers;
    for (NetDeviceContainer::Iterator i = bsDevices.Begin(); i != bsDevices.End(); ++i) {
        Ptr<BsCommandManager> commandManager = CreateObject<BsCommandManager> ();
        commandManager->SetNetDevice(*i);
        m_bsId++;
        commandManager->SetBsId(m_bsId);
        NS_LOG_INFO("[WimaxCommandManagerHelper::InstallBsCommandManagers] bsid=" << m_bsId);
        commandManager->InitSfidIndex();
        commandManager->SetOnOff(false);
        commandManager->InitiateBroadcastConnection();
        commandManager->InitiateMulticastConnections(groups);
        (*i)->AggregateObject(commandManager);
        managers.Add(commandManager);
        NS_LOG_INFO("[WimaxCommandManagerHelper::InstallBsCommandManagers]");
    }
    return managers;
}

SsCommandManagerContainer
WimaxCommandManagerHelper::InstallSsCommandManagers(NetDeviceContainer ssDevices) {
    SsCommandManagerContainer managers;
    for (NetDeviceContainer::Iterator i = ssDevices.Begin(); i != ssDevices.End(); ++i) {
        Ptr<SsCommandManager> commandManager = CreateObject<SsCommandManager> ();
        commandManager->SetNetDevice(*i);
        commandManager->SetIdRegisteredBs(0);
        commandManager->SetMulticastGroupId(0);
        commandManager->SetOnOff(false);
        (*i)->AggregateObject(commandManager);
        managers.Add(commandManager);
    }
    return managers;
}

void
WimaxCommandManagerHelper::LinkBsAndSsCommandManagers(BsCommandManagerContainer bsCommandManagers, SsCommandManagerContainer ssCommandManagers) {
    for (SsCommandManagerContainer::Iterator ssIter = ssCommandManagers.Begin(); ssIter != ssCommandManagers.End(); ++ssIter) {
        for (BsCommandManagerContainer::Iterator bsIter = bsCommandManagers.Begin(); bsIter != bsCommandManagers.End(); ++bsIter) {
            (*ssIter)->AddBsManager(*bsIter);
            (*bsIter)->AddNonRegisteredSsManager(*ssIter);
        }
    }
}

} // namespace ns3
