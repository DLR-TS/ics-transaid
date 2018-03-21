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


#include "dvbh-link-layer-BaseStation.h"
#include "ns3/log.h"
#include "dvbh-controlpacket.h"
#include "ns3/simulator.h"
#include "dvbh-phy-layer-BaseStation.h"
#include "mpe-header.h"
#include <list>

NS_LOG_COMPONENT_DEFINE ("DVBHLinkLayerBaseStation");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHLinkLayerBaseStation);

static int transportStreamNumber;

DVBHLinkLayerBaseStation::DVBHLinkLayerBaseStation ()
{
NS_LOG_FUNCTION(this);
  m_pmtSendTime = Seconds (0.5);
  m_patSendTime = Seconds (0.5);
  m_sdtSendTime = Seconds (0.5);

  m_fecCalculationTime = Seconds (0.000001);

  m_pmtTableTimer = Simulator::Schedule (m_pmtSendTime,&DVBHLinkLayerBaseStation::PMTTableHandler,this);
  m_patTableTimer = Simulator::Schedule (m_patSendTime,&DVBHLinkLayerBaseStation::PATTableHandler,this);
  m_sdtTableTimer = Simulator::Schedule (m_sdtSendTime,&DVBHLinkLayerBaseStation::SDTTableHandler,this);

  mpeFecEnabled = false;
  m_fecTableRows = 1024;

  m_nodeIdentifier = 0;
  m_sectionCounter = 0;
}

TypeId
DVBHLinkLayerBaseStation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHLinkLayerBaseStation")
    .SetParent<Object> ()

  ;
  return tid;
}

void
DVBHLinkLayerBaseStation::DoDispose ()
{


}

void
DVBHLinkLayerBaseStation::SetFecCalculationTime (Time time)
{
  m_fecCalculationTime = time;
}

void
DVBHLinkLayerBaseStation::SetPhyLayer (Ptr<DVBHPhyLayerBaseStation> phyLayer_)
{
  m_phyLayer = phyLayer_;
  m_phyLayer->SetForwardUpCallback (MakeCallback (&DVBHLinkLayerBaseStation::ReceiveFromBelow,this));
  
}

void
DVBHLinkLayerBaseStation::SetForwardUpCallback (Callback<void,Ptr<Packet>,Ptr<DVBHControlPacket> > upCallback)
{
  m_forwardUp = upCallback;
}

void
DVBHLinkLayerBaseStation::ForwardUp (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  m_forwardUp (packet,controlpacket);
}

void
DVBHLinkLayerBaseStation::SetMacAddress (Address address)
{
  m_myMacAddress = address;
}

Address
DVBHLinkLayerBaseStation::GetMacAddress ()
{
  return m_myMacAddress;
}

void
DVBHLinkLayerBaseStation::EnableMpeFec ()
{
  mpeFecEnabled = true;
  m_phyLayer->EnableMpeFec ();
}

void
DVBHLinkLayerBaseStation::DisableMpeFec ()
{
  mpeFecEnabled = false;
  m_phyLayer->DisableMpeFec ();
}

void
DVBHLinkLayerBaseStation::ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket)
{
  /*This is not necessary. There will not be a communication destined to the base station in DVB-H*/
}

void
DVBHLinkLayerBaseStation::ReceiveFromAbove (Ptr<Packet> packet,Time delta_t,uint8_t  lastSection)
{
  if (mpeFecEnabled == false)
    {
      CreateMPEFrames (packet,delta_t,lastSection);
    }
  else
    {
//      CalculateMPEFECTime(packet,delta_t);	The use of MPE-FEC is not really necessary for iTETRIS
    }


}

void
DVBHLinkLayerBaseStation::SetNodeIdentifier (uint32_t nodeIdentifier)
{
  m_nodeIdentifier = nodeIdentifier;
  m_phyLayer->SetNodeIdentifier (nodeIdentifier);
}

uint32_t
DVBHLinkLayerBaseStation::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHLinkLayerBaseStation::ResetSectionCounter ()
{
  m_sectionCounter = 0;
}

int
DVBHLinkLayerBaseStation::GetSectionCounter ()
{
  return m_sectionCounter;
}

void
DVBHLinkLayerBaseStation::NewServiceStreamAdded (int streamId)
{
  m_phyLayer->CreateNewTSQueue (streamId);
}

void
DVBHLinkLayerBaseStation::CalculateMPEFECTime (struct ElementaryStream stream,Time delta_t)
{

  // Simulator::Schedule(fecCalculationTime,&DVBHLinkLayerBaseStation::CreateMPEFECFrames,this,stream,delta_t);

}

void
DVBHLinkLayerBaseStation::CreateMPEFECFrames (Ptr<Packet> packet,Time delta_t)
{

  // CreateMPEFrames(packet,delta_t);
  // CreateMPEFECRsPackets(stream.id,delta_t);

}

void
DVBHLinkLayerBaseStation::CreateMPEFECRsPackets (int pid,Time delta_t)
{
  for (int i = 0; i < 64; i++)
    {      
      MPEPacket packet;
      packet.currentSection = i;
      packet.tableId = 0x78;
      packet.lastSection = 63;
      packet.size = m_fecTableRows;
      packet.streamId = pid;
      packet.delta_t = delta_t;
      // SendDownMPEPacket(packet);

    }
}


void
DVBHLinkLayerBaseStation::CreateMPEFrames (Ptr<Packet> packet,Time delta_t,uint8_t lastSection)
{
  
  if(packet->GetSize()<40000)
  {
    MpeHeader mpeHeader;
    mpeHeader.SetSectionLength (packet->GetSize ());
        
    mpeHeader.SetSectionNumber (m_sectionCounter);
    mpeHeader.SetLastSectionNumber (lastSection);

    packet->AddHeader (mpeHeader);
	
    SendDownMPEPacket (packet->Copy ());

    if (m_sectionCounter == 255)
      {
	m_sectionCounter = 0;
      }
    else if (m_sectionCounter == lastSection)
      {
	m_sectionCounter = 0;
      }
    else
      {
	m_sectionCounter++;
      }
  }
  else
  {
    int fragments=packet->GetSize()/4000;
    int index=0;
    for(index=0;index<fragments;index++)
    {
      MpeHeader mpeHeader;
      mpeHeader.SetSectionLength (4000);
      mpeHeader.SetSectionNumber (m_sectionCounter);
      mpeHeader.SetLastSectionNumber (lastSection);

      Ptr<Packet> newPacket=packet->Copy();
      newPacket->RemoveAtEnd(packet->GetSize()-4000);
      
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
	  
	  if(!newPacket->PeekPacketTag(*tag))
	  {
	    newPacket->AddPacketTag(*tag);
	  }
		  
	}
      
      newPacket->AddHeader (mpeHeader);
	  
      SendDownMPEPacket (newPacket->Copy ());

      if (m_sectionCounter == 255)
	{
	  m_sectionCounter = 0;
	}
      else if (m_sectionCounter == lastSection)
	{
	  m_sectionCounter = 0;
	}
      else
	{
	  m_sectionCounter++;
	}
     }
     
    MpeHeader mpeHeader;
    mpeHeader.SetSectionLength (packet->GetSize()-4000*fragments);
    mpeHeader.SetSectionNumber (m_sectionCounter);
    mpeHeader.SetLastSectionNumber (lastSection);

    Ptr<Packet> newPacket=packet->Copy();
    newPacket->RemoveAtEnd(4000*fragments);
    
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
	  
	  if(!newPacket->PeekPacketTag(*tag))
	  {
	    newPacket->AddPacketTag(*tag);
	  }
		  
	}
    
    newPacket->AddHeader (mpeHeader);
	
    SendDownMPEPacket (newPacket->Copy ());

    if (m_sectionCounter == 255)
      {
	m_sectionCounter = 0;
      }
    else if (m_sectionCounter == lastSection)
      {
	m_sectionCounter = 0;
      }
    else
      {
	m_sectionCounter++;
      }
     
     
    
    
  }
 

}

void
DVBHLinkLayerBaseStation::SendDownMPEPacket (Ptr<Packet> packet)
{
  m_phyLayer->ReceiveFromAbove (packet);
}

void
DVBHLinkLayerBaseStation::AddNewTS (std::string name,double transmissionRate)
{
  TransportStream stream;
  stream.name = name;
  stream.id = transportStreamNumber;
  transportStreamNumber++;
  stream.bandwidth = transmissionRate;
  streamList.push_back (stream);
}

void
DVBHLinkLayerBaseStation::RemoveTS (std::string name)
{
  for (StreamListI i = streamList.begin (); i != streamList.end (); i++)
    {
      if ((*i).name == name)
        {
          streamList.erase (i);
        }
    }
}

void
DVBHLinkLayerBaseStation::AddSdtSection (int transportStreamId)
{
  m_sdtTable.tableId = 0x0011;
  m_sdtTable.transportStreamId = transportStreamId;
  m_sdtTable.size = 15;

  sdtSectionList.push_back (m_sdtTable);

}


void
DVBHLinkLayerBaseStation::AddEntrySDTTable (int programNumber,std::string type,int transportStreamId)
{
  SDTEntry entry;
  entry.programNumber = programNumber;
  entry.descriptor = type;

  NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK// Adding new entry to the SDT Table. Program Id " << programNumber << " with Type " << type << " .Time " << Simulator::Now ());
  for (SdtSectionListI i = sdtSectionList.begin (); i != sdtSectionList.end (); i++)
    {
      if ((*i).transportStreamId == transportStreamId)
        {
          (*i).entryList.push_back (entry);
          break;
        }
    }
}

void
DVBHLinkLayerBaseStation::AddPatSection (int transportStreamId)
{
  m_patTable.tableId = 0x0000;
  m_patTable.transportStreamId = transportStreamId;
  m_patTable.size = 12;

  patSectionList.push_back (m_patTable);

  NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK// Adding PAT Section for TS " << transportStreamId << " .Time " << Simulator::Now ());

}

void
DVBHLinkLayerBaseStation::RemovePatSection (int transportStreamId)
{
  for (PatSectionListI i = patSectionList.begin (); i != patSectionList.end (); i++)
    {
      if ((*i).transportStreamId == transportStreamId)
        {
          patSectionList.erase (i);
        }
    }
}

void
DVBHLinkLayerBaseStation::AddEntryPATTable (int programNumber,int programMapPid,int transportStreamId)
{
  PATEntry entry;
  entry.programNumber = programNumber;
  entry.programMapId = programMapPid;
  NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK// Adding new entry to the PAT Table. Program Id " << programNumber << " .Time " << Simulator::Now ());
  for (PatSectionListI i = patSectionList.begin (); i != patSectionList.end (); i++)
    {
      if ((*i).transportStreamId == transportStreamId)
        {
          (*i).entryList.push_back (entry);
          CreateNewPMTTable (programNumber,programMapPid);
          break;
        }
    }


}

void
DVBHLinkLayerBaseStation::CreateNewPMTTable (int programNumber,int tableId)
{
  PMTTable table;
  table.tableId = tableId;
  table.programNumber = programNumber;
  table.size = 12;
  pmtSectionList.push_back (table);
  NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK// Creating new PMT Table with Id " << tableId << " .Time " << Simulator::Now ());

}

void
DVBHLinkLayerBaseStation::RemovePMTTable (int tableId)
{
  for (PmtSectionListI i = pmtSectionList.begin (); i != pmtSectionList.end (); i++)
    {
      if ((*i).tableId == tableId)
        {
          pmtSectionList.erase (i);
          break;
        }
    }
}

void
DVBHLinkLayerBaseStation::AddEntryPMTTable (int programId,int elementaryStreamPID,std::string streamType)
{
  NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK// Adding new entry to the PMT Table. Program Id " << elementaryStreamPID << " .Time " << Simulator::Now ());
  for (PmtSectionListI i = pmtSectionList.begin (); i != pmtSectionList.end (); i++)
    {
      if ((*i).programNumber == programId)
        {
          PMTEntry entry;
          entry.elementaryPID = elementaryStreamPID;
          entry.streamType = streamType;

          (*i).size = (*i).size + 5;
          (*i).entryList.push_back (entry);
          NS_LOG_INFO("BaseStation " << m_phyLayer->GetNodeIdentifier () << " LINK//Found the PMT Table. Adding the new entry .Time " << Simulator::Now ());
          break;

        }
    }
}

void
DVBHLinkLayerBaseStation::PMTTableHandler ()
{
NS_LOG_FUNCTION(this);
  m_pmtTableTimer = Simulator::Schedule (m_pmtSendTime,&DVBHLinkLayerBaseStation::PMTTableHandler,this);

  if (!pmtSectionList.empty ())
    {
      for (PmtSectionListI i = pmtSectionList.begin (); i != pmtSectionList.end (); i++)
        {          
          m_phyLayer->CreateTSPacketFromPMT ((*i));
        }

    }

}

void
DVBHLinkLayerBaseStation::PATTableHandler ()
{
NS_LOG_FUNCTION(this);
  m_patTableTimer = Simulator::Schedule (m_patSendTime,&DVBHLinkLayerBaseStation::PATTableHandler,this);

  if (!patSectionList.empty ())
    {
      for (PatSectionListI i = patSectionList.begin (); i != patSectionList.end (); i++)
        {
          NS_LOG_INFO("BaseStation "<<m_nodeIdentifier<<" LINK// Sending PAT Table Section.Time "<<Simulator::Now());
          m_phyLayer->CreateTSPacketFromPAT ((*i));
        }

    }

}

void
DVBHLinkLayerBaseStation::SDTTableHandler ()
{
NS_LOG_FUNCTION(this);
  m_sdtTableTimer = Simulator::Schedule (m_sdtSendTime,&DVBHLinkLayerBaseStation::SDTTableHandler,this);

  if (!sdtSectionList.empty ())
    {
      for (SdtSectionListI i = sdtSectionList.begin (); i != sdtSectionList.end (); i++)
        {

          m_phyLayer->CreateTSPacketFromSDT ((*i));
        }

    }

}


}
