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

#ifndef PHY_NODEB_H
#define PHY_NODEB_H

#include <stdint.h>

#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/timer.h"
#include "ns3/event-id.h"
#include "umts-manager.h"
#include "ns3/propagation-loss-model.h"



namespace ns3 {

struct ue_info
{
  int paging_group;
  double bytes_slot;
  int in_seq_;
  int count;
  int phyAddress;
  // For Interference Model
  unsigned long ul_CommonErrorRate;             // 1 / BLER per frequence
  unsigned long ul_DedicatedErrorRate;          // 1 / BLER per frequence
  double CommonLastRate;                        // last rate arrived for this UE
  double DedicatedLastRate;                             // last rate arrived within the common channel for this UE
  double DedicatedPrx;                                  // Power received from this UE
  int ul_freq;                          // freq used for this UE
  double CommonPrx;                             // samples taken for interference calculation
  double dedicatedTxPower;
  int transmitting;
  Ptr<UMTSChannel> channel;
};

struct UmtsServiceMapping
{
  uint32_t serviceId;
  std::list<uint32_t > subscriberList;
};

class ControlPacket;
class UmtsPhyLayerUE;
class UMTSQueue;
class UMTSManager;
class OvsfTree;
class BlerTable;
class UMTSChannel;

enum PowerChangeStep
{
  Step0_5,
  Step1,
  Step1_5,
  Step2
};

class UmtsPhyLayerBS : public Object
{
public:
  UmtsPhyLayerBS ();
  static TypeId GetTypeId (void);
  void DoDispose ();  
  void SetMyIpAddress (Ipv4Address address);

  //NEW
  void SetMyIpv6Address (Ipv6Address address);
  void SetChannel (Ptr<UMTSChannel> channel_);
  Ptr<UMTSChannel> GetChannel ();
  void SetSharedChannel (Ptr<UMTSChannel> channel);  
  void SetRxCallback (Callback<void,Ptr<Packet>,uint8_t > callback);  
  void SetInterferenceCallback (Callback<void> interferenceCallBack_);
  
  void CalculateExternalInterference ();
  void AlertInterferenceMeasure ();

  void ForwardUp (Ptr<Packet> packet);  
  void SetInterferenceCalculationTime (double time);
  double GetInterferenceCalculationTime () const;
  
  void ReceiveFACHMessage (Ptr<Packet> packet,uint8_t packetType);
  void ReceivedDataPacketFromAbove (Ptr<Packet> packet,uint8_t packetType);

  void PacketArrivalFromChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void ReceivePacketFromBelow (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);

  void EnqueueDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k);
  void SendPacketsToChannel (std::pair<Ptr<Packet>, Ptr<ControlPacket> > myPacket);

  // Utility Functions
  int NumberOfDpdch (int id);
  int LookForUE (uint32_t phyaddr);
  int IsPacketForMe (Ptr<ControlPacket> controlpacket);  
  void StoreSlotBytes (int pdest, double bytes);
  void RegisterUE (Ptr<ControlPacket> controlpacket);
  uint32_t GetChannelCodingSize (uint32_t size,uint8_t codeTag);
  uint32_t GetChannelDecodingSize (uint32_t size,uint8_t codeTag);

  void TxAich (Ptr<ControlPacket> controlpacket);
  void TxFach (Ptr<Packet> packet);
  void MakePCCPCH (void);
  void MakeDPXCH (void);
  void UpLinkDemux (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void DownLinkMux (Ptr<Packet> packet);

  void Decode (uint32_t id);

  void RemoveResources (uint32_t addr);
  void DownSlotHandler ();
  void AichSlotHandler ();
  void BlerHandler ();
  void ErrorCalculationTimerHandler ();

  int CheckError (int pos,int type);
  void UpLinkInterference (uint32_t addr[], unsigned long error[],unsigned long error2[]);

  void SetMobility (Ptr<MobilityModel> mobility);
  Ptr<MobilityModel> GetMobility ();

  int GetPrimaryScramblingCode ();
  double CalculateRxPower (Ptr<UmtsPhyLayerUE> phy);

  ue_info m_nodeUEInformation[MAX_NUM_UE];
  uint32_t m_nodeUEIdRegistry[MAX_NUM_UE];

  void SetDedicatedTxPower (int index,double value);
  double GetDedicatedTxPower (int phyAddress);
  void SetCommonTxPower (double value);
  double GetCommonTxPower () const ;
  void SetSystemLoss (double value);
  double GetSystemLoss () const;
  void SetTxFrequency (double value);
  double GetTxFrequency () const;
  void SetMinDistance (double value);
  double GetMinDistance () const;
  void SetAntennaGain (double value);
  double GetAntennaGain () const;
  void SetAntennaHeight (double value);
  double GetAntennaHeight () const;
  void SetDedicatedPower (double value);
  double GetDedicatedPower () const;
  void SetShadowing (double value);
  double GetShadowing () const;
  void SetSnrTarget (double value);
  double GetSnrTarget () const;

  void StoreFachSlotBytes (double bytes);

  void RemoveDedicatedChannel (uint32_t id);
  void SetDownlinkFrequency (int freq);

  void CalculateExternalInterference (double interference);
  int GetInternalInterference (int index);
  void SetCoverageRange(double value);

  double GetCoverageRange() const;
  int GetTxState ();

  void ChangeDedicatedTxPower (int index,int sign);
  void SetControlAlgothmType (enum PowerChangeStep step);
  enum PowerChangeStep GetControlAlgothmType () const;

  Ptr<UMTSChannel> GetSharedChannel ();
  void NotifyTxPowerChange (int phyAddress,int sign);
  
  void MakeSCCPCHPackets ();
  void TxFachDataPacket (Ptr<Packet> packet,uint8_t packetType);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();
  void AddServiceList(std::list<uint64_t > serviceList_);
  void AddNewUserToService(uint64_t serviceId,uint32_t nodeIdentifier);
  std::list<uint32_t > ReturnSubscribers(uint64_t serviceId);

private:
  
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > m_rxDpdchQueue[MAX_NUM_UE][MAX_NUM_DPDCH];
   
  wcdma_control m_wcdmaParams;      // control parameters for wcdma coding
  dpcch_contr m_dpdchControlParams[MAX_NUM_UE];    // control parameters for DPDCH multiplexing
  int m_usedSignatureForUE[MAX_NUM_UE];      // signature used for each ue
 
  Ptr<Packet> m_fachPacket;
  
  EventId m_aichTimer;
  EventId m_blerTimer;
  EventId m_errorCalculationTimer;
  
  Ptr<UMTSQueue> m_aichPacketsToTx;  
  Ptr<UMTSQueue> m_packetToTxQueue;
  Ptr<UMTSQueue> m_dedicatedMacDataQueue[MAX_NUM_UE];        // packet received in the current tti from MAC
  Ptr<UMTSQueue> m_commonMacDataQueue;
  
  Ptr<UMTSChannel> m_channel;
  Ptr<UMTSChannel> m_sharedChannel;
  
  Ipv4Address m_myIp; 

  Ipv6Address m_myIpv6; 
  int m_downlinkSlotCounter;
  double m_tti;  
  int m_SequenceNumber;

  static double start_time_; // The start time for whole WCDMA scheduling.
  static double slot_time_;  // The duration of each WCDMA slot
  static double aich_slot_time_;  // The duration of each AICH slot

  double m_externalInterference;                  // Interference power receceived from other NodeB
  double m_internalCommonInterference[MAX_NUM_UE];                        // Interference power from UEs in our cell within the common channel
  double m_internalDedicatedInterference[MAX_NUM_UE];                     // Interference power from UEs in our cell within the common channel
  double m_totalInternalInterference[MAX_NUM_UE];                 // Interference power from UEs in our cell
  int m_numberExternalInterference;                       // Number of samples of externalInter
  int m_numberCommonInternalInterference[MAX_NUM_UE];                     // Number of samples of internalInterference on the common channel
  int m_numberDedicatedInternalInterference[MAX_NUM_UE];                  // Number of samples of externalInter

  double m_bytesPerFachSlot;

  uint32_t m_addr[MAX_NUM_UE];                         // for passing results to MAC

  BlerTable* m_blerTable;
  Callback<void, Ptr<Packet>,uint8_t > m_rxCallback;

  errormodule m_uplinkInterference[MAX_NUM_UE];    // structure for error model
  Ptr<MobilityModel> m_mobility;

  double m_commonTxPower;
  double m_dedicatedTxPower;
  double m_antennaHeight;
  double m_systemLoss;
  double m_minDistance;
  double m_txFrequency;
  double m_antennaGain;
  double m_shadowing;
  double m_snrTarget;
  
  typedef std::list<struct UmtsServiceMapping> ServiceTable;
  typedef std::list<struct UmtsServiceMapping>::iterator ServiceTableI;
  ServiceTable m_serviceTable;

  int m_downlinkFrequency;

  Callback<void> m_interferenceCallBack;
  int m_transmitting;

  double m_powerControlStep;
  uint32_t m_nodeIdentifier;
  double m_calculationTime;
  double m_coverageRange;
        
};

} // namespace ns3


#endif /*PHY_NODEB_H */
