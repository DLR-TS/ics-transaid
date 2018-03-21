/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */


#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
#include "ns3/ipv4-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/mac48-address.h"
#include "dvbh-controlpacket.h"

NS_LOG_COMPONENT_DEFINE ("DVBHControlPacket");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHControlPacket);

TypeId
DVBHControlPacket::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHControlPacket")
    .SetParent<Object> ()
    .AddConstructor<DVBHControlPacket> ()
  ;
  return tid;
}

DVBHControlPacket::DVBHControlPacket ()
{

}

DVBHControlPacket::~DVBHControlPacket ()
{

}

void
DVBHControlPacket:: DoDispose ()
{


}

void
DVBHControlPacket::SetTransmissionSize (int size_)
{
  m_transmissionSize = size_;
}

int
DVBHControlPacket::GetTransmissionSize ()
{
  return m_transmissionSize;
}

void
DVBHControlPacket::SetPESPacket (struct PESPacket packet)
{
  m_pesPacket = packet;
}

struct PESPacket
DVBHControlPacket::GetPESPacket ()
{
  return m_pesPacket;
}

struct MPEPacket
DVBHControlPacket::GetMPEPacket ()
{
  return m_mpePacket;
}

void
DVBHControlPacket::SetMPEPacket (struct MPEPacket packet)
{
  m_mpePacket = packet;
}

void
DVBHControlPacket::SetPMTTable (struct PMTTable pmtTable_)
{
  m_pmtTable = pmtTable_;
}

struct PMTTable
DVBHControlPacket::GetPMTTable ()
{
  return m_pmtTable;
}

void
DVBHControlPacket::SetPATTable (struct PATTable patTable_)
{
  m_patTable = patTable_;
}

struct PATTable
DVBHControlPacket::GetPATTable ()
{
  return m_patTable;
}

void
DVBHControlPacket::SetSDTTable (struct SDTTable sdtTable_)
{
  m_sdtTable = sdtTable_;
}

struct SDTTable
DVBHControlPacket::GetSDTTable ()
{
  return m_sdtTable;
}

void
DVBHControlPacket::SetPacketType (enum DVBHPacketType  packetType_)
{
  m_packetType = packetType_;
}

enum DVBHPacketType
DVBHControlPacket::GetPacketType ()
{
  return m_packetType;
}




} // namespace ns3
