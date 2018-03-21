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

#ifndef UmtsUserEquipmentManager_H
#define UmtsUserEquipmentManager_H

#include "umts-phy-layer-ue.h"
#include "umts-rrc-layer-ue.h"
#include "umts-basestation-manager.h"
#include "ns3/mobility-model.h"
#include "umts-manager.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/itetris-types.h"
#include "cost231_model.h"

namespace ns3 {


class UmtsUserEquipmentManager : public Object
{

public:
  UmtsUserEquipmentManager ();
  static TypeId GetTypeId (void);
  void SetPhy (Ptr<UmtsPhyLayerUE> phy_);
  void SetRRC (Ptr<UmtsRrcLayerUE> rrc_);
  Ptr<UmtsRrcLayerUE> GetRRC ();
  Ptr<UmtsBaseStationManager> GetUmtsBaseStationManager (uint32_t nodeIdentifier);
  
  bool IsInCoverageArea (Ptr<UmtsBaseStationManager> manager_);
  Ptr<UmtsBaseStationManager> ScanningNodeB (void);
  void PeriodicScanning (void);
  //NEW
  void PeriodicScanning (STACK stack);
  void AddNodeBStation (Ptr<UmtsBaseStationManager> manager_);
  void SetupConnection ();
  void ConnectionReply (Ptr<ControlPacket> controlpacket);
  void ResourceReplyReceived (uint32_t applicationId,uint32_t size);
  void SetNodeIdentifier(uint32_t identifier);
  uint32_t GetNodeIdentifier();
  
  void ReleaseReply (uint32_t flowId);  
  void HandoverProcedure ();
  Ipv4Address GetIpAddress ();
  //NEW
  Ipv6Address GetIpv6Address ();
  void SetIpAddress(Ipv4Address address);
  //NEW
  void SetIpv6Address (Ipv6Address address);
  void PagingReceived (uint32_t address,std::string type,uint32_t applicationId);

  uint64_t NewApplicationConfiguration (std::string type,uint32_t applicationId,double rate,uint32_t size);
  Ptr<UmtsPhyLayerUE> GetPhy ();
  double CalculateExternalInterference (Ptr<UmtsPhyLayerBS> phyNodeB);
  void CalculateDownlinkInterference ();
  void ConnectionLoss ();
  double ReturnCost231Loss (Ptr<MobilityModel> a,Ptr<MobilityModel> b,double lambda,double shadowing,double minDistance,
                            double nodeBHeight,double nodeUEHeight);

  void JoinMulticastGroup(std::string serviceName);
  void LeaveMulticastGroup(std::string serviceName);
  
  Ptr<UmtsBaseStationManager> GetBestUmtsBaseStationManager(void);
  Ptr<UmtsBaseStationManager> GetBestUmtsBaseStationManager(STACK stack);
  void RemoveApplication(uint32_t applicationId,std::string type);
  void RemoveDownLinkFlow(uint32_t applicationId);
  
  uint32_t GetNodeBIdentifier();

private:
  Ptr<UmtsPhyLayerUE> m_phy;
  Ptr<UmtsRrcLayerUE> m_rrc;

  uint32_t m_nodeBIdentifier;  
  Ipv4Address m_myIp;
  //NEW
  Ipv6Address m_myIpv6;
  Ipv4Address m_nodeBIp;
  Ipv6Address m_nodeBIp6;
  Ptr<UmtsBaseStationManager> m_nodeBManager;
  
  typedef std::list<Ptr<UmtsBaseStationManager> > NodeBList;
  typedef std::list<Ptr<UmtsBaseStationManager> >::iterator NodeBListI;

  NodeBList m_nodeBList;    
  Ptr<Cost231Propagation> m_costPropagationModel;
  uint32_t m_nodeIdentifier;
  static uint64_t m_applicationId;

};

}

#endif
