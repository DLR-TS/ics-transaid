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


#ifndef UmtsBaseStationManager_H
#define UmtsBaseStationManager_H

#include "umts-phy-layer-bs.h"
#include "umts-rrc-layer-bs.h"
#include "ns3/mobility-model.h"
#include "ns3/itetris-types.h"
#include "umts-manager.h"
#include "cost231_model.h"

namespace ns3 {
  
class UmtsBaseStationManager : public Object
{

public:

  UmtsBaseStationManager ();
  
  typedef std::list<Ptr<UmtsUserEquipmentManager> > NodeUEList;
  typedef std::list<Ptr<UmtsUserEquipmentManager> >::iterator NodeUEListI;
  

  NodeUEList m_registeredUsers;
  static TypeId GetTypeId (void);
  double GetCoverageRange (void);
  void SetCoverageRange (double range);
  Ptr<MobilityModel> GetMobility ();
  void SetPhy (Ptr<UmtsPhyLayerBS> phy_);
  void SetRRC (Ptr<UmtsRrcLayerBS> rrc_);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();        
  void SetIpAddress(Ipv4Address address);
  //NEW
   void SetIpv6Address(Ipv6Address address);
  void RemoveNodeUE (Ptr<UmtsUserEquipmentManager> manager_);
  void AddNodeUE (Ptr<UmtsUserEquipmentManager> manager_);
  int NewConnection ( Ptr<ControlPacket> controlpacket);
  //NEW
  int NewConnection ( Ptr<ControlPacket> controlpacket, STACK stack);
  int ResourceRequestReceived (Ptr<UmtsUserEquipmentManager> manager_,std::string type,double rate,uint32_t applicationId,uint32_t size);
  Ptr<UmtsUserEquipmentManager> FindNodeUE (uint32_t identifier,NodeUEList nodeUEList_);
  Ptr<UmtsUserEquipmentManager> FindNodeUE (Ipv4Address destinationIp,NodeUEList nodeUEList_);
  void ULApplicationReleaseRequest (uint32_t flowId,uint32_t source);
  Ipv4Address GetIpAddress ();
  //NEW
  Ipv6Address GetIpv6Address ();
  void StartNewCommunication ();
  Ptr<UmtsUserEquipmentManager> ObtainRandomNodeUE ();  
  int PagingResponse (uint32_t uePhyAddress,Ipv4Address address,uint32_t applicationId);
  void PagingRequest(Ptr<UmtsUserEquipmentManager> manager_,std::string type,double rate,uint32_t applicationId);  
  uint64_t NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t destinationId,Ipv4Address address);
  
  //NEW
  uint64_t NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t destinationId,Ipv6Address address);
  
  void AlertReceivedFromBelow (Ptr<ControlPacket> controlpacket);
  void ReleaseReplyReceived (uint32_t destinationId,uint32_t applicationId);
  uint32_t GetNodeUEIdentifier(Ipv4Address address);
  //NEW
  uint32_t GetNodeUEIdentifier(Ipv6Address address);
  void NodeUEConnectionLoss (Ptr<UmtsUserEquipmentManager> manager_);
  void ObtainFrequency ();
  void TriggerNodeUEScanning(uint32_t nodeIdentifier);
  void AddUmtsBaseStationManager (Ptr<UmtsBaseStationManager> manager_);
  int GetDownlinkFrequency ();
  void ReturnExternalUplinkInterference ();
  Ptr<UmtsPhyLayerBS> GetPhy ();
  double CalculateExternalUplinkInterference (Ptr<UmtsPhyLayerBS> phyNodeB_);
  double CalculateDownlinkInterference (Ptr<UmtsPhyLayerUE> phyNodeUE);
  double CalculatePropagationLoss (Ptr<UmtsPhyLayerUE> phyNodeUE,double power);
          
  void RemoveNodeUEFromServices (uint32_t identifier);  
  void ConnectionRelease (Ptr<UmtsUserEquipmentManager> manager_);
  double ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                            double nodeBHeight,double nodeUEHeight);
			    
  void AddNewSubscriber(std::string serviceName,uint32_t identifier);
  void DeleteSubscriber(std::string serviceName,uint32_t identifier);  
  Ipv4Address* GetNodeUEIpAddress(uint32_t nodeIdentifier);

  //NEW
  Ipv6Address* GetNodeUEIpv6Address(uint32_t nodeIdentifier);
  void TriggerNodeUEScanning();
  
  void RemoveApplication(uint32_t applicationId,std::string type,uint32_t destinationId);
  
  void RegisterNewUser(Ptr<UmtsUserEquipmentManager> manager_);

  uint32_t getRegistedUsers();

  //NodeUEList getUsersList();

private:
  double m_coverageRange;  
  Ipv4Address m_myIp;
  Ipv6Address m_myIp6;
  Ptr<UmtsPhyLayerBS> m_phy;
  Ptr<UmtsRrcLayerBS> m_rrc;
  //NEW
  Ptr<UmtsRrcLayerBS> m_rrc6;

  typedef std::list<Ptr<UmtsBaseStationManager> > NodeBList;
  typedef std::list<Ptr<UmtsBaseStationManager> >::iterator NodeBListI;

  typedef std::list<struct UmtsService > ServiceList;
  typedef std::list<struct UmtsService >::iterator ServiceListI;
    
  NodeBList m_nodeBList;
  NodeUEList m_nodeUEList;
  //NodeUEList m_registeredUsers;
  ServiceList m_serviceList;

  Ptr<Cost231Propagation> m_costPropagationModel;

  int m_downlinkFrequency;
  int m_uplinkFrequency;
  
  uint32_t m_nodeIdentifier;    
  static uint64_t m_applicationId;
};

}

#endif
