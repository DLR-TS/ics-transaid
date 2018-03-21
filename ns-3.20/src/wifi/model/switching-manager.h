/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
#ifndef SwitchingManager_H
#define SwitchingManager_H

#include <stdint.h>
#include "ns3/object.h"
#include <map>
#include "ns3/ptr.h"
#include "qos-utils.h" 

namespace ns3 {

class NetDevice;
class Packet;
class WifiMacQueue;
class MacTxMiddle; 
class EdcaTxopN;
class WifiMacHeader;

class SwitchingManager : public Object
{
public:

  static TypeId GetTypeId (void);
  SwitchingManager ();
  virtual ~SwitchingManager ();
  void SetNetDevice (Ptr<NetDevice> device);
  void SetMacQueue(Ptr< EdcaTxopN > queue);

  // Switch channel after current PHY transmission has been completed. DcaTxop/EdcaTxop queues are flushed
  void SwitchChannel (uint16_t channelNumber); 

  // Switch channel after current MAC transmission has been completed (rts/cts/data/ack).DcaTxop/EdcaTxop queues are flushed
  void SwitchChannelAfterMacTxon (uint16_t channelNumber); 

  // Switch channel after all enqueued packets have been transmitted.
  void SwitchChannelAfterMacQueuesServed (uint16_t channelNumber); 

  // Called before performing a channel switching.
  // Later on, the stored packets may be enqueued and transmited in any other channel.
  uint16_t StorePacketsForLaterTxon (void); 

  bool InsertPacketsFront (uint16_t fromChannel);
  bool InsertPacketsBack (uint16_t fromChannel);

protected:

  virtual void DoDispose (void);

private:

  typedef std::vector<Ptr< EdcaTxopN > > Queues; 
  typedef std::vector<Ptr< WifiMacQueue > > WifiMacQueues;
  typedef std::map<uint16_t, WifiMacQueues* > BufferedPackets;

  void SetMacQueues (void);
  void SetEndTransmissionCallback (void); 
  void EndTransmissionCallback (WifiMacHeader const& hdr);
  void EndTransmission (void);
  void FlushEnqueuedPackets (void); 
  bool PendingPackets (void);
  WifiMacQueues* CreateQueue (uint16_t channel);
  bool DeleteQueue(uint16_t channel);
  bool LookupQueue (uint16_t channel);

  Ptr<NetDevice> m_device;
  Queues m_queues;  
  BufferedPackets m_bufferedPackets;
  uint16_t m_channelToSwitch;

};

} // namespace ns3 

#endif /* SwitchingManager_H */
