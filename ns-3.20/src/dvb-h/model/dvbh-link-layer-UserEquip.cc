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


#include "dvbh-link-layer-UserEquip.h"
#include "dvbh-manager.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "dvbh-phy-layer-UserEquip.h"
#include "dvbh-controlpacket.h"
#include "ns3/nstime.h"
#include "mpe-header.h"
#include "dvbh-tags.h"
#include <list>

NS_LOG_COMPONENT_DEFINE ("DVBHLinkLayerUserEquip");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHLinkLayerUserEquip);


DVBHLinkLayerUserEquip::DVBHLinkLayerUserEquip ()
{
  m_mpeFecEnabled = false;
  m_fecProcessTime = Seconds (0.000001);
  m_nodeIdentifier = 0;
}

TypeId
DVBHLinkLayerUserEquip::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHLinkLayerUserEquip")
    .SetParent<Object> ()

  ;
  return tid;
}

void
DVBHLinkLayerUserEquip::DoDispose ()
{

}

void
DVBHLinkLayerUserEquip::SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHLinkLayerUserEquip::ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  m_forwardUp (packet,controlpacket);
}

void
DVBHLinkLayerUserEquip::SetScanningCallback (Callback<void,bool> scanningCallback)
{
  m_scanning = scanningCallback;
}

void DVBHLinkLayerUserEquip::InformAboutScan (bool switchedOn)
{
  m_scanning (switchedOn);
}

void
DVBHLinkLayerUserEquip::SetMacAddress (Address address)
{
  m_myMacAddress = address;
}

Address
DVBHLinkLayerUserEquip::GetMacAddress ()
{
  return m_myMacAddress;
}

void
DVBHLinkLayerUserEquip::SetFecProcessTime (Time time)
{
  m_fecProcessTime = time;
}

Time
DVBHLinkLayerUserEquip::GetFecProcessTime ()
{
  return m_fecProcessTime;
}


void
DVBHLinkLayerUserEquip::SetPhyLayer (Ptr<DVBHPhyLayerUserEquip> phyLayer_)
{
  m_phyLayer = phyLayer_;
  m_phyLayer->SetForwardUpCallback (MakeCallback (&DVBHLinkLayerUserEquip::ReceiveFromBelow,this));
  m_phyLayer->SetWakeUpCallback (MakeCallback (&DVBHLinkLayerUserEquip::ReceiverAwake,this));  
}

void
DVBHLinkLayerUserEquip::SetNodeIdentifier (uint32_t nodeIdentifier)
{
  m_nodeIdentifier = nodeIdentifier;
  m_phyLayer->SetNodeIdentifier (nodeIdentifier);
}

uint32_t
DVBHLinkLayerUserEquip::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHLinkLayerUserEquip::ReceiveFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{

  if (controlpacket == 0) // Case of Receiving a Data Packet
    {
      ProcessMPEPacket (packet,controlpacket);
    }
  else
    {      
      ForwardUp (packet,controlpacket);
    }

}

void
DVBHLinkLayerUserEquip::AddPidToDesiredList (int value,int id)
{
  m_phyLayer->AddPidToDesiredList (value,id);
}

void
DVBHLinkLayerUserEquip::EnableMpeFec ()
{
  m_mpeFecEnabled = true;
  m_phyLayer->EnableMpeFec ();
}

void
DVBHLinkLayerUserEquip::DisableMpeFec ()
{
  m_mpeFecEnabled = false;
  m_phyLayer->DisableMpeFec ();
}

void
DVBHLinkLayerUserEquip::ProcessMPEPacket (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  if (m_mpeFecEnabled == false)
    {
      MpeHeader mpeHeader;
      packet->PeekHeader (mpeHeader);
      RxPowerTag pwTag;
      packet->PeekPacketTag(pwTag);

      NS_LOG_INFO("Node UE LINK Layer // MPE Packet Received with size " << packet->GetSize () << " Count " << (uint32_t)mpeHeader.GetSectionNumber () << " Last " 
      << (uint32_t)mpeHeader.GetLastSectionNumber ()<<" POWER "<<pwTag.Get());
      if (mpeHeader.GetSectionNumber () == mpeHeader.GetLastSectionNumber ()&&mpeHeader.GetTableId () == 0x3E)
        {
          // phyLayer->Sleep(controlpacket->GetMPEPacket().delta_t);
          InformAboutScan (true);

        }


      ForwardUp (packet,controlpacket);

    }
  else	//Case of enabling MPE-FEC
    {
//       if(controlpacket->GetMPEPacket().tableId==0xE3)
//       {
//        MyPacket myPacket;
//        myPacket.controlpacket=controlpacket;
//        myPacket.packet=packet;
//        myPacketList.push_back(myPacket);
//       }
//       if(controlpacket->GetMPEPacket().currentSection==controlpacket->GetMPEPacket().lastSection&&controlpacket->GetMPEPacket().tableId==0x78)
//       {
//       // phyLayer->Sleep(controlpacket->GetMPEPacket().delta_t);
//        Simulator::Schedule(fecProcessTime,&DVBHLinkLayerUserEquip::ProcessFecStream,this);
//       }

    }
}

void
DVBHLinkLayerUserEquip::ProcessFecStream ()
{
  if (!m_myPacketList.empty ())
    {
      for (MyPacketListI i = m_myPacketList.begin (); i != m_myPacketList.end (); i++)
        {
          ForwardUp ((*i).packet,(*i).controlpacket);
        }

      m_myPacketList.clear ();
    }

}

void
DVBHLinkLayerUserEquip::ReceiverAwake ()
{
  InformAboutScan (false);
}




}
