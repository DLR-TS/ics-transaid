/*
 * This file is part of the iTETRIS Control System (https://github.com/DLR-TS/ics-transaid)
 * Copyright (c) 2008-2021 iCS development team and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
/****************************************************************************************
 * Edited by Panagiotis Matzakos <matzakos@eurecom.fr>
 * EURECOM 2015
 * Added IPv6 support
***************************************************************************************/

/**
 * \ingroup applications
 * \defgroup itetris-app
 *
 * Generic iTETRIS application to send packets on C2C or IP stacks
 *
*/

/**
 * \ingroup itetris-app
 *
 * \brief This is the iTETRIS C-ITS Application, based on which DENM,CAM and C2C-IP are based.
 *
 *  @author Michele Rondinone <mrondinone@umh.es>
 *
 */

#ifndef ITETRIS_APPLICATION_H
#define ITETRIS_APPLICATION_H

#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/channel-tag.h"
#include "ns3/callback.h"
#include "ns3/storage.h"

namespace ns3 {

class c2cAddress;

class iTETRISApplication: public Application {
public:
    static TypeId GetTypeId(void);

    iTETRISApplication();
    virtual ~iTETRISApplication();

    void SetServiceType(std::string servicetype);
    void SetServiceIndex(uint32_t app_index);
    typedef Callback<bool, uint32_t, std::string, uint32_t, uint32_t, uint32_t, tcpip::Storage*> ReceiveCallback;
    void SetReceiveCallback(iTETRISApplication::ReceiveCallback cb);
    void UnsetReceiveCallback(void);

    virtual void SetFrequency(double frequency);
    virtual void SetMessRegenerationTime(double MessRegenerationTime);
    virtual void SetMsgLifeTime(uint8_t MsgLifeTime);
    virtual void SetPacketSize(uint32_t PacketSize);
    virtual void SetC2CAddress(Ptr<c2cAddress> address);
    virtual void SetIPAddress(Ipv4Address address);
    virtual void SetSockets(void);
    virtual void SetApplicationId(uint64_t applicationId);
    virtual void SetChTag(ChannelTag ch_tag);

    virtual void StartTransmitting(Ipv4Address address);
    //NEW
    virtual void StartTransmitting(Ipv6Address address);
    virtual void StartTransmitting(Ptr<c2cAddress> address);
    virtual void StopTransmitting(void);

    virtual void SetMessageId(uint32_t messageId);

    virtual void SetV2XMessageType(uint32_t msgType);

private:
    /**
     * \brief Application specific startup code
     *
     * The StartApplication method is called at the start time specifed by Start
     * This method should be overridden by all or most application
     * subclasses.
     */
    virtual void StartApplication(void);

    /**
     * \brief Application specific shutdown code
     *
     * The StopApplication method is called at the stop time specifed by Stop
     * This method should be overridden by all or most application
     * subclasses.
     */
    virtual void StopApplication(void);

protected:
    virtual void DoDispose(void);
    void AddInciPacketTags(Ptr<Packet> p);
    void RetrieveInciPacketTags(Ptr<Packet> p);
    void InitializeINCIvariables(void);

    std::string m_servicetype;
    std::string m_composedServiceType;
    Ptr<c2cAddress> m_c2cAddress;
    uint32_t m_packetSize;
    double m_frequency;
    double m_MessRegenerationTime;
    uint8_t m_MsgLifeTime;
    Ipv4Address m_IPAddress;
    Ipv6Address m_IPv6Address;
    ChannelTag m_channeltag;

    // inci variables for message tracking in the iCS
    uint32_t m_currentTimeStep;
    uint32_t m_previousTimeStep;
    uint32_t m_stepSequenceNumber;
    uint32_t m_app_index;
    iTETRISApplication::ReceiveCallback m_forwardIcs;

    uint32_t m_messageId;
    uint32_t m_V2XmessageType;

private:
    std::string m_mwServiceType;
};

} // namespace ns3

#endif   /* ITETRIS_APPLICATION_H  */
