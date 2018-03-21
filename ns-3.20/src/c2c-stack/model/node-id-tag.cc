/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
#include "node-id-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId 
NodeIdTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NodeIdTag")
    .SetParent<Tag> ()
    .AddConstructor<NodeIdTag> ()
    .AddAttribute ("nodeId", "Id of the sender",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NodeIdTag::Get),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

TypeId
NodeIdTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

NodeIdTag::NodeIdTag()
{}

uint32_t 
NodeIdTag::GetSerializedSize (void) const
{
  return 4;
}

void
NodeIdTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_nodeId);
}

void
NodeIdTag::Deserialize (TagBuffer i)
{
  m_nodeId = i.ReadU32 ();
}

void
NodeIdTag::Set (uint32_t nodeId)
{
  m_nodeId = nodeId;
}

uint32_t
NodeIdTag::Get () const
{
  return m_nodeId;
}

void
NodeIdTag::Print (std::ostream &os) const
{
  os << "NodeId=" << m_nodeId;
}

} //namespace ns3
