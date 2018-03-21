/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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

#include "switching-manager-container.h"
#include "ns3/names.h"

namespace ns3 {

SwitchingManagerContainer::SwitchingManagerContainer ()
{}
SwitchingManagerContainer::SwitchingManagerContainer (Ptr<SwitchingManager> manager)
{
  m_devManagers.push_back (manager);
}
SwitchingManagerContainer::SwitchingManagerContainer (std::string managerName)
{
  Ptr<SwitchingManager> mng = Names::Find<SwitchingManager> (managerName);
  m_devManagers.push_back (mng);
}
SwitchingManagerContainer::SwitchingManagerContainer (const SwitchingManagerContainer &a, const SwitchingManagerContainer &b)
{
  *this = a;
  Add (b);
}

SwitchingManagerContainer::Iterator 
SwitchingManagerContainer::Begin (void) const
{
  return m_devManagers.begin ();
}
SwitchingManagerContainer::Iterator 
SwitchingManagerContainer::End (void) const
{
  return m_devManagers.end ();
}

uint32_t 
SwitchingManagerContainer::GetN (void) const
{
  return m_devManagers.size ();
}
Ptr<SwitchingManager> 
SwitchingManagerContainer::Get (uint32_t i) const
{
  return m_devManagers[i];
}
void 
SwitchingManagerContainer::Add (SwitchingManagerContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_devManagers.push_back (*i);
    }
}
void 
SwitchingManagerContainer::Add (Ptr<SwitchingManager> manager)
{
  m_devManagers.push_back (manager);
}
void 
SwitchingManagerContainer::Add (std::string managerName)
{
  Ptr<SwitchingManager> manager = Names::Find<SwitchingManager> (managerName);
  m_devManagers.push_back (manager);
}

} // namespace ns3
