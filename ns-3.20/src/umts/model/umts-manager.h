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

#ifndef UMTS_MANAGER_H
#define UMTS_MANAGER_H

#include <stdint.h>
#include <list>

#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "umts-net-device.h"

#define DATA_DURATION           5
// in Umts, the mac/rlc hdr is 8 bits.
#define MAX_FLOWS 10
#define MAX_LOAD        (MAX_FLOWS * MAX_NUM_UE)        // max of flows that can support

#define UMTS_FrameTime          0.010                   // 10 ms
#define UMTS_TBLength           2560                    // 2560 chips
#define UMTS_SlotTime           0.0006666667    // 0.667 ms
#define CHIP_RATE                       3840000         // 3.84 Mcps
#define Pn                              6.1e-14         // Noise Power (Thermal and Equipment)
// Nrf*ChipRate
// Nrf = k*T + NF (NF aprox 6dB)
//#define BROADCAST (-1)
//#define MULTICAST (-2)

#define MAX_NUM_FREQ 12  // maximum number of frequencies permissiblein the cell
#define MAX_NUM_UE 101    // maximum number of UE's permissible in the cell
#define MAX_NUM_BS 10    // maximum number of NodeB's permissible in the simulation
#define MAX_NUM_NODES (MAX_NUM_UE * MAX_NUM_BS) // maximum number of Nodes permissible in the simulation
#define SLOTS_PER_FRAME 15 // number of slots in evry WCDMA frame
#define MAX_NUM_DPDCH 6  // maximum number of DPDCH
#define MAX_NUM_SCRAM 16  // maximum number of scrambling codes in a group/cell
#define MAX_NUM_RACH 16  // maximum number of RACH in a group/cell
#define MAX_NUM_SIG 16  // maximum number of signatures
#define MAX_NUM_PREAMBLE 8 // maximum number of preamble tx.
#define MAX_NUM_FLOWS 5 // maximum number of flows.
#define MAX_NUM_BYTES_PER_SLOT 80 // maximum number of bytes per slot.
#define MAX_NUM_BYTES_PER_SLOT_DL 160 // maximum number of bytes per slot in dl.
#define MAX_NUM_FLOWS_DL 100
#define PHY_HDR_SZ 2 // physical header size in bytes.
#define RLC_HDR_SZ      5 //1 // rlc header size in bytes.
#define MAC_HDR_SZ                2 // mac header size in bytes.
#define MAX_NUM_IPv6	5

#define DEBUG_UmtsUserEquipmentManager 0
  
#define DedicatedUnicastData 0
#define CommonUnicastData 1
#define BroadcastData 2
#define Ack 3
#define MulticastData 4
#define DirectPrimive 5
#define OtherData 6

#define TURBO 0
#define CONV_HALF 1
#define CONV_THIRD 2
#define DEBUG_RLCNODEB 1

namespace ns3 {

class ControlPacket;
class UMTSQueue;

struct Item
{
  Ptr<Packet> packet;
  Ptr<ControlPacket> controlInfo;
};


struct dpcch_contr
{
  int numberOfDpdch;                    // number of DPDCHs multiplexed with DPCCH
  double dpdchRate;             // rate tx in DPDCHs
};

struct Service
{
  char *serviceName;
  int serviceUsers[MAX_NUM_UE];
  int packetSize;
  double startTime;
  bool multicast;
  bool joined;
  double duration;
  double sizeReceived;
  double txRate;
  Ipv4Address ipAddress;  
  std::list<int > subscriberList;

};

struct UmtsService
{
  char *name;  
  uint32_t burstSize;
  double rate;
  std::string type;
  uint64_t id;
  double waitTime;  
  Ipv4Address ipAddress;
  std::list<uint32_t > subscriberList;

};

enum TxType
{
  Dedicated = 0,
  Common = 1
};



enum LLType
{
  LL_SETUP = 0,
  LL_RELEASE_REQ = 1,
  LL_FAILURE = 2,
  LL_RELEASE_REPLY = 3,
  LL_RES_REQ = 4,
  LL_PAGING = 5,
  LL_RES_REPLY = 6,
  LL_HANDOVER = 7,
  LL_SETUP_REPLY = 8,
  LL_DATA = 9,  
  LL_MBMS_INFOR = 10,
  LL_MBMS_MULTICAST = 11,
  LL_MBMS_BROADCAST = 12,
  OTHER = 13,
  LL_BCH_BROADCAST=14
};

enum UmtsChannelType
{
  CCCH = 0,
  FROM_RRC = 1,
  TO_RRC = 2,
  RACH = 3,
  PRACH = 4,
  DPCCH = 5,
  SCH = 6,
  AICH = 7,
  PICH = 8,
  FACH = 9,
  PCCPCH = 10,
  SCCPCH = 11,
  DCH = 12,
  DTCH = 13,
  PCH = 14,
  DPDCH = 15,
  PCCH = 16
};

struct wcdma_control
{
  int p_scrambling_;       // primary scrambling code
  int signature_;       // signature used in the last RACH access
  int scram_tx_;        // scrambling code used in the las RACH access
  int rach_tx_;         // rach used
  int free_res_[MAX_NUM_SCRAM][MAX_NUM_SIG + 1];        // array with the available ul scrambling codes (0 available)
                                                        // and signatures
  int free_rach_[MAX_NUM_RACH];         // available RACHs
};

enum PacketType
{
  Default = 0,
  PT_ACK = 1,
  PT_RRC_FAILURE = 2,
  PT_PREAMBLE = 3

};

struct PhyInformation
{

  uint32_t phySourceAddress;

  uint32_t phyDestinationAddress;
  int firstAccess;      // for initial access to a cell
  int scramblingCode;
  int signature;
  int spreadingFactor;
  int channelisationCode;
  int frequency;
  int Frequency;                // freq given to the UE
  double rxPower;
  int primaryScramblingCode;    // primary scrambling code (only SCH)
  int usedRach;                 // rach used
  int beginOfSequence;
  int sequenceNumber;
  int endOfSequence;
  int pagingIndicator;
  int freeResources[MAX_NUM_SCRAM][MAX_NUM_SIG + 1]; // array with the available ul scrambling codes and signatures
  int rach[MAX_NUM_RACH];  // available RACH
  float txRate;
  int paging_ok;
  double dataSize;
  dpcch_contr dpcch_control_;   // control information in DPCCH
  int pagingGroup;

// for DPDCH
  Ptr<UMTSQueue> dpdch_control_; // control information in DPDCH;

};


struct CommonInformation
{
  Ipv4Address source;
  Ipv4Address destination;
  Ipv6Address sourcev6;
  Ipv6Address destinationv6;  
  LLType lltype;
  UmtsChannelType channel;
  PacketType ptype;
  Ptr<UmtsPhyLayerUE> phyNodeUE;
  Ptr<UmtsPhyLayerBS> phyNodeB;
  bool packetRRCFailure;  
  int size;
      
};

struct flows_nodeb
{
  uint32_t flow_;  
  uint32_t destination;
  int fraged_;
  int acked_;
  uint32_t lastFragment;             
  uint32_t seqno_;                                   // link-layer sequence number
  uint32_t ackno_;                                   // ACK received so far
  int rackno_;                          // seq no of left most pkt
  uint32_t window_;                          // window size for sack
  Ptr<UMTSQueue> Txbuf_;                // Tx buffer
  Ptr<UMTSQueue> Rxbuf_;                // Rx buffer
  int unackseqno_;
  int inseq_;
  int numdups_;
  Ptr<Packet> lastRx_;                         // last packet received in sequence  
  int handover_;
  enum TxType txType;
};

struct flow_inf
{
  int flow_;
  int fraged_;
  int acked_;
  enum TxType txType;
  int seqno_;                                   // link-layer sequence number
  int ackno_;                                   // ACK received so far
  int lastFragment;             
  int rackno_;                          // seq no of left most pkt
  int window_;                          // window size for sack
  Ptr<UMTSQueue> Txbuf_;                // Tx buffer
  Ptr<UMTSQueue> Rxbuf_;                // Rx buffer
  int unackseqno_;
  int numdups_;
  Item lastRx_;                         // last packet received in sequence
};

struct location                 // for the static matrix
{
  uint32_t ue_;              // UE IP address
  uint32_t nodeb_;           // Node B IP address at which is attached the UE
};

struct NodeUEInformation        // for internal purposes
{
  Ipv4Address ipaddr_;                  // UE IP address
  //NEW
  Ipv6Address ipv6addr_[MAX_NUM_IPv6];                  // UE IPv6 address
  uint32_t identifier;                 // UE physical address
  int dltempsf_;                // spreading factor temporal storage
  int dlsf_;                            // downlink spreading factor assigned to that UE
  int dlfreq_;                  // downlink frequence assigned to that UE
  double dltotalr_;                     // downlink total physical rate to that UE
};


// structure for error model
struct errormodule
{
  uint32_t phyaddr_;                         // physical address for the UE
  int CommonError_;                                     // 0 not erroneus, 1 error in Transport Block
  int DedicatedError_;                                  // 0 not erroneus, 1 error in Transport Block
  unsigned long ul_dedicated_error_;            // error probability in tx block.
  unsigned long ul_common_error_;
};

struct apps_info
{
  uint64_t flowid_;                  // flow id
  Ipv4Address dest_;                    // IP address of node destiny
  //NEW
  Ipv6Address destv6_;                    //IPv6 address of node destiny                    
  int wait_;                            // 0, the flow has asked for resources to the Node B
                                        // and is waiting for them; 1, resources allocated
  std::string serviceId;
  int sf_;                              // spreading factor assigned to that application
  double rate_;                 // physical rate of that application
  double user_rate_;            // rate coming from the applicatin
  double interval_;                     // interval between the arrival of two packets
  uint32_t c_coding_;             // channel coding applied to that application
  enum TxType txType;
  int isMulticast;
  int isBroadcast;

};



} // namespace ns3


#endif /*UMTS_MANAGER_h */
