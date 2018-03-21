#include "wifi-ip-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("WifiIpInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WifiIpInstaller);

TypeId WifiIpInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiIpInstaller")
    .SetParent<Object> ()     
    .AddConstructor<WifiIpInstaller> ()             
    ;
  return tid;
}
    
WifiIpInstaller::WifiIpInstaller () {

  wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.Set ("RxGain", DoubleValue (-10) ); 

  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  wifiMac = NqosWifiMacHelper::Default ();
  std::string phyMode ("wifib-1mbs");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                   "ControlMode",StringValue(phyMode));
  wifiMac.SetType ("ns3::AdhocWifiMac");

  list.Add (staticRouting, 0);
  list.Add (olsr, 10);
  internet.SetRoutingHelper (list);

} 

void
WifiIpInstaller::Install (NodeContainer container) {

  wifi.Install (wifiPhy, wifiMac, container);
  internet.Install (container);

}

}
