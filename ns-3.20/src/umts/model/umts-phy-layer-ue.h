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

#ifndef PHY_NODEUE_H
#define PHY_NODEUE_H

#include <stdint.h>

#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"
#include "ns3/object.h"
#include "ns3/timer.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/event-id.h"
#include "umts-manager.h"
#include "ns3/mobility-model.h"
#include "ns3/propagation-loss-model.h"



namespace ns3 {


class UmtsPhyLayerBS;
class UMTSQueue;
class BlerTable;
class UMTSChannel;

enum PowerChangeStepUE
{
  StepUE1,
  StepUE2,
  StepUE3
};

class UmtsPhyLayerUE : public Object
{

public:
  UmtsPhyLayerUE ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  
  void SetMyIpAddress (Ipv4Address address);

  //NEW
  void SetMyIpAddress (Ipv6Address address);
  void SetDedicatedChannel (Ptr<UMTSChannel> channel_);
  Ptr<UMTSChannel> GetChannel ();
  void SetSharedChannel (Ptr<UMTSChannel> channel);

  void SetHandoverState (int value);
  void SetDownLinkFrequencyUsed (int freq);
  int GetNodeBPhyAddress ();
  void SetAttachedNodeB (Ptr<UmtsPhyLayerBS> nodeB_);
  void SetForwardToRRCCallback (Callback<void,uint32_t > rrcCallback);

  void SetRxCallback (Callback<void,Ptr<Packet>,uint8_t > callback);

  void SetInterferenceCallback (Callback<void> interferenceCallBack_);
  void CalculateExternalInterference (double interference);
  void AlertInterferenceMeasure ();

  void ForwardUp (Ptr<Packet> packet);

  // Send Control Packets directly to the RRC Layer
  void ForwardToRRC (uint32_t applicationId);

  // Reception from the Upper Layers
  void ReceiveRACHMessage (Ptr<Packet> packet,uint8_t packetType);
  void ReceivedDataPacketFromAbove (Ptr<Packet> packet,uint8_t packetType);
  
  // Receive Packet from Below
  void ReceivePacketFromBelow (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void PacketArrivalFromChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);

  // Send Data Packet or Control Packet to the Channel
  void SendPacketToChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void EnqueueDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k);

  // Creating Control Packets and prepare them to be sent
  void MakeRachWithoutPreamble (Ptr<Packet> packet);
  void MakeDPXCH (void);
  void TxRachPreamble ();
  void SendRachWithPreamble ();

  // Other functions
  int ScramblingCodeAllocation ();
  void UpLinkMux (Ptr<Packet> packet);
  void DownLinkDemux (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo);
  uint32_t GetChannelCodingSize (uint32_t size,uint8_t codeTag);
  uint32_t GetChannelDecodingSize (uint32_t size,uint8_t codeTag);
  void Decode ();
  int GetNumberOfDpdchArrived ();
  void StoreSlotBytes (double bytes);

  void PreambleHandler ();
  void RachUpSlotHandler ();
  void UpSlotHandler ();
  void BlerHandler ();
  void ErrorCalculationTimerHandler ();

  void DownLinkInterference (unsigned long error);
  int CheckError ();

  void SetMobility (Ptr<MobilityModel> mobility);
  Ptr<MobilityModel> GetMobility ();

  void SetupReplyReceived (Ptr<ControlPacket> controlpacket);
  void SetUplinkFrequency (int value);

  void SetDedicatedTxPower (double value);
  double GetDedicatedTxPower () const;
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
  void SetSnrTarget (double value);
  double GetSnrTarget () const;

  enum PowerChangeStepUE GetControlAlgothmType () const;
  void SetControlAlgothmType (enum PowerChangeStepUE step);
  void StoreRachSlotBytes (double bytes);
  void ChangeTxPower (int sign);
  int GetTransmitting ();
  void SetExternalInterference (double value);
  void SetCalculatedInternalInterferenceDownlink (double value);
  void MakeRACHDataPackets ();
  void TxRachDataPacket (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket);
  void EnqueueRachDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();  
  void SetInterferenceCalculationTime (double time);
  double GetInterferenceCalculationTime () const;
  
private:
  Ptr<Packet> m_usingRachPacket;
  Ptr<ControlPacket> m_usingRachControlInfo;

  static double slot_time_;  // The duration of each WCDMA slot
  static double rach_slot_time_;  // The duration of each RACH slot
  int m_slotNumber;

  // Parameters related to RACH Channel

  int m_txPreambleNumber;   // number of RACH tx. retries
  int m_selectedScramCode;                     // primary scrambling code selected
  int m_waitRachProcedure;            // wait to tx the next preamble
  int m_handover;
  wcdma_control m_wcdmaParams;      // control parameters for wcdma coding  
        
  // Parameters related to the Handover and Setup Procedure

  int m_packetsInSequence;                  // counter for decoding
  int m_packetsInSequenceH;                 // counter for decoding during handover procedure
  int m_ulfreq;                           // UpLink frequence
  int m_dlfreq;                           // DownLink frequence
  double m_dedicatedLastRate;                     // last rate arrived
  double m_commonLastRate;                        // last rate arrived
  double m_dedicatedRxPw;                          // Power received from NodeB
  double m_commonRxPw;                             // Power received from NodeB
  double m_dedicatedInterference;
  double m_commonInterference;
  int m_commonInterferenceSamples;
  int m_dedicatedInterferenceSamples;
  
  dpcch_contr m_dpdchControlParams;        // control parameters for DPDCH multiplexing
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > m_rxDpdchQueue[MAX_NUM_DPDCH];

  double m_bytesPerSlot;
  double m_bytesPerRachSlot;
  int m_uplinkSlotCounter;
  int m_scramCodeDedicatedCh;                    // ul scrambling code used in dedicated channels during a frame
    
  double m_tti;
  unsigned long m_downlinkErrorRate;           // 1 / BLER per frequence  
  int m_dedicatedSequenceNumber;                    // counter for multiplexing
  int m_commonSequenceNumber;                        // counter for RACH multiplexing

  Ptr<UMTSChannel> m_sharedChannel;
  
  uint32_t m_nodeBIdentifier;
  
  Ipv4Address m_nodeBIp;
  Ipv4Address m_myIp;

  //NEW
  Ipv6Address m_myIpv6;
    
  EventId m_preambleTimer;
  EventId m_upSlotTimer;
  EventId m_rachUpSlotTimer;  
  EventId m_blerTimer;
  EventId m_errorCalculationTimer;

  Ptr<UmtsPhyLayerBS> m_nodeBPhyLayer;
  
  bool m_rachTotallySent;

  Ptr<UMTSQueue> m_rachQueue;
  Ptr<UMTSQueue> m_prachTemp;
  Ptr<UMTSQueue> m_packetToTxQueue; // packets to tx in the current slot
  Ptr<UMTSQueue> m_dedicatedMacDataQueue;    // packet received in the current slot from MAC
  Ptr<UMTSQueue> m_commonMacDataQueue;

  Callback<void,uint32_t > m_forwardToRRC;
  BlerTable* m_blerTable;

  Ptr<UMTSChannel> m_channel;
  Callback<void, Ptr<Packet>,uint8_t > m_rxCallback;
  
  int m_errorIndicator;           // 0 not erroneous, 1 error in TB
  unsigned long m_calculatedErrorRate;       // error probability in transport block

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

  double m_externalInterference;
  double m_internalInterference;

  Callback<void> m_interferenceCallBack;
  int m_powerControlStep;
  int m_transmitting;

  int m_powerControlNumber[5];
  
  uint32_t m_nodeIdentifier;
  double m_calculationTime;
      
};

} // namespace ns3


#endif /*PHY_NODEUE_H */
