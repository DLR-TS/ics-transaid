/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "wimax-bs-installer.h"
#include "ns3/boolean.h"
#include "ns3/wimax-bs-mgnt.h"
#include "ns3/ip-base-sta-mgnt.h"
#include "ns3/iTETRISns3Facilities.h"
#include "ns3/IPCIU-facilities-helper.h"
#include "ns3/IPCIUFacilities.h"
#include "ns3/log.h"
#include "ns3/cost231-propagation-loss-model.h"

NS_LOG_COMPONENT_DEFINE ("WimaxBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WimaxBsInstaller);

TypeId WimaxBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WimaxBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<WimaxBsInstaller>()
    ;
  return tid;
}

NetDeviceContainer 
WimaxBsInstaller::DoInstall (NodeContainer container)
{
  NS_LOG_INFO ("*** WimaxBsInstaller ***");

  Ptr<SimpleOfdmWimaxChannel> channel;
  if (m_baseStationContainer.GetN () == 0)
    {
      channel = m_channel;   
    }
  else
    {
      channel = CreateObject<SimpleOfdmWimaxChannel> ();
      channel->SetPropagationModel (SimpleOfdmWimaxChannel::COST231_PROPAGATION);
    }

  NetDeviceContainer devices = m_wimax.Install (container, WimaxHelper::DEVICE_TYPE_BASE_STATION, WimaxHelper::SIMPLE_PHY_TYPE_OFDM, channel, WimaxHelper::SCHED_TYPE_SIMPLE, ns3::WIMAX_VERSION_ITETRIS); 
  
  m_baseStationContainer.Add (container);
  m_baseStationDeviceContainer.Add(devices);    
    
  BsCommandManagerContainer mgnrCont = m_commandMgnr.InstallBsCommandManagers (devices, 30.0);
  m_bsMgnrCont.Add (mgnrCont);
  if (m_ssMgnrCont.GetN () > 0)
    {
      m_commandMgnr.LinkBsAndSsCommandManagers (mgnrCont, m_ssMgnrCont);
    }

  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the base station has the object WimaxBsMgnt already installed
      Ptr<WimaxBsMgnt> wimaxBsMgnt = (*i)->GetObject <WimaxBsMgnt> ();
      if (wimaxBsMgnt  == NULL)
	{
          Ptr<NetDevice> device = devices.Get(index);
          wimaxBsMgnt = CreateObject <WimaxBsMgnt> ();
          wimaxBsMgnt->SetNode (*i);
          wimaxBsMgnt->SetNetDevice (device);
          wimaxBsMgnt->SetBsCommandManager ();
          (*i)->AggregateObject (wimaxBsMgnt);
          NS_LOG_INFO ("The object WimaxBsMgnt has been installed in the base station");
        }
               
      // Check if the vehicle has the Facilties already installed
      Ptr<IPCIUFacilities> facilities = (*i)->GetObject <IPCIUFacilities> ();
      if (facilities == NULL)
	{
	  IPCIUFacilitiesHelper facilitiesHelper;
	  facilitiesHelper.SetServiceListHelper (m_servListHelper);
	  facilitiesHelper.Install (*i);
	  NS_LOG_INFO ("The object IPCIUFacilities has been installed in the base station");
	}
      
      index ++;
    }

  return devices;
    
}

void 
WimaxBsInstaller::DoSetWimaxParameters (NetDeviceContainer devices)
{
  for (NetDeviceContainer::Iterator i = devices.Begin (); i != devices.End (); i++)
    {
      Ptr<WimaxNetDevice> device = DynamicCast<WimaxNetDevice> (*i); 
      Ptr<SimpleOfdmWimaxChannel> channel = DynamicCast<SimpleOfdmWimaxChannel> (device->GetChannel ());
      Ptr<Cost231PropagationLossModel> lossModel = DynamicCast<Cost231PropagationLossModel> (channel->GetPropagation ());
      lossModel->SetBSAntennaHeight (m_wimaxParameters.bsAntennaHeight);
      lossModel->SetSSAntennaHeight (m_wimaxParameters.ssAntennaHeight);
      lossModel->SetLambda (m_wimaxParameters.frequency*1000.0, 300000000.0);
      Ptr<BsCommandManager> manager = device->GetObject<BsCommandManager> ();
      manager-> SetCoverageRange (m_wimaxParameters.coverageRange);
    }
}

void 
WimaxBsInstaller::DoProcessApplicationInstall (std::string appName)
{
  m_servListHelper->Add (m_wimaxAppHelper, appName);
  NS_LOG_DEBUG ("Wimax application with itetrisName="<<appName<<" has been added to the ServiceListHelper"); 
  m_wimaxAppHelper = NULL;
}

} // namespace ns3
