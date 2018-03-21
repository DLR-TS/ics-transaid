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


#include "dvbh-phy-layer-BaseStation.h"
#include "dvbh-manager.h"
#include "ns3/log.h"
#include "dvbh-queue.h"
#include "ns3/event-id.h"
#include "dvbh-ofdm-layer.h"
#include "ns3/simulator.h"
#include "dvbh-tags.h"
#include "dvbh-controlpacket.h"
#include "pes-header.h"
#include "mpe-header.h"
#include "ts-header.h"
#include "ns3/mobility-model.h"
#include <list>

#include "ns3/node-id-tag.h"

NS_LOG_COMPONENT_DEFINE ("DVBHPhyLayerBaseStation");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHPhyLayerBaseStation);

TypeId
DVBHPhyLayerBaseStation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHPhyLayerBaseStation")
    .SetParent<Object> ()

  ;
  return tid;
}

DVBHPhyLayerBaseStation::DVBHPhyLayerBaseStation ()
{
  m_PESQueue = Create<DVBHQueue> ();
  m_TSQueue = Create<DVBHQueue> ();


  m_transmissionTime = Seconds (0.0001);

  m_usingPesPacket = Create<Packet> ();  
  m_fragmentCount = 0;
  m_offTime = Seconds (0);
  m_mpeFecEnabled = false;
}

void
DVBHPhyLayerBaseStation::DoDispose ()
{

}

void
DVBHPhyLayerBaseStation::SetNodeIdentifier(uint32_t nodeIdentifier)
{
 m_nodeIdentifier=nodeIdentifier; 
 m_ofdmLayer->SetNodeIdentifier(nodeIdentifier);
}

uint32_t
DVBHPhyLayerBaseStation::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHPhyLayerBaseStation::SetOffTime (Time time)
{
  m_offTime = time;
}

Time
DVBHPhyLayerBaseStation::GetOffTime ()
{
  return m_offTime;
}

void
DVBHPhyLayerBaseStation::EnableMpeFec ()
{
  m_mpeFecEnabled = true;

}

void
DVBHPhyLayerBaseStation::DisableMpeFec ()
{
  m_mpeFecEnabled = false;

}

void
DVBHPhyLayerBaseStation::SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHPhyLayerBaseStation::ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  m_forwardUp (packet,controlpacket);
}

void
DVBHPhyLayerBaseStation::SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdmLayer_)
{
  m_ofdmLayer = ofdmLayer_;
  
}

void
DVBHPhyLayerBaseStation::ReceiveFromAbove (Ptr<Packet> packet)
{
  int value = 0;

  PesHeader header;
  header.SetSid (-1);

  MpeHeader mpeHeader;
  packet->PeekHeader (mpeHeader);

  DvbhServiceIdTag tag;
  packet->RemovePacketTag (tag);  
  value = CreatePESPacket (packet,m_usingPesPacket,header,tag.Get ());
    
  if (value == 1)
    {
      SplitPESPackets ();
      m_usingPesPacket = 0;

      DownStreamHandler (tag.Get ());
    }
}

/**The following function creates a PES Packet introducing MPE Packets within it.
**/

int
DVBHPhyLayerBaseStation::CreatePESPacket (Ptr<Packet> mpePacket,Ptr<Packet> pesPacket,PesHeader pesHeader,uint32_t serviceId)
{

  MpeHeader mpeHeader;
  mpePacket->PeekHeader (mpeHeader);

  if (pesPacket != 0&&pesPacket->GetSize () != 0 && (PES_MAX_SZ - pesPacket->GetSize ()) > mpePacket->GetSize ()) // We introduce the new MPE packet in a previously created PES Packet
    {
      
      pesHeader.SetPacketLength (pesPacket->GetSize () + mpePacket->GetSize ());
      pesHeader.SetSid (serviceId);
      pesPacket->AddAtEnd (mpePacket);

      if (m_mpeFecEnabled == false)
        {
          if (mpeHeader.GetSectionNumber () == mpeHeader.GetLastSectionNumber ()&&mpeHeader.GetTableId () == 0x3E)      // We have arrived to the last section to send
            {
              pesPacket->AddHeader (pesHeader);
	      
	      PacketTagIterator i = pesPacket->GetPacketTagIterator ();
	      
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
		  
	      
              m_PESQueue->Enqueue (pesPacket->Copy ());

              NS_LOG_INFO("BaseStation " << m_nodeIdentifier << " PHY//Last Section of MPE Data. PES packet's size is " << pesPacket->GetSize () << " and PID " << pesHeader.GetSid () << ".Time " << Simulator::Now ());
              pesPacket = 0;
              return 1;
            }
          else
            {
              m_usingPesPacket = pesPacket;
            }

        }
      else
        {
//              if(mpePacket.currentSection==mpePacket.lastSection&&mpePacket.tableId==0x78)	//We have arrived to the last section to send
//              {
//
//                  PESQueue->EnqueuePES(packet);
//                  usingPESPacket.size=0;
//                  usingPESPacket.freeSize=0;
//                  usingPESPacket.id=-1;
//                  usingPESPacket.sizeToSend=0;
//                  usingPESPacket.fragmentList.clear();
//                 //std::cout<<"BaseStation "<<m_nodeIdentifier<<" PHY//Last Section of the RS.Time "<<Simulator::Now()<<" \n"<<std::endl;
//                  return 1;
//              }
//              else
//                  usingPESPacket=packet;
        }


      return 0;
    }
  else if (pesPacket != 0&&pesPacket->GetSize () != 0 && (PES_MAX_SZ - pesPacket->GetSize ()) < mpePacket->GetSize ())  // There is one PES created but the size is not enough
    {

      NS_LOG_INFO("BaseStation " << m_nodeIdentifier << " PHY//PES with not the enough size PID " << pesHeader.GetSid () << ".Time " << Simulator::Now ());

      pesHeader.SetPacketLength (pesPacket->GetSize ());
      pesHeader.SetSid (serviceId);
      pesPacket->AddHeader (pesHeader);
      
      PacketTagIterator i = pesPacket->GetPacketTagIterator ();
	      
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

      Ptr<Packet> newPesPacket = Create<Packet> ();
      newPesPacket->AddAtEnd (mpePacket);
      m_usingPesPacket = newPesPacket;

      return 0;

    }
  else          // There is not a PES Packet created. Creating a new one
    {
      NS_LOG_INFO("BaseStation " << m_nodeIdentifier << " PHY// Creating a new PES Packet with section " << (uint32_t)mpeHeader.GetSectionNumber () << " ID " << serviceId << ".Time " << Simulator::Now ());
      NS_LOG_INFO("BaseStation " << m_nodeIdentifier << " PHY// Adding MPE Packet of size "<<mpePacket->GetSize()<<" to new PES Packet with .Time " << Simulator::Now ());
      Ptr<Packet> newPesPacket = Create<Packet> ();
      PesHeader pesHeader;

      pesHeader.SetSid (serviceId);
      pesHeader.SetPacketLength (0);

      newPesPacket->AddAtEnd (mpePacket);

      if (m_mpeFecEnabled == false)
        {
          if (mpeHeader.GetSectionNumber () == mpeHeader.GetLastSectionNumber ()&&mpeHeader.GetTableId () == 0x3E)      // We have arrived to the last section to send
            {

               NS_LOG_INFO("BaseStation "<<m_nodeIdentifier<<" PHY// Creating a new PES Packet. Last Section arrived. PES packet's size is "<<newPesPacket->GetSize()<<" SID "<<serviceId<<".Time "<<Simulator::Now());

              newPesPacket->AddHeader (pesHeader);
	      
	       PacketTagIterator i = mpePacket->GetPacketTagIterator ();
	      
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
		  
		  if(!newPesPacket->PeekPacketTag(*tag))
		  {
		    newPesPacket->AddPacketTag(*tag);
		  }
			  
		}
	      
              m_PESQueue->Enqueue (newPesPacket->Copy ());

              newPesPacket = 0;

              return 1;

            }
          else
            {
              m_usingPesPacket = newPesPacket;
              return 0;
            }

        }
      else
        {
//              if(mpePacket.currentSection==mpePacket.lastSection&&mpePacket.tableId==0x78)	//We have arrived to the last section to send
//              {
//
//                  //std::cout<<"BaseStation "<<m_nodeIdentifier<<" PHY// Creating a new PES Packet. Last Section arrived.Time "<<Simulator::Now()<<" \n"<<std::endl;
//                  PESQueue->EnqueuePES(newPacket);
//                  usingPESPacket.size=0;
//                  usingPESPacket.freeSize=0;
//                  usingPESPacket.id=-1;
//
//                  usingPESPacket.fragmentList.clear();
//                  usingPESPacket.sizeToSend=0;
//                  return 1;
//
//              }
//              else
//              {
//                usingPESPacket=newPacket;
//                return 0;
//              }

          return 2;
        }
    }

}

void
DVBHPhyLayerBaseStation::SplitPESPackets ()
{
  int value = 1;
  int count = 0;
  int first = 1;
  //std::cout<<"BaseStation "<<m_nodeIdentifier<<" PHY// Going to split packets size "<<m_PESQueue->GetSize()<<".Time "<<Simulator::Now()<<" \n"<<std::endl;

  while (m_PESQueue->GetSize () > 0)
    {

      Ptr<Packet> packet = m_PESQueue->Dequeue ();
      value = 1;
      uint8_t *buffer = new uint8_t [PES_MAX_SZ];
      packet->CopyData (buffer,packet->GetSize ());
      
      PesHeader pesHeader;
      m_usingPesPacket = packet->Copy();
            
      packet->PeekHeader (pesHeader);      
      Ptr<DVBHQueue> streamTsQueue = GetTSQueue (pesHeader.GetSid ());

      if (streamTsQueue != NULL)
        {
          count = 0;
          first = 1;           
          while (value == 1)
            {
              value = CreateTSPacket (m_usingPesPacket->Copy(),streamTsQueue,pesHeader.GetSid (),count,first);
              count++;
              first = 0;
              if (count == 16)
                {
                  count = 0;
                }
            }
        }

    }

}

void
DVBHPhyLayerBaseStation::CreateNewTSQueue (int streamId)
{
  Ptr<DVBHQueue> streamTsQueue = Create<DVBHQueue> ();  
  m_tsQueueList.push_back (std::make_pair (streamTsQueue,streamId));
}

Ptr<DVBHQueue>
DVBHPhyLayerBaseStation::GetTSQueue (uint32_t streamId)
{
  for (TsQueueListI i = m_tsQueueList.begin (); i != m_tsQueueList.end (); i++)
    {
      if ((*i).second == streamId)
        {
          return (*i).first;
        }

    }
  return NULL;
}


int
DVBHPhyLayerBaseStation::CreateTSPacket (Ptr<Packet> pesPacket,Ptr<DVBHQueue> streamTsQueue,uint32_t sid,int count,int first)
{
  uint32_t maximum = TS_MAX_SZ - TS_HDR_SZ;
  
  if (pesPacket->GetSize () > maximum)
    {

      Ptr<Packet> tspacket = pesPacket->CreateFragment (0,184);

      TsHeader tsHeader;
      tsHeader.SetPayloadUnitStart ((uint16_t)first);
      tsHeader.SetPid ((uint16_t)sid);
      tsHeader.SetCounter ((uint8_t)count);

      tspacket->AddHeader (tsHeader);

      streamTsQueue->Enqueue (tspacket->Copy ());

      pesPacket->RemoveAtStart (184);      
      m_usingPesPacket = pesPacket;
      m_fragmentCount++;

      if (m_fragmentCount == (int)PES_MAX_SZ / 184)
        {
          m_fragmentCount = 0;
        }               
      return 1;
    }
  else
    {
      Ptr<Packet> tspacket = pesPacket->Copy ();

      TsHeader tsHeader;
      tsHeader.SetPayloadUnitStart ((uint16_t)0);
      tsHeader.SetPid ((uint16_t)sid);
      tsHeader.SetCounter ((uint8_t)count);

      tspacket->AddHeader (tsHeader);      
      streamTsQueue->Enqueue (tspacket->Copy ());

      m_fragmentCount = 0;
      pesPacket = 0;
      return 0;
    }

}

void
DVBHPhyLayerBaseStation::CreateTSPacketFromPMT (struct PMTTable pmtTable)
{
  int value = 0;
  int size = pmtTable.size;
  
  while (value == 0)
    {
      if (size < TS_MAX_SZ - TS_HDR_SZ)
        {
          TSPacket tsPacket;
          tsPacket.id = pmtTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;

          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetPMTTable (pmtTable);
          tsPacket.controlpacket = controlpacket;
	  
          SendDownPackets (tsPacket);
          value = 1;
        }
      else
        {
          TSPacket tsPacket;
          tsPacket.id = pmtTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;

          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetPMTTable (pmtTable);
          size -= TS_MAX_SZ - TS_HDR_SZ;
          tsPacket.controlpacket = controlpacket;
	  
          SendDownPackets (tsPacket);
          value = 0;
        }
    }
}

void
DVBHPhyLayerBaseStation::CreateTSPacketFromSDT (struct SDTTable sdtTable)
{
  int value = 0;
  int size = sdtTable.size;

  while (value == 0)
    {
      if (size < TS_MAX_SZ - TS_HDR_SZ)
        {
          TSPacket tsPacket;
          tsPacket.id = sdtTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;

          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetSDTTable (sdtTable);
          tsPacket.controlpacket = controlpacket;
	            
          SendDownPackets (tsPacket);
          value = 1;
        }
      else
        {
          TSPacket tsPacket;
          tsPacket.id = sdtTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;
	  
          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetSDTTable (sdtTable);
          size -= TS_MAX_SZ - TS_HDR_SZ;
          tsPacket.controlpacket = controlpacket;
          SendDownPackets (tsPacket);
          value = 0;
        }
    }
}


void
DVBHPhyLayerBaseStation::CreateTSPacketFromPAT (struct PATTable patTable)
{
  int value = 0;
  int size = patTable.size;

  while (value == 0)
    {
      if (size < TS_MAX_SZ - TS_HDR_SZ)
        {
          TSPacket tsPacket;
          tsPacket.id = patTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;

          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetPATTable (patTable);
          tsPacket.controlpacket = controlpacket;
	  
          SendDownPackets (tsPacket);
          value = 1;
        }
      else
        {
          TSPacket tsPacket;
          tsPacket.id = patTable.tableId;
          tsPacket.size = TS_MAX_SZ - TS_HDR_SZ;

          Ptr<DVBHControlPacket> controlpacket = Create<DVBHControlPacket> ();
          controlpacket->SetPATTable (patTable);
          size -= TS_MAX_SZ - TS_HDR_SZ;
          tsPacket.controlpacket = controlpacket;
	  
          SendDownPackets (tsPacket);
          value = 0;
        }
    }
}


void
DVBHPhyLayerBaseStation::SendDownPackets (struct TSPacket packet)
{  
  
  m_ofdmLayer->ReceivePacketsFromAbove (packet);
}

void
DVBHPhyLayerBaseStation::SendDownBurst (Ptr<DVBHQueue> queue,uint32_t serviceId)
{
    m_ofdmLayer->ReceiveBurstFromAbove (queue,serviceId);
}

void
DVBHPhyLayerBaseStation::DownStreamHandler (uint32_t pid)
{

  for (TsQueueListI i = m_tsQueueList.begin (); i != m_tsQueueList.end (); i++)
    {
      if ((*i).second == pid)
        {          	  
          SendDownBurst ((*i).first->Copy (),(*i).second);
          (*i).first->RemoveAll ();

          break;
        }

    }

}

void
DVBHPhyLayerBaseStation::ReceiveFromBelow (struct TSPacket packet,bool sleep)
{
    //Not implemented
}

}
