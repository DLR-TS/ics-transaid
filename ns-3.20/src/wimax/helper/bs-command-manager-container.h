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

#ifndef BS_COMMAND_MANAGER_CONTAINER_H
#define BS_COMMAND_MANAGER_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/bs-command-manager.h"

namespace ns3 {

class BsCommandManagerContainer
{
public:

  typedef std::vector<Ptr<BsCommandManager> >::const_iterator Iterator;
  BsCommandManagerContainer ();
  BsCommandManagerContainer (Ptr<BsCommandManager> manager);
  BsCommandManagerContainer (std::string managerName);
  BsCommandManagerContainer (const BsCommandManagerContainer &a, const BsCommandManagerContainer &b);
  Iterator Begin (void) const;
  Iterator End (void) const;
  uint32_t GetN (void) const;
  Ptr<BsCommandManager> Get (uint32_t i) const;
  void Add (BsCommandManagerContainer other);
  void Add (Ptr<BsCommandManager> manager);
  void Add (std::string managerName);

private:
  std::vector<Ptr<BsCommandManager> > m_commandManagers;
};

} // namespace ns3

#endif /* BS_COMMAND_MANAGER_CONTAINER_H */
