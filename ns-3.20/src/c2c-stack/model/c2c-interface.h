/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

#ifndef C2C_INTERFACE_H
#define C2C_INTERFACE_H

#include <list>
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/timer.h"

#include "ns3/itetris-technologies.h"
#include "ns3/address.h" // Added by Ramon Bauza
#include "ns3/netdevice-router.h" //Added by Panos

namespace ns3 {

class NetDevice;
class Packet;
class Node;

/**
 * \brief  iTETRIS [WP600] - The c2c representation of a network interface.
 *
 * @author Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 */

class c2cInterface  : public Object
{
public:
	static TypeId GetTypeId (void);

	c2cInterface ();
	virtual ~c2cInterface();

	void SetNode (Ptr<Node> node);
	void SetDevice (Ptr<NetDevice> device);
        void SetDeviceRouter (Ptr<NetDeviceRouter> DeviceRouter);        
        
	Ptr<Node> GetNode (void);

	/**
	 * \returns the underlying NetDevice. This method cannot return zero.
	 */
	virtual Ptr<NetDevice> GetDevice (void) const;

        virtual Ptr<NetDeviceRouter> GetDeviceRouter (void) const;

    void SetDeviceType (NetDeviceType device);

    virtual NetDeviceType GetDeviceType (void) const;

    bool IsUp (void) const;

    bool IsDown (void) const;

    /**
    * Enable this interface
    */
    void SetUp (void);

    /**
     * Disable this interface
     */
    void SetDown (void);

    /**
     * \returns true if this interface is enabled for IP forwarding of input datagrams
     */
    bool IsForwarding (void) const;

    /**
     * \param val Whether to enable or disable IP forwarding for input datagrams
     */
    void SetForwarding (bool val);

    /**
     * \param p packet to send
     *
     * This method will eventually call the private
     * SendTo method which must be implemented by subclasses.
     */
    void Send(Ptr<Packet> p);

    void Send(Ptr<Packet> p, const Address &dest); // Added by Ramon Bauza

protected:
  virtual void DoDispose (void);
private:
  void DoSetup (void);
  bool m_ifup;
  bool m_forwarding;  // IN_DEV_FORWARD
  Ptr<Node> m_node;
  Ptr<NetDevice> m_device;
  NetDeviceType m_deviceType;
  Ptr<NetDeviceRouter> m_netdevicerouter;
};

}; // namespace ns3

#endif  //C2C_INTERFACE_H
