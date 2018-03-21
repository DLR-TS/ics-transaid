#ifndef WIFI_IP_INSTALLER_H
#define WIFI_IP_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "comm-module-installer.h"

namespace ns3
{

class WifiIpInstaller : public CommModuleInstaller
{
  public:
    static TypeId GetTypeId (void);
    WifiIpInstaller();
    void Install (NodeContainer container); 

  private:

  WifiHelper wifi;
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel;
  NqosWifiMacHelper wifiMac;
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper list;
  InternetStackHelper internet;
 
};

}

#endif





