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

#ifndef DVBH_MANAGER_H
#define DVBH_MANAGER_H

#include <stdint.h>
#include <list>

#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"

#define BROADCAST -1
#define MULTICAST -2

#define DVBH_MAX_NUM_UE 100    // maximum number of UE's permissible in the cell

#define PES_HDR_SZ      6 // PES packet's header size
#define PES_OPTIONAL_HDR_SZ     2 // PES packet's OPTIONAL header size
#define TS_HDR_SZ                 4 // TS packet's header size in bytes.
#define PES_MAX_SZ                65536 // PES Packet's maximum size
#define TS_MAX_SZ               188     // TS Packet maximum size
#define MPE_HDR_SZ              12      // MPE Frame's header size
#define MPE_FEC_SZ              4       // MPE Frame's FEC size

#define emergencyInfo = 0,
#define  weatherInfo = 1,
#define newsInfo = 2,
#define trafficInfo = 3,
#define  otherInfo = 4,
#define  none = 5,

namespace ns3 {


enum StreamType
{
  audio = 0,
  video = 1,
  other = 2,
};

enum DVBHPacketType
{
  pmtTablePacket = 0,
  patTablePacket = 1,
  mpePacket = 2,
  sdtTablePacket = 3,
};

enum FrameDurationCode
{
  FRAME_DURATION_2_POINT_5_MS,
  FRAME_DURATION_4_MS,
  FRAME_DURATION_5_MS,
  FRAME_DURATION_8_MS,
  FRAME_DURATION_10_MS,
  FRAME_DURATION_12_POINT_5_MS,
  FRAME_DURATION_20_MS
};

enum ModulationType // Table 356 and 362
{
  MODULATION_TYPE_QPSK_12,
  MODULATION_TYPE_QPSK_34,
  MODULATION_TYPE_QAM16_12,
  MODULATION_TYPE_QAM16_34,
};

enum OfdmOperationMode
{
  Mode2_k,
  Mode4_k,
  Mode8_k,
};

enum PhyState
{
  PHY_STATE_IDLE, PHY_STATE_SCANNING, PHY_STATE_TX, PHY_STATE_RX, PHY_STATE_OFF, PHY_STATE_ON
};

struct TransportStream
{

  std::string name;
  int id;
  double bandwidth;
};

struct ElementaryStream
{
  std::string name;
  int id;
  int totalSize;
  int sizeToSend;
  int numberOfPackets;
};

struct MPEPacket
{
  int size;
  int streamId;
  int currentSection;
  int lastSection;
  int tableId;
  Time delta_t;
};

struct PATEntry
{
  int programNumber;
  int programMapId;

};

struct SDTTable
{
  int8_t tableId;
  int8_t transportStreamId;
  int size;
  std::list< struct SDTEntry> entryList;
};

struct SDTEntry
{
  int programNumber;
  std::string descriptor;

};

struct PATTable
{
  int8_t tableId;
  int8_t transportStreamId;
  int size;
  std::list< struct PATEntry> entryList;
};

struct PMTTable
{
  int8_t tableId;
  int programNumber;
  int size;
  std::list< struct PMTEntry> entryList;

};

struct PMTEntry
{
  std::string streamType;
  int elementaryPID;
};

struct PESPacket
{
  int size;
  int freeSize;
  int sizeToSend;
  int id;
  std::list<struct MPEPacket> fragmentList;
};









} // namespace ns3


#endif /*DVBH_MANAGER_h */
