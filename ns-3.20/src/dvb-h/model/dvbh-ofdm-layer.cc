/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008, 2009 INRIA, UDcast
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
 * Author:Sendoa Vaz
 */

#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "dvbh-channel.h"
#include "ns3/log.h"
#include "dvbh-ofdm-layer.h"
#include "dvbh-queue.h"
#include "dvbh-tags.h"
#include "ts-header.h"
#include "ns3/enum.h"

NS_LOG_COMPONENT_DEFINE ("DVBHOfdmLayer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED ( DVBHOfdmLayer);


static int ident = 0;

TypeId
DVBHOfdmLayer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHOfdmLayer")
    .SetParent<Object> ()
    .AddConstructor<DVBHOfdmLayer> ()
    .AddAttribute ("AntennaHeight",
                   "Height of the Antenna in metres (m)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::m_antennaHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("AntennaGain",
                   "Antenna Gain (dB)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::m_antennaGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SystemLoss",
                   "Loss associated to the System (dB)",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::SetSystemLoss,
                                       &DVBHOfdmLayer::GetSystemLoss),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxPower",
                   "Transmission Power (dB)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::m_txPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxFrequency",
                   "Transmission Frequency (GHz)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::SetTxFrequency,
				       &DVBHOfdmLayer::GetTxFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RxFrequency",
                   "Reception Frequency (GHz)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::SetRxFrequency,
                                       &DVBHOfdmLayer::GetRxFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinDistance",
                   "Minimum Distance to consider for Path Loss calculation (m)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::SetMinDistance,
                                       &DVBHOfdmLayer::GetMinDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Shadowing",
                   "Loss associated to the Shadowing (dB)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::m_shadowing),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TotalInterference",
                   "Estimated total Interference (dB)",
                   DoubleValue (0),
                   MakeDoubleAccessor (&DVBHOfdmLayer::m_interference),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TransmissionMode",
                   "Selected TransmissionMode (2K,4K,8K)",
                   EnumValue (Mode4_k),
                   MakeEnumAccessor(&DVBHOfdmLayer::SetOperationMode,
                                       &DVBHOfdmLayer::GetOperationMode),
                   MakeEnumChecker(Mode2_k,"2K",Mode4_k,"4K",Mode8_k,"8K"))
    .AddAttribute ("Modulation",
                   "Selected Modulation (MODULATION_TYPE_QPSK_12,MODULATION_TYPE_QPSK_34,MODULATION_TYPE_QAM16_12,MODULATION_TYPE_QAM16_34)",
                   EnumValue (MODULATION_TYPE_QPSK_12),
                   MakeEnumAccessor(&DVBHOfdmLayer::SetModulationType,
                                       &DVBHOfdmLayer::GetModulationType),
                   MakeEnumChecker(MODULATION_TYPE_QPSK_12,"MODULATION_TYPE_QPSK_12",MODULATION_TYPE_QPSK_34,
				   "MODULATION_TYPE_QPSK_34",MODULATION_TYPE_QAM16_12,"MODULATION_TYPE_QAM16_12",
				   MODULATION_TYPE_QAM16_34,"MODULATION_TYPE_QAM16_34"))  

  ;
  return tid;
}

DVBHOfdmLayer::DVBHOfdmLayer ()
{
  myIdentifier = ident;
  ident++;

  m_fecBlockSize = 320;
  m_noiseFigure = 5; // dB
  m_txPower = 30; // dBm
  m_bandWidth = 10000000; // 10Mhz
  m_nbErroneousBlock = 0;
  m_txFrequency = 0;
  m_rxFrequency = 0;
  m_modulationType = MODULATION_TYPE_QPSK_12;
  m_blockTime = Seconds (0);
  m_burstCopy = Create<DVBHQueue> ();
  SetOperationMode(Mode2_k);

  m_interference = 0;
  m_shadowing = 0;
  m_nodeIdentifier = 0;

}

void
DVBHOfdmLayer::SetForwardUpCallback (Callback<void,Ptr<Packet>,bool > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHOfdmLayer::ForwardUp (Ptr<Packet> packet,bool sleep )
{
  m_forwardUp (packet,sleep );
}

void
DVBHOfdmLayer::SetForwardUpPsiInfoCallback (Callback<void,struct TSPacket,bool > upCallback)
{
  m_forwardUpPsiInfo = upCallback;
}

void
DVBHOfdmLayer::ForwardUpPsiInfo (struct TSPacket packet,bool sleep )
{
  m_forwardUpPsiInfo (packet,sleep );
}

int
DVBHOfdmLayer::GetIdentifier ()
{
  return myIdentifier;
}

void
DVBHOfdmLayer::DoDispose (void)
{
  m_receivedFecBlocks.clear ();
}

void
DVBHOfdmLayer::SetChannel (Ptr<DVBHChannel> channel_)
{
  m_channel = channel_;
  m_channel->SetOfdmOfBaseStation (this);
}

Ptr<DVBHChannel>
DVBHOfdmLayer::GetChannel ()
{
  return m_channel;
}

void
DVBHOfdmLayer::SetNodeIdentifier (uint32_t nodeIdentifier)
{
  m_nodeIdentifier = nodeIdentifier;
  DoSetDataRates ();
}

uint32_t
DVBHOfdmLayer::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHOfdmLayer::SetShadowing (double shadowing)
{
  m_shadowing = shadowing;
}

double
DVBHOfdmLayer::GetShadowing () const
{
  return m_shadowing;
}

void
DVBHOfdmLayer::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
}

Ptr<MobilityModel>
DVBHOfdmLayer::GetMobility ()
{
  return m_mobility;
}

double
DVBHOfdmLayer::GetSystemLoss () const
{
  return m_systemLoss;
}

void
DVBHOfdmLayer::SetSystemLoss(double value)
{
  m_systemLoss=value;
}

double
DVBHOfdmLayer::GetLambda () const
{
  return m_lambda;
}

void 
DVBHOfdmLayer::SetLambda(double value)
{
  m_lambda=value;
}

double
DVBHOfdmLayer::GetMinDistance () const
{
  return m_minDistance;
}

void
DVBHOfdmLayer::SetMinDistance(double value)
{
  m_minDistance=value;
}

double
DVBHOfdmLayer::GetAntennaGain () const
{
  return m_antennaGain;
}

void
DVBHOfdmLayer::SetAntennaGain(double value)
{
  m_antennaGain=value;
}

double
DVBHOfdmLayer::GetAntennaHeight () const
{
  return m_antennaHeight;
}

void
DVBHOfdmLayer::SetAntennaHeight(double value)
{
  m_antennaHeight=value;
}

uint64_t
DVBHOfdmLayer::GetTxFrequency () const
{
  return m_txFrequency;
}

void
DVBHOfdmLayer::SetTxFrequency(double frequency)
{

  m_txFrequency = frequency;
  double value=300000000/(frequency*1000000);
  
  SetLambda(value);
}

uint64_t
DVBHOfdmLayer::GetRxFrequency () const
{
  return m_rxFrequency;
}

void
DVBHOfdmLayer::SetRxFrequency (double frequency)
{
  m_rxFrequency = frequency;
}

void
DVBHOfdmLayer::SetTotalInterference (double value)
{
  m_interference = value;
}

double
DVBHOfdmLayer::GetTotalInterference () const
{
  return m_interference;
}

Time
DVBHOfdmLayer::GetSymbolDuration ()
{
  return m_symbolDuration;
}

void
DVBHOfdmLayer::SetSymbolDuration (Time symbolDuration)
{
  m_symbolDuration = symbolDuration;
}

uint32_t
DVBHOfdmLayer::GetNrCarriers ()
{
  return m_numberCarriers;
}

void
DVBHOfdmLayer::SetNrCarriers (uint32_t carriers)
{
  m_numberCarriers = carriers;
}

void
DVBHOfdmLayer::SetModulationType (enum ModulationType modulationType)
{
  m_modulationType = modulationType;
}

enum ModulationType
DVBHOfdmLayer::GetModulationType () const
{
  return m_modulationType;
}


void
DVBHOfdmLayer::ConfigureSystem (double lambda_,double minDistance_,double systemLoss_,double antennaGain_,
                                double txPower,double antennaHeight_,enum OfdmOperationMode mode,enum ModulationType modulationType)
{
  m_lambda = lambda_;
  m_minDistance = minDistance_;
  m_systemLoss = systemLoss_;
  m_antennaGain = antennaGain_;
  m_txPower = txPower;
  m_antennaHeight = antennaHeight_;

  SetOperationMode (mode);
  SetModulationType (modulationType);

}

void
DVBHOfdmLayer::SetOperationMode (enum OfdmOperationMode mode)
{
  m_operationMode = mode;

  if (m_operationMode == Mode2_k)
    {
      SetSymbolDuration (MicroSeconds (224));
      SetNrCarriers (2048);

    }
  else if (m_operationMode == Mode4_k)
    {
      SetSymbolDuration (MicroSeconds (448));
      SetNrCarriers (4096);
    }
  else
    {
      SetSymbolDuration (MicroSeconds (896));
      SetNrCarriers (8192);
    }
}

enum OfdmOperationMode
DVBHOfdmLayer::GetOperationMode () const
{
  return m_operationMode;
}

double
DVBHOfdmLayer::GetBandwidth (void) const
{
  return m_bandWidth;
}

void
DVBHOfdmLayer::SetBandwidth (double BW)
{
  m_bandWidth = BW;
}

double
DVBHOfdmLayer::GetTxPower (void) const
{
  return m_txPower;
}
void
DVBHOfdmLayer::SetTxPower (double txPower)
{
  m_txPower = txPower;
}

uint8_t
DVBHOfdmLayer::DoGetFrameDurationCode (void) const
{
  // double duration = GetFrameDuration ().GetMilliSeconds (); //to milliseconds
  double duration = 0;
  if (duration == 2.5)
    {
      return FRAME_DURATION_2_POINT_5_MS;
    }
  else if (duration == 4)
    {
      return FRAME_DURATION_4_MS;
    }
  else if (duration == 5)
    {
      return FRAME_DURATION_5_MS;
    }
  else if (duration == 8)
    {
      return FRAME_DURATION_8_MS;
    }
  else if (duration == 10)
    {
      return FRAME_DURATION_10_MS;
    }
  else if (duration == 12.5)
    {
      return FRAME_DURATION_12_POINT_5_MS;
    }
  else if (duration == 20)
    {
      return FRAME_DURATION_20_MS;
    }

  NS_FATAL_ERROR ("Invalid frame duration");
  return 0;
}

Time
DVBHOfdmLayer::DoGetFrameDuration (uint8_t frameDurationCode) const
{
  switch (frameDurationCode)
    {
    case FRAME_DURATION_2_POINT_5_MS:
      return Seconds (2.5);
      break;
    case FRAME_DURATION_4_MS:
      return Seconds (4);
      break;
    case FRAME_DURATION_5_MS:
      return Seconds (5);
      break;
    case FRAME_DURATION_8_MS:
      return Seconds (8);
      break;
    case FRAME_DURATION_10_MS:
      return Seconds (10);
      break;
    case FRAME_DURATION_12_POINT_5_MS:
      return Seconds (12.5);
      break;
    case FRAME_DURATION_20_MS:
      return Seconds (20);
      break;
    default:
      NS_FATAL_ERROR ("Invalid modulation type");
    }
  return Seconds (0);
}

/*
 Retruns number of blocks (FEC blocks) the burst will be splitted in.
 The size of the block is specific for each modulation type.
 */
uint16_t
DVBHOfdmLayer::GetNrBlocks (uint32_t burstSize) const
{
  uint32_t blockSize = GetFecBlockSize ();
  uint16_t nrBlocks = (burstSize * 8) / blockSize;

  if ((burstSize * 8) % blockSize > 0)
    {
      nrBlocks += 1;
    }

  return nrBlocks;
}


double
DVBHOfdmLayer::GetNoiseFigure (void)
{
  return m_noiseFigure;
}

void
DVBHOfdmLayer::SetNoiseFigure (double noiseFigure)
{
  m_noiseFigure = noiseFigure;
}

uint16_t
DVBHOfdmLayer::DoGetNfft (void) const
{
  return 256; // Nfft, see pages 429 and 812
}

double
DVBHOfdmLayer::DoGetSamplingFactor (void) const
{
  
  // uint32_t channelBandwidth = GetChannelBandwidth();
  uint32_t channelBandwidth = 1000;

  if (channelBandwidth % 1750000 == 0)
    {
      return (double) 8 / 7;
    }
  else if (channelBandwidth % 1500000 == 0)
    {
      return (double) 86 / 75;
    }
  else if (channelBandwidth % 1250000 == 0)
    {
      return (double) 144 / 125;
    }
  else if (channelBandwidth % 2750000 == 0)
    {
      return (double) 316 / 275;
    }
  else if (channelBandwidth % 2000000 == 0)
    {
      return (double) 57 / 50;
    }
  else
    {
      NS_LOG_DEBUG ("Oops may be wrong channel bandwidth for OFDM PHY!");
      NS_FATAL_ERROR ("wrong channel bandwidth for OFDM PHY");
    }

  return (double) 8 / 7;
}

double
DVBHOfdmLayer::DoGetSamplingFrequency (void) const
{

  return 0;
  // return (DoGetSamplingFactor () * GetChannelBandwidth () / 8000) * 8000;
}

double
DVBHOfdmLayer::DoGetGValue (void) const
{
  // taking highest of the four values, may be user shall be allowed to specify
  return (double) 1 / 4;
}

void
DVBHOfdmLayer::ReceivePacketsFromAbove (struct TSPacket packet)
{
  SendPacketsToChannel (packet);
}

void
DVBHOfdmLayer::ReceiveBurstFromChannel (Ptr<DVBHQueue> burst,double rxPower)
{
  while (burst->GetSize ())
    {
      Ptr<Packet> packet = burst->Dequeue ();

      TsHeader tsHeader;
      RxPowerTag tag;
      tag.Set (rxPower);
      packet->PeekHeader (tsHeader);
      packet->AddPacketTag (tag);
      

       NS_LOG_INFO("DVBH UserEquip PHY//Dequeing the burst Packet Size"<<packet->GetSize()<<" Count "<<(uint32_t)tsHeader.GetCounter()<<" QueueSize "<<burst->GetSize()<<".Time "<<Simulator::Now());

      if (burst->GetSize () > 0)
        {
          ForwardUp (packet,false);
        }
      else
        {
          ForwardUp (packet,true);
        }

    }

}

void
DVBHOfdmLayer::ReceivePacketFromChannel (struct TSPacket packet)
{  
  ForwardUpPsiInfo (packet,false);
}

void
DVBHOfdmLayer::SendPacketsToChannel (struct TSPacket packet)
{
  m_channel->ReceiveFromBaseStation (packet);
}


void
DVBHOfdmLayer::ReceiveBurstFromAbove (Ptr<DVBHQueue> burst,uint32_t serviceId)
{
  m_nrFecBlocksSent = 0;
  m_burstCopy = burst->Copy ();

  SetBlockParameters (burst->GetSize () * 188);
  CreateFecBlocks (burst,serviceId);

  StartSendFecBlock (true,0);
  
}

void
DVBHOfdmLayer::SetBlockParameters (uint32_t size)
{
  m_blockSize = GetFecBlockSize ();
  m_nrBlocks = GetNrBlocks (size);
  m_paddingBits = (m_nrBlocks * m_blockSize) - (size * 8);
  
  NS_ASSERT (m_paddingBits >= 0);
}

void
DVBHOfdmLayer::CreateFecBlocks (Ptr<DVBHQueue> burst,uint32_t serviceId)
{
  m_ofdmBlockBurst.clear ();
  for (uint32_t i = 0, j = m_nrBlocks; j > 0; i += m_blockSize, j--)
    {
      OFDMBlock block;
      block.serviceId = serviceId;
      m_ofdmBlockBurst.push_back (block);
    }
  
}


void
DVBHOfdmLayer::StartSendFecBlock (bool isFirstBlock,uint32_t symbolIndex)
{
  OFDMBlock fecBlock = m_ofdmBlockBurst.front ();
   Time time= DoGetTransmissionTime ();  
//    Simulator::Schedule(time,&DVBHChannel::SendToUE,m_channel,fecBlock,isFirstBlock,GetTxFrequency (),m_burstCopy, m_txPower);
   Simulator::ScheduleNow(&DVBHChannel::SendToUE,m_channel,fecBlock,isFirstBlock,GetTxFrequency (),m_burstCopy, m_txPower);
   m_ofdmBlockBurst.clear();
}

void
DVBHOfdmLayer::EndSendFecBlock (uint32_t symbolIndex)
{
  m_nrFecBlocksSent++;

  // this is the last FEC block of the burst
  if (m_nrFecBlocksSent * m_blockSize != m_burstCopy->GetSize () * 188 * 8 + m_paddingBits)
    {
      Simulator::ScheduleNow (&DVBHOfdmLayer::StartSendFecBlock,this, false, symbolIndex);
    }

}

void
DVBHOfdmLayer::StartReceive (struct OFDMBlock block,bool isFirstBlock, uint64_t frequency,Ptr<DVBHQueue> burst,double rxPower)
{
      if (isFirstBlock)
        {
          m_burstCopy = burst;          
          SetBlockParameters (m_burstCopy->GetSize () * 188);	  
          m_blockTime = GetBlockTransmissionTime ();
// 	  Simulator::Schedule (m_blockTime,&DVBHOfdmLayer::EndReceive, this,/*m_receivedFecBlocks,*/rxPower);
	  EndReceive(rxPower);
       }    
}


void
DVBHOfdmLayer::EndReceiveFecBlock (struct OFDMBlock block,double rxPower)
{

}

void
DVBHOfdmLayer::EndReceive (double rxPower)
{  
  m_receivedFecBlocks.clear ();
  
  ReceiveBurstFromChannel (m_burstCopy,rxPower);


}


void
DVBHOfdmLayer::DoSetDataRates (void)
{
  m_dataRateQpsk12 = CalculateDataRate (MODULATION_TYPE_QPSK_12); // 13824000
  m_dataRateQpsk34 = CalculateDataRate (MODULATION_TYPE_QPSK_34); // 20736000
  m_dataRateQam16_12 = CalculateDataRate (MODULATION_TYPE_QAM16_12); // 27648000
  m_dataRateQam16_34 = CalculateDataRate (MODULATION_TYPE_QAM16_34); // 41472000
}

void
DVBHOfdmLayer::GetModulationFecParams (ModulationType modulation,uint8_t &bitsPerSymbol,double &fecCode) const
{  
  switch (modulation)
    {

    case MODULATION_TYPE_QPSK_12:
      bitsPerSymbol = 2;
      fecCode = (double) 1 / 2;
      break;
    case MODULATION_TYPE_QPSK_34:
      bitsPerSymbol = 2;
      fecCode = (double) 3 / 4;
      break;
    case MODULATION_TYPE_QAM16_12:
      bitsPerSymbol = 4;
      fecCode = (double) 1 / 2;
      break;
    case MODULATION_TYPE_QAM16_34:
      bitsPerSymbol = 4;
      fecCode = (double) 3 / 4;
      break;

      break;
    }
}

uint32_t
DVBHOfdmLayer::CalculateDataRate (ModulationType modulationType)
{
  uint8_t bitsPerSymbol = 0;
  double fecCode = 0;

  GetModulationFecParams (modulationType,bitsPerSymbol, fecCode);

  double symbolsPerSecond = 1 / GetSymbolDuration ().GetSeconds ();


  uint16_t bitsTransmittedPerSymbol = (uint16_t)(bitsPerSymbol * GetNrCarriers () * fecCode);
  // 96, 192, 288, 384, 576, 767 and 864 bits per symbol for the seven modulations, respectively
 
  return (uint32_t) symbolsPerSecond * bitsTransmittedPerSymbol;
}

uint32_t
DVBHOfdmLayer::DoGetDataRate () const
{
  switch (m_modulationType)
    {
    case MODULATION_TYPE_QPSK_12:
      return m_dataRateQpsk12;
      break;
    case MODULATION_TYPE_QPSK_34:
      return m_dataRateQpsk34;
      break;
    case MODULATION_TYPE_QAM16_12:
      return m_dataRateQam16_12;
      break;
    case MODULATION_TYPE_QAM16_34:
      return m_dataRateQam16_34;
      break;

    }
  NS_FATAL_ERROR ("Invalid modulation type");
  return 0;
}

/*
 Since at the PHY the size of the block (note: burst is splitted into blocks) transmitted
 always becomes 320 bits (there m_fecBlockSize is set to 320), therefore to calculate the
 transmission time of a burst, original size of the burst cannot be used. Hence first it is
 determined how many blocks the burst will be splitted in to and then transmission time is
 calculated considering each block of 320 bits size.
 Note: first parameter could have been burst size if there was no problem of
 overloading (there is some problem when calls to overloaded functions are scheduled)
 */
Time
DVBHOfdmLayer::GetBlockTransmissionTime () const
{
  ////std::cout<<" GETDATARATE "<<(double) m_fecBlockSize / DoGetDataRate()<<std::endl;
  return Seconds ((double) m_fecBlockSize / DoGetDataRate ());
}

Time
DVBHOfdmLayer::DoGetTransmissionTime ()
{
  /*adding one extra femto second to cope with the loss of precision problem.
   the time is internally stored in a 64 bit hence a floating-point time would loss
   precision, e.g., 0.00001388888888888889 seconds will become 13888888888 femtoseconds.*/  
  return Seconds ((double)(m_burstCopy->GetSize () * 188 * 8 + m_paddingBits) / DoGetDataRate ()) + FemtoSeconds (1);
}

uint64_t
DVBHOfdmLayer::DoGetNrSymbols (uint32_t size)
{
  Time transmissionTime = Seconds ((double)(GetNrBlocks (size)
                                            * m_fecBlockSize) / DoGetDataRate ());
  return (uint64_t) ceil (transmissionTime.GetSeconds ()
                          / GetSymbolDuration ().GetSeconds ());
}

uint64_t
DVBHOfdmLayer::DoGetNrBytes (uint32_t symbols)
{
  Time transmissionTime =
    Seconds (symbols * GetSymbolDuration ().GetSeconds ());
  return (uint64_t) floor ((transmissionTime.GetSeconds () * DoGetDataRate ()) * (GetFecBlockSize () / m_fecBlockSize)) / 8;
}

uint32_t
DVBHOfdmLayer::GetFecBlockSize () const
{
  uint32_t blockSize = 0;
  switch (m_modulationType)
    {
    case MODULATION_TYPE_QPSK_12:
      blockSize = 24;
      break;
    case MODULATION_TYPE_QPSK_34:
      blockSize = 36;
      break;
    case MODULATION_TYPE_QAM16_12:
      blockSize = 48;
      break;
    case MODULATION_TYPE_QAM16_34:
      blockSize = 72;
      break;

    default:
      NS_FATAL_ERROR ("Invalid modulation type");
      break;
    }
  return blockSize * 8; // in bits
}

/*---------------------PHY parameters functions-----------------------*/

void
DVBHOfdmLayer::DoSetPhyParameters (void)
{
  SetNrCarriers (192);

  m_fecBlockSize = 320;

}







} // namespace ns3
