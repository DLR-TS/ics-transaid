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

#ifndef DVBHCONTROL_PACKET_H
#define DVBHCONTROL_PACKET_H

#include <stdint.h>
#include <list>
#include <string.h>
#include "dvbh-manager.h"
#include "ns3/callback.h"
#include "ns3/ipv4-address.h"

namespace ns3 {



class DVBHControlPacket : public Object
{
public:
  DVBHControlPacket ();
  ~DVBHControlPacket ();
  static TypeId GetTypeId (void);
  void DoDispose ();

  void SetTransmissionSize (int size_);
  int GetTransmissionSize ();
  void SetMPEPacket (struct MPEPacket packet);
  struct MPEPacket GetMPEPacket ();
  void SetPESPacket (struct PESPacket packet);
  struct PESPacket GetPESPacket ();
  void SetPMTTable (struct PMTTable pmtTable_);
  struct PMTTable GetPMTTable ();
  void SetPATTable (struct PATTable patTable_);
  struct PATTable GetPATTable ();
  void SetSDTTable (struct SDTTable sdtTable_);
  struct SDTTable GetSDTTable ();

  void SetPacketType (enum DVBHPacketType  packetType_);
  enum DVBHPacketType  GetPacketType ();


private:
  int m_transmissionSize;  
  struct PESPacket m_pesPacket;
  struct PATTable m_patTable;
  struct PMTTable m_pmtTable;
  struct SDTTable m_sdtTable;
  struct MPEPacket m_mpePacket;
  enum DVBHPacketType  m_packetType;


};



} // namespace ns3


#endif
