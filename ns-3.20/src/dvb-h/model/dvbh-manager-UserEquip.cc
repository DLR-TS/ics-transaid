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
#include "dvbh-manager-UserEquip.h"
#include "dvbh-manager-BaseStation.h"
#include "dvbh-channel.h"
#include "ns3/random-variable.h"
#include "ns3/log.h"
#include "dvbh-ofdm-layer.h"
#include "ns3/simulator.h"
#include "dvbh-controlpacket.h"
#include "ns3/mobility-model.h"
#include "dvbh-tags.h"
#include "mpe-header.h"
#include <list>
#include "ns3/uinteger.h"
#include <math.h>
#include "dvbh-ip-per.h"

NS_LOG_COMPONENT_DEFINE ("DVBHManagerUserEquip");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DVBHManagerUserEquip);

TypeId
DVBHManagerUserEquip::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DVBHManagerUserEquip")
    .SetParent<Object> ()

  ;
  return tid;
}


DVBHManagerUserEquip::DVBHManagerUserEquip ()
{
  m_nodeBIp.Set ("0");
  m_myIp.Set ("0");
  m_mpeFecEnabled = false;
  m_scanningEnabled = true;
  m_simultaneousServiceNumber = 1;
  m_desiredServicetype = "none";
  m_nodeIdentifier = 0;
  m_perCalculator=Create<DvbhIpPer>() ;
  m_size = 0;
}

void
DVBHManagerUserEquip::DoDispose ()
{

}

void
DVBHManagerUserEquip::SetLinkLayer (Ptr<DVBHLinkLayerUserEquip> linkLayer_)
{
  m_linkLayer = linkLayer_;
  m_linkLayer->SetForwardUpCallback (MakeCallback (&DVBHManagerUserEquip::ReceiveFromBelow,this));
  m_linkLayer->SetScanningCallback (MakeCallback (&DVBHManagerUserEquip::SwitchOffReceiver,this));
  
}

void
DVBHManagerUserEquip::SetNodeIdentifier (uint32_t nodeIdentifier)
{
  m_nodeIdentifier = nodeIdentifier;
  m_linkLayer->SetNodeIdentifier(nodeIdentifier);
}

uint32_t
DVBHManagerUserEquip::GetNodeIdentifier ()
{
  return m_nodeIdentifier;
}

void
DVBHManagerUserEquip::SwitchOffReceiver (bool switchedOff)
{
  m_scanningEnabled = switchedOff;
}

void
DVBHManagerUserEquip::SetForwardUpCallback (Callback<void,Ptr<Packet> > upCallback)
{
  m_forwardUp = upCallback;

}

void
DVBHManagerUserEquip::DecisionMaker ()
{
  // Simulator::Schedule(Seconds(2),&DVBHManagerUserEquip::DecisionMaker,this);
  // DecideToJoinService();
}

void
DVBHManagerUserEquip::ForwardUp (Ptr<Packet> packet)
{
  RxPowerTag pwTag;
  packet->PeekPacketTag(pwTag);
  
  if(CheckIpPER(pwTag.Get()))
  {
     m_forwardUp (packet); 
  }
  
}

void
DVBHManagerUserEquip::SetIpAddress (Ipv4Address address)
{
  m_myIp = address;
}

//NEW FUNCTION
void
DVBHManagerUserEquip::SetIpv6Address (Ipv6Address address)
{
  m_myIpv6 = address;
}

Ipv4Address
DVBHManagerUserEquip::GetIpAddress ()
{
  return m_myIp;
}

Ipv6Address
DVBHManagerUserEquip::GetIpv6Address ()
{
  return m_myIpv6;
}


void
DVBHManagerUserEquip::SetMobility (Ptr<MobilityModel> mobility)
{
  m_mobility = mobility;
  m_ofdm->SetMobility (mobility);
}

Ptr<MobilityModel>
DVBHManagerUserEquip::GetMobility ()
{
  return m_mobility;
}

void
DVBHManagerUserEquip::SetOfdmLayer (Ptr<DVBHOfdmLayer> ofdm_)
{
  m_ofdm = ofdm_;
}

Ptr<DVBHOfdmLayer>
DVBHManagerUserEquip::GetOfdmLayer ()
{
  return m_ofdm;
}

void
DVBHManagerUserEquip::SetSimultaneousServiceNumber (int number)
{
  m_simultaneousServiceNumber = number;
}

void
DVBHManagerUserEquip::PresetDesiredServiceType (std::string type)
{
  m_desiredServicetype = type;
}


void
DVBHManagerUserEquip::SetupConnection ()
{
  DecisionMaker ();
}

void
DVBHManagerUserEquip::ReceiveFromBelow (Ptr<Packet> packet, Ptr<DVBHControlPacket> controlpacket)
{
  if (controlpacket == 0) // Case of Receiving a Data Packet
    {
      ReceiveDataFromBelow (packet,controlpacket);
    }
  else
    {            
      if (controlpacket->GetPacketType () == patTablePacket)
        {
          ReceivePATTableFromBelow (packet,controlpacket);
        }
      else if (controlpacket->GetPacketType () == sdtTablePacket)
        {
          ReceiveSDTTableFromBelow (packet,controlpacket);
        }
      else if (controlpacket->GetPacketType () == pmtTablePacket)
        {
          ReceivePMTTableFromBelow (packet,controlpacket);
        }
    }
}

void
DVBHManagerUserEquip::EnableMpeFec ()
{
  m_mpeFecEnabled = true;
  m_linkLayer->EnableMpeFec ();
}

void
DVBHManagerUserEquip::DisableMpeFec ()
{
  m_mpeFecEnabled = false;
  m_linkLayer->DisableMpeFec ();
}


void
DVBHManagerUserEquip::ReceivePATTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  AddPATTableSection (controlpacket->GetPATTable ());
}

void
DVBHManagerUserEquip::ReceiveSDTTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{  
  AddSDTTableSection (controlpacket->GetSDTTable ());
}

void
DVBHManagerUserEquip::ReceivePMTTableFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{  
  ReadPidValuesFromPMT (controlpacket->GetPMTTable ());
}

void
DVBHManagerUserEquip::ReceiveDataFromBelow (Ptr<Packet> packet,Ptr<DVBHControlPacket> controlpacket)
{
  DvbhServiceIdTag tag;
  packet->RemovePacketTag (tag);
  
  NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//LAST Packet Size "<<packet->GetSize()<<".Time " << Simulator::Now ());
  
  MpeHeader mpeHeader;

  uint8_t *buffer = new uint8_t [10];
  packet->CopyData (buffer,10);
  if (buffer[0] == 0x3E)
    {
      packet->RemoveHeader (mpeHeader);
    }

  m_size += packet->GetSize ();
  if (mpeHeader.GetSectionNumber () == mpeHeader.GetLastSectionNumber ()&&mpeHeader.GetTableId () == 0x3E)
    {

      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//LAST Packet from service with Id " << tag.Get () << " Size "<<packet->GetSize()<<".Time " << Simulator::Now ());
      m_size = 0;
    }
  else
    {
      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager// Packet from service with Id " << tag.Get () << ".Time " << Simulator::Now ());

    }

  ForwardUp (packet);

}

bool
DVBHManagerUserEquip::CheckIpPER(double receivedPw)
{
  double snr=receivedPw-m_ofdm->GetTotalInterference();
  
  double value=m_perCalculator->GetIpPer(snr,0);
  uint32_t ent=(1/value);

  int n = 0;

  n = RandomVariable (UniformVariable (0,ent)).GetInteger ();
     
  if (n == 0)           // error
    {
      NS_LOG_INFO("Dvbh User Equipment " << m_nodeIdentifier << " **** Error Sending UP the IP Packet. Error Rate : " << (1/value)<< ". Time " << Simulator::Now ());
      return false;           // ERROR
    }
    
  return true;            // not error

      
}


void
DVBHManagerUserEquip::ReadPidValuesFromPMT (struct PMTTable table)
{
  for (std::list<struct PMTEntry >::iterator i = table.entryList.begin (); i != table.entryList.end (); i++)
    {
      m_linkLayer->AddPidToDesiredList (1,(*i).elementaryPID);
    }
}


void
DVBHManagerUserEquip::AddPATTableSection (struct PATTable table)
{
  int repeated = 0;
  for (PatSectionListI i = m_patSectionList.begin (); i != m_patSectionList.end (); i++)
    {
      if ((*i).transportStreamId == table.transportStreamId)
        {
          repeated = 1;
          break;
        }
    }
  if (repeated == 0)
    {
      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager// Adding new PAT Section.Time " << Simulator::Now ());
      m_patSectionList.push_back (table);
    }
}

void
DVBHManagerUserEquip::AddSDTTableSection (struct SDTTable table)
{
  int repeated = 0;
  for (SdtSectionListI i = m_sdtSectionList.begin (); i != m_sdtSectionList.end (); i++)
    {
      if ((*i).transportStreamId == table.transportStreamId)
        {
          repeated = 1;
          break;
        }
    }
  if (repeated == 0)
    {
      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager// Adding new SDT Section.Time " << Simulator::Now ());
      m_sdtSectionList.push_back (table);
    }
}

void
DVBHManagerUserEquip::AddPMTTableSection (struct PMTTable table)
{
  int repeated = 0;
  for (PmtSectionListI i = m_pmtSectionList.begin (); i != m_pmtSectionList.end (); i++)
    {
      if ((*i).tableId == table.tableId)
        {
          repeated = 1;
          break;
        }
    }
  if (repeated == 0)
    {
      m_pmtSectionList.push_back (table);
    }

}

void
DVBHManagerUserEquip::JoinService(uint32_t serviceId,std::string type)
{        
    m_linkLayer->AddPidToDesiredList (0,serviceId);
            
    if(type=="MULTICAST")
    {
	InformBaseStationAboutSubscription (serviceId);
    }
    
  
}

void
DVBHManagerUserEquip::DecideToJoinService ()
{
  double decisionValue = 0;
  int transportStreamId = 0;
  bool alreadyJoined = false;
  
  if (m_attachedStreams.size () < m_simultaneousServiceNumber)
    {
      if (!m_patSectionList.empty ())
        {
          transportStreamId = RandomVariable (UniformVariable (0,m_patSectionList.size ())).GetInteger (); // Elegimos un transpor stream al azar

          for (PatSectionListI i = m_patSectionList.begin (); i != m_patSectionList.end (); i++)
            {
              if ((*i).transportStreamId == transportStreamId)          // Seleccionamos la seccion de tabla del TS Seleccionado
                {
                  if (!(*i).entryList.empty ())         // Comprobamos si tenemos algún programa asociado al TS seleccionado
                    {
                      for (std::list< struct PATEntry>::iterator entry = (*i).entryList.begin (); entry != (*i).entryList.end (); entry++)
                        {
                          alreadyJoined = false;

                          if (m_attachedStreams.empty ())         // No estamos asociados a ningún servicio aún
                            {
                              if (m_desiredServicetype == "none")
                                {
                                  decisionValue = RandomVariable (UniformVariable (0,1)).GetValue ();
                                  if (decisionValue > 0.5)
                                    {
                                      m_attachedStreams.push_back ((*entry));
                                      m_linkLayer->AddPidToDesiredList (0,(*entry).programMapId);                                      
                                      InformBaseStationAboutSubscription ((*entry).programNumber);
                                    }
                                }
                              else              // Este Nodo tiene unos servicios prefijados, los buscamos
                                {
                                  for (SdtSectionListI j = m_sdtSectionList.begin (); j != m_sdtSectionList.end (); j++)
                                    {
                                      if ((*j).transportStreamId == transportStreamId)
                                        {
                                          for (std::list< struct SDTEntry>::iterator sdtEntry = (*j).entryList.begin (); sdtEntry != (*j).entryList.end (); sdtEntry++)
                                            {
                                              if ((*sdtEntry).programNumber == (*entry).programNumber)
                                                {
                                                  if ((*sdtEntry).descriptor == m_desiredServicetype)
                                                    {
                                                      m_attachedStreams.push_back ((*entry));
                                                      m_linkLayer->AddPidToDesiredList (0,(*entry).programMapId);                                                      
                                                      InformBaseStationAboutSubscription ((*entry).programNumber);
                                                    }
                                                  break;
                                                }
                                            }
                                        }
                                    }
                                }


                            }
                          else          // Ya estamos asociados a algún programa, comprobamos que no nos asociamos a uno ya previamente asociado
                            {
                              for (std::list< struct PATEntry >::iterator iterator = m_attachedStreams.begin (); iterator != m_attachedStreams.end (); iterator++)
                                {
                                  if ((*iterator).programNumber == (*entry).programNumber)      // Ya estamos asociados al programa
                                    {
                                      alreadyJoined = true;
                                      break;
                                    }
                                }

                              if (alreadyJoined == false)       // Nos asociamos al programa
                                {
                                  if (m_desiredServicetype == "none")
                                    {
                                      decisionValue = RandomVariable (UniformVariable (0,1)).GetValue ();
                                      if (decisionValue > 0.5)
                                        {
                                          m_attachedStreams.push_back ((*entry));
                                          m_linkLayer->AddPidToDesiredList (0,(*entry).programMapId);                                          
                                          InformBaseStationAboutSubscription ((*entry).programNumber);
                                        }
                                    }
                                  else          // Este Nodo tiene unos servicios prefijados, los buscamos
                                    {
                                      for (SdtSectionListI j = m_sdtSectionList.begin (); j != m_sdtSectionList.end (); j++)
                                        {
                                          if ((*j).transportStreamId == transportStreamId)
                                            {
                                              for (std::list< struct SDTEntry>::iterator sdtEntry = (*j).entryList.begin (); sdtEntry != (*j).entryList.end (); sdtEntry++)
                                                {
                                                  if ((*sdtEntry).programNumber == (*entry).programNumber)
                                                    {
                                                      if ((*sdtEntry).descriptor == m_desiredServicetype)
                                                        {
                                                          m_attachedStreams.push_back ((*entry));
                                                          m_linkLayer->AddPidToDesiredList (0,(*entry).programMapId);                                                          
                                                          InformBaseStationAboutSubscription ((*entry).programNumber);
                                                        }
                                                      break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

    }



}

void
DVBHManagerUserEquip::InformBaseStationAboutSubscription (int serviceId)
{
  NS_LOG_INFO("Node UE " << m_nodeIdentifier << " adding to the service ID " << serviceId);
  m_nodeBManager->AddNewSubscriber (serviceId,this);
}

void
DVBHManagerUserEquip::PeriodicScanning (void)
{
  Ipv4Address nullAddress;
  nullAddress.Set ("0");
  Ptr<DVBHManagerBaseStation> newBaseStationManager = ScanningBaseStation ();
  Ipv4Address newBaseStationIpAddress;
  newBaseStationIpAddress.Set ("0");

  if (newBaseStationManager != 0)
    {
      newBaseStationIpAddress = newBaseStationManager->GetIpAddress ();
    }


  if (newBaseStationIpAddress == nullAddress && m_nodeBIp != nullAddress )
    {
      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//Lost Coverage .Time " << Simulator::Now ());
      HandoverProcedure ();
      m_nodeBManager = 0;
      m_nodeBIp = nullAddress;
    }
  else if (newBaseStationIpAddress == nullAddress && m_nodeBIp == nullAddress)
    {      
      m_nodeBManager = 0;
      m_nodeBIp.Set ("0");

    }
  else if (newBaseStationIpAddress != nullAddress && m_nodeBIp == nullAddress)
    {

      m_nodeBManager = newBaseStationManager;
      if (newBaseStationManager->NewConnection (this))
        {
          NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//Connection to the Node B with IP " << newBaseStationManager->GetIpAddress () << " .Time " << Simulator::Now ());
          newBaseStationManager->GetSharedChannel (this);
          m_nodeBIp = newBaseStationIpAddress;          
        }
      else
        {
          NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//Connection to the Node B with IP " << newBaseStationManager->GetIpAddress () << " REFUSED .Time " << Simulator::Now ());
          m_nodeBIp = nullAddress;
          m_nodeBManager = 0;
        }

    }
  else if (newBaseStationIpAddress != nullAddress && m_nodeBIp != newBaseStationIpAddress)
    {
      NS_LOG_INFO("DVBH UserEquip " << m_myIp << " Manager//Handover Procedure. Last Node B was " << newBaseStationIpAddress);

      NS_LOG_INFO("UserEquipManager " << m_myIp << " Found a new BaseStation");
      HandoverProcedure ();

      if (newBaseStationManager->NewConnection (this))
        {
          m_nodeBManager = newBaseStationManager;
          m_nodeBIp = newBaseStationIpAddress;          
        }
      else
        {          
          m_nodeBManager = 0;
          m_nodeBIp.Set ("0");
        }


    }
  else if (newBaseStationIpAddress != nullAddress && m_nodeBIp == newBaseStationIpAddress)
    {
     
	//Left for debugging purposes
    }

}

void
DVBHManagerUserEquip::HandoverProcedure ()
{
  m_nodeBManager->RemoveUserEquip (this);
}

Ptr<DVBHManagerBaseStation>
DVBHManagerUserEquip::ScanningBaseStation (void)
{
  Ipv4Address nullAddress;
  nullAddress.Set ("0");
  // testeo si está registrado en alguna BS ( BsId != 0)
  if (m_nodeBIp != nullAddress)
    {
      // obtengo el CommandManager de la BS con la que está registrado
      Ptr<DVBHManagerBaseStation> nodeBManager = GetBaseStationManager (m_nodeBIp);

      // testeo si estoy en su zona de cobertura
      if (IsInCoverageArea (nodeBManager))
        {
          // devuelvo el mismo BsId de la BS con la que estaba registrado, ya que sigo teniendo cobertura con ella

          return nodeBManager;
        }
    }

// si no estaba registrado con ninguna BS (BsId==0), o bien ya no tiene cobertura con su BS
  else
    {

// recorro la lista de BS (CommandManagers), testeo distancia, y devuelvo el BsId de la primera con la que encuentre cobertura. Si estoy fuera de cobertura devuelvo BsId=0
      for (BaseStationListI iter = m_nodeBList.begin (); iter != m_nodeBList.end (); ++iter)
        {

          if (IsInCoverageArea ((*iter)))
            {              
              return (*iter);
            }
        }
    }

  return 0 ;
}

bool
DVBHManagerUserEquip::IsInCoverageArea (Ptr<DVBHManagerBaseStation> manager_)
{
  Ptr<MobilityModel> nodeBMobilityModel = manager_->GetMobility ();

  double nodeBRange = manager_->GetCoverageRange ();
  double distanceToBs = m_mobility->GetDistanceFrom (nodeBMobilityModel);

// testeo si la distancia a la BS a la que estoy registrado es menor que el rango de cobertura de ésta

  if (distanceToBs < nodeBRange )
    {
      return true;
    }

  return false;
}

Ptr<DVBHManagerBaseStation>
DVBHManagerUserEquip::GetBaseStationManager (Ipv4Address ipAddress)
{
  for (BaseStationListI i = m_nodeBList.begin (); i != m_nodeBList.end (); i++)
    {

      if ((*i)->GetIpAddress () == ipAddress)
        {
          return (*i);

        }
    }
  return 0;

}

void
DVBHManagerUserEquip::ServiceStartNotice (struct DVBHService service)
{
  //In case needed should be implemented
}

void
DVBHManagerUserEquip::ServiceStopNotice (struct DVBHService service)
{
  //In case needed should be implemented
}


void
DVBHManagerUserEquip::AddBaseStation(Ptr<DVBHManagerBaseStation> manager_)
{
  m_nodeBList.push_back (manager_);
}

}
