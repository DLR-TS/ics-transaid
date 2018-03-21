/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include "ns3/c2c-list-routing.h"
#include "ns3/node.h"
#include "c2c-list-routing-helper.h"

namespace ns3 {

c2cListRoutingHelper::c2cListRoutingHelper()
{}

c2cListRoutingHelper::~c2cListRoutingHelper()
{
  for (std::list<std::pair<const c2cRoutingHelper *, int16_t> >::iterator i = m_list.begin ();
       i != m_list.end (); ++i)
    {
      delete i->first;
    }
}

c2cListRoutingHelper::c2cListRoutingHelper (const c2cListRoutingHelper &o)
{
  std::list<std::pair<const c2cRoutingHelper *, int16_t> >::const_iterator i;
  for (i = o.m_list.begin (); i != o.m_list.end (); ++i)
    {
      m_list.push_back (std::make_pair (const_cast<const c2cRoutingHelper *> (i->first->Copy ()), i->second));
    }
}

c2cListRoutingHelper* 
c2cListRoutingHelper::Copy (void) const 
{
  return new c2cListRoutingHelper (*this); 
}

void 
c2cListRoutingHelper::Add (const c2cRoutingHelper &routing, int16_t priority)
{
  m_list.push_back (std::make_pair (const_cast<const c2cRoutingHelper *> (routing.Copy ()), priority));
}

Ptr<c2cRoutingProtocol> 
c2cListRoutingHelper::Create (Ptr<Node> node) const
{
  Ptr<c2cListRouting> list = CreateObject<c2cListRouting> ();
  for (std::list<std::pair<const c2cRoutingHelper *, int16_t> >::const_iterator i = m_list.begin ();
       i != m_list.end (); ++i)
    {
      Ptr<c2cRoutingProtocol> prot = i->first->Create (node);
      list->AddRoutingProtocol (prot,i->second);
    }
  return list;
}

} // namespace ns3
