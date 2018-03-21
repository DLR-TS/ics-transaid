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
#ifndef SWITCHING_MANAGER_CONTAINER_H
#define SWITCHING_MANAGER_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/switching-manager.h"

namespace ns3 {

class SwitchingManagerContainer
{
public:
  typedef std::vector<Ptr<SwitchingManager> >::const_iterator Iterator;

  SwitchingManagerContainer ();
  SwitchingManagerContainer (Ptr<SwitchingManager> manager);
  SwitchingManagerContainer (std::string managerName);
  SwitchingManagerContainer (const SwitchingManagerContainer &a, const SwitchingManagerContainer &b);

  Iterator Begin (void) const;
  Iterator End (void) const;

  uint32_t GetN (void) const;
  Ptr<SwitchingManager> Get (uint32_t i) const;
  void Add (SwitchingManagerContainer other);
  void Add (Ptr<SwitchingManager> manager);
  void Add (std::string managerName);

private:
  std::vector<Ptr<SwitchingManager> > m_devManagers;
};

} // namespace ns3

#endif /* SWITCHING_MANAGER_CONTAINER_H */
