/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 */

#ifndef C2C_INTERFACE_HELPER_H
#define C2C_INTERFACE_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/itetris-technologies.h"
#include "c2c-interface-container.h"


namespace ns3 {

/**
 * \brief  iTETRIS [WP600] - Helper class to assign c2c Interfaces.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

class c2cInterfaceHelper
{
public:

  c2cInterfaceHelper ();

  c2cInterfaceContainer Assign (const NetDeviceType deviceType, const NetDeviceContainer &c);
  c2cInterfaceContainer Assign (const NetDeviceContainer &c);

};

}; // namespace ns3

#endif /* C2C_INTERFACE_HELPER_H */
