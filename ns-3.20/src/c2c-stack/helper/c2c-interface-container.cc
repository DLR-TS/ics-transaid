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

#include "c2c-interface-container.h"
#include "ns3/node-list.h"
#include "ns3/names.h"

namespace ns3 {

c2cInterfaceContainer::c2cInterfaceContainer ()
{}

void 
c2cInterfaceContainer::Add (c2cInterfaceContainer other)
{
    for (InterfaceVector::const_iterator i = other.m_interfaces.begin (); i != other.m_interfaces.end (); i++)
    {
      m_interfaces.push_back (*i);
    }
}

uint32_t 
c2cInterfaceContainer::GetN (void) const
{
  return m_interfaces.size ();
}
void 
c2cInterfaceContainer::Add (Ptr<c2c> c2c, uint32_t interface)
{
  m_interfaces.push_back (std::make_pair (c2c, interface));
}
void c2cInterfaceContainer::Add (std::pair<Ptr<c2c>, uint32_t> a)
  {
    Add (a.first, a.second);
  }
void 
c2cInterfaceContainer::Add (std::string c2cName, uint32_t interface)
{
  Ptr<c2c> C2C = Names::Find<c2c> (c2cName);
  m_interfaces.push_back (std::make_pair (C2C, interface));
}

 std::pair<Ptr<c2c>, uint32_t>
c2cInterfaceContainer::Get(uint32_t i) const
 {
   return m_interfaces[i];
 }
  

} // namespace ns3
