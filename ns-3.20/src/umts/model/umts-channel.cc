/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) YEAR COPYRIGHTHOLDER
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
 * Author: Sendoa Vaz
 */

#include "umts-channel.h"
#include "umts-phy-layer-ue.h"
#include "umts-phy-layer-bs.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/net-device.h"
#include "controlpacket.h"
#include "umts-manager.h"
#include "umts-userequipment-manager.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "cost231_model.h"

NS_LOG_COMPONENT_DEFINE ("UMTSChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (UMTSChannel);

TypeId
UMTSChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UMTSChannel")
    .SetParent<Channel> ()
    .AddConstructor<UMTSChannel> ()
  ;
  return tid;
}


UMTSChannel::UMTSChannel ()
{
  m_associatedNodes = 0;  
  m_costPropagationModel = Create<Cost231Propagation> ();
  m_phyNodeUEDedicated = 0;
      
}

UMTSChannel::~UMTSChannel ()
{
  
}

void
UMTSChannel::ReceiveMulticastFromNodeB (std::list<uint32_t > destinationNodeIdentifiers, Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  // To be implemented
 //std::cout<<"UMTSChannel::ReceiveMulticastFromNodeB: "<<controlpacket->GetChannel()<<std::endl;
  //std::cout<<"UMTSChannel::ReceiveMulticastFromNodeB: "<<controlpacket->GetDestinationNodeIdentifier ()<<std::endl;
 //std::cout<<"m_phylist SIZE:"<<m_PhyList.size()<<std::endl;
  for (UMTSPhyList::const_iterator i = m_PhyList.begin (); i != m_PhyList.end (); i++)
    {
      Simulator::ScheduleWithContext (i->phyNodeUE->GetNodeIdentifier(),Seconds(0),&UMTSChannel::SendToUE, this, i->phyNodeUE, packet->Copy(),controlpacket->Copy ());
    }   
}

void
UMTSChannel::ReceiveBroadcastFromNodeB (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{  
NS_LOG_FUNCTION(this);
  for (UMTSPhyList::const_iterator i = m_PhyList.begin (); i != m_PhyList.end (); i++)
    {      
      Simulator::ScheduleWithContext (i->phyNodeUE->GetNodeIdentifier(),Seconds(0),&UMTSChannel::SendToUE, this, i->phyNodeUE, packet->Copy(),controlpacket->Copy ());
    }
}

void
UMTSChannel::ReceiveUnicastFromNodeB (uint32_t destinationPhyAddress, Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{  
NS_LOG_FUNCTION(this);
  if (controlpacket->GetChannel () == DPDCH||controlpacket->GetChannel () == DPCCH)
    {

      if (m_phyNodeUEDedicated != 0)
        {
	  Simulator::ScheduleWithContext (m_phyNodeUEDedicated->GetNodeIdentifier(),Seconds(0.00000001),&UMTSChannel::SendToUE, this, m_phyNodeUEDedicated, packet->Copy(),controlpacket->Copy ());
          
          return;
        }
      else
        {
          return;
        }
    }
    else
    {
      
      for (UMTSPhyList::const_iterator i = m_PhyList.begin (); i != m_PhyList.end (); i++)
      {
	
	if (destinationPhyAddress == i->phyAddress)
	  {
	    
	    Ptr<ControlPacket> newControlPacket = controlpacket->Copy ();	    
	    Simulator::ScheduleWithContext (i->phyNodeUE->GetNodeIdentifier(),Seconds(0.00000001),&UMTSChannel::SendToUE, this, i->phyNodeUE, packet->Copy(),controlpacket->Copy ());
	    break;
	  }
      }
    }
       
}

void
UMTSChannel::SendToUE (Ptr<UmtsPhyLayerUE> phyNodeUE,Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //std::cout<<"UMTSChannel::SENDTOUE!!!"<<std::endl;  
  controlpacket->SetRxPower (CalculateRxPowerFromNodeB (phyNodeUE,controlpacket->GetTxType ()));
 
  phyNodeUE->PacketArrivalFromChannel (packet,controlpacket);
}


double
UMTSChannel::CalculateRxPowerFromNodeB (Ptr<UmtsPhyLayerUE> phy,enum TxType type)
{
  double txpower = 0;

  if (type == Dedicated)
    {
      txpower = m_phyNodeB->GetDedicatedTxPower (phy->GetNodeIdentifier()) + m_phyNodeB->GetAntennaGain () + phy->GetAntennaGain ();
    }
  else
    {
      txpower = m_phyNodeB->GetCommonTxPower () + m_phyNodeB->GetAntennaGain () + phy->GetAntennaGain ();

    }

  double loss = ReturnCost231Loss (m_phyNodeB->GetMobility (),phy->GetMobility (),300000000 / phy->GetTxFrequency (),10,phy->GetMinDistance (),m_phyNodeB->GetAntennaHeight (),phy->GetAntennaHeight ());
  
//  //std::cout << "Channel // Calculated Path Loss From BS. Pos="<< phy->GetMobility ()->GetPosition().x <<" Loss=" << loss <<" RxPower="<< (loss + txpower) << " WrongRxPower="<< (txpower-loss) <<"\n";

  return txpower-loss;

}

void
UMTSChannel::PowerChangeNotificationFromNodeB (int sign)
{
  m_phyNodeUEDedicated->ChangeTxPower (sign);
}

double
UMTSChannel::CalculateRxPowerFromNodeUE (Ptr<UmtsPhyLayerUE> phy,enum TxType type)
{
  double txpower = 0;

  if (type == Dedicated)
    {
      txpower = phy->GetDedicatedTxPower () + phy->GetAntennaGain () + m_phyNodeB->GetAntennaGain ();
    }
  else
    {
      txpower = phy->GetCommonTxPower () + phy->GetAntennaGain () + m_phyNodeB->GetAntennaGain ();
    }

  double loss = ReturnCost231Loss (phy->GetMobility (),m_phyNodeB->GetMobility (),300000000 / phy->GetTxFrequency (),5,phy->GetMinDistance (),
                                   m_phyNodeB->GetAntennaHeight (),phy->GetAntennaHeight ());
  
  NS_LOG_DEBUG("Channel // Calculated Path Loss From Vehicle "<<phy->GetNodeIdentifier()<<"." << loss <<". Rx Power is "<<loss + txpower<<"\n");

//  //std::cout << "Channel // Calculated Path Loss From Vehicle "<<phy->GetNodeIdentifier()<<"." << loss <<". Rx Power is "<< (loss + txpower) 
// <<"\n";

//   return txpower-loss;
  return txpower+loss;

}

double
UMTSChannel::ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                                double nodeBHeight,double nodeUEHeight)
{
  m_costPropagationModel->SetLambda (lambda);
  m_costPropagationModel->SetShadowing (shadowing);
  m_costPropagationModel->SetMinDistance (minDistance);
  m_costPropagationModel->SetBSAntennaHeight (nodeBHeight);
  m_costPropagationModel->SetSSAntennaHeight (nodeUEHeight);
  double loss = m_costPropagationModel->GetLoss (a,b);

  return loss;

}

void
UMTSChannel::ReceiveFromNodeUE (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{  
  controlpacket->SetRxPower (CalculateRxPowerFromNodeUE (controlpacket->GetUmtsPhyLayerUEPointer (),controlpacket->GetTxType ()));
  Simulator::ScheduleWithContext (m_phyNodeB->GetNodeIdentifier(),Seconds(0.00000001),&UMTSChannel::SendToNodeB, this,packet,controlpacket);
}

void
UMTSChannel::SetUmtsPhyLayerBS (Ptr<UmtsPhyLayerBS> phyNodeB_)
{
  m_phyNodeB = phyNodeB_;
}

void
UMTSChannel::Add (Ptr<UmtsPhyLayerUE> phy,uint32_t phyAddress)
{
 //std::cout<<"UMTSChannel::Add"<<std::endl;
  PhyNode phyNode;
  phyNode.phyNodeUE = phy;
  phyNode.phyAddress = phyAddress;
  m_PhyList.push_back (phyNode);      
  m_associatedNodes++;
}

void
UMTSChannel::SetDedicatedChannelPeer (Ptr<UmtsPhyLayerUE> phy)
{
  m_phyNodeUEDedicated = phy;

  if (m_phyNodeUEDedicated != 0)
    {
      m_phyNodeUEDedicated->SetDedicatedChannel (this);
    }
}

void
UMTSChannel::RemoveNode (Ptr<UmtsPhyLayerUE> phyNodeUE)
{
  for (UMTSPhyList::iterator i = m_PhyList.begin (); i != m_PhyList.end (); i++)
    {
      if (phyNodeUE == (*i).phyNodeUE)
        {
          m_PhyList.erase (i);
          m_associatedNodes--;
        }
    }
}

Ptr<UMTSChannel>
UMTSChannel::GetChannel ()
{
  return this;
}


void
UMTSChannel::SendToNodeB (Ptr<Packet> packet,Ptr<ControlPacket> controlpacket)
{
  //packet->Print(std::cout);
  m_phyNodeB->PacketArrivalFromChannel (packet,controlpacket);

}


} // namespace ns3
