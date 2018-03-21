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

#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "controlpacket.h"
#include "umts-channel.h"
#include "ns3/random-variable.h"
#include "ns3/mobility-model.h"
#include "umts-phy-layer-ue.h"
#include "umts-phy-layer-bs.h"
#include "umts-queue.h"
#include "umts-manager.h"
#include "blerTable.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/system-thread.h"
#include "ns3/ptr.h"
#include "ns3/enum.h"
#include "ns3/double.h"
#include "umts-tags.h"
#include "ns3/app-index-tag.h"
#include "umts-rlc-ack-header.h"
#include "ns3/itetris-types.h"

NS_LOG_COMPONENT_DEFINE ("UmtsPhyLayerUE");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsPhyLayerUE);

double UmtsPhyLayerUE::slot_time_ = 0;
double UmtsPhyLayerUE::rach_slot_time_ = 0;
int m_dedicatedSequenceNumber = 0;

TypeId
UmtsPhyLayerUE::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsPhyLayerUE")
    .SetParent<Object> ()
    .AddConstructor<UmtsPhyLayerUE> ()
    .AddAttribute ("AntennaHeight",
                   "Height of the Antenna in metres (m)",
                   DoubleValue (5.0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetAntennaHeight,
                                       &UmtsPhyLayerUE::GetAntennaHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("AntennaGain",
                   "Antenna Gain (dB)",
                   DoubleValue (10.0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetAntennaGain,
                                       &UmtsPhyLayerUE::GetAntennaGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SystemLoss",
                   "Loss associated to the System (dB)",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetSystemLoss,
                                       &UmtsPhyLayerUE::GetSystemLoss),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DedicatedTxPower",
                   "Transmission Power (dB)",
                   DoubleValue (-9.0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetDedicatedTxPower,
                                       &UmtsPhyLayerUE::GetDedicatedTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxFrequency",
                   "TxFrequency (GHz)",
                   DoubleValue (300000000),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetTxFrequency,
                                       &UmtsPhyLayerUE::GetTxFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("CommonTxPower",
                   "Common Tx Power (dB)",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetCommonTxPower,
                                       &UmtsPhyLayerUE::GetCommonTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinDistance",
                   "Minimum Distance to consider for Path Loss calculation (m)",
                   DoubleValue (10),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetMinDistance,
                                       &UmtsPhyLayerUE::GetMinDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TargetSNR",
                   "SNR Value above which the transmission is considered correct (lineal)",
                   DoubleValue (100),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetSnrTarget,
                                       &UmtsPhyLayerUE::GetSnrTarget),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PowerControlStep",
                   "It controls how the transmission power is decreased/increased (dB)",
                   EnumValue (StepUE1),
                   MakeEnumAccessor (&UmtsPhyLayerUE::SetControlAlgothmType,
                                     &UmtsPhyLayerUE::GetControlAlgothmType),
                   MakeEnumChecker (StepUE1,"step1",StepUE2,"step2",StepUE3,"step3"))
                   
    .AddAttribute ("InterferenceCalculationFreq",
                   "It indicates how often the calculation of the interference is carried out",
                   DoubleValue (0),
                   MakeDoubleAccessor (&UmtsPhyLayerUE::SetInterferenceCalculationTime,
                                     &UmtsPhyLayerUE::GetInterferenceCalculationTime),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

static int externalError = 0;

UmtsPhyLayerUE::UmtsPhyLayerUE ()
{NS_LOG_FUNCTION(this);
  m_nodeIdentifier = (uint32_t)-1;
  m_nodeBIdentifier = (uint32_t)-1;
  slot_time_ = 0.000667 ; // umts slot is of 666.6 microseconds
  rach_slot_time_ = 0.00125 ; // umts slot is of 1.25 miliseconds
        
  m_bytesPerRachSlot = 0;
  m_bytesPerSlot = 0.0;
  m_txPreambleNumber = 0;
  m_waitRachProcedure = 0;
  m_selectedScramCode = -1;
  
  m_handover = 0;
  m_packetsInSequence = 0;
  m_packetsInSequenceH = 0;  
  m_dedicatedSequenceNumber = 0;
  m_slotNumber = 0;
  m_nodeBIp.Set ("0");
  m_myIp.Set ("0");
  m_tti = 0.01;  
  m_usingRachControlInfo=Create<ControlPacket>();
  m_downlinkErrorRate = 1000000000;
  m_dedicatedLastRate = 0;
  m_commonLastRate = 0;
  m_dedicatedRxPw = 0;
  m_commonRxPw = 0;
  m_dlfreq = -1;
  m_ulfreq = -1;
  m_commonSequenceNumber = 0;
  m_wcdmaParams.p_scrambling_ = -1;
  m_wcdmaParams.signature_ = -1;
  
  m_uplinkSlotCounter = 0;

  m_upSlotTimer = Simulator::Schedule (Seconds (0),&UmtsPhyLayerUE::UpSlotHandler,this);
  m_rachUpSlotTimer = Simulator::Schedule (Seconds (0),&UmtsPhyLayerUE::RachUpSlotHandler,this);
  m_blerTimer = Simulator::ScheduleNow (&UmtsPhyLayerUE::BlerHandler,this);

  m_rachQueue = Create<UMTSQueue> ();
  m_prachTemp = Create<UMTSQueue> ();
  m_packetToTxQueue = Create<UMTSQueue> (); // packets to tx in the current slot
  m_dedicatedMacDataQueue = Create<UMTSQueue> ();       // packet received in the current slot from MAC
  m_commonMacDataQueue = Create<UMTSQueue> ();

  m_errorIndicator = 0;                   // not error
  m_calculatedErrorRate = 1000000000;        // null error probability
  
  m_errorCalculationTimer = Simulator::ScheduleNow (&UmtsPhyLayerUE::ErrorCalculationTimerHandler,this);
  
  m_transmitting = 0;

  m_externalInterference = 0;
  m_internalInterference = 0;
  m_dedicatedInterference = 0;
  m_commonInterference = 0;
  m_commonInterferenceSamples = 0;
  m_dedicatedInterferenceSamples = 0;

  for (int i = 0; i < 5; i++)
    {
      m_powerControlNumber[i] = -1;
    }
    
  m_rachTotallySent=true;

}

void
UmtsPhyLayerUE::DoDispose ()
{

}

void
UmtsPhyLayerUE::SetHandoverState (int value)
{
  m_handover = value;
}

void
UmtsPhyLayerUE::SetSharedChannel (Ptr<UMTSChannel> channel)
{
  m_sharedChannel = channel;
  m_sharedChannel->Add (this,m_nodeIdentifier);
}

void
UmtsPhyLayerUE::SetDedicatedChannel (Ptr<UMTSChannel> channel_)
{
  m_channel = channel_;
}

Ptr<UMTSChannel>
UmtsPhyLayerUE::GetChannel ()
{
  return m_channel;
}

void
UmtsPhyLayerUE::SetInterferenceCalculationTime (double time)
{
  m_calculationTime=time; 
}

double
UmtsPhyLayerUE::GetInterferenceCalculationTime() const
{
  return m_calculationTime;
}

void
UmtsPhyLayerUE::SetMobility (Ptr<MobilityModel> mobility)
{

  m_mobility = mobility;
}

Ptr<MobilityModel>
UmtsPhyLayerUE::GetMobility ()
{
  return m_mobility;
}

void
UmtsPhyLayerUE::SetSystemLoss (double value)
{
  m_systemLoss = value;
}

double
UmtsPhyLayerUE::GetSystemLoss () const
{
  return m_systemLoss;
}

void
UmtsPhyLayerUE::SetTxFrequency (double value)
{
  m_txFrequency = value;
}

double
UmtsPhyLayerUE::GetTxFrequency () const
{
  return m_txFrequency;
}

void
UmtsPhyLayerUE::SetSnrTarget (double value)
{
  m_snrTarget = value;
}

double
UmtsPhyLayerUE::GetSnrTarget () const
{
  return m_snrTarget;
}

void
UmtsPhyLayerUE::SetMinDistance (double value)
{
  m_minDistance = value;
}

double
UmtsPhyLayerUE::GetMinDistance () const
{
  return m_minDistance;
}

void
UmtsPhyLayerUE::SetAntennaGain (double value)
{
  m_antennaGain = value;
}

double
UmtsPhyLayerUE::GetAntennaGain () const
{
  return m_antennaGain;
}

void
UmtsPhyLayerUE::SetAntennaHeight (double value)
{
  m_antennaHeight = value;
}

double
UmtsPhyLayerUE::GetAntennaHeight () const
{
  return m_antennaHeight;
}

void
UmtsPhyLayerUE::SetDedicatedTxPower (double value)
{
  m_dedicatedTxPower = value;
}

double
UmtsPhyLayerUE::GetDedicatedTxPower () const
{
  return m_dedicatedTxPower;
}

void
UmtsPhyLayerUE::SetCommonTxPower (double value)
{
  m_commonTxPower = value;
}

double
UmtsPhyLayerUE::GetCommonTxPower () const
{
  return m_commonTxPower;
}


void
UmtsPhyLayerUE::SetControlAlgothmType (enum PowerChangeStepUE step)
{
  if (step == StepUE1)
    {
      m_powerControlStep = 1;
    }
  else if (step == StepUE2)
    {
      m_powerControlStep = 2;
    }
  else
    {
      m_powerControlStep = 3;
    }

}

enum PowerChangeStepUE
UmtsPhyLayerUE::GetControlAlgothmType () const
{
  if (m_powerControlStep == 1)
    {
      return StepUE1;
    }
  else if (m_powerControlStep == 2)
    {
      return StepUE2;
    }
  else
    {
      return StepUE3;
    }

}

void
UmtsPhyLayerUE::ChangeTxPower (int sign)
{

  for (int i = 0; i < 5; i++)
    {
      if (m_powerControlNumber[i] == -1)
        {
          m_powerControlNumber[i] = sign;
        }
    }

  if (m_powerControlNumber[4] != -1)
    {
      if (m_powerControlNumber[0] == m_powerControlNumber[1]&&m_powerControlNumber[1] == m_powerControlNumber[2]&&m_powerControlNumber[2] == m_powerControlNumber[3]
          && m_powerControlNumber[3] == m_powerControlNumber[4]&&m_powerControlNumber[0] == 1)
        {
          m_dedicatedTxPower = m_dedicatedTxPower + m_powerControlStep;          
        }
      else if (m_powerControlNumber[0] == m_powerControlNumber[1]&&m_powerControlNumber[1] == m_powerControlNumber[2]&&m_powerControlNumber[2] == m_powerControlNumber[3]
               && m_powerControlNumber[3] == m_powerControlNumber[4]&&m_powerControlNumber[0] == 0)
        {
          m_dedicatedTxPower = m_dedicatedTxPower - m_powerControlStep;          
        }

    }
}

void
UmtsPhyLayerUE::SetUplinkFrequency (int value)
{

  m_ulfreq = value;
}

void
UmtsPhyLayerUE::SetDownLinkFrequencyUsed (int freq)
{
  m_dlfreq = freq;

}

void
UmtsPhyLayerUE::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;  
}

uint32_t
UmtsPhyLayerUE::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

int
UmtsPhyLayerUE::GetTransmitting ()
{
  return m_transmitting;
}

void
UmtsPhyLayerUE::SetRxCallback (Callback<void,Ptr<Packet>,uint8_t > callback)
{
  m_rxCallback = callback;
}

void
UmtsPhyLayerUE::SetMyIpAddress (Ipv4Address address)
{
  m_myIp = address;
}

void
UmtsPhyLayerUE::SetMyIpAddress (Ipv6Address address)
{
  m_myIpv6 = address;
}

int
UmtsPhyLayerUE::GetNodeBPhyAddress ()
{
  return m_nodeBIdentifier;
}

void
UmtsPhyLayerUE::SetAttachedNodeB (Ptr<UmtsPhyLayerBS> nodeB_)
{
  m_nodeBPhyLayer = nodeB_;
}

void
UmtsPhyLayerUE ::SetForwardToRRCCallback (Callback<void,uint32_t > rrcCallback)
{

  m_forwardToRRC = rrcCallback;
}

void
UmtsPhyLayerUE ::SetInterferenceCallback (Callback<void> m_interferenceCallBack_)
{
  m_interferenceCallBack = m_interferenceCallBack_;
}

// Return the number of DPDCHs arrived
int
UmtsPhyLayerUE::GetNumberOfDpdchArrived ()
{
  int i, n = 0;
  for (i = 0 ; i < MAX_NUM_DPDCH ; i++)
    {
      if (m_rxDpdchQueue[i].first != 0)
        {
          n++;
        }
    }
  return (n);
}

void
UmtsPhyLayerUE::StoreSlotBytes (double bytes)
{  
  m_bytesPerSlot = bytes;
}

void
UmtsPhyLayerUE::StoreRachSlotBytes (double bytes)
{
  m_bytesPerRachSlot = bytes;
}


// for channel decoding
uint32_t
UmtsPhyLayerUE::GetChannelDecodingSize (uint32_t size,uint8_t codeTag)
{
  uint32_t newSize=0;
  switch (codeTag)
    {
    case TURBO:              // turbo coding
      newSize=size / 3;
      break;
    case CONV_HALF:              // convolutional half
      newSize=size / 2;
      break;
    case CONV_THIRD:              // convolutional third
      newSize=size / 3;
      break;
    default:
      break;
    }

  return newSize;
}


// for channel coding
uint32_t
UmtsPhyLayerUE::GetChannelCodingSize (uint32_t size,uint8_t codeTag)
{
  uint32_t newSize=0;
  switch (codeTag)
    {
    case TURBO:             // turbo coding
      newSize=3 * size;
      break;
    case CONV_HALF:             // convolutional half
      newSize=2 * size;
      break;
    case CONV_THIRD:              // convolutional third
     newSize=3 * size;
      break;
    default:
      break;
    }
    //std::cout<<"!!!!!Coding Size:"<<newSize<<std::endl;
    return newSize;

}

void
UmtsPhyLayerUE ::ForwardToRRC (uint32_t applicationId)
{

  m_forwardToRRC (applicationId);


}

void
UmtsPhyLayerUE::ForwardUp (Ptr<Packet> packet)
{
  UmtsPacketTypeTag typeTag;
  packet->PeekPacketTag(typeTag);

  if (typeTag.Get() != Ack)
    {
      if (m_errorIndicator)
        {
          NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY  ********** Cannot Send the Packet Upwards.Time " << Simulator::Now () << "\n");
        }
      else
        {          
          m_rxCallback (packet,typeTag.Get());
        }
    }
  else
    {      
      m_rxCallback (packet,typeTag.Get());
    }
}

void
UmtsPhyLayerUE::ReceiveRACHMessage (Ptr<Packet> packet,uint8_t packetType)
{

  UmtsCodeTag codeTag;
  packet->PeekPacketTag(codeTag);
     
  Ptr<ControlPacket> controlpacket=Create<ControlPacket>();
  
  uint32_t newSize=GetChannelCodingSize (packet->GetSize(),codeTag.Get());
  
  packet->AddPaddingAtEnd(newSize-packet->GetSize());
  
  controlpacket->SetDataSize (packet->GetSize ());      // stores the packet size
  //std::cout<<"!!!Control Packet size:"<<packet->GetSize ()<<std::endl;
  
  if (packetType == Ack)
    {
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY// The packet arrived from the RRC is ACK .Time " << Simulator::Now () << " \n");      
      MakeRachWithoutPreamble (packet);          // send RACH message without preamble
      return;
    }

  if (m_waitRachProcedure == 1)
    {           // already in other RACH procedure.
      if (packetType == CommonUnicastData)
        {          
          controlpacket->SetChannel (PRACH);         
          m_prachTemp->PhyEnqueue (packet->Copy(),controlpacket);

        }
      else
        {
          m_prachTemp->PhyEnqueue (packet->Copy(),controlpacket);
        }

      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY// Already in other RACH Procedure.Time " << Simulator::Now ());
    }
  else
    {
      m_waitRachProcedure = 1;      
      controlpacket->SetUmtsPhyLayerUEPointer (this);
      m_usingRachControlInfo=controlpacket->Copy();
            
      if (packetType == CommonUnicastData)
        {
          m_usingRachPacket = packet->Copy();
          m_usingRachControlInfo->SetChannel (PRACH);
          m_commonMacDataQueue->PhyEnqueue (packet,m_usingRachControlInfo);          
          TxRachPreamble ();                    // send preamble for RACH procedure          
        }
      else
        {
          m_usingRachPacket = packet->Copy();          
          TxRachPreamble ();
        }
    }
}

void
UmtsPhyLayerUE::ReceivedDataPacketFromAbove (Ptr<Packet> packet,uint8_t packetType)
{  
  if (packetType == DedicatedUnicastData)
    {      
      UpLinkMux (packet);         // to uplink multiplexing chain
    }

  else 
    {
      ReceiveRACHMessage(packet,packetType);

    }
}

void
UmtsPhyLayerUE::TxRachDataPacket (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  controlpacket->SetDataSize (controlpacket->GetSize ());      // stores the packet size
  controlpacket->SetChannel (PRACH);  
  m_commonMacDataQueue->PhyEnqueue (packet,controlpacket);
}

void
UmtsPhyLayerUE::PacketArrivalFromChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //std::cout<<"UmtsPhyLayerUE::PacketArrivalFromChannel!!!"<<std::endl;  
  if (controlpacket->GetChannel () == PCCPCH)
    {
      ReceivePacketFromBelow (packet,controlpacket);
      return;
    }
      
  UmtsPacketTypeTag typeTag;
  packet->PeekPacketTag(typeTag);
      
  if (typeTag.Get() == DedicatedUnicastData)
    {
      double power = pow (10.0,controlpacket->GetRxPower () / 10.0);
      m_dedicatedRxPw = power;
      m_dedicatedLastRate = controlpacket->GetDpdchRate ();           // and rate
    }
    else if(typeTag.Get() == CommonUnicastData||typeTag.Get() == BroadcastData||typeTag.Get() == MulticastData)
    {
      
      double power = pow (10.0,controlpacket->GetRxPower () / 10.0);   
      m_commonRxPw = power;
      m_commonLastRate = controlpacket->GetTxRate ();
    }

  ReceivePacketFromBelow (packet,controlpacket);
  

}

void
UmtsPhyLayerUE::UpLinkMux (Ptr<Packet> packet)
{

  Ptr<ControlPacket> controlInfo=Create<ControlPacket>();
  
  UmtsCodeTag tag;
  packet->PeekPacketTag(tag);
  
  uint32_t newSize=GetChannelCodingSize (packet->GetSize(),tag.Get()); 
  packet->AddPaddingAtEnd(newSize-packet->GetSize());	//Increment the packet size according the coding
  
  controlInfo->SetDataSize (packet->GetSize ());    
  controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);
  controlInfo->SetDestinationNodeIdentifier (m_nodeBIdentifier);

  m_dedicatedMacDataQueue->PhyEnqueue (packet,controlInfo);       // store packet for sending it over the air interface
  return;
}


void
UmtsPhyLayerUE::MakeRACHDataPackets ()
{
  int   index_c_;
  double temp,total_size, tbytes;

  tbytes = 0.0;
  total_size = 1;

  int slotIndex = 0;
 
  if (m_usingRachControlInfo->GetDataSize () > 0)
    {

      index_c_ =m_wcdmaParams.scram_tx_;     
      Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();

      if (m_bytesPerRachSlot == 0)
        {
          NS_LOG_DEBUG("NodeUE " << m_nodeIdentifier << " PHY//Connectivity loss, it is impossible to send the packets."
          << "Removing all the packets. Time " << Simulator::Now () << "\n");          
          m_usingRachPacket = 0;
          return;
        }

      // Extract packets from the buffer to transmit

      while (slotIndex < 15 && m_usingRachControlInfo->GetDataSize () > 0)
        {

          tbytes = 0.0;

          while ((tbytes < total_size) && (m_usingRachControlInfo->GetDataSize () > 0))
            {
              controlpacket = m_usingRachControlInfo->Copy ();
	      UmtsTxRateTag rateTag;
	      m_usingRachPacket->PeekPacketTag(rateTag);
	      
	      controlpacket->SetTxRate(rateTag.Get());
	      controlpacket->SetDestinationNodeIdentifier(m_nodeBIdentifier);

              total_size = m_bytesPerRachSlot;              
              tbytes += controlpacket->GetDataSize ();
                            
              if (tbytes <= total_size)
                {
                  // Checks if it is possible to send the whole packet in one Slot
                  controlpacket->SetSequenceNumber (m_commonSequenceNumber);

                  if (slotIndex == 0&&m_commonSequenceNumber == 0)
                    {                      
                      controlpacket->SetBeginOfSequence (1);
                    }
                  else
                    {
                      controlpacket->SetBeginOfSequence (0);
                    }
                  
                  controlpacket->SetEndOfSequence (1);
		  controlpacket->SetEndOfPacket (1);

                  m_commonSequenceNumber = 0;
                  EnqueueRachDataPackets (m_usingRachPacket,controlpacket->Copy (), index_c_, controlpacket->GetSpreadingFactor (), controlpacket->GetSpreadingFactor () / 4);             // send packet over the air interface
                  m_usingRachControlInfo->SetDataSize (0);
		  m_rachTotallySent=true;

                }
              else
                {
                  // no space to allocate all the packet in the current Slot
                  tbytes -= controlpacket->GetDataSize ();
                  temp = controlpacket->GetDataSize ();
                  controlpacket->SetDataSize (total_size - tbytes);
                  controlpacket->SetSequenceNumber (m_commonSequenceNumber);

                  if (slotIndex == 0&&m_commonSequenceNumber == 0)
                    {
                      controlpacket->SetBeginOfSequence (1);                       
                    }
                  else
                    {
                      controlpacket->SetBeginOfSequence (0);
                    }
                    
                    if(slotIndex==14)
		      controlpacket->SetEndOfPacket (1);
		    else
		      controlpacket->SetEndOfPacket (0);
		    
		  controlpacket->SetEndOfSequence (0);

                  EnqueueRachDataPackets (m_usingRachPacket,controlpacket->Copy (), index_c_, controlpacket->GetSpreadingFactor (), controlpacket->GetSpreadingFactor () / 4);             // send packet over the air interface
                  controlpacket->SetDataSize (temp - (total_size - tbytes));

                  m_commonSequenceNumber++;
                  
                  m_usingRachControlInfo = controlpacket->Copy ();
                  break;
                }
            }
          slotIndex++;

        }
      if(m_usingRachControlInfo->GetDataSize()>0)
      {
	    m_rachTotallySent=false;
      }

    }

}




/** Builds the DPDCH and DPCCH packets to transmit
*/

void
UmtsPhyLayerUE::MakeDPXCH (void)
{
  // Rate matching
  int i, sf_, k_, numberOfDpdchNeeded;
  double index, temp, sum, mod, tbytes, total_size;

  tbytes = 0.0;
  sum = 0.0;
  total_size = m_bytesPerSlot;        // maximum physical bytes that is possible sending in one slot

  UmtsTxRateTag rateTag;


  if (m_dedicatedMacDataQueue->GetPhySize () > 0)
    {      
      if (m_bytesPerSlot == 0)
        {
          NS_LOG_DEBUG("NodeUE " << m_nodeIdentifier << " PHY// Connectivity loss, it is impossible to send"<<
          "the packets. Removing all the packets. Time " << Simulator::Now () <<"\n");
          m_dedicatedMacDataQueue->PhyRemoveAll ();
          return;
        }
             
      // looking for an available Scrambling Code
      if (m_uplinkSlotCounter == 0)
        {
          m_scramCodeDedicatedCh = ScramblingCodeAllocation ();
        }
        
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem;

      if (total_size > MAX_NUM_BYTES_PER_SLOT)
        {
          // is necesary 2 or more DPDCH, with SF=4
	  
          // building DPDCH packets
	  
          numberOfDpdchNeeded = (int)(total_size / MAX_NUM_BYTES_PER_SLOT);
          mod = (int)(total_size) % (MAX_NUM_BYTES_PER_SLOT);

          if (mod > 0)
            {
              numberOfDpdchNeeded++;
            }
          if (numberOfDpdchNeeded > MAX_NUM_DPDCH)
            {
              numberOfDpdchNeeded = MAX_NUM_DPDCH;
            }
          // n_dpdch is the number of necessary DPDCH for that application

          for (i = 0; i < numberOfDpdchNeeded; i++)
            {
              Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();

              controlInfo->SetTxType (Dedicated);
              controlInfo->SetLLType (LL_DATA);
              controlInfo->SetDataSize (0);
              controlInfo->SetNumberOfDpdch (i + 1);

              sum = 0.0;
              // extracting packets from the buffer to transmit
              while (m_dedicatedMacDataQueue->GetPhySize () > 0)
                {
                  newItem = m_dedicatedMacDataQueue->PhyDequeue ();
		  newItem.first->PeekPacketTag(rateTag);
		  
                  controlInfo->SetTxRate (rateTag.Get());
                  sum += newItem.second->GetDataSize ();
                  // check if there is enough space in that DPDCH
                  if (sum <= MAX_NUM_BYTES_PER_SLOT)
                    {
                      // there is enough space for the whole packet
                      newItem.second->SetSequenceNumber (m_dedicatedSequenceNumber);
                      newItem.second->SetEndOfSequence (1);
                      controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                      controlInfo->SetDataSize (controlInfo->GetDataSize () + newItem.second->GetDataSize ());
                      m_dedicatedSequenceNumber = 0;

                      if (sum == MAX_NUM_BYTES_PER_SLOT)
                        {
                          break;
                        }
                    }
                  else
                    {
                      // no space to allocate all the packet in the current DPDCH
                      sum -= newItem.second->GetDataSize ();
                      temp = newItem.second->GetDataSize ();
                      newItem.second->SetDataSize (MAX_NUM_BYTES_PER_SLOT - sum);
                      newItem.second->SetSequenceNumber (m_dedicatedSequenceNumber);
                      newItem.second->SetEndOfSequence (0);

                      controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                      controlInfo->SetDataSize (controlInfo->GetDataSize () + MAX_NUM_BYTES_PER_SLOT_DL - sum);
                      newItem.second->SetDataSize (temp - (MAX_NUM_BYTES_PER_SLOT_DL - sum));
                      m_dedicatedSequenceNumber++;
                      // storing the rest of the packet in the buffer
                      m_dedicatedMacDataQueue->PhyEnqueueHead (newItem.first,newItem.second);
                      break;
                    }
                }
              sf_ = 4;                   // spreading factor
              // calculate k_
              if (i == 0 || i == 1)
                {
                  k_ = 1;
                }
              if (i == 2 || i == 3)
                {
                  k_ = 3;
                }
              if (i == 4 || i == 5)
                {
                  k_ = 2;
                }
              controlInfo->SetChannel (DPDCH);
              EnqueueDataPackets (newItem.first,controlInfo, m_scramCodeDedicatedCh, sf_, k_);                   // sending DPDCH              

            }
        }
      else
        {
          // only 1 DPDCH is needed. Building DPDCH packet

          numberOfDpdchNeeded = 1;
          // calculate the necessary spreading factor for that application
          index = total_size;
          index = (320 / total_size);
          index = (int)(log (index) / log (2));
          sf_ = 2;
          for (i = 0; i < ((int)index - 1); i++)
            {
              sf_ *= 2;
            }
          // sf_ is the spreading factor chosen

          Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();
          controlInfo->SetTxType (Dedicated);
          controlInfo->SetLLType (LL_DATA);
          controlInfo->SetDataSize (0);
          controlInfo->SetNumberOfDpdch (1);

          numberOfDpdchNeeded = 1;
          // extract packets from the buffer to transmit


          while ((tbytes < total_size) && (m_dedicatedMacDataQueue->GetPhySize () > 0))
            {
              newItem = m_dedicatedMacDataQueue->PhyDequeue ();
	      
	      newItem.first->PeekPacketTag(rateTag);		  
              controlInfo->SetTxRate (rateTag.Get());	                                 
              tbytes += newItem.second->GetDataSize ();
              
              if (tbytes <= total_size)
                {
                  newItem.second->SetSequenceNumber (m_dedicatedSequenceNumber);
                  newItem.second->SetEndOfSequence (1);
                  
                  controlInfo->InsertInDpdchQueue (newItem.first->Copy (),newItem.second);
                  controlInfo->SetDataSize (controlInfo->GetDataSize () + newItem.second->GetDataSize ());
                  m_dedicatedSequenceNumber = 0;

                }
              else
                {
                  // no space to allocate all the packet in the current DPDCH
                  tbytes -= newItem.second->GetDataSize ();
                  temp = newItem.second->GetDataSize ();
                  newItem.second->SetDataSize (total_size - tbytes);
                  newItem.second->SetSequenceNumber (m_dedicatedSequenceNumber);
                  newItem.second->SetEndOfSequence (0);


                  // sending a part of the message
                  controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                  controlInfo->SetDataSize (controlInfo->GetDataSize () + total_size - tbytes);
                  newItem.second->SetDataSize (temp - (total_size - tbytes));

                  m_dedicatedSequenceNumber++;
                  // storing the rest of the packet in the buffer
                  m_dedicatedMacDataQueue->PhyEnqueueHead (newItem.first,newItem.second);
		  
		  NS_LOG_DEBUG("Node UE "<<m_nodeIdentifier<<" PHY No space to allocate all the packet in the current DPDCH "<<"\n");
                  break;
                }
            }

          k_ = sf_ / 4;              // calculates k_
          controlInfo->SetChannel (DPDCH);
          EnqueueDataPackets (newItem.first,controlInfo, m_scramCodeDedicatedCh, sf_, k_);              // send packet over the air interface
          
        }
      // building DPCCH

      // calculate de physical rate transmitted
      Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();
      controlInfo->SetDpdchRate ((2560 / (UMTS_SlotTime * sf_)) * numberOfDpdchNeeded);
      controlInfo->SetChannel (DPCCH);
      controlInfo->SetDataSize (1.25);
      controlInfo->SetNumberOfDpdch (numberOfDpdchNeeded);
           
      Ptr<Packet> packet = Create<Packet> ();
      UmtsPacketTypeTag typeTag;
      typeTag.Set(OtherData);
      packet->AddPacketTag(typeTag);
      
      EnqueueDataPackets (packet,controlInfo, m_scramCodeDedicatedCh, 256, 0);      
      return;

    }

  return;
}

void
UmtsPhyLayerUE ::AlertInterferenceMeasure ()
{
  m_interferenceCallBack ();
}

void
UmtsPhyLayerUE::EnqueueRachDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k)
{
  controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);      // source address
  controlInfo->SetDestinationNodeIdentifier (m_nodeBIdentifier);      // destination address
  controlInfo->SetScramblingCode (scram_c);                     // scrambling code
  controlInfo->SetSpreadingFactor (sf);                         // spreading factor
  controlInfo->SetChannelisationCode (k);                       // Channelisation Code
  controlInfo->SetFrequency (m_ulfreq);
  controlInfo->SetUmtsPhyLayerUEPointer (this);
  controlInfo->SetTxType (Common);
  controlInfo->SetLLType(LL_DATA);

  UmtsPacketTypeTag typetag;
  packet->PeekPacketTag(typetag);   
  m_rachQueue->PhyEnqueue (packet->Copy (),controlInfo->Copy ());            // prepare pkt to tx onto air interface


}


// Enqueue Data Packets
void
UmtsPhyLayerUE::EnqueueDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k)
{
  controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);      // source address
  controlInfo->SetDestinationNodeIdentifier (m_nodeBIdentifier);      // destination address
  controlInfo->SetScramblingCode (scram_c);                     // scrambling code
  controlInfo->SetSpreadingFactor (sf);                         // spreading factor
  controlInfo->SetChannelisationCode (k);                       // Channelisation Code
  controlInfo->SetFrequency (m_ulfreq);
  controlInfo->SetUmtsPhyLayerUEPointer (this);
  controlInfo->SetTxType (Dedicated);

  if (controlInfo->GetChannel () == DPDCH)
    {
      controlInfo->SetLLType (LL_DATA);
    }
  else
    {
      controlInfo->SetLLType (OTHER);
    }
  controlInfo->SetSize ((int)(UMTS_TBLength / (sf * 8)));

  // calculates the size in bytes depending on the spreading factor chosen

  if ((UMTS_TBLength % (controlInfo->GetSpreadingFactor () * 8)) != 0)
    {
      int value = controlInfo->GetSize ();
      value++;
      controlInfo->SetSize (value);
    }  

  m_packetToTxQueue->PhyEnqueue (packet->Copy (),controlInfo);                    // prepare pkt to tx onto air interface

}



// ---------------------------------------------------------------------------------------------------------------//
// ------------------------RACH CHANNEL RECEPTIONS----------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------------------//

//  RACH without preamble
void
UmtsPhyLayerUE::MakeRachWithoutPreamble (Ptr<Packet> packet)
{
  int i, j, index_c_;
  
  Ptr<ControlPacket> controlpacket=Create<ControlPacket>();

  // looking for an available Scrambling Code
  index_c_ = ScramblingCodeAllocation ();
  if (index_c_ == -1)
    {
      // there is not any available Scrambling Code
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY// Without preamble. No available Scrambling Code. Sending a Failure Message to the RRC Layer.\t\t\t\t  Time " << Simulator::Now () << " \n");
      
      m_waitRachProcedure = 0;
      m_txPreambleNumber = 0;
      
      AppIndexTag indexTag;
      packet->PeekPacketTag(indexTag);
      ForwardToRRC (indexTag.Get());

      return;
    }
  else
    {
      // index_c_ is the available Scrambling Code
      controlpacket->SetScramblingCode (index_c_);
      controlpacket->SetUsedRach (m_wcdmaParams.rach_tx_);

      // choose a random spreading factor
      i = RandomVariable (UniformVariable (0,3)).GetInteger () + 5;

      int sf = 2;
      for (j = 0; j < i - 1; j++)
        {
          sf *= 2;
        }

      controlpacket->SetSpreadingFactor (sf);
      controlpacket->SetChannelisationCode (0);
      controlpacket->SetBeginOfSequence (1);
      controlpacket->SetDestination (m_nodeBIp);
      controlpacket->SetFrequency (m_ulfreq);
      controlpacket->SetEndOfSequence (1);
      controlpacket->SetPacketType(PT_ACK);
      controlpacket->SetLLType(OTHER);
     
      controlpacket->SetChannel (PRACH);          // ch mapping

      UmtsReceiverIdTag recTag;
      packet->PeekPacketTag(recTag);
                  
      controlpacket->SetDestinationNodeIdentifier(recTag.Get());
      controlpacket->SetSourceNodeIdentifier(m_nodeIdentifier);

      m_rachQueue->PhyEnqueue(packet->Copy(),controlpacket);           // store PRACH message in the RACH transmission queue
    }
  return;
}

// looking for an available Scrambling Code
int
UmtsPhyLayerUE::ScramblingCodeAllocation ()
{
  int index_c_, i = 0;
  // begin in an aleatory place

  index_c_ = RandomVariable (UniformVariable (0,MAX_NUM_SCRAM - 1)).GetInteger ();
  while (i < MAX_NUM_SCRAM)
    {
      if (m_wcdmaParams.free_res_[index_c_][0] == 0)
        {
          return (index_c_);               // index_c is an available scrambling code
        }
      i++;
      index_c_++;
      if (index_c_ == MAX_NUM_SCRAM)
        {
          index_c_ = 0;
        }
    }
  return (-1);        // no free resources
}

// for transmitting the RACH preamble
void
UmtsPhyLayerUE::TxRachPreamble ()
{NS_LOG_FUNCTION(this);
  int i, j, index_c_, index_s_;

  // looking for an available RACH
  i = 0;
  j = RandomVariable (UniformVariable (0,MAX_NUM_RACH - 1)).GetInteger ();
  while (i < MAX_NUM_RACH)
    {
      if (m_wcdmaParams.free_rach_[j] == 0)            // j is an available RACH
        {
          break;
        }
      i++;
      j++;
      if (j == MAX_NUM_RACH)
        {
          j = 0;
        }
    }

  // check if there is an available RACH
  if (i == MAX_NUM_RACH)
    {
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//No available RACH.Rescheduling the Preamble Timer. \t\t\t\t  Time " << Simulator::Now ());

      m_preambleTimer.Cancel ();

      // arbitly limited the backoff timer from 4 to 12 slots.Integer(K) gives a uniformly distributed integer btwn 0 and k-1
      // maybe shud backoff for some num of frames instead.
      int temp =  RandomVariable (UniformVariable (0,8)).GetInteger ();
      Time time = (Seconds)((6 * 0.00125) + (double)(temp * 0.00125));
      m_preambleTimer = Simulator::Schedule (time,&UmtsPhyLayerUE::PreambleHandler,this);              // start Preamble timer
      return;
    }
  else
    {
      // j is the available RACH to use

      // looking for an available Scrambling Code
      index_c_ = ScramblingCodeAllocation ();
      // check if there is an available Scrambling Code
      if (index_c_ == -1)
        {
          NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//No Available Scrambling Code.Rescheduling the Preamble Timer. \t\t\t\t  Time " << Simulator::Now ());
          m_preambleTimer.Cancel ();

          // arbitly limited the backoff timer from 4 to 12 slots.

          int temp =  RandomVariable (UniformVariable (0,8)).GetInteger ();
          Time time = (Seconds)((6 * 0.00125) + (double)(temp * 0.00125));
          m_preambleTimer = Simulator::Schedule (time,&UmtsPhyLayerUE::PreambleHandler,this);                  // start Preamble timer
          return;
        }
      else
        {
          // looking for an available Signature
          i = 0;
          index_s_ = RandomVariable (UniformVariable (0,MAX_NUM_SIG - 1)).GetInteger () + 1;

          while (i < MAX_NUM_SIG)
            {
              if (m_wcdmaParams.free_res_[index_c_][index_s_] == 0)                    // available Signature
                {
                  break;
                }
              i++;
              index_s_++;
              if (index_s_ == MAX_NUM_SIG + 1)
                {

                  index_s_ = 1;
                }
            }
          // check if there is an available Signature
          if (i == MAX_NUM_SIG)
            {
              NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//No Available Signature.Rescheduling the Preamble Timer. \t\t\t\t  Time " << Simulator::Now ());
              m_preambleTimer.Cancel ();

              // arbitly limited the backoff timer from 4 to 12 slots.

              int temp =  RandomVariable (UniformVariable (0,8)).GetInteger ();
              Time time = (Seconds)((6 * 0.00125) + (double)(temp * 0.00125));
              m_preambleTimer = Simulator::Schedule (time,&UmtsPhyLayerUE::PreambleHandler,this);                      // start Preamble timer
              return;
            }
          else
            {             
              // storing preamble characteristics

              m_wcdmaParams.signature_ = index_s_;
              m_wcdmaParams.rach_tx_ = j;
              m_wcdmaParams.scram_tx_ = index_c_;

              // building the preamble message
              Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();

              controlpacket->SetInitialAccess (0);
              controlpacket->SetDestinationNodeIdentifier (m_nodeBIdentifier);                                // destination address

              controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);                        // source address
              controlpacket->SetScramblingCode (index_c_);
              controlpacket->SetSignature (m_wcdmaParams.signature_);
              controlpacket->SetUsedRach (m_wcdmaParams.rach_tx_);              
              controlpacket->SetSpreadingFactor (256);                  
              controlpacket->SetChannelisationCode (m_wcdmaParams.signature_);
              controlpacket->SetPacketType (PT_PREAMBLE);
              controlpacket->SetLLType (OTHER);
              controlpacket->SetUmtsPhyLayerUEPointer (this);              
              controlpacket->SetChannel (PRACH);
              controlpacket->SetEndOfSequence (0);

              int value = (int)(UMTS_TBLength / (controlpacket->GetSpreadingFactor () * 8));                // bits = 2560/SF
              controlpacket->SetSize (value);

              if ((UMTS_TBLength % (controlpacket->GetSpreadingFactor () * 8)) != 0)
                {
                  int integer = controlpacket->GetSize ();
                  integer++;
                  controlpacket->SetSize (integer);
                }

	      Ptr<Packet> packet=Create<Packet>();
	      UmtsPacketTypeTag typeTag;
	      typeTag.Set(OtherData);
	      
	      UmtsReceiverIdTag receiverTag;
	      receiverTag.Set(m_nodeBIdentifier);
	      
	      packet->AddPacketTag(receiverTag);
	      packet->AddPacketTag(typeTag);
	      
              m_rachQueue->PhyEnqueueHead (packet,controlpacket);                   // store preamble in the RACH transmission queue

              m_preambleTimer.Cancel ();

              // arbitly limited the backoff timer from 4 to 12 slots.

              int temp =  RandomVariable (UniformVariable (0,8)).GetInteger ();
              Time time = (Seconds)((6 * 0.00125) + (double)(temp * 0.00125));
             
              m_preambleTimer = Simulator::Schedule (time,&UmtsPhyLayerUE::PreambleHandler,this);// start Preamble timer
              m_txPreambleNumber++;
            }
        }
    }
  return;
}


// ---------------------------------------------------------------------------------------------------------------//
// ------------------------Setup and Handover Procedures----------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------------------//


// -------------------------------------------Receiving Control Packets From The Channel------------------------------------------------//

void
UmtsPhyLayerUE::ReceivePacketFromBelow (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //std::cout<<"UmtsPhyLayerUE::ReceivePacketFromBelow!!!"<<controlpacket->GetChannel ()<<std::endl;
  
  int i, j, len;
  controlpacket->SetUmtsPhyLayerUEPointer (this);
  
  UmtsPacketTypeTag typeTag;
  packet->PeekPacketTag(typeTag);
    
  /*if(controlpacket->GetChannel ()==SCCPCH)
  {
    //std::cout<<"ControlPacket::GetChannel SCCPCH"<<std::endl;
  }*/
  switch (controlpacket->GetChannel ())
    {
    case AICH:
      
      if ((controlpacket->GetScramblingCode () == m_wcdmaParams.scram_tx_) && (controlpacket->GetSignature () == m_wcdmaParams.signature_)
          && (controlpacket->GetDestinationNodeIdentifier () == m_nodeIdentifier))
        {
          // Response of a Preamble, now is possible to tx RACH

          m_preambleTimer.Cancel ();
          m_txPreambleNumber = 0;
	  
	  UmtsPacketTypeTag rachTypeTag;
	  m_usingRachPacket->PeekPacketTag(rachTypeTag);
	  
          if (rachTypeTag.Get() == CommonUnicastData)
            {              
              MakeRACHDataPackets ();
            }
          else
            {
              SendRachWithPreamble ();                          // send message part RACH
            }
        }
      else
        {

        }
      break;
    case PCCPCH:
      //std::cout<<"UmtsPhyLayerUE::ReceivePacketFromBelow PCCPCH"<<std::endl;
      if (controlpacket->GetScramblingCode () == m_wcdmaParams.p_scrambling_)
        {

          for (i = 0; i < MAX_NUM_SCRAM; i++)                           // update free resources
            {
              for (j = 0; j < MAX_NUM_SIG + 1; j++)
                {
                  m_wcdmaParams.free_res_[i][j] = controlpacket->GetFreeResources (i,j);
                }
            }

          for (i = 0; i < MAX_NUM_RACH; i++)                    // update free RACH
            {
              m_wcdmaParams.free_rach_[i] = controlpacket->GetAvailableRach (i);
            }
          
        }
      break;

    case SCCPCH:                // carrying FACH and PCH transport channels
      //std::cout<<"SCCPCH CASE:"<<std::endl;
      if (((controlpacket->GetScramblingCode () == m_wcdmaParams.p_scrambling_) || (controlpacket->GetScramblingCode () == m_selectedScramCode))
          && (controlpacket->GetDestinationNodeIdentifier () == m_nodeIdentifier||controlpacket->GetDestinationNodeIdentifier () ==ID_BROADCAST||
          controlpacket->GetDestinationNodeIdentifier () == ID_MULTICAST))
        {
          if (controlpacket->GetPacketType () == PT_ACK)
            {                           // RLC-Acknowledge mode              
              controlpacket->SetChannel (FACH);                                 // ch mapping
              ForwardUp(packet);
	      break;
            }

          if (controlpacket->GetLLType () == LL_DATA)
            {
              if (controlpacket->GetEndOfSequence () == 1)
                {
                  controlpacket->SetChannel (FACH);                             // ch mapping
                  DownLinkDemux (packet,controlpacket);
                }
            }

          if (controlpacket->GetLLType () == LL_MBMS_BROADCAST)
            {              
              if (controlpacket->GetEndOfSequence () == 1)
                {
                  controlpacket->SetChannel (FACH);
                  DownLinkDemux (packet,controlpacket);
                }
            }
          //!!! NEW ADDED EXTRA CASE FOR MULTICAST !!!
          if (controlpacket->GetLLType () == LL_MBMS_MULTICAST)
            {              
              if (controlpacket->GetEndOfSequence () == 1)
                {
                  controlpacket->SetChannel (FACH);
                  DownLinkDemux (packet,controlpacket);
                }
            } 

        }

      break;
    case DPCCH:                         // Dedicated control channel
      if (controlpacket->GetDestinationNodeIdentifier () == m_nodeIdentifier)
        {                               // we are the destination         
          m_dpdchControlParams.numberOfDpdch = controlpacket->GetNumberOfDpdch ();
          len = GetNumberOfDpdchArrived ();
          if (len != m_dpdchControlParams.numberOfDpdch)                           // check if all DPDCH have arrived
            {
              for (j = 0; j < len; j++)
                {                  
                  m_rxDpdchQueue[j].first = 0 ;
                  m_rxDpdchQueue[j].second = 0;

                }
              
              break;
            }
          
          Decode ();                                    // all DPDCHs received, starting decoding
          

        }

      break;
    case DPDCH:

      if (controlpacket->GetDestinationNodeIdentifier () == m_nodeIdentifier)                           // we are the destination
        {
          len = GetNumberOfDpdchArrived ();         
          m_rxDpdchQueue[len].first = packet;                        // store DPDCH packet in buffer
          m_rxDpdchQueue[len].second = controlpacket;
        }

    default:
      break;
    }
}

void
UmtsPhyLayerUE::SetupReplyReceived (Ptr<ControlPacket> controlpacket)
{  
  // if (m_handover = 1)
  m_ulfreq = controlpacket->GetFrequency ();        // take new uplink freq for transmitting
  m_dlfreq = m_ulfreq + 12;
  m_handover = 0;
  if (controlpacket->GetPrimaryScramblingCode () == -1)
    {
      m_wcdmaParams.p_scrambling_ = m_selectedScramCode;
    }
  else
    {
      m_wcdmaParams.p_scrambling_ = controlpacket->GetPrimaryScramblingCode ();
    }
  
  m_nodeBIdentifier = controlpacket->GetSourceNodeIdentifier ();
  m_nodeBPhyLayer = controlpacket->GetUmtsPhyLayerBSPointer ();
  m_nodeBIp = controlpacket->GetSource ();
  m_packetsInSequenceH = m_packetsInSequence;
  m_packetsInSequence = 0;
  m_selectedScramCode = -1;  
  controlpacket->SetChannel (FACH);     // ch mapping
  controlpacket->SetChannel (CCCH);
  controlpacket->SetSize (controlpacket->GetSize () - MAC_HDR_SZ - RLC_HDR_SZ);

  
}


/*This function is accessed when we have recieved some DPDCH packets from the UE Node and we decode them (reassemble) in order to pass
** them to the higher layers
*/

void
UmtsPhyLayerUE::Decode ()
{
  int i;


  NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY Layer / Received " << m_dpdchControlParams.numberOfDpdch << " DPDCH packets.Start decoding them.Time " << Simulator::Now () << " \n");


  // demux DPDCH and extract packets
  for (i = 0; i < m_dpdchControlParams.numberOfDpdch; i++)
    {
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem = m_rxDpdchQueue[i];
      Ptr<ControlPacket> controlInfo = newItem.second;

      // go through the array extracting packets

      while (controlInfo->GetDpdchQueueSize () > 0)
        {
          std::pair<Ptr<Packet>, Ptr<ControlPacket> > item;
          item = controlInfo->GetDpdchPacket ();
          Ptr<ControlPacket> controlInfo2 = Create<ControlPacket> ();
          controlInfo2 = item.second;


          if (controlInfo2->GetSourceNodeIdentifier () == m_nodeBIdentifier)
            {              
              if (m_packetsInSequence == -1)                // Previusly packets arrived not in sequence
                {                  
                  if (controlInfo2->GetSequenceNumber () == 0)                          // first physical fragment of a packet
                    {
                      if (controlInfo2->GetEndOfSequence () == 1)                       // last physical fragment of a packet
                        {
                          controlInfo2->SetChannel (DCH);                          
                          DownLinkDemux (item.first->Copy (),controlInfo2->Copy ());
                          m_packetsInSequence = 0;

                        }
                      else                              // is not the last fragment, drop it
                        {
                          m_packetsInSequence = 1;                   // waiting fragment number 1

                        }
                    }
                  else                       // is not the first fragment, drop the packet
                    {
 
                    }
                }
              else if (controlInfo2->GetSequenceNumber () == m_packetsInSequence)                   // Previous packets arrived in sequence
                {                  
                  // is the fragment that we expected
                  if (controlInfo2->GetEndOfSequence () == 1)                           // is the last physical fragment, send it to MAC
                    {
                      controlInfo2->SetChannel (DCH);                      
                      DownLinkDemux (item.first->Copy (),controlInfo2);
                      m_packetsInSequence = 0;

                    }
                  else                                  // is not the last fragment, drop the packet
                    {
                      m_packetsInSequence = m_packetsInSequence + 1;                     
                    }
                }
              else                              // Previous packets arrived in sequence
                {                 
                  m_packetsInSequence = -1;
                }
            }
        }
      m_rxDpdchQueue[i].first = 0;
      m_rxDpdchQueue[i].second = 0;
    }
  return;
}

void
UmtsPhyLayerUE::DownLinkDemux (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo)
{
  //std::cout<<"UmtsPhyLayerUE::DownLinkDemux"<<std::endl;
  UmtsCodeTag codeTag;
  packet->PeekPacketTag(codeTag);
    
  uint32_t newSize=GetChannelDecodingSize (packet->GetSize(),codeTag.Get());                         // channel decoding
  packet->RemoveAtEnd(packet->GetSize()-newSize);  
  ForwardUp (packet);

  return;
}


void
UmtsPhyLayerUE::SendRachWithPreamble ()
{
  int i, j, index_c_;

  // looking for an available Scrambling Code
  index_c_ = ScramblingCodeAllocation ();
    
  // check if there is an available Scrambling Code
  if (index_c_ == -1)
    {
      // there is not any available Scrambling Code

      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY// No available Scrambling Code.Sending a Failure Message to the RRC Layer.   \t\t\t\t   Time " << Simulator::Now () << "\n");	        
      m_waitRachProcedure = 0;
      m_txPreambleNumber = 0;      
      ForwardToRRC (-1);
            
      return;
    }
  else
    {
      // index_c_ is the available Scrambling Code      
      m_usingRachControlInfo->SetScramblingCode (index_c_);
      m_usingRachControlInfo->SetUsedRach (m_wcdmaParams.rach_tx_);
      m_usingRachControlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);
      m_usingRachControlInfo->SetDestinationNodeIdentifier (m_nodeBIdentifier);
      // choose a random spreading factor
      i = RandomVariable (UniformVariable (0,3)).GetInteger () + 5;

      int sf = 2;
      for (j = 0; j < i - 1; j++)
        {
          sf *= 2;
        }

      m_usingRachControlInfo->SetSpreadingFactor (sf);
      m_usingRachControlInfo->SetChannelisationCode ((m_usingRachControlInfo->GetSpreadingFactor ()) * m_wcdmaParams.signature_ / 16);       // k for the channelisation code
      m_usingRachControlInfo->SetEndOfSequence (0);
      m_usingRachControlInfo->SetSize ((int)(UMTS_TBLength / (sf * 8)));

      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//Building PRACH control and data parts.Time " << Simulator::Now ());

      if ((UMTS_TBLength % (sf * 8)) != 0)
        {
          int value = m_usingRachControlInfo->GetSize ();
          value++;
          m_usingRachControlInfo->SetSize (value);
        }

      m_usingRachControlInfo->SetChannel (PRACH);

      Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();
      controlpacket = m_usingRachControlInfo->Copy ();
      m_usingRachControlInfo->SetUmtsPhyLayerUEPointer (this);
      controlpacket->SetUmtsPhyLayerUEPointer (this);
      controlpacket->SetSpreadingFactor (256);
      controlpacket->SetLLType (OTHER);
      controlpacket->SetChannelisationCode (16 * m_wcdmaParams.signature_ + 15);       // k for the channelisation code
      controlpacket->SetSize ((int)(UMTS_TBLength / (controlpacket->GetSpreadingFactor () * 8)));


      if ((UMTS_TBLength % (controlpacket->GetSpreadingFactor () * 8)) != 0)
        {
          int value = controlpacket->GetSize ();
          value++;
          controlpacket->SetSize (value);
        }


      // length of a RACH message part is a frame (8 slots)
      for (i = 0; i < 8; i++)
        {
          if (i == 0)
            {
              m_usingRachControlInfo->SetBeginOfSequence (1);
              controlpacket->SetBeginOfSequence (1);
            }
          else
            {
              m_usingRachControlInfo->SetBeginOfSequence (0);
              controlpacket->SetBeginOfSequence (0);
            }
          if (i == 7)
            {
              m_usingRachControlInfo->SetEndOfSequence (1);                // end of RACH tx
              controlpacket->SetEndOfSequence (1);
	      m_usingRachControlInfo->SetEndOfPacket (1);                // end of RACH tx
              controlpacket->SetEndOfPacket (1);
            }


          m_rachQueue->PhyEnqueue (m_usingRachPacket,m_usingRachControlInfo->Copy ());            // store PRACH data part in the RACH transmission queue
          m_rachQueue->PhyEnqueue (0,controlpacket->Copy ());              // store PRACH control part in the RACH transmission queue


        }


    }


  return;

}

/**----------------------------------------Packet Transmissions------------------------------*/

void
UmtsPhyLayerUE::SendPacketToChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //std::cout<<"SendPacketToChannel::"<<std::endl;
  //packet->Print(std::cout);
  //std::cout<<"UmtsPhyLayerUE::SendPacketToChannel"<<packet->GetSize()<<std::endl;
  controlpacket->SetUmtsPhyLayerUEPointer (this);
  controlpacket->SetFrequency (m_ulfreq);

  if (controlpacket->GetChannel () == DPDCH||controlpacket->GetChannel () == DPCCH)
    {     
      m_channel->ReceiveFromNodeUE (packet,controlpacket);
    }
  else
    {
      if (controlpacket->GetLLType () == LL_DATA)
        {
          m_sharedChannel->ReceiveFromNodeUE (packet,controlpacket);
        }
      else
        {
          NS_LOG_DEBUG("NodeUE " << m_nodeIdentifier << " PHY // Sending a Non Data Data Packet to the Channel to the NodeB with TxRate " <<
          controlpacket->GetTxRate () << "UID "<<packet->GetUid()<<".Time " << Simulator::Now () << "\n");
          m_sharedChannel->ReceiveFromNodeUE (packet,controlpacket);
        }
    }
}


/**----------------------------------------End of Packet Transmissions-----------------------*/


void
UmtsPhyLayerUE::SetExternalInterference (double value)
{
  m_externalInterference = value;
}


/**The following functions are related to the Error Control
*/

void
UmtsPhyLayerUE::DownLinkInterference (unsigned long error)
{
  m_calculatedErrorRate = error;             // update m_calculatedErrorRate
  return;
}

// returns if the actual transport block is erroneous
int
UmtsPhyLayerUE::CheckError ()
{
  int n = RandomVariable (UniformVariable (0,m_calculatedErrorRate)).GetInteger ();    // generate random number based on ul_error probability

  if (n == 0)           // error
    {      
      return(1);           // ERROR
    }
  return(0);            // not error
}

void
UmtsPhyLayerUE::CalculateExternalInterference (double interference)
{
  m_externalInterference = interference;
}

void
UmtsPhyLayerUE::SetCalculatedInternalInterferenceDownlink (double value)
{
  m_internalInterference = value;
}

// ---------------------------------------------------------------------------------------------------------------//
// ------------------------Timer Handlers-------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------------------------------------//

// Called when PreambleTimer expires.
// We have not received the waited AICH as a response of the PREAMBLE sent
void
UmtsPhyLayerUE::PreambleHandler ()
{
  
  if (m_txPreambleNumber < MAX_NUM_PREAMBLE)
    {
      // we can continue transmitting preambles      
      TxRachPreamble ();           // transmit RACH preamble
    }
  else
    {
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//We have transmitted the maximum preambles.         Time " << Simulator::Now ());
      externalError++;            
      m_waitRachProcedure = 0;
      m_txPreambleNumber = 0;
      
      AppIndexTag appTag;
      m_usingRachPacket->PeekPacketTag(appTag);            
      ForwardToRRC (appTag.Get());
    }
}


// Called when UpSlotUmtsTimer expires.
void
UmtsPhyLayerUE::UpSlotHandler ()
{NS_LOG_FUNCTION(this);
  // Restart timer for next slot.
  m_transmitting = 0;
  m_upSlotTimer = Simulator::Schedule (Seconds (slot_time_),&UmtsPhyLayerUE::UpSlotHandler,this);
  MakeDPXCH ();        // DPDCH Multiplexing and DPCCH creation

  while (m_packetToTxQueue->GetPhySize () > 0)
    {

      std::pair<Ptr<Packet>, Ptr<ControlPacket> > packetStorage = m_packetToTxQueue->PhyDequeue ();
      m_transmitting = 1;
      SendPacketToChannel (packetStorage.first,packetStorage.second);           // transmit to Node B
    }

  // program the next slot
  m_uplinkSlotCounter++;
  if (m_uplinkSlotCounter == SLOTS_PER_FRAME)
    {
      m_uplinkSlotCounter = 0;
    }
  return;
}



// Called when RachUpSlotUmtsTimer expires.
void
UmtsPhyLayerUE::RachUpSlotHandler ()
{NS_LOG_FUNCTION(this);
  m_transmitting = 0;  
  m_rachUpSlotTimer = Simulator::Schedule (Seconds (rach_slot_time_),&UmtsPhyLayerUE::RachUpSlotHandler,this);
  
AGAIN:
  // check if there is any packet in the buffer
  if (m_rachQueue->GetPhySize () > 0)
    {

      std::pair<Ptr<Packet>, Ptr<ControlPacket> > packetStorage = m_rachQueue->PhyDequeue ();

      Ptr<ControlPacket> controlpacket = packetStorage.second;
      Ptr<Packet> packet = packetStorage.first;

      if (controlpacket->GetLLType () == LL_DATA)
        {
          m_transmitting = 1;
        }
        
       
      if (controlpacket->GetPacketType() == PT_ACK)
        {

	  UmtsRlcAckHeader ackHeader;
	  packet->PeekHeader(ackHeader);	            
          SendPacketToChannel (packet,controlpacket);               // transmit to Node B
          goto AGAIN;                // send more packets
        }

      SendPacketToChannel (packet,controlpacket);                 

      // end of transmision of RACH message
    
	if (controlpacket->GetEndOfPacket() == 1)
        {	 
          m_waitRachProcedure = 0;
        }

    }
    
    if(!m_rachTotallySent)
    {
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//The RACH Packet has not been sent totally, sending a preamble for it. \t\t\t\t Time " << Simulator::Now ());
      m_waitRachProcedure = 1;
      m_txPreambleNumber = 0;
      TxRachPreamble ();            // transmit preamble
      m_rachTotallySent=true;
          
    }
    else if ((m_waitRachProcedure == 0) && (m_prachTemp->GetPhySize () > 0))
    {
      // check if there is other rach waiting to be transmited
      
      NS_LOG_DEBUG ("NodeUE " << m_nodeIdentifier << " PHY//Other RACH waiting, sending a preamble for them. \t\t\t\t Time " << Simulator::Now () << "\n");
      // begin RACH procedure
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > packetStorage = m_prachTemp->PhyDequeue ();
      UmtsPacketTypeTag typeTag;
      packetStorage.first->PeekPacketTag(typeTag);
      
      if (typeTag.Get() == CommonUnicastData)
        {
	  m_usingRachPacket=packetStorage.first;
          m_usingRachControlInfo = packetStorage.second;
        }

      m_waitRachProcedure = 1;
      m_txPreambleNumber = 0;
      TxRachPreamble ();            // transmit preamble

    }
      

  
  
  return;
}

// Called when BlerTimer expires.
// Timer for calculating the Interference
void
UmtsPhyLayerUE::BlerHandler ()
{NS_LOG_FUNCTION(this);
  double Eb_No, bler, I = 0;
  // Restart timer for next time.
  AlertInterferenceMeasure ();
  
  double time=0;
  
  if(m_calculationTime==0)
  {
    time= 5 * UMTS_FrameTime;
  }
  else
  {
    time=m_calculationTime;
  }
  
  m_blerTimer = Simulator::Schedule (Seconds (time),&UmtsPhyLayerUE::BlerHandler,this);

  if ((m_dedicatedLastRate > 0)||(m_commonLastRate > 0))
    {
      // if there has been any interference
      I = m_externalInterference + m_internalInterference;

      if (I != 0)
        {         
          Eb_No = (CHIP_RATE * (m_dedicatedRxPw + m_commonRxPw)) / ((m_dedicatedLastRate + m_commonLastRate) * (I + Pn));
        }
      else
        {
          Eb_No = 1e38;
        }
    }
  else
    {
      // no interference at all
      Eb_No = 1e38;
    }

  if (m_dedicatedLastRate > 0)
    {
      bler = m_blerTable->getbler (Eb_No);      // consult table (Eb_No)
      m_downlinkErrorRate = (int)(1 / bler) + 1;
    }
  else
    {
      m_downlinkErrorRate = 1000000000;
    }

  DownLinkInterference (m_downlinkErrorRate);      // send the results to MAC
  return;
}

void
UmtsPhyLayerUE::ErrorCalculationTimerHandler ()
{NS_LOG_FUNCTION(this);
  m_errorCalculationTimer = Simulator::Schedule (Seconds (m_tti),&UmtsPhyLayerUE::ErrorCalculationTimerHandler,this);   // schedule the next m_tti
  m_errorIndicator = CheckError ();       // update error_
}


} // namespace ns3
