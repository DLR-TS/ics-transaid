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


#ifndef DVBH_OFDM_LAYER_H
#define DVBH_OFDM_LAYER_h

#include <stdint.h>
#include <list>
#include "ns3/callback.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/event-id.h"
#include "dvbh-queue.h"


namespace ns3 {

class DVBHChannel;



struct OFDMBlock
{
  uint32_t serviceId;
};

class DVBHOfdmLayer : public Object
{
public:
  int number;
  DVBHOfdmLayer ();
  static TypeId GetTypeId (void);
  void DoDispose ();
  void SetForwardUpPsiInfoCallback (Callback<void,struct TSPacket,bool > upCallback);
  void ForwardUpPsiInfo (struct TSPacket packet,bool sleep);
  void SetForwardUpCallback (Callback<void,Ptr<Packet>,bool > upCallback);
  void ForwardUp (Ptr<Packet>,bool sleep);


  Ptr<DVBHChannel> GetChannel ();
  void SetChannel (Ptr<DVBHChannel> channel_);
  void ReceivePacketsFromAbove (struct TSPacket packet);
  void SendPacketsToChannel (struct TSPacket packet);
  void ReceivePacketFromChannel (struct TSPacket packet);
  int GetIdentifier ();
  double GetBandwidth (void) const;
  void SetBandwidth (double BW);
  double GetTxPower (void) const;
  void SetTxPower (double txPower);

  double GetNoiseFigure (void);
  void SetNoiseFigure (double txPower);
  uint16_t GetNrBlocks (uint32_t burstSize) const;
  void ReceiveBurstFromAbove (Ptr<DVBHQueue> burst,uint32_t serviceId);
  void SetBlockParameters (uint32_t size);
  void CreateFecBlocks (Ptr<DVBHQueue> burst,uint32_t serviceId);

  void StartSendFecBlock (bool isFirstBlock,uint32_t symbolIndex);
  void ReceiveBurstFromChannel (Ptr<DVBHQueue> burst,double rxPower);
  void StartReceive (struct OFDMBlock block,bool isFirstBlock, uint64_t frequency,Ptr<DVBHQueue> burst,double rxPower);

  Time GetBlockTransmissionTime () const;
  Time DoGetTransmissionTime (uint32_t size) ;
  uint64_t DoGetNrSymbols (uint32_t size) ;
  Time DoGetTransmissionTime ();
  uint64_t DoGetNrBytes (uint32_t symbols);

  void SetNrCarriers (uint32_t carriers);
  uint32_t GetNrCarriers ();
  uint32_t GetFecBlockSize () const;
  uint64_t GetTxFrequency () const;
  void SetTxFrequency(double frequency);
  uint64_t GetRxFrequency () const;
  void SetRxFrequency (double frequency);

  void SetSymbolDuration (Time symbolDuration);
  Time GetSymbolDuration ();

  void SetOperationMode (enum OfdmOperationMode mode);
  enum OfdmOperationMode GetOperationMode () const;

  void SetModulationType (enum ModulationType modulationType);
  enum ModulationType GetModulationType ()const;

  void SetMobility (Ptr<MobilityModel> mobility);
  Ptr<MobilityModel> GetMobility ();

  void SetNodeIdentifier (uint32_t nodeIdentifier);

  uint32_t GetNodeIdentifier ();

  void ConfigureSystem (double lambda_,double minDistance_,double systemLoss_,double antennaGain_,
                        double txPower,double antennaHeight_,enum OfdmOperationMode mode,enum ModulationType modulationType);

  double GetSystemLoss () const;
  double GetLambda ()const;
  double GetMinDistance () const;
  double GetAntennaGain () const;
  double GetAntennaHeight ()const;
  
  void SetAntennaHeight(double value);
  void SetSystemLoss(double value);
  void SetLambda(double value);
  void SetMinDistance(double value);
  void SetAntennaGain(double value);

  double GetTotalInterference ()const;
  void SetTotalInterference (double value);
  void SetShadowing (double shadowing);

  double GetShadowing () const;
  
  

private:
  Ptr<DVBHChannel> m_channel;
  Ptr<MobilityModel> m_mobility;
  Callback<void,struct TSPacket,bool > m_forwardUpPsiInfo;
  Callback<void,Ptr<Packet>,bool > m_forwardUp;

  int myIdentifier;
  typedef std::list<Ptr<DVBHTsQueue> > TsQueueList;
  typedef std::list<Ptr<DVBHTsQueue> >::iterator TsQueueListI;

  typedef std::list<struct OFDMBlock> OfdmBlockBurst;
  typedef std::list<struct OFDMBlock>::iterator OfdmBlockBurstI;
  OfdmBlockBurst m_ofdmBlockBurst;
  
  OfdmBlockBurst m_receivedFecBlocks;

  Ptr<DVBHQueue> m_burstCopy;

  double m_bandWidth;
  double m_txPower;
  double m_noiseFigure;
  uint64_t m_rxFrequency;
  uint64_t m_txFrequency;
  Time m_symbolDuration;
  uint32_t m_numberCarriers;

  double m_systemLoss;
  double m_minDistance;
  double m_lambda;
  double m_antennaGain;
  double m_antennaHeight;
  double m_interference;
  double m_shadowing;

  ModulationType m_modulationType;

  uint16_t DoGetTtg (void) const;
  uint16_t DoGetRtg (void) const;
  uint8_t DoGetFrameDurationCode (void) const;
  Time DoGetFrameDuration (uint8_t frameDurationCode) const;
  uint16_t DoGetNfft (void) const;
  double DoGetSamplingFactor (void) const;
  double DoGetSamplingFrequency (void) const;
  double DoGetGValue (void) const;
  void EndSendFecBlock (uint32_t symbolIndex);

  void EndReceive (/*OfdmBlockBurst burst,*/double rxPower);
  void EndReceiveFecBlock (struct OFDMBlock block,double rxPower);

  void DoSetDataRates (void);
  void GetModulationFecParams (ModulationType modulationType,uint8_t &bitsPerSymbol, double &fecCode) const;
  uint32_t CalculateDataRate (ModulationType modulationType) ;
  uint32_t DoGetDataRate () const;

  void DoSetPhyParameters (void);

  uint16_t m_fecBlockSize;   // in bits, size of FEC block transmitted after PHY operations
  uint32_t m_nrFecBlocksSent;   // counting the number of FEC blocks sent (within a burst)

  Time m_blockTime;

  // data rates for this Phy
  uint32_t m_dataRateQpsk12, m_dataRateQpsk34,m_dataRateQam16_12, m_dataRateQam16_34;

  // parameters to store for a per burst life-time
  uint16_t m_nrBlocks;
  uint16_t m_blockSize;
  uint32_t m_paddingBits;
  uint8_t m_nbErroneousBlock;

  enum OfdmOperationMode m_operationMode;

  int m_nodeIdentifier;

};

} // namespace ns3

#endif /* DVBH_OFDM_LAYER_h */
