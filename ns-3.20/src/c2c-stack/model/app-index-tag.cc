/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Michele Rondinone <mrondinone@umh.es>
 */
#include "app-index-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
AppIndexTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AppIndexTag")
    .SetParent<Tag> ()
    .AddConstructor<AppIndexTag> ()
    .AddAttribute ("appindex", "index of this application in the application container of the node",
                   UintegerValue (0),
                   MakeUintegerAccessor (&AppIndexTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
AppIndexTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

AppIndexTag::AppIndexTag()
{
}

uint32_t 
AppIndexTag::GetSerializedSize (void) const
{
  return 4;
}

void
AppIndexTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_appindex);
}

void
AppIndexTag::Deserialize (TagBuffer i)
{
  m_appindex = i.ReadU32 ();
}

void
AppIndexTag::Set (uint32_t appindex)
{
  m_appindex = appindex;
}

uint32_t
AppIndexTag::Get () const
{
  return m_appindex;
}

void
AppIndexTag::Print (std::ostream &os) const
{
  os << "application index=" << m_appindex;
}

} //namespace ns3
