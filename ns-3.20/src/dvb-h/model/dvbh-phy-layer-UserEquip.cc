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


#include "dvbh-phy-layer-UserEquip.h"
#include "dvbh-manager.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "dvbh-queue.h"
#include "ns3/simulator.h"
#include "dvbh-ofdm-layer.h"
#include "ns3/mobility-model.h"
#include "ts-header.h"
#include "pes-header.h"
#include "mpe-header.h"
#include "dvbh-tags.h"
#include <list>

#include "ns3/node-id-tag.h"

NS_LOG_COMPONENT_DEFINE ("DVBHPhyLayerUserEquip");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHPhyLayerUserEquip);

TypeId
DVBHPhyLayerUserEquip::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHPhyLayerUserEquip")
    .SetParent<Object> ()

  ;
  return tid;
}

DVBHPhyLayerUserEquip::DVBHPhyLayerUserEquip ()
{
  m_PESQueue = Create<DVBHQueue> ();
  m_switchedOff = false;
  m_mpeFecEnabled = false;
  m_multipleServices = false;

}

void
DVBHPhyLayerUserEquip::Sleep (Time sleepTime)
{
NS_LOG_FUNCTION(this);
  if (m_multipleServices == false)
    {
      m_switchedOff = true;      
      double tiempo = sleepTime.GetSeconds () * 0.95;
      Time time = Seconds (tiempo);
      Simulator::Schedule (time,&DVBHPhyLayerUserEquip::WakeUp,this);

    }
     RebuildStreams ();
}

void
DVBHPhyLayerUserEquip::WakeUp ()
{
  int empty = 1;
  if (!m_tsQueueList.empty ())
    {
      for (TsQueueListI i = m_tsQueueList.begin (); i != m_tsQueueList.end (); i++)
        {
          if ((*i).first->GetSize () == 0)
            {
              empty = 1;
            }
          else
            {

              empty = 0;
              break;
            }
        }

      if (empty)
        {          
          Simulator::Schedule (MilliSeconds (500),&DVBHPhyLayerUserEquip::WakeUp,this);
        }
      else
        {
          m_switchedOff = false;     
          SendWakeUp ();
        }

    }
  else
    {      
      Simulator::Schedule (MilliSeconds (500),&DVBHPhyLayerUserEquip::WakeUp,this);
    }


}

void
DVBHPhyLayerUserEquip::SetWakeUpCallback (Callback<void> upCallback)
{
  m_wakeUp = upCallback;
}

void
DVBHPhyLayerUserEquip::SendWakeUp ()
{
  m_wakeUp ();
}

void
DVBHPhyLayerUserEquip::DoDispose ()
{

}

void
DVBHPhyLayerUserEquip::EnableMpeFec ()
{
  m_mpeFecEnabled = true;

}

void
DVBHPhyLayerUserEquip::DisableMpeFec ()
{
  m_mpeFecEnabled = false;

}

void
DVBHPhyLayerUserEquip::SetNodeIdentifier(uint32_t nodeIdentifier)
{
 m_nodeIdentifier=nodeIdentifier; 
 m_ofdmLayer->SetNodeIdentifier(nodeIdentifier);
}

uint32_t
DVBHPhyLayerUserEquip::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHPhyLayerUserEquip::SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHPhyLayerUserEquip::ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  m_forwardUp (packet,controlpacket);
}

void
DVBHPhyLayerUserEquip::SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_)
{
  m_ofdmLayer = ofdm_;
  m_ofdmLayer->SetForwardUpCallback (MakeCallback (&DVBHPhyLayerUserEquip::ReceiveFromBelow,this));
  m_ofdmLayer->SetForwardUpPsiInfoCallback (MakeCallback (&DVBHPhyLayerUserEquip::ReceivePsiInfoFromBelow,this));
  
}

void
DVBHPhyLayerUserEquip::ReceiveFromBelow (Ptr<Packet> packet,bool sleep )
{
  TsHeader tsHeader;
  packet->PeekHeader (tsHeader);
  
  NodeIdTag idTag;
  packet->PeekPacketTag(idTag);
  
             
  if (CheckTsPacketPid (tsHeader.GetPid ()) == 2)
    {      
      DemultiplexStream (packet,tsHeader.GetPid ());

      if (sleep )
      {	
        Sleep (MilliSeconds (1000));
      }
      else if (m_switchedOff == true)
        {
          WakeUp ();
        }
    }


}

void
DVBHPhyLayerUserEquip::ReceivePsiInfoFromBelow (struct TSPacket packet,bool sleep )
{

  int value = CheckTsPacketPid (packet.id);
  if (value == 0)
    {
      packet.controlpacket->SetPacketType (patTablePacket);
      ForwardUp (packet.packet,packet.controlpacket);
    }
  else if (value == 1)
    {
      packet.controlpacket->SetPacketType (pmtTablePacket);
      ForwardUp (packet.packet,packet.controlpacket);
    }
  else if (value == 2)
    {
//          packet.controlpacket->SetPacketType(mpePacket);
//          DemultiplexStream(packet);
//
//          if(sleep)
//          {
//            Sleep(MilliSeconds(1000));
//          }
//          else if(switchedOff==true)
//          {
//              WakeUp();
//          }

//          if(switchedOff)
//          {
//              //std::cout<<"DVBH UserEquip "<<m_nodeIdentifier<<" PHY// Packet received on Switched OFF State. PID "<<packet.id<<" counter "<<packet.counter<<".Time "<<Simulator::Now()<<" \n"<<std::endl;
//          }
//          else
//          {
//              packet.controlpacket->SetPacketType(mpePacket);
//              DemultiplexStream(packet);
//          }
    }
  else if (value == 3)
    {
      packet.controlpacket->SetPacketType (sdtTablePacket);
      ForwardUp (packet.packet,packet.controlpacket);
    }

}

void
DVBHPhyLayerUserEquip::CreateNewTSQueue (uint32_t streamId)
{
  Ptr<DVBHQueue> streamTsQueue = Create<DVBHQueue> ();
  m_tsQueueList.push_back (std::make_pair (streamTsQueue,streamId));

}

void
DVBHPhyLayerUserEquip::DemultiplexStream (Ptr<Packet> packet,uint32_t serviceId)
{
  if (!m_tsQueueList.empty ())
    {
      for (TsQueueListI i = m_tsQueueList.begin (); i != m_tsQueueList.end (); i++)
        {

          if ((*i).second == serviceId)
            {
              TsHeader tsHeader;
              packet->PeekHeader (tsHeader);              
              (*i).first->Enqueue (packet->Copy ());
            }
        }
    }
}

void
DVBHPhyLayerUserEquip::RebuildStreams ()
{
  if (!m_tsQueueList.empty ())
    {
      for (TsQueueListI i = m_tsQueueList.begin (); i != m_tsQueueList.end (); i++)
        {
          CreatePESPackets ((*i).first);
        }
    }

  Simulator::Schedule (Seconds (3),&DVBHPhyLayerUserEquip::RebuildStreams,this);
}

void
DVBHPhyLayerUserEquip::CreatePESPackets (Ptr<DVBHQueue> streamTsQueue)
{
  Ptr<Packet> previousPesPacket = Create<Packet> ();
  PesHeader pesHeader;
  pesHeader.SetSid (-1);
  pesHeader.SetPacketLength (0);

  while (streamTsQueue->GetSize () != 0)
    {
      Ptr<Packet> packet = streamTsQueue->Dequeue ();
      TsHeader tsHeader;
      packet->RemoveHeader (tsHeader);
     

      if (tsHeader.GetPayloadUnitStart () == 1) // Case of being the first fragment of a PES packet
        {
          if ((int)pesHeader.GetSid () == -1) // There is not a previous Pes Packet waiting to be Forwarded UP
            {
              if (streamTsQueue->GetSize () == 0) // Case of being the first and only fragment of a Pes Packet
                {
                  pesHeader.SetSid (tsHeader.GetPid ());
                  pesHeader.SetPacketLength (pesHeader.GetPacketLength () + packet->GetSize ());
                  previousPesPacket->AddAtEnd (packet);                  
		  
		  PacketTagIterator i = packet->GetPacketTagIterator ();
	      
		  while (i.HasNext ())
		    {
		      PacketTagIterator::Item item = i.Next ();
		      NS_ASSERT (item.GetTypeId ().HasConstructor ());
		      Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
		      NS_ASSERT (!constructor.IsNull ());
		      ObjectBase *instance = constructor ();
		      Tag *tag = dynamic_cast<Tag *> (instance);
		      NS_ASSERT (tag != 0);
		      item.GetTag (*tag);
		      
		      if(!previousPesPacket->PeekPacketTag(*tag))
		      {
			previousPesPacket->AddPacketTag(*tag);
		      }
			      
		    }
		  
                  m_PESQueue->Enqueue (previousPesPacket);                  

                  pesHeader.SetSid (-1);
                }
              else      // Caseof being the first fragment but not the last
                {
                  pesHeader.SetSid (tsHeader.GetPid ());
                  pesHeader.SetPacketLength (packet->GetSize ());
                  previousPesPacket->AddAtEnd (packet);                  
                }
            }
          else          // There is a previous PES packet waiting to be Forwarded UP
            {
              // First we are going to send UP the packet waiting for being Forwarded UP

	      PacketTagIterator i = packet->GetPacketTagIterator ();
	      
	      while (i.HasNext ())
		{
		  PacketTagIterator::Item item = i.Next ();
		  NS_ASSERT (item.GetTypeId ().HasConstructor ());
		  Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
		  NS_ASSERT (!constructor.IsNull ());
		  ObjectBase *instance = constructor ();
		  Tag *tag = dynamic_cast<Tag *> (instance);
		  NS_ASSERT (tag != 0);
		  item.GetTag (*tag);
		  
		  if(!previousPesPacket->PeekPacketTag(*tag))
		  {
		    previousPesPacket->AddPacketTag(*tag);
		  }
			  
		}
	      
              m_PESQueue->Enqueue (previousPesPacket);

              Ptr<Packet> pesPacket = Create<Packet> ();
              pesPacket->AddAtEnd (packet);
              pesHeader.SetPacketLength (packet->GetSize ());
              pesHeader.SetSid (tsHeader.GetPid ());

              if (streamTsQueue->GetSize () == 0) // Case of being the first and only fragment of a Pes Packet
                {                  
		   PacketTagIterator i = packet->GetPacketTagIterator ();
	      
		  while (i.HasNext ())
		    {
		      PacketTagIterator::Item item = i.Next ();
		      NS_ASSERT (item.GetTypeId ().HasConstructor ());
		      Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
		      NS_ASSERT (!constructor.IsNull ());
		      ObjectBase *instance = constructor ();
		      Tag *tag = dynamic_cast<Tag *> (instance);
		      NS_ASSERT (tag != 0);
		      item.GetTag (*tag);
		      
		      if(!pesPacket->PeekPacketTag(*tag))
		      {
			pesPacket->AddPacketTag(*tag);
		      }
			      
		    }
                  m_PESQueue->Enqueue (pesPacket);

                  pesHeader.SetSid (-1);
                }
              else      // Caseof being the first fragment but not the last
                {
                  previousPesPacket = pesPacket;
                }
            }

        }
      else      // Case of being one fragment distinct from the first fragment of a PES Packet
        {
          if (streamTsQueue->GetSize () == 0) // Case of being the last fragment of a Pes Packet
            {


              previousPesPacket->AddAtEnd (packet);
              pesHeader.SetPacketLength (pesHeader.GetPacketLength () + packet->GetSize ());
              
              NS_LOG_INFO("DVBH UserEquip " << m_nodeIdentifier << " PHY//Fragment Arrived, It is the last: PID " << tsHeader.GetPid () << ",payloadStart " << tsHeader.GetPayloadUnitStart () << " with count " << (uint32_t)tsHeader.GetCounter () << " and size " << packet->GetSize () << " Forming PES " << previousPesPacket->GetSize () << ".Time " << Simulator::Now ());
              	      	       
	      PacketTagIterator i = packet->GetPacketTagIterator ();
	      
	      while (i.HasNext ())
		{
		  PacketTagIterator::Item item = i.Next ();
		  NS_ASSERT (item.GetTypeId ().HasConstructor ());
		  Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor ();
		  NS_ASSERT (!constructor.IsNull ());
		  ObjectBase *instance = constructor ();
		  Tag *tag = dynamic_cast<Tag *> (instance);
		  NS_ASSERT (tag != 0);
		  item.GetTag (*tag);
		  
		  if(!previousPesPacket->PeekPacketTag(*tag))
		  {
		    previousPesPacket->AddPacketTag(*tag);
		  }
			  
		}
	      
              m_PESQueue->Enqueue (previousPesPacket);


            }
          else
            {
              previousPesPacket->AddAtEnd (packet);
              pesHeader.SetPacketLength (pesHeader.GetPacketLength () + packet->GetSize ());
              
            }
        }
    }

  if (m_PESQueue->GetSize ())
    {
      ExtractMPEPackets ();
    }

}

void
DVBHPhyLayerUserEquip::ExtractMPEPackets ()
{
  Ptr<Packet> usingMpePacket = Create<Packet> ();
  usingMpePacket = 0;

  while (m_PESQueue->GetSize ())
    {
            
      Ptr<Packet> packet = m_PESQueue->Dequeue ();
      MpeHeader mpeHeader;
      PesHeader pesHeader;
      packet->RemoveHeader (pesHeader);

      DvbhServiceIdTag tag;
      tag.Set (pesHeader.GetSid ());
            
      uint8_t *buffer = new uint8_t [PES_MAX_SZ];
      packet->CopyData (buffer,packet->GetSize ());

      uint32_t iterator = 0;

      uint32_t size = 0;
            

      while (packet->GetSize ())
        {
          Ptr<Packet> mpePacket = Create<Packet> ();
          size = buffer[iterator + 2];
	  	  
          size = (size | (buffer[iterator + 1] << 8)) & 4095;	  	  

          mpePacket = packet->CreateFragment (0,size + 12);
          packet->RemoveAtStart (size + 12);
          iterator = size + 12;
          
          mpePacket->AddPacketTag (tag);	  
          ForwardUp (mpePacket->Copy (),0);
        }

    }
}



int
DVBHPhyLayerUserEquip::CheckTsPacketPid (int pid)
{
  if (pid == 0x0000)
    {
      return 0;
    }
  if (pid == 0x0011)
    {
      return 3;
    }

  if (!m_desiredPmtList.empty ())
    {
      for (std::list< int >::iterator i = m_desiredPmtList.begin (); i != m_desiredPmtList.end (); i++)
        {
          if ((*i) == pid)
            {
              return 1;
            }
        }
    }

  if (!m_desiredStreamList.empty ())
    {
      for (std::list< int >::iterator i = m_desiredStreamList.begin (); i != m_desiredStreamList.end (); i++)
        {	  
          if ((*i) == pid)
            {
              return 2;
            }
        }
    }

  return -1;

}

void
DVBHPhyLayerUserEquip::AddPidToDesiredList (int value,int pid)
{
  int repeated = 0;

  if (value == 0)
    {

      for (std::list<int >::iterator i = m_desiredPmtList.begin (); i != m_desiredPmtList.end (); i++)
        {
          if ((*i) == pid)
            {
              repeated = 1;
            }
        }
      if (repeated == 0)
        {
          NS_LOG_INFO("DVBH UserEquip " << m_nodeIdentifier << " Phy// Adding desired service with Id " << pid << ".Time " << Simulator::Now ());
          m_desiredPmtList.push_back (pid);
        }

    }
  else
    {
      for (std::list<int >::iterator i = m_desiredStreamList.begin (); i != m_desiredStreamList.end (); i++)
        {
          if ((*i) == pid)
            {
              repeated = 1;
            }
        }
      if (repeated == 0)
        {
          NS_LOG_INFO("DVBH UserEquip " << m_nodeIdentifier << " Phy//PMT Adding desired stream with Id " << pid << ".Time " << Simulator::Now ());
          m_desiredStreamList.push_back (pid);
          CreateNewTSQueue (pid);
          if (m_desiredStreamList.size () > 1)
            {
              m_multipleServices = true;
            }
          else
            {
              m_multipleServices = false;
            }
        }
    }

}

void
DVBHPhyLayerUserEquip::RemovePidFromDesiredList (int value,int pid)
{
  if (value == 0)
    {
      for (std::list< int >::iterator i = m_desiredPmtList.begin (); i != m_desiredPmtList.end (); i++)
        {
          if ((*i) == pid)
            {
              m_desiredPmtList.erase (i);
            }
        }
    }
  else
    {
      for (std::list< int >::iterator i = m_desiredStreamList.begin (); i != m_desiredStreamList.end (); i++)
        {
          if ((*i) == pid)
            {
              m_desiredStreamList.erase (i);
            }
        }
    }
}

}
