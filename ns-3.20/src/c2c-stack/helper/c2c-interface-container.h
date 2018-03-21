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

#ifndef C2C_INTERFACE_CONTAINER_H
#define C2C_INTERFACE_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/c2c.h"

namespace ns3 {

/**
 * \brief  iTETRIS [WP600] - holds a vector of std::pair of Ptr<c2c> and interface index.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 * Typically ns-3 c2cInterfaces are installed on devices using an c2c Interface
 * helper.  The helper's Assign() method takes a NetDeviceContainer which holds 
 * some number of Ptr<NetDevice>.  For each of the NetDevices in the 
 * NetDeviceContainer the helper will find the associated Ptr<Node> and
 * Ptr<c2c>.  It makes sure that an interface exists on the node for the
 * device. The helper then converts the Ptr<c2c> and the interface index to a std::pair and adds them to a
 * container -- a container of this type.
 *
 * The point is then to be able to implicitly associate an index into the 
 * original NetDeviceContainer (that identifies a particular net device) with
 * an identical index into the c2cInterfaceContainer that has a std::pair with
 * the Ptr<c2c> and interface index you need to play with the interface.
 *
 */
class c2cInterfaceContainer
{
public:
  /**
   * Create an empty c2cInterfaceContainer.
   */
  c2cInterfaceContainer ();

  /**
   * Concatenate the entries in the other container with ours.
   * \param other container
   */
  void Add (c2cInterfaceContainer other);

  /**
   * \returns the number of Ptr<c2c> and interface pairs stored in this
   * c2cInterfaceContainer.
   */
  uint32_t GetN (void) const;

  /**
   * Manually add an entry to the container consisting of the individual parts
   * of an entry std::pair.
   *
   * \param c2c pointer to c2c object
   * \param interface interface index of the c2cInterface to add to the container
   *
   * @see c2cInterfaceContainer
   */
  void Add (Ptr<c2c> c2c, uint32_t interface);

  /**
   * Manually add an entry to the container consisting of a previously composed 
   * entry std::pair.
   *
   * \param ipInterfacePair the pair of a pointer to c2c object and interface index of the c2cInterface to add to the container
   *
   * @see c2cInterfaceContainer
   */
  void Add (std::pair<Ptr<c2c>, uint32_t> c2cInterfacePair);

  /**
   * Manually add an entry to the container consisting of the individual parts
   * of an entry std::pair.
   *
   * \param c2cName std:string referring to the saved name of an c2c Object that
   *        has been previously named using the Object Name Service.
   * \param interface interface index of the c2cInterface to add to the container
   *
   * @see c2cInterfaceContainer
   */
  void Add (std::string c2cName, uint32_t interface);

  /**
   * Get the std::pair of an Ptr<c2c> and interface stored at the location
   * specified by the index.
   *
   * \param i the index of the entry to retrieve.
   *
   * @see c2cInterfaceContainer
   */
  std::pair<Ptr<c2c>, uint32_t> Get (uint32_t i) const;

 private:
  
  typedef std::vector<std::pair<Ptr<c2c>,uint32_t> > InterfaceVector;
  InterfaceVector m_interfaces;
};

} // namespace ns3

#endif /* C2C_INTERFACE_CONTAINER_H */
