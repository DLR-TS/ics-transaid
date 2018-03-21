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
#include "ovsfTrees.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "controlpacket.h"
#include "ns3/double.h"
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/random-variable.h"
#include "umts-phy-layer-bs.h"
#include "umts-phy-layer-ue.h"
#include "umts-manager.h"
#include "umts-userequipment-manager.h"
#include "umts-queue.h"
#include "blerTable.h"
#include "umts-channel.h"
#include "ns3/enum.h"
#include "ns3/propagation-loss-model.h"
#include "umts-tags.h"
#include "ns3/node-id-tag.h"
#include "ns3/app-index-tag.h"
#include "ns3/itetris-types.h"

NS_LOG_COMPONENT_DEFINE ("UmtsPhyLayerBS");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UmtsPhyLayerBS);

// variables
double UmtsPhyLayerBS::slot_time_ = 0;
double UmtsPhyLayerBS::aich_slot_time_ = 0;

TypeId
UmtsPhyLayerBS::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsPhyLayerBS")
    .SetParent<Object> ()
    .AddConstructor<UmtsPhyLayerBS> ()
    .AddAttribute ("AntennaHeight",
                   "Height of the Antenna in metres (m)",
                   DoubleValue (5.0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetAntennaHeight,
                                       &UmtsPhyLayerBS::GetAntennaHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("AntennaGain",
                   "Antenna Gain (dB)",
                   DoubleValue (10.0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetAntennaGain,
                                       &UmtsPhyLayerBS::GetAntennaGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SystemLoss",
                   "Loss associated to the System (dB)",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetSystemLoss,
                                       &UmtsPhyLayerBS::GetSystemLoss),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DedicatedTxPower",
                   "Transmission Power (dB)",
                   DoubleValue (-9.0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetDedicatedPower,
                                       &UmtsPhyLayerBS::GetDedicatedPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxFrequency",
                   "TxFrequency (GHz)",
                   DoubleValue (300000000),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetTxFrequency,
                                       &UmtsPhyLayerBS::GetTxFrequency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("CommonTxPower",
                   "Common Tx Power (dB)",
                   DoubleValue (2.0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetCommonTxPower,
                                       &UmtsPhyLayerBS::GetCommonTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinDistance",
                   "Minimum Distance to consider for Path Loss calculation (m)",
                   DoubleValue (10),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetMinDistance,
                                       &UmtsPhyLayerBS::GetMinDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Shadowing",
                   "Loss associated to the Shadowing (dB)",
                   DoubleValue (5),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetShadowing,
                                       &UmtsPhyLayerBS::GetShadowing),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TargetSNR",
                   "SNR Value above which the transmission is considered correct (lineal)",
                   DoubleValue (100),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetSnrTarget,
                                       &UmtsPhyLayerBS::GetSnrTarget),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PowerControlStep",
                   "It controls how the transmission power is decreased/increased (dB)",
                   EnumValue (Step1),
                   MakeEnumAccessor (&UmtsPhyLayerBS::SetControlAlgothmType,
                                     &UmtsPhyLayerBS::GetControlAlgothmType),
                   MakeEnumChecker (Step0_5,"step0_5",Step1,"step1",Step1_5,"step1_5",Step2,"step2"))
    .AddAttribute ("CoverageRange",
                   "Considering a circular coverage area sorrounding a particular Base Station, the radio equals the Coverage Range",
                   DoubleValue (500),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetCoverageRange,
                                     &UmtsPhyLayerBS::GetCoverageRange),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("InterferenceCalculationFreq",
                   "It indicates how often the calculation of the interference is carried out",
                   DoubleValue (0),
                   MakeDoubleAccessor (&UmtsPhyLayerBS::SetInterferenceCalculationTime,
                                     &UmtsPhyLayerBS::GetInterferenceCalculationTime),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}


UmtsPhyLayerBS::UmtsPhyLayerBS ()
{
NS_LOG_FUNCTION(this);
  m_nodeIdentifier = (uint32_t)-1;
  slot_time_ = 0.000667 ; // umts slot is of 666.6 microseconds
  aich_slot_time_ = 0.00125 ; // umts slot is of 1.25 miliseconds
  
  m_tti = 0.01;

  m_myIp.Set ("0");
  int i,j;
  
  for (i = 0; i < MAX_NUM_SCRAM; i++)
    {
      for (j = 0; j < MAX_NUM_SIG + 1; j++)
        {
          m_wcdmaParams.free_res_[i][j] = 0;
        }
    }

  for (j = 0; j < MAX_NUM_UE; j++)
    {
      m_nodeUEIdRegistry[j] = (uint32_t)-1;      
      m_addr[j] = (uint32_t)-1;
    }
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > nullItem;
  nullItem.first = 0;
  nullItem.second = 0;
  for (j = 0; j < MAX_NUM_UE; j++)
    {
      for (i = 0; i < MAX_NUM_DPDCH; i++)
        {
          m_rxDpdchQueue[j][i] = nullItem;
        }
    }

  for (j = 0; j < MAX_NUM_UE; j++)
    {
      m_nodeUEInformation[j].paging_group = -1;
      m_nodeUEInformation[j].bytes_slot = 0.0;
      m_nodeUEInformation[j].in_seq_ = 0;
      m_nodeUEInformation[j].count = 0;
      m_nodeUEInformation[j].ul_CommonErrorRate = 1000000000;
      m_nodeUEInformation[j].ul_DedicatedErrorRate = 1000000000;
      m_nodeUEInformation[j].CommonLastRate = 0;
      m_nodeUEInformation[j].DedicatedLastRate = 0;
      m_nodeUEInformation[j].DedicatedPrx = 0;
      m_nodeUEInformation[j].ul_freq = -1;
      m_nodeUEInformation[j].CommonPrx = 0;
      m_nodeUEInformation[j].dedicatedTxPower = 0;
      m_nodeUEInformation[j].channel = Create<UMTSChannel> ();
      m_nodeUEInformation[j].transmitting = 0;
      m_nodeUEInformation[j].phyAddress = 0;
    }
  m_fachPacket = Create<Packet> ();

  for (j = 0; j < MAX_NUM_UE; j++)
    {
      m_dedicatedMacDataQueue[j] = Create<UMTSQueue> ();
      m_internalCommonInterference[j] = 0;
      m_internalDedicatedInterference[j] = 0;
      m_numberCommonInternalInterference[j] = 0;
      m_numberDedicatedInternalInterference[j] = 0;
      m_totalInternalInterference[j] = 0;
    }
  m_SequenceNumber = 0;
  m_externalInterference = 0;

  m_numberExternalInterference = 0;


  for (i = 0; i < MAX_NUM_RACH; i++)
    {
      m_wcdmaParams.free_rach_[i] = 0;
    }

  m_wcdmaParams.p_scrambling_ = m_nodeIdentifier + 1000;

  for (j = 0; j < MAX_NUM_UE; j++)
    {
      m_uplinkInterference[j].phyaddr_ = (uint32_t)-1;
      m_uplinkInterference[j].ul_dedicated_error_ = 1000000000;
      m_uplinkInterference[j].ul_common_error_ = 1000000000;
      m_uplinkInterference[j].CommonError_ = 0;
      m_uplinkInterference[j].DedicatedError_ = 0;
    }

  Simulator::ScheduleWithContext (m_nodeIdentifier,Seconds(0),&UmtsPhyLayerBS::DownSlotHandler,this);
  m_aichTimer = Simulator::ScheduleNow (&UmtsPhyLayerBS::AichSlotHandler,this);
  m_blerTimer = Simulator::Schedule (Seconds (0),&UmtsPhyLayerBS::BlerHandler,this);
  m_errorCalculationTimer = Simulator::Schedule (Seconds (0),&UmtsPhyLayerBS::ErrorCalculationTimerHandler,this);
  
  m_aichPacketsToTx = Create<UMTSQueue> ();
  m_packetToTxQueue = Create<UMTSQueue> ();
  m_commonMacDataQueue = Create<UMTSQueue> ();
  m_downlinkFrequency = 0;  
  m_transmitting = 0;

  m_bytesPerFachSlot = 0;
}

void
UmtsPhyLayerBS::DoDispose ()
{

}

void
UmtsPhyLayerBS::SetSharedChannel (Ptr<UMTSChannel> channel)
{
  m_sharedChannel = channel;
  m_sharedChannel->SetUmtsPhyLayerBS (this);

}

Ptr<UMTSChannel>
UmtsPhyLayerBS::GetSharedChannel ()
{
  return m_sharedChannel;
}

void
UmtsPhyLayerBS::SetChannel (Ptr<UMTSChannel> channel_)
{
  m_channel = channel_;
}

Ptr<UMTSChannel>
UmtsPhyLayerBS::GetChannel ()
{
  return m_channel;
}

void
UmtsPhyLayerBS::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
}

Ptr<MobilityModel>
UmtsPhyLayerBS::GetMobility ()
{
  return m_mobility;
}

int
UmtsPhyLayerBS::GetPrimaryScramblingCode ()
{
  return m_wcdmaParams.p_scrambling_;
}

void
UmtsPhyLayerBS::SetInterferenceCalculationTime (double time)
{
  m_calculationTime=time; 
}

double
UmtsPhyLayerBS::GetInterferenceCalculationTime() const
{
  return m_calculationTime;
}

void
UmtsPhyLayerBS::SetControlAlgothmType (enum PowerChangeStep step)
{
  if (step == Step0_5)
    {
      m_powerControlStep = 0.5;
    }
  else if (step == Step1)
    {
      m_powerControlStep = 1;
    }
  else if (step == Step1_5)
    {
      m_powerControlStep = 1.5;
    }
  else if (step == Step2)
    {
      m_powerControlStep = 2;
    }
}

enum PowerChangeStep
UmtsPhyLayerBS::GetControlAlgothmType () const
{
  if (m_powerControlStep == 0.5)
    {
      return Step0_5;
    }
  else if (m_powerControlStep == 1)
    {
      return Step1;
    }
  else if (m_powerControlStep == 1.5)
    {
      return Step1_5;
    }
  else
    {
      return Step2;
    }
}

void
UmtsPhyLayerBS::SetCoverageRange(double value)
{  
 m_coverageRange=value; 
}

double
UmtsPhyLayerBS::GetCoverageRange() const
{
  return m_coverageRange;
}

void
UmtsPhyLayerBS::ChangeDedicatedTxPower (int index,int sign)
{
  if (sign == 1)
    {
      m_nodeUEInformation[index].dedicatedTxPower = m_nodeUEInformation[index].dedicatedTxPower + m_powerControlStep;
    }
  else if (sign == -1)
    {
      m_nodeUEInformation[index].dedicatedTxPower = m_nodeUEInformation[index].dedicatedTxPower - m_powerControlStep;
    }
}


void
UmtsPhyLayerBS::SetMyIpAddress (Ipv4Address address)
{
  m_myIp = address;
}

//!!! NEW FUNCTION !!!
void
UmtsPhyLayerBS::SetMyIpv6Address (Ipv6Address address)
{
  m_myIpv6 = address;
}



void
UmtsPhyLayerBS::SetNodeIdentifier(uint32_t identifier)
{
  m_nodeIdentifier=identifier;
}

uint32_t
UmtsPhyLayerBS::GetNodeIdentifier()
{
  return m_nodeIdentifier;
}

void
UmtsPhyLayerBS::SetSnrTarget (double value)
{
  m_snrTarget = value;
}

double
UmtsPhyLayerBS::GetSnrTarget () const
{
  return m_snrTarget;
}

void
UmtsPhyLayerBS::SetDownlinkFrequency (int freq)
{
  m_downlinkFrequency = freq;
}

void
UmtsPhyLayerBS::SetSystemLoss (double value)
{
  m_systemLoss = value;
}

double
UmtsPhyLayerBS::GetSystemLoss () const
{
  return m_systemLoss;
}

void
UmtsPhyLayerBS::SetTxFrequency (double value)
{
  m_txFrequency = value;
}

double
UmtsPhyLayerBS::GetTxFrequency () const
{
  return m_txFrequency;
}

void
UmtsPhyLayerBS::SetMinDistance (double value)
{
  m_minDistance = value;
}

double
UmtsPhyLayerBS::GetMinDistance () const
{
  return m_minDistance;
}

void
UmtsPhyLayerBS::SetAntennaGain (double value)
{
  m_antennaGain = value;
}

double
UmtsPhyLayerBS::GetAntennaGain () const
{
  return m_antennaGain;
}

void
UmtsPhyLayerBS::SetAntennaHeight (double value)
{
  m_antennaHeight = value;
}

double
UmtsPhyLayerBS::GetAntennaHeight () const
{
  return m_antennaHeight;
}

int
UmtsPhyLayerBS::GetTxState ()
{
  return m_transmitting;
}

void
UmtsPhyLayerBS::SetShadowing (double value)
{
  m_shadowing = value;
}

double
UmtsPhyLayerBS::GetShadowing () const
{
  return m_shadowing;
}

void
UmtsPhyLayerBS::SetDedicatedPower (double value)
{
  m_dedicatedTxPower = value;

  for (int j = 0; j < MAX_NUM_UE; j++)
    {
      SetDedicatedTxPower (j,m_dedicatedTxPower);
    }
}

double
UmtsPhyLayerBS::GetDedicatedPower () const
{
  return m_dedicatedTxPower;
}

double
UmtsPhyLayerBS::GetDedicatedTxPower (int phyAddress)
{
  int i = LookForUE (phyAddress);

  return m_nodeUEInformation[i].dedicatedTxPower;
}

double
UmtsPhyLayerBS::GetCommonTxPower () const
{
  return m_commonTxPower;
}

void
UmtsPhyLayerBS::SetDedicatedTxPower (int index,double value)
{
  m_nodeUEInformation[index].dedicatedTxPower = value;
}

void
UmtsPhyLayerBS::SetCommonTxPower (double value)
{
  m_commonTxPower = value;
}

void
UmtsPhyLayerBS::SetRxCallback (Callback<void,Ptr<Packet>,uint8_t > callback)
{

  m_rxCallback = callback;
}

void
UmtsPhyLayerBS::SetInterferenceCallback (Callback<void> m_interferenceCallBack_)
{
  m_interferenceCallBack = m_interferenceCallBack_;
}

void
UmtsPhyLayerBS::AlertInterferenceMeasure ()
{
  m_interferenceCallBack ();
}

/**The following Function calculates the necessary packet size for a concrete codification when decoding a packet
*/

uint32_t
UmtsPhyLayerBS::GetChannelDecodingSize (uint32_t size,uint8_t codeTag)
{
  uint32_t newSize=0;
 //std::cout<<"UmtsPhyLayerBS::GetChannelDecodingSize:PACKET SIZE!!!"<<size<<std::endl;
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

/**The following Function calculates the necessary packet size for a concrete codification when coding a packet
*/

uint32_t
UmtsPhyLayerBS::GetChannelCodingSize (uint32_t size,uint8_t codeTag)
{
  uint32_t newSize=0;
  switch (codeTag)
    {
    case TURBO:             // turbo coding
      newSize=3 *size;
      break;
    case CONV_HALF:             // convolutional half
      newSize=2 * size;
      break;
    case CONV_THIRD:              // convolutional third
      newSize=3 *size;
      break;
    default:
      break;
    }
  return newSize;
}

/** Checks the number of dpdch received
*/
int
UmtsPhyLayerBS::NumberOfDpdch (int id)
{
  int i, n = 0;
  for (i = 0 ; i < MAX_NUM_DPDCH ; i++)
    {
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > item = m_rxDpdchQueue[id][i];
      if (item.second != 0 )
        {
          n++;
        }
    }
  return (n);       // number of dpdch received and stored of a user
}

void
UmtsPhyLayerBS::ForwardUp (Ptr<Packet> packet)
{
  //std::cout<<"FORWARD UP!!!"<<std::endl;
  int i;
 ////std::cout<<"UmtsPhyLayerBS::ForwardUp Printing Attempt 1:"<<std::endl;
  packet->Print(std::cout);
  UmtsPacketTypeTag typeTag;
  NodeIdTag senderTag;
        
  packet->PeekPacketTag(typeTag);
  if(typeTag.Get()==Ack)
  {    
    m_rxCallback (packet,typeTag.Get());
  }
  else
  {
      packet->PeekPacketTag(senderTag);
      for (i = 0; i < MAX_NUM_UE; i++)
        {               // find the UE
	  
          if (m_uplinkInterference[i].phyaddr_ == senderTag.Get())
            {

              if (typeTag.Get() == DedicatedUnicastData)
                {
                 //std::cout<<"DEDICATED"<<std::endl;
                  if (m_uplinkInterference[i].DedicatedError_ == 1)
                    {
                      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY  ********** Error Sending Data Packet from UE " << senderTag.Get() << " Upwards.Time " << Simulator::Now () << "\n");

                    }
                  else
                    {
                     //std::cout<<"DEDICATED 2"<<std::endl;                      
                      m_rxCallback (packet,typeTag.Get());
                    }
                }
              else
                {
                  //std::cout<<"NOT DEDICATED"<<std::endl;
                  if (m_uplinkInterference[i].CommonError_ == 1)
                    {
                     //std::cout<<"INTERFERENCE"<<std::endl;
                      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY  ********** Error Sending Data Packet from UE " << senderTag.Get() << " Upwards.Time " << Simulator::Now ());
                    }
                  else
                    {
                      //std::cout<<"CALLBACK"<<std::endl;                      
                      m_rxCallback (packet,typeTag.Get());
                     //std::cout<<"UmtsPhyLayerBS::ForwardUp 2"<<std::endl;
                    }
                }

              return;
            }
        }
      // end of array. Store the UE


      for (i = 0; i < MAX_NUM_UE; i++)
        {

          if (m_uplinkInterference[i].phyaddr_ == (uint32_t)-1)
            {                            
              m_uplinkInterference[i].phyaddr_ = senderTag.Get() ;
              m_uplinkInterference[i].CommonError_ = 0;
              m_uplinkInterference[i].DedicatedError_ = 0;

              m_rxCallback (packet,typeTag.Get());

              break;

            }
        }
  }

}

void
UmtsPhyLayerBS::RemoveDedicatedChannel (uint32_t id)
{
  int i = LookForUE (id);  
  m_nodeUEInformation[i].channel->SetDedicatedChannelPeer (0);  
  m_nodeUEInformation[i].DedicatedPrx = 0;
  m_nodeUEInformation[i].DedicatedLastRate = 0;

}

/** Looks for a UE possition in the UE Registry array
*/

int
UmtsPhyLayerBS::LookForUE (uint32_t phyaddr)
{
  int i;
  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_nodeUEIdRegistry[i] == phyaddr)
        {
          return(i);                // internal address of the UE with physical address: phyaddr
        }
    }
  return(-1);       // UE not found
}

/**The following function is accessed when the RRC Layer sends a control packet directly to the PHY Layer
*/

void
UmtsPhyLayerBS::ReceiveFACHMessage (Ptr<Packet> packet,uint8_t packetType)
{
  if (packetType == Ack)
    {
      TxFach(packet);
    }
  else if (packetType == CommonUnicastData)
    {
      m_fachPacket = packet->Copy();
      UmtsReceiverIdTag tag;
      packet->PeekPacketTag(tag);
      
      TxFachDataPacket (m_fachPacket,packetType);
    }
  //HERE WE ADDED THE MULTICAST CASE! HOPE THAT IT'S CORRECT
  else if (packetType== BroadcastData ||packetType == MulticastData)
    {
      
      m_fachPacket = packet;
      TxFachDataPacket (m_fachPacket,packetType);      
    }
  
}

/**The following function receives data packets with the correspondent control information from the MAC Layer
*/

void
UmtsPhyLayerBS::ReceivedDataPacketFromAbove (Ptr<Packet> packet,uint8_t packetType)
{ 
 //std::cout<<"UMTSPHYLAYERBS"<<std::endl;
  if (packetType == DedicatedUnicastData)
    {
      //std::cout<<"UMTSPHYLAYERBS 2"<<std::endl;      
      DownLinkMux (packet);          // calls to the downlink multiplexing channel
    }
  else 
    {
      //std::cout<<"UMTSPHYLAYERBS 3"<<std::endl;
      ReceiveFACHMessage (packet,packetType);        
    }
}


/**This function is accessed when a packet from a channel arrives.If the packet is for me then the function passess the packet to the reception
** process function.
*/

void
UmtsPhyLayerBS::PacketArrivalFromChannel (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
      //std::cout<<"UmtsPhyLayerBS::PacketArrivalFromChannel"<<std::endl;
     // packet->Print(std::cout);
      UmtsReceiverIdTag recTag;
      packet->PeekPacketTag(recTag);
      
      UmtsPacketTypeTag typeTag;
      packet->PeekPacketTag(typeTag);
                       
      if (IsPacketForMe (controlpacket))                // checks if the user is registered in its cell
        {
          //std::cout<<"Packet for me!!!"<<std::endl;	  
	  if(typeTag.Get()==Ack||typeTag.Get()==OtherData)
	  {
	    ReceivePacketFromBelow (packet,controlpacket);
	  }
	  else
	  {	    
	    double power = pow (10.0,controlpacket->GetRxPower () / 10.0);
	    
	    if(typeTag.Get()==DedicatedUnicastData)
	    {	      
	      m_nodeUEInformation[controlpacket->GetSourceNodeIdentifier ()].DedicatedPrx = power;               // power
	      m_nodeUEInformation[controlpacket->GetSourceNodeIdentifier ()].DedicatedLastRate = controlpacket->GetTxRate ();
	      m_internalDedicatedInterference[controlpacket->GetSourceNodeIdentifier ()] += controlpacket->GetRxPower ();
	      m_numberDedicatedInternalInterference[controlpacket->GetSourceNodeIdentifier ()]++;
	    }
	    else 
	    {      
	      m_nodeUEInformation[controlpacket->GetSourceNodeIdentifier ()].CommonPrx = power;               // power
	      m_nodeUEInformation[controlpacket->GetSourceNodeIdentifier ()].CommonLastRate = controlpacket->GetTxRate ();
	      m_internalCommonInterference[controlpacket->GetSourceNodeIdentifier ()] += controlpacket->GetRxPower ();
	      m_numberCommonInternalInterference[controlpacket->GetSourceNodeIdentifier ()]++;
	    }
	    
	    ReceivePacketFromBelow(packet,controlpacket);
            //std::cout<<"End of UmtsPhyLayerBS::PacketArrivalFromChannel"<<std::endl;
	    
	  }
                    
        }
      else
        {
          NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY//Packet Arrive for me.The origin is not registered in my Cell.Time " << Simulator::Now () << "\n");
        }

}

/** Process the packets received from the Channel (air interface). The packets could be either data or control packets.
*/

void
UmtsPhyLayerBS::ReceivePacketFromBelow (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  int j, len;
  std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem;

  switch (controlpacket->GetChannel ())
    {

    case PRACH:
      // it can be a RACH preamble or a RACH message

      if (controlpacket->GetPacketType () == PT_PREAMBLE)
        {
          // RACH preamble received

          if (m_wcdmaParams.free_res_[controlpacket->GetScramblingCode ()][controlpacket->GetSignature ()] == 0)
            {              
              // store the signature used by the preamble to use it in AICH
              m_usedSignatureForUE[controlpacket->GetSourceNodeIdentifier ()] = controlpacket->GetSignature ();
              TxAich (controlpacket);                           // transmit AICH message
            }
          else
            {
              // the signature was not free
              NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY ********** Rach Preamble from node UE Received.Signature was not free.           Time " << Simulator::Now ());
              break;
            }
        }
      else
        {
          // RACH control part, we don't need it, so drop it
          if ((controlpacket->GetSpreadingFactor () == 256)
              && (controlpacket->GetChannelisationCode () == 16 * m_usedSignatureForUE[controlpacket->GetSourceNodeIdentifier ()] + 15))
            {

              break;
            }
          else
            {
//            The RACH Message Carries Data Part .Process It

              if (controlpacket->GetBeginOfSequence () == 1)
                {
                  // begin of RACH data part, check if the resources were free
                  if (m_wcdmaParams.free_res_[controlpacket->GetScramblingCode ()][0] == 0)
                    {
                      // the resources were free, allocate them
                      m_wcdmaParams.free_res_[controlpacket->GetScramblingCode ()][0] = 1;

                      if (controlpacket->GetEndOfPacket() == 1)
                        {
                          // free resources
                          m_wcdmaParams.free_res_[controlpacket->GetScramblingCode ()][0] = 0;                          
			  if(controlpacket->GetEndOfSequence()==1)
			  {
			    controlpacket->SetSourceNodeIdentifier (m_nodeUEIdRegistry[controlpacket->GetSourceNodeIdentifier ()]);                                    // change the source address
			    controlpacket->SetChannel (CCCH);
			    UpLinkDemux (packet,controlpacket);
			  }
                        }
                    }
                  else
                    {
                      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY ********** Resources were not free to source " << controlpacket->GetSource ());
                      break;
                    }

                }
              else
                {
                  // is the end of RACH message
                  if (controlpacket->GetEndOfPacket () == 1)
                    {
                      // free resources
                      m_wcdmaParams.free_res_[controlpacket->GetScramblingCode ()][0] = 0;
                      
		      if(controlpacket->GetEndOfSequence()==1)
		      {
			controlpacket->SetSourceNodeIdentifier (m_nodeUEIdRegistry[controlpacket->GetSourceNodeIdentifier ()]);                                        // change the source address
			controlpacket->SetChannel (CCCH);
			UpLinkDemux (packet,controlpacket);
		      }
                      
                    }
                }
            }
        }
      break;

    case DPCCH:

      if ((controlpacket->GetSpreadingFactor () == 256) && (controlpacket->GetChannelisationCode () == 0))
        {

          m_dpdchControlParams[controlpacket->GetSourceNodeIdentifier ()].numberOfDpdch = controlpacket->GetNumberOfDpdch ();
          len = NumberOfDpdch (controlpacket->GetSourceNodeIdentifier ());                      // check the number of dpdchs received
          // check if all dpdchs transmitted has been received
          if (len != m_dpdchControlParams[controlpacket->GetSourceNodeIdentifier ()].numberOfDpdch)
            {
              // they have not been received, drop the dpdch stored
              for (j = 0; j < len; j++)
                {                  
                  m_rxDpdchQueue[controlpacket->GetSourceNodeIdentifier ()][j].first = 0;
                  m_rxDpdchQueue[controlpacket->GetSourceNodeIdentifier ()][j].second = 0;
                }
              break;
            }
          
          // all dpdch has been received
          Decode (controlpacket->GetSourceNodeIdentifier ());                      // decode the dpdchs

        }

      break;
    case DPDCH:
      
      // store DPDCH packet in buffer in the next free possition
      len = NumberOfDpdch (controlpacket->GetSourceNodeIdentifier ());

      newItem.first = packet;
      newItem.second = controlpacket;
      m_rxDpdchQueue[controlpacket->GetSourceNodeIdentifier ()][len] = newItem;
      
      break;
    default:
      break;
    }
  //std::cout<<"End of UmtsPhyLayerBS::ReceivePacketFromBelow"<<std::endl;
  return;
}


/** Check if the packet is for this Node B
*/

int
UmtsPhyLayerBS::IsPacketForMe (Ptr<ControlPacket> controlpacket)
{
  int i;
  i = LookForUE (controlpacket->GetSourceNodeIdentifier ());       // Check if the User is already registered in the Node B

  if (i != -1)
    {
      // the user is already registered
      controlpacket->SetSourceNodeIdentifier (i);          // change the user address with the internal addressing
      return (1);
    }

  // the user is not registered
  if (controlpacket->GetInitialAccess () == 1)
    {
      // if the first access of that user: setup or handover
      for (i = 0; i < MAX_NUM_UE; i++)
        {
          // Check if there is any space free in the internal resoures to store the new user's info
          if (m_nodeUEIdRegistry[i] == (uint32_t)-1)
            {
              // register the user in the table of addresses
              m_nodeUEIdRegistry[i] = controlpacket->GetSourceNodeIdentifier ();
              controlpacket->SetSourceNodeIdentifier (i);                  // change the user address with the internal addressing              
              return (1);
            }
        }
    }
  NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY ********** FAILURE Registering UE " << controlpacket->GetSourceNodeIdentifier () << ". No Free Space in the Address Table. Time " << Simulator::Now () << " \n");
  // the user is not registered in our cell
  return (0);
}

/**---------------------------------------------Transmission of Packets through the channel----------------------------*/

/**The following Function creates and sends an AICH Message to an UE Node
*/

void
UmtsPhyLayerBS::TxAich (Ptr<ControlPacket> controlpacket)
{
  Ptr<ControlPacket> newControlpacket = Create<ControlPacket> ();
  Ptr<Packet> packet = Create<Packet> ();
  UmtsPacketTypeTag typeTag;
  typeTag.Set(OtherData);
  newControlpacket->SetDataSize (1.25);
  newControlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);
  newControlpacket->SetDestinationNodeIdentifier (m_nodeUEIdRegistry[controlpacket->GetSourceNodeIdentifier ()]);
  newControlpacket->SetScramblingCode (controlpacket->GetScramblingCode ());
  newControlpacket->SetSignature (controlpacket->GetSignature ());
  newControlpacket->SetSpreadingFactor (256);
  newControlpacket->SetChannelisationCode (controlpacket->GetChannelisationCode ());
  newControlpacket->SetSize ((int)(UMTS_TBLength / (256 * 8)));

  if ((UMTS_TBLength % (newControlpacket->GetSpreadingFactor () * 8)) != 0)
    {
      int value = newControlpacket->GetSize ();
      value++;
      newControlpacket->SetSize (value);
    }
  
  newControlpacket->SetChannel (AICH);

  packet->AddPacketTag(typeTag);
  m_aichPacketsToTx->PhyEnqueue (packet->Copy(),newControlpacket->Copy ());     // stores the packet in the AICH transmission buffer

}

void
UmtsPhyLayerBS::TxFachDataPacket (Ptr<Packet> packet,uint8_t packetType)
{
  //std::cout<<"UmtsPhyLayerBS::TxFachDataPacket!!!"<<std::endl;
  Ptr<ControlPacket> controlpacket=Create<ControlPacket>();
  
  UmtsCodeTag tag;
  packet->PeekPacketTag(tag);
    
  if(packetType==BroadcastData)
  {    
    controlpacket->SetLLType(LL_MBMS_BROADCAST);
  }
  else if(packetType==MulticastData)
  {    
    controlpacket->SetLLType(LL_MBMS_MULTICAST);
  }
  else
  {    
    controlpacket->SetLLType(LL_DATA);
  }
      
  uint32_t newSize=GetChannelCodingSize (packet->GetSize(),tag.Get());
  packet->AddPaddingAtEnd(newSize-packet->GetSize());
  
  controlpacket->SetDataSize (packet->GetSize ());      // stores the packet size
  //std::cout<<"UmtsPhyLayerBS::TxFachDataPacket!!! Before SetChannel (SCCPCH)"<<std::endl;
  controlpacket->SetChannel (SCCPCH);
 //std::cout<<"UmtsPhyLayerBS::TxFachDataPacket!!! GetChannel 1:"<<controlpacket->GetChannel()<<std::endl;
  
  UmtsReceiverIdTag rectag;
  packet->PeekPacketTag(rectag);
  
  NodeIdTag idTag;
  packet->PeekPacketTag(idTag);
      
  controlpacket->SetDestinationNodeIdentifier(rectag.Get());  
  m_commonMacDataQueue->PhyEnqueue (packet->Copy(),controlpacket->Copy());
 //std::cout<<"UmtsPhyLayerBS::TxFachDataPacket!!! GetChannel 2:"<<controlpacket->GetChannel()<<std::endl;
}

/**The following Function creates and sends an FACH Message to an UE Node
*/

void
UmtsPhyLayerBS::TxFach (Ptr<Packet> packet)
{
  Ptr<ControlPacket> controlpacket=Create<ControlPacket>();
  controlpacket->SetScramblingCode (m_wcdmaParams.p_scrambling_);           // primary scrambling code
  controlpacket->SetSpreadingFactor (256);

  controlpacket->SetSize ((int)(UMTS_TBLength / (256 * 8)));

  // calculates the size in bytes depending on the spreading factor chosen

  if ((UMTS_TBLength % (controlpacket->GetSpreadingFactor () * 8)) != 0)
    {
      int value = controlpacket->GetSize ();
      value++;
      controlpacket->SetSize (value);
    }

  controlpacket->SetChannel (SCCPCH);
  controlpacket->SetPacketType(PT_ACK);
  controlpacket->SetLLType(OTHER);
  
  UmtsReceiverIdTag rectag;
  packet->PeekPacketTag(rectag);
        
  controlpacket->SetDestinationNodeIdentifier(rectag.Get()); 

  // transmit only in one slot, more is not needed
  controlpacket->SetBeginOfSequence (1);          // begin of FACH tx
  controlpacket->SetEndOfSequence (1);          // end of FACH tx

  m_packetToTxQueue->PhyEnqueueHead (packet->Copy(),controlpacket->Copy ());             // stores the packet in the transmission buffer
  

}


void
UmtsPhyLayerBS::MakeSCCPCHPackets ()
{
  //std::cout<<"UmtsPhyLayerBS::MakeSCCPCHPackets"<<std::endl;
  int   index_c_/*,m_SequenceNumber*/;
  double temp,total_size, tbytes;

  tbytes = 0.0;
  total_size = 1;
  UmtsTxRateTag rateTag;
  

  if (m_commonMacDataQueue->GetPhySize () > 0)
    {
      index_c_ = m_wcdmaParams.p_scrambling_;            // primary scrambling code
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem;

      // Extract packets from the buffer to transmit
                 
      while ((tbytes < total_size) && (m_commonMacDataQueue->GetPhySize () > 0))
        {

          newItem = m_commonMacDataQueue->PhyDequeue ();
	  newItem.first->PeekPacketTag(rateTag);
	  newItem.second->SetTxRate (rateTag.Get());
	  
          total_size = m_bytesPerFachSlot;
          tbytes += newItem.second->GetDataSize ();
	  	  	  
          newItem.second->SetChannel (SCCPCH);
	  newItem.second->SetPacketType(Default);
	  newItem.second->SetTxType(Common);

          if (tbytes <= total_size)
            {
              // Checks if it is possible to send the whole packet in one Slot
              newItem.second->SetSequenceNumber (m_SequenceNumber);
              newItem.second->SetEndOfSequence (1);

              m_SequenceNumber = 0;
              EnqueueDataPackets (newItem.first,newItem.second, index_c_, newItem.second->GetSpreadingFactor (), newItem.second->GetSpreadingFactor () / 4);              // send packet over the air interface

            }
          else
            {
              // no space to allocate all the packet in the current Slot
              tbytes -= newItem.second->GetDataSize ();
              temp = newItem.second->GetDataSize ();
              newItem.second->SetDataSize (total_size - tbytes);
              newItem.second->SetSequenceNumber (m_SequenceNumber);
              newItem.second->SetEndOfSequence (0);

              EnqueueDataPackets (newItem.first,newItem.second, index_c_, newItem.second->GetSpreadingFactor (), newItem.second->GetSpreadingFactor () / 4);              // send packet over the air interface
              newItem.second->SetDataSize (temp - (total_size - tbytes));

              m_SequenceNumber++;
              // storing the rest of the packet in the buffer
              
              m_commonMacDataQueue->PhyEnqueueHead (newItem.first,newItem.second);
              break;
            }
        }
    }

}

/**The following Function creates and makes a  P-CCPCH for broadcasting
*/
void
UmtsPhyLayerBS::MakePCCPCH (void)
{
  int i, j;

  Ptr<ControlPacket> controlpacket = Create<ControlPacket> ();
  controlpacket->SetDataSize (1.125);
  controlpacket->SetLLType (LL_BCH_BROADCAST);
  controlpacket->SetPacketType(Default);

  // for broadcast
  for (i = 0; i < MAX_NUM_SCRAM; i++)
    {
      for (j = 0; j < MAX_NUM_SIG + 1; j++)
        {
          // broadcast the free uplink scrambling codes and signatures

          controlpacket->SetFreeResources (i,j,m_wcdmaParams.free_res_[i][j]);
        }
    }

  for (i = 0; i < MAX_NUM_RACH; i++)
    {
      // broadcasts the free RACH channels
      controlpacket->SetAvailableRach (i, m_wcdmaParams.free_rach_[i]);
    }

  controlpacket->SetSourceNodeIdentifier (m_nodeIdentifier);
  controlpacket->SetDestinationNodeIdentifier (ID_BROADCAST);
  controlpacket->SetScramblingCode (m_wcdmaParams.p_scrambling_);           // primary scrambling code
  controlpacket->SetSpreadingFactor (256);
  controlpacket->SetChannelisationCode (1);

  controlpacket->SetSize ((int)(2304 / (256 * 8)));                     // 2304 chips per slot

  // calculates the size in bytes depending on the spreading factor chosen

  if ((2304 % (controlpacket->GetSpreadingFactor () * 8)) != 0)
    {
      int value = controlpacket->GetSize ();
      value++;
      controlpacket->SetSize (value);
    }
  
  controlpacket->SetChannel (PCCPCH);
  Ptr<Packet> packet=Create<Packet>();
  m_packetToTxQueue->PhyEnqueueHead (packet,controlpacket->Copy ());

}

/**This function is accessed when we have recieved some DPDCH packets from the UE Node and we decode them (reassemble) in order to pass
** them to the higher layers
*/

void
UmtsPhyLayerBS::Decode (uint32_t id)
{
  int i;
  
  // demux DPDCH and extract packets
  for (i = 0; i < m_dpdchControlParams[id].numberOfDpdch; i++)
    {
      std::pair<Ptr<Packet>, Ptr<ControlPacket> > item = m_rxDpdchQueue[id][i];

      // go through the array extracting packets
      while (item.second->GetDpdchQueueSize () > 0)
        {
          std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem = item.second->GetDpdchPacket ();
          Ptr<Packet> packet = newItem.first;
          Ptr<ControlPacket> controlInfo = newItem.second;
          
          // Previusly packets arrived not in sequence
          if (m_nodeUEInformation[id].in_seq_ == -1)
            {
              if (controlInfo->GetSequenceNumber () == 0)
                {
                  // first physical fragment of a packet
                  if (controlInfo->GetEndOfSequence () == 1)
                    {
                      // last physical fragment of a packet                      
                      controlInfo->SetChannel (DCH);                          // ch mapping
                      UpLinkDemux (packet->Copy (),controlInfo);                         // send it to uplink demux chain
                      m_nodeUEInformation[id].in_seq_ = 0;                            // waiting for fragment 0
                    }
                  else
                    {
                      // is not the last fragment, drop it
                      m_nodeUEInformation[id].in_seq_ = 1;                            // waiting fragment number 1

                    }
                }
              else
                {
                  // is not the first fragment, do nothing
                }
            }
          else if (controlInfo->GetSequenceNumber () == m_nodeUEInformation[id].in_seq_)                 // Previous packets arrived in sequence
            {                   // is the fragment that we expected
              if (controlInfo->GetEndOfSequence () == 1)
                {
                  // is the last physical fragment, send it to MAC
                  
                  controlInfo->SetChannel (DCH);                      // ch mapping
                  UpLinkDemux (packet->Copy (),controlInfo);                     // send it to uplink demux chain
                  m_nodeUEInformation[id].in_seq_ = 0;                        // waiting for fragment 0
                }
              else
                {
                  // is not the last fragment, drop it                  
                  m_nodeUEInformation[id].in_seq_++;                        // waiting fragment number 1
                }
            }
          else
            {
              // Previous packets arrived in sequence
              m_nodeUEInformation[id].in_seq_ = -1;                    // not in sequence

            }
        }

      item.first = 0;
      item.second = 0;
      m_rxDpdchQueue[id][i] = item;
    }
  return;
}

/** Builds the pdpdchs and dpcch to transmit
*/

void
UmtsPhyLayerBS::MakeDPXCH (void)
{
  int i, uid, sf_, k_, index_c_, numberOfDpdchNeeded;
  double index, temp, sum, mod, total_size, tbytes;
  Ipv4Address ipdaddr;
  UmtsTxRateTag rateTag;
  
  
  // builds dpdchs and dpcch for each UE receiving in its cell
  for (uid = 0; uid < MAX_NUM_UE; uid++)
    {

      tbytes = 0.0;
      ipdaddr.Set ("0");
      total_size = m_nodeUEInformation[uid].bytes_slot;

      // We look if there is any data stored in the queue belonging to the correspondent UID Node UE

      if (m_dedicatedMacDataQueue[uid]->GetPhySize () > 0)
        {
          
          index_c_ = m_wcdmaParams.p_scrambling_;                // primary scrambling code
          std::pair<Ptr<Packet>, Ptr<ControlPacket> > newItem;

          if (total_size > MAX_NUM_BYTES_PER_SLOT_DL)
            {
              // It is necessary two or more dpdchs because each slot contains more bytes than the maximum amount that
              // can be carried in one DPDCH

              numberOfDpdchNeeded = (int)(total_size / MAX_NUM_BYTES_PER_SLOT_DL);
              mod = (int)(total_size) % MAX_NUM_BYTES_PER_SLOT_DL;
              if (mod > 0)
                {
                  numberOfDpdchNeeded++;
                }
              if (numberOfDpdchNeeded > MAX_NUM_DPDCH)
                {
                  numberOfDpdchNeeded = MAX_NUM_DPDCH;
                }

              // Once we know how many DPDCH we are going to neet we create and Tx them.

              for (i = 0; i < numberOfDpdchNeeded; i++)
                {
                  Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();
                  controlInfo->SetDestinationNodeIdentifier (uid);
                  controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);
                  controlInfo->SetTxType (Dedicated);		  
                  controlInfo->SetLLType (LL_DATA);		  
                  controlInfo->SetDataSize (0);
                  controlInfo->SetNumberOfDpdch (i + 1);

                  sum = 0.0;

                  // Take a look at the Data_Temp Queue and see if we have any packet stored. We are going to send
                  // the minimum number of stored packets which fill the DPDCH Frame.

                  while (m_dedicatedMacDataQueue[uid]->GetPhySize () > 0)
                    {
                      newItem = m_dedicatedMacDataQueue[uid]->PhyDequeue ();			
		      newItem.first->PeekPacketTag(rateTag);		  
		      controlInfo->SetTxRate (rateTag.Get());
		                            
                      ipdaddr = newItem.second->GetDestination ();

                      sum += newItem.second->GetDataSize ();

                      // Check if there is enough space in that DPDCH

                      if (sum <= MAX_NUM_BYTES_PER_SLOT_DL)
                        {
                          // We have space to insert a new packet inside the DPDCH

                          newItem.second->SetSequenceNumber (m_nodeUEInformation[uid].count);
                          newItem.second->SetEndOfSequence (1);
                          newItem.second->SetSourceNodeIdentifier (m_nodeIdentifier);
                          controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                          controlInfo->SetDataSize (controlInfo->GetDataSize () + newItem.second->GetDataSize ());
                          m_nodeUEInformation[uid].count = 0;
                          
                          if (sum == MAX_NUM_BYTES_PER_SLOT_DL)
                            {
                              break;
                            }
                        }
                      else
                        {
                          // No space to allocate all the packet in the current DPDCH

                          sum -= newItem.second->GetDataSize ();
                          temp = newItem.second->GetDataSize ();
                          newItem.second->SetDataSize (MAX_NUM_BYTES_PER_SLOT_DL - sum);
                          newItem.second->SetSourceNodeIdentifier (m_nodeIdentifier);
                          newItem.second->SetSequenceNumber (m_nodeUEInformation[uid].count);
                          newItem.second->SetEndOfSequence (0);

                          // sending a part of the message                          
                          controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                          controlInfo->SetDataSize (controlInfo->GetDataSize () + MAX_NUM_BYTES_PER_SLOT_DL - sum);
                          newItem.second->SetDataSize (temp - (MAX_NUM_BYTES_PER_SLOT_DL - sum));
                          m_nodeUEInformation[uid].count++;
                          // storing the rest of the packet in the buffer
                          m_dedicatedMacDataQueue[uid]->PhyEnqueueHead (newItem.first,newItem.second);
                          break;
                        }
                    }
                  sf_ = 4;                       // spreading factor
                  // calculates k_
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

                  Ptr<Packet> packet = Create<Packet> ();
                  EnqueueDataPackets (packet,controlInfo, index_c_, sf_, k_);                       // sending DPDCH
                }
            }
          else
            {
              // Only 1 DPDCH is needed so we create it

              numberOfDpdchNeeded = 1;
              // calculate the necessary spreading factor for that application
              index = total_size;
              index = (2 * 320 / total_size);
              index = (int)(log (index) / log (2));
              sf_ = 2;
              for (i = 0; i < ((int)index - 1); i++)
                {
                  sf_ *= 2;
                }
              // sf_ is the spreading factor chosen
	      	      
              Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();

              controlInfo->SetDestinationNodeIdentifier (uid);
              controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);
              controlInfo->SetTxType (Dedicated);	      
              controlInfo->SetLLType (LL_DATA);
              controlInfo->SetDataSize (0);
              controlInfo->SetNumberOfDpdch (1);

              numberOfDpdchNeeded = 1;

              // Extract packets from the buffer to transmit
              
              while ((tbytes < total_size) && (m_dedicatedMacDataQueue[uid]->GetPhySize () > 0))
                {
                  newItem = m_dedicatedMacDataQueue[uid]->PhyDequeue ();
		  newItem.first->PeekPacketTag(rateTag);
		  
		  controlInfo->SetTxRate (rateTag.Get());
                  ipdaddr = newItem.second->GetDestination ();
                  newItem.second->SetSourceNodeIdentifier (m_nodeIdentifier);
                  tbytes += newItem.second->GetDataSize ();

                  if (tbytes <= total_size)
                    {
                      // Checks if it is possible to send the whole packet
                      newItem.second->SetSequenceNumber (m_nodeUEInformation[uid].count);
                      newItem.second->SetEndOfSequence (1);
                      controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                      controlInfo->SetDataSize (controlInfo->GetDataSize () + newItem.second->GetDataSize ());
                      
                      m_nodeUEInformation[uid].count = 0;

                    }
                  else
                    {
                      // no space to allocate all the packet in the current DPDCH
                      tbytes -= newItem.second->GetDataSize ();
                      temp = newItem.second->GetDataSize ();
                      newItem.second->SetDataSize (total_size - tbytes);
                      newItem.second->SetSequenceNumber (m_nodeUEInformation[uid].count);
                      newItem.second->SetEndOfSequence (0);

                      // sending a part of the message
                      controlInfo->InsertInDpdchQueue (newItem.first,newItem.second);
                      controlInfo->SetDataSize (controlInfo->GetDataSize () + total_size - tbytes);
                      newItem.second->SetDataSize (temp - (total_size - tbytes));
                    
                      m_nodeUEInformation[uid].count++;
                      // storing the rest of the packet in the buffer
                      m_dedicatedMacDataQueue[uid]->PhyEnqueueHead (newItem.first,newItem.second);
                      break;
                    }
                }
              k_ = sf_ / 4;                  // calculates k_
              controlInfo->SetChannel (DPDCH);
              // meter paquete en la cola de tx.
              Ptr<Packet> packet = Create<Packet> ();
              EnqueueDataPackets (packet,controlInfo, index_c_, sf_, k_);                  // send packet over the air interface
            }

          // building DPCCH

          // calculate de physical rate transmitted
          Ptr<ControlPacket> controlInfo = Create<ControlPacket> ();
          controlInfo->SetDpdchRate ((2 * 2560 / (UMTS_SlotTime * sf_)) * numberOfDpdchNeeded);
          controlInfo->SetDestinationNodeIdentifier (uid);
          controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);
          controlInfo->SetChannel (DPCCH);
          controlInfo->SetDataSize (1.25);
          controlInfo->SetNumberOfDpdch (numberOfDpdchNeeded);

          Ptr<Packet> packet = Create<Packet> ();
          EnqueueDataPackets (packet,controlInfo, index_c_, 256, 0);

          continue;
        }
    }
  return;
}

/** Sends data packets to the Channel
*/

void
UmtsPhyLayerBS::EnqueueDataPackets (Ptr<Packet> packet,Ptr<ControlPacket> controlInfo, int scram_c, int sf, int k)
{
  int ue_address;
  UmtsReceiverIdTag idTag;
  packet->PeekPacketTag(idTag);
    
  // get the ue destination address
  if (controlInfo->GetTxType () == Dedicated)
    {
      ue_address = m_nodeUEIdRegistry[controlInfo->GetDestinationNodeIdentifier()];
      controlInfo->SetDestinationNodeIdentifier (ue_address);               // destination address
    }
  else
    {
      controlInfo->SetSourceNodeIdentifier (m_nodeIdentifier);          // source address
    }

  controlInfo->SetScramblingCode (scram_c);                     // scrambling code
  controlInfo->SetSpreadingFactor (sf);                         // spreading factor
  controlInfo->SetChannelisationCode (k);                       // Channelisation Code
  m_packetToTxQueue->PhyEnqueue (packet->Copy (),controlInfo->Copy ());                   // prepare pkt to tx onto air interface
  
}

/**When it's time to Tx packets this function sends the packet received as parameter to the Channel
*/

void
UmtsPhyLayerBS::SendPacketsToChannel (std::pair<Ptr<Packet>, Ptr<ControlPacket> > myPacket)
{
  //std::cout<<"SENDPACKETSTOCHANNEL!!!"<<std::endl;
  if (myPacket.second->GetChannel () == DPDCH||myPacket.second->GetChannel () == DPCCH)
    {
      int i = LookForUE (myPacket.second->GetDestinationNodeIdentifier ());     
      m_nodeUEInformation[i].channel->ReceiveUnicastFromNodeB (myPacket.second->GetDestinationNodeIdentifier (),myPacket.first,myPacket.second);
    }
  else
    {
      
      if (myPacket.second->GetLLType () == LL_MBMS_MULTICAST)
        {
          //std::cout<<"UmtsPhyLayerBS::SendPacketsToChannel: LL_MBMS_MULTICAST"<<std::endl;
	  AppIndexTag idTag;
	  myPacket.first->PeekPacketTag(idTag);          
	  m_sharedChannel->ReceiveMulticastFromNodeB (ReturnSubscribers(idTag.Get()),myPacket.first,myPacket.second);
        }
      else if(myPacket.second->GetLLType () == LL_MBMS_BROADCAST||myPacket.second->GetLLType () == LL_BCH_BROADCAST)
	{
	  m_sharedChannel->ReceiveBroadcastFromNodeB (myPacket.first,myPacket.second);
	}
      else
	{	  
 	  m_sharedChannel->ReceiveUnicastFromNodeB (myPacket.second->GetDestinationNodeIdentifier (),myPacket.first,myPacket.second);	  
	}
            
    }
}

void
UmtsPhyLayerBS::AddServiceList(std::list<uint64_t > serviceList_)
{
  for(std::list<uint64_t >::iterator iterator=serviceList_.begin();iterator!=serviceList_.end();iterator++)
  {
    UmtsServiceMapping serviceMap; 
    serviceMap.serviceId=(*iterator);
    m_serviceTable.push_back(serviceMap);
  }
}

void
UmtsPhyLayerBS::AddNewUserToService(uint64_t serviceId,uint32_t nodeIdentifier)
{
  for(ServiceTableI iterator=m_serviceTable.begin();iterator!=m_serviceTable.end();iterator++)
  {
    if((*iterator).serviceId==serviceId)
    {
      (*iterator).subscriberList.push_back(nodeIdentifier);
      break;
    }
    
  }
}

std::list<uint32_t >
UmtsPhyLayerBS::ReturnSubscribers(uint64_t serviceId)
{
  std::list<uint32_t > list;
  for(ServiceTableI iterator=m_serviceTable.begin();iterator!=m_serviceTable.end();iterator++)
  {
    if((*iterator).serviceId==serviceId)
    {
      return (*iterator).subscriberList;
      
    }
    
  }
  
  return list;
  
}


/**This function does the demux in the Uplink
*/

void
UmtsPhyLayerBS::UpLinkDemux (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //std::cout<<"!!!UpLinkDemux!!!"<<std::endl;
  //packet->Print(std::cout);  
  UmtsCodeTag codeTag;
  packet->PeekPacketTag(codeTag);
  //packet->Print(std::cout);
  uint32_t newSize=GetChannelDecodingSize(packet->GetSize(),codeTag.Get());
  //std::cout<<"REMOVED SIZE:"<<packet->GetSize()-newSize<<std::endl;
  packet->RemoveAtEnd(packet->GetSize()-newSize);
  packet->Print(std::cout);
  ForwardUp (packet);      // sends the packet to the MAC layer
  
}


/**This function stores the packets in the proper queue to send them to the correct destination afterwards
*/

void
UmtsPhyLayerBS::DownLinkMux (Ptr<Packet> packet)
{
  int i;
  
  UmtsReceiverIdTag tag;
  packet->PeekPacketTag(tag); 
  
  UmtsCodeTag codeTag;
  packet->PeekPacketTag(codeTag);
  
  UmtsTxRateTag rateTag;
  packet->PeekPacketTag(rateTag);
  
  Ptr<ControlPacket> controlInfo=Create<ControlPacket>();
  
  uint32_t newSize= GetChannelCodingSize(packet->GetSize(),codeTag.Get());      // channel coding   
  packet->AddPaddingAtEnd(newSize-packet->GetSize());
      
  controlInfo->SetDataSize (packet->GetSize ());      // stores the packet size
 
  i = LookForUE (tag.Get());
  // stores the packet in each user buffer to send them in dpdchs
  m_dedicatedMacDataQueue[i]->PhyEnqueue (packet,controlInfo);
  return;
}

/**The following function stores the bytes that can be sent in one slot for a given user
*/

void
UmtsPhyLayerBS::StoreSlotBytes (int pdest, double bytes)
{
  int j;

  j = LookForUE (pdest);
  m_nodeUEInformation[j].bytes_slot = bytes;

  return;
}

void
UmtsPhyLayerBS::StoreFachSlotBytes (double bytes)
{
  m_bytesPerFachSlot = bytes;
}

/**The following function is called to Register new UE Resources in the PHY Layer
*/

void
UmtsPhyLayerBS::RegisterUE (Ptr<ControlPacket> controlpacket)
{
  int i, j;

  if (controlpacket->GetLLType () != LL_FAILURE)
    {
      // there is enough resources for the new UE
      for (i = 0; i < MAX_NUM_UE; i++)
        {
          j = RandomVariable (UniformVariable (0,10)).GetInteger ();
          if (m_nodeUEIdRegistry[i] == controlpacket->GetDestinationNodeIdentifier ())
            {
              // gives the UE a random paging group              
              m_nodeUEInformation[i].paging_group = j;
              m_nodeUEInformation[i].phyAddress = controlpacket->GetDestinationNodeIdentifier ();

              break;
            }
        }


    }
  else
    {
      NS_LOG_DEBUG ("NodeB " << m_nodeIdentifier << " PHY **********  Not enough resources to add UE " << controlpacket->GetDestinationNodeIdentifier () << ".Time " << Simulator::Now () << " \n");
      // there is not enough resources to support the new UE
      i = LookForUE (controlpacket->GetDestinationNodeIdentifier ());
      if (i != -1)
        {
          // removes the resources already allocated for that UE
          m_nodeUEIdRegistry[i] = (uint32_t)-1;
        }
    }
  return;
}

void
UmtsPhyLayerBS::CalculateExternalInterference (double interference)
{
  m_externalInterference = interference;
}

void
UmtsPhyLayerBS::NotifyTxPowerChange (int phyAddress,int sign)
{ 
  m_nodeUEInformation[phyAddress].channel->PowerChangeNotificationFromNodeB (sign);
}

/** Called when the DownSlotTimer expires
*/

void
UmtsPhyLayerBS::DownSlotHandler ()
{NS_LOG_FUNCTION(this);
  //std::cout<<"UmtsPhyLayerBS::DownSlotHandler"<<std::endl;
  for (int i = 0; i < MAX_NUM_UE; i++)
    {
      m_nodeUEInformation[i].transmitting = 0;
    }
  Simulator::ScheduleWithContext (m_nodeIdentifier,Seconds (slot_time_),&UmtsPhyLayerBS::DownSlotHandler,this);

  int i = -1;
  MakeDPXCH ();         // DPDCH Multiplexing and DPCCH creation

  MakePCCPCH ();        // PCCPCH tx

  MakeSCCPCHPackets ();

  while (m_packetToTxQueue->GetPhySize () > 0)
    {
      // start transmission

      std::pair<Ptr<Packet>, Ptr<ControlPacket> > newPacket = m_packetToTxQueue->PhyDequeue ();
      
      if (newPacket.second->GetLLType () == LL_DATA)
        {
          i = LookForUE (newPacket.second->GetDestinationNodeIdentifier ());          
          m_nodeUEInformation[i].transmitting = 1;
	  	  
        }
        
       SendPacketsToChannel (newPacket);                 // transmit to UE
    }

  // program next slot
  m_downlinkSlotCounter++;
  if (m_downlinkSlotCounter == SLOTS_PER_FRAME)
    {
      m_downlinkSlotCounter = 0;           // ie wrap around.
    }

  return;
}

// Called when AichSlotUmtsTimer expires.
void
UmtsPhyLayerBS::AichSlotHandler ()
{NS_LOG_FUNCTION(this);
  m_aichTimer = Simulator::Schedule (Seconds (aich_slot_time_),&UmtsPhyLayerBS::AichSlotHandler,this);

  if (m_aichPacketsToTx->GetPhySize () > 0)
    {

      // start transmission
      SendPacketsToChannel (m_aichPacketsToTx->PhyDequeue ());              // transmit to UE
    }

  return;
}

// for passing results from PHY
void
UmtsPhyLayerBS::UpLinkInterference (uint32_t addr[], unsigned long error[],unsigned long error2[])
{
  int i,j;
  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (addr[i] != (uint32_t)-1)
        {
          for (j = 0; j < MAX_NUM_UE; j++)
            {
              if (addr[i] == m_uplinkInterference[j].phyaddr_)                     // UE found
                {
                  m_uplinkInterference[j].ul_common_error_ = error[i];                             // update ul_error_
                  m_uplinkInterference[j].ul_dedicated_error_ = error2[i];                  
                  break;
                }
            }
        }
      else              // UE not found
        {
          break;
        }
    }
  return;
}

// returns if the actual transport block is erroneous
int
UmtsPhyLayerBS::CheckError (int pos,int type)
{
  int n = 0;
  if (type == 0)
    {
      n = RandomVariable (UniformVariable (0,m_uplinkInterference[pos].ul_common_error_)).GetInteger ();
    }
  else
    {
      n = RandomVariable (UniformVariable (0,m_uplinkInterference[pos].ul_dedicated_error_)).GetInteger ();
    }

  if (n == 0)           // error
    {
      return(1);           // ERROR
    }
  return(0);            // not error
}

int
UmtsPhyLayerBS::GetInternalInterference (int index)
{
  return m_numberDedicatedInternalInterference[index];
}

// timer for error calculation, each m_tti
void
UmtsPhyLayerBS::ErrorCalculationTimerHandler ()
{NS_LOG_FUNCTION(this);
  int i;
  m_errorCalculationTimer = Simulator::Schedule (Seconds (m_tti),&UmtsPhyLayerBS::ErrorCalculationTimerHandler,this);    // schedule the next m_tti
  for (i = 0; i < MAX_NUM_UE; i++)      // update error_ for each UE
    {
      if (m_uplinkInterference[i].phyaddr_ != (uint32_t)-1)
        {
          m_uplinkInterference[i].CommonError_ = CheckError (i,0);
          m_uplinkInterference[i].DedicatedError_ = CheckError (i,1);
        }
    }
  return;
}

// free resources of m_uplinkInterference[]
void UmtsPhyLayerBS::RemoveResources (uint32_t addr)
{
  int i;
  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_uplinkInterference[i].phyaddr_ == addr)                // UE found, update m_uplinkInterference[]
        {
          m_uplinkInterference[i].CommonError_ = 0;
          m_uplinkInterference[i].DedicatedError_ = 0;
          m_uplinkInterference[i].phyaddr_ = (uint32_t)-1;
          m_uplinkInterference[i].ul_common_error_ = 1000000000;
          m_uplinkInterference[i].ul_dedicated_error_ = 1000000000;
          break;
        }
    }

  /**Esto es mio, mirarlo por si acaso**/
  for (i = 0; i < MAX_NUM_UE; i++)
    {
      if (m_nodeUEIdRegistry[i] == addr)
        {          
          m_nodeUEIdRegistry[i] = (uint32_t)-1;
          break;
        }

    }

  return;
}

// Called when BlerTimer expires.
// Timer for calculating the Interference
void
UmtsPhyLayerBS::BlerHandler ()
{NS_LOG_FUNCTION(this);
  double Eb_No, bler, I = 0;
  double Eb_No2 = 0;
  double internal = 0;
  int i,k, j = 0;
  int interferentes = 0;
  unsigned long error[MAX_NUM_UE];                      // for passing results to MAC
  unsigned long error2[MAX_NUM_UE];                     // for passing results to MAC
  // Restart timer for next time.
  double time=0;
  
  if(m_calculationTime==0)
  {
    time= 5 * UMTS_FrameTime;
  }
  else
  {
    time=m_calculationTime;
  }
    
  m_blerTimer = Simulator::Schedule (Seconds (time),&UmtsPhyLayerBS::BlerHandler,this);

  AlertInterferenceMeasure ();      // Measure the external interference

  for (i = 0; i < MAX_NUM_UE; i++)
    {
      interferentes = 0;
      internal = 0;

      // We check if there is any user registered in the system

      if (m_nodeUEIdRegistry[i] != (uint32_t)-1)
        {
          // We are going to check wether an user has transmit something through the common or the dedicated channel

          if (m_nodeUEInformation[i].CommonLastRate > 0||m_nodeUEInformation[i].DedicatedLastRate > 0)
            {
              for (k = 0; k < MAX_NUM_UE; k++)
                {
                  // We take into acm_SequenceNumber the interferences created by the rest of the users

                  if ((m_nodeUEInformation[k].DedicatedLastRate > 0||m_nodeUEInformation[k].CommonLastRate > 0)&& k != i )
                    {
                      interferentes++;
                      if (m_nodeUEInformation[k].DedicatedLastRate > 0)
                        {
                          internal += pow (10.0,m_internalDedicatedInterference[k] / (10.0 * m_numberDedicatedInternalInterference[k]));
                        }

                      if (m_nodeUEInformation[k].CommonLastRate > 0)
                        {
                          internal += pow (10.0,m_internalCommonInterference[k] / (10.0 * m_numberCommonInternalInterference[k]));
                        }
                    }
                }

              // The internal interference is reduced by the orthogonality factor

              internal = internal * 0.0966;

              if (m_externalInterference > 0 ||internal > 0)
                {
                  if (m_externalInterference == 0)
                    {
                      I = internal;
                    }
                  else
                    {
                      I = m_externalInterference + internal;
                    }


                  if (m_nodeUEInformation[i].CommonLastRate > 0)
                    {
                      Eb_No = (CHIP_RATE * m_nodeUEInformation[i].CommonPrx) / (m_nodeUEInformation[i].CommonLastRate * I);
                    }
                  else
                    {
                      Eb_No = 1e38;
                    }

                  if (m_nodeUEInformation[i].DedicatedLastRate > 0)
                    {

                      Eb_No2 = (CHIP_RATE * m_nodeUEInformation[i].DedicatedPrx) / (m_nodeUEInformation[i].DedicatedLastRate * I);

                    }
                  else
                    {
                      Eb_No2 = 1e38;
                    }

                  if (Eb_No2 < m_snrTarget)
                    {

                      NotifyTxPowerChange (i,1);
                    }
                  else if (Eb_No2 > m_snrTarget&&Eb_No2 != 1e38)
                    {

                      NotifyTxPowerChange (i,0);
                    }
                }
              else
                {
                  Eb_No = 1e38;
                  Eb_No2 = 1e38;
                }
            }
          else
            {
              Eb_No = 1e38;
              Eb_No2 = 1e38;
            }


          if (m_nodeUEInformation[i].CommonLastRate > 0)
            {
              bler = m_blerTable->getbler (Eb_No);                           // consult table (Eb_No)
              m_nodeUEInformation[i].ul_CommonErrorRate = (int)(1 / bler) + 1;
            }
          else
            {
              m_nodeUEInformation[i].ul_CommonErrorRate = 1000000000;
            }

          if (m_nodeUEInformation[i].DedicatedLastRate > 0)
            {
              bler = m_blerTable->getbler (Eb_No2);                          // consult table (Eb_No)
              m_nodeUEInformation[i].ul_DedicatedErrorRate = (int)(1 / bler) + 1;
            }
          else
            {
              m_nodeUEInformation[i].ul_DedicatedErrorRate = 1000000000;
            }

          error[j] = m_nodeUEInformation[i].ul_CommonErrorRate;
          error2[j] = m_nodeUEInformation[i].ul_DedicatedErrorRate;
          m_addr[j] = m_nodeUEIdRegistry[i];

          j++;
        }
    }

  for (i = 0; i < MAX_NUM_UE; i++)
    {
      m_internalCommonInterference[i] = 0;
      m_internalDedicatedInterference[i] = 0;
      m_numberCommonInternalInterference[i] = 0;
      m_numberDedicatedInternalInterference[i] = 0;
      m_nodeUEInformation[i].CommonLastRate = 0;
      m_nodeUEInformation[i].DedicatedLastRate = 0;
      m_nodeUEInformation[i].CommonPrx = 0;
      m_nodeUEInformation[i].DedicatedPrx = 0;
    }


  m_externalInterference = 0;
  // put bounds to the arrays error[] and addr[]
  error[j] = 0;
  error2[j] = 0;
  m_addr[j] = (uint32_t)-1;
  UpLinkInterference (m_addr, error,error2);      // send the results to MAC
  return;
}

} // namespace ns3
