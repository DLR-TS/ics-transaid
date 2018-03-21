/* -*- Mode:C++; c-file-style:"gnu"; indent-tass-mode:nil; -*- */
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

#include "ss-command-manager-container.h"
#include "ns3/names.h"

namespace ns3 {

SsCommandManagerContainer::SsCommandManagerContainer ()
{}
SsCommandManagerContainer::SsCommandManagerContainer (Ptr<SsCommandManager> manager)
{
  m_commandManagers.push_back (manager);
}
SsCommandManagerContainer::SsCommandManagerContainer (std::string managerName)
{
  Ptr<SsCommandManager> mng = Names::Find<SsCommandManager> (managerName);
  m_commandManagers.push_back (mng);
}
SsCommandManagerContainer::SsCommandManagerContainer (const SsCommandManagerContainer &a, const SsCommandManagerContainer &b)
{
  *this = a;
  Add (b);
}


SsCommandManagerContainer::Iterator 
SsCommandManagerContainer::Begin (void) const
{
  return m_commandManagers.begin ();
}
SsCommandManagerContainer::Iterator 
SsCommandManagerContainer::End (void) const
{
  return m_commandManagers.end ();
}

uint32_t 
SsCommandManagerContainer::GetN (void) const
{
  return m_commandManagers.size ();
}
Ptr<SsCommandManager> 
SsCommandManagerContainer::Get (uint32_t i) const
{
  return m_commandManagers[i];
}
void 
SsCommandManagerContainer::Add (SsCommandManagerContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_commandManagers.push_back (*i);
    }
}
void 
SsCommandManagerContainer::Add (Ptr<SsCommandManager> manager)
{
  m_commandManagers.push_back (manager);
}
void 
SsCommandManagerContainer::Add (std::string managerName)
{
  Ptr<SsCommandManager> manager = Names::Find<SsCommandManager> (managerName);
  m_commandManagers.push_back (manager);
}

} // namespace ns3
