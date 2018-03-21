/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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

/****************************************************************************************
 *                          iTETRIS-Application.cc
 * This is a generic iTETRIS C-ITS application. As an interface, it is
 * inherited by all iTETRIS applications (CAM, DENM, C2C-IP etc..)
 *
***************************************************************************************/

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/c2c-address.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node-id-tag.h" 
#include "ns3/time-step-tag.h" 
#include "ns3/TStep-sequence-number-tag.h"
#include "ns3/app-index-tag.h"
#include "ns3/rssi-tag.h"
#include "ns3/tx-power-tag.h"
#include "ns3/mac-low.h"  // for the snr Tag
#include "ns3/snr-tag.h"
#include "ns3/itetris-types.h"  // for the tag types
#include "ns3/storage.h" // to transmit tags to the iCS
#include "ns3/message-id-tag.h"
#include "ns3/qos-tag.h"

#include "iTETRIS-Application.h"

NS_LOG_COMPONENT_DEFINE ("iTETRISApplication");

using namespace std;

namespace ns3
{

	NS_OBJECT_ENSURE_REGISTERED (iTETRISApplication);

	TypeId iTETRISApplication::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::iTETRISApplication").SetParent<Application>().AddConstructor<iTETRISApplication>();
		return tid;
	}

	iTETRISApplication::iTETRISApplication()
	{
		m_messageId = 0;
	}

	iTETRISApplication::~iTETRISApplication()
	{
	}

	void iTETRISApplication::DoDispose(void)
	{
		NS_LOG_FUNCTION_NOARGS();
		Application::DoDispose();
	}

	void iTETRISApplication::SetServiceType(std::string servicetype)
	{
		if (servicetype.find("-") != string::npos)
		{
			m_composedServiceType = servicetype;
			int index = m_composedServiceType.find("-");
			m_servicetype = m_composedServiceType.substr(index + 1);
		} else
		{
			m_composedServiceType = servicetype;
			m_servicetype = m_composedServiceType;
		}
	}

	void iTETRISApplication::SetServiceIndex(uint32_t app_index)
	{
		m_app_index = app_index;
	}

	void iTETRISApplication::AddInciPacketTags(Ptr<Packet> p)
	{
		// Sender ID tag
		Ptr<Node> local = GetNode();
		uint32_t nodeID = local->GetId();

		NodeIdTag nodeTag;
		nodeTag.Set(nodeID);
		p->AddPacketTag(nodeTag);

		//Set message id;
		MessageIdTag msgTag;
		msgTag.Set(m_messageId);
		p->AddPacketTag(msgTag);

		// Application Index tag
		AppIndexTag appindexTag;
		appindexTag.Set(m_app_index);
		p->AddPacketTag(appindexTag);

		// update of timestep and time step sequence number
		m_currentTimeStep = (static_cast<uint32_t>(Simulator::Now().GetMilliSeconds()));
		if (m_currentTimeStep > m_previousTimeStep)
		{
			m_stepSequenceNumber = 0;
			m_previousTimeStep = m_currentTimeStep;
		} else
		{
			m_stepSequenceNumber++;
		}

		NS_LOG_INFO("\n");
		NS_LOG_INFO(
				"[ns3][iTETRISApplication]service =  " << m_servicetype << "         app index =  " << m_app_index
						<< "          sender nodeID = " << nodeID << "      timestep = " << m_currentTimeStep
						<< "        timestepSeqNo = " << m_stepSequenceNumber << "\n");

		// time step tag
		TimeStepTag timeStepTag;
		timeStepTag.Set(m_currentTimeStep);
		p->AddPacketTag(timeStepTag);

		// time step sequence number tag
		TStepSequenceNumberTag TSSeqNTag;
		TSSeqNTag.Set(m_stepSequenceNumber);
		p->AddPacketTag(TSSeqNTag);

        // add channel tag
		ChannelTag ch_tag;
        ch_tag.Set(CCH); //CCH by default
		p->AddPacketTag(ch_tag);
        // add EDCA queue
        // deprecated: the EDCA queues will be set according to the TC in the Communication Facilities
        QosTag qos_tag;
        qos_tag.SetTid(0); // AC_BE by default.
        p->AddPacketTag(qos_tag);

	}

	void iTETRISApplication::RetrieveInciPacketTags(Ptr<Packet> packet)
	{
		NodeIdTag nodeTag;
		bool found;
		found = packet->PeekPacketTag(nodeTag);
		NS_ASSERT(found);
		uint32_t senderId = nodeTag.Get();

		TimeStepTag timestepTag;
		found = packet->PeekPacketTag(timestepTag);
		NS_ASSERT(found);
		uint32_t timeStep = timestepTag.Get();

		TStepSequenceNumberTag timestepSNTag;
		found = packet->PeekPacketTag(timestepSNTag);
		NS_ASSERT(found);
		uint32_t timeStepSN = timestepSNTag.Get();

		MessageIdTag messageTag;
		found = packet->PeekPacketTag(messageTag);
		NS_ASSERT(found);
		uint32_t messageId = messageTag.Get();

		// iTETRIS Extension for EU FP7 COLOMBO - generic container to transmit ns-3 data to the Application
		//JHNote (04/09/2013): take any other type of Tag that may be of interest for the iCS and Application module and put it in a vector

		tcpip::Storage *genericTagContainer = new tcpip::Storage(); // alternative if Ptr does not work here

		RSSITag rssiTag;
		uint16_t rssi = -1;
		genericTagContainer->writeUnsignedByte(TAG_RSSI);
		if (packet->PeekPacketTag(rssiTag))
			rssi = rssiTag.Get();
		genericTagContainer->writeShort((short) rssi);
//		genericTagContainer->writeUnsignedByte(TAG_RSSI);
//		genericTagContainer->writeShort((short) messageId);

		SnrTag snrTag;
		double snr = -1.0;
		genericTagContainer->writeUnsignedByte(TAG_SNR);
		if (packet->PeekPacketTag(snrTag))
			snr = snrTag.Get();
		genericTagContainer->writeDouble(snr);

		TxPowerTag txPower;
		uint8_t power = 0;
		genericTagContainer->writeUnsignedByte(TAG_TXPOWER);
		if (packet->PeekPacketTag(txPower))
			power = txPower.Get();
		genericTagContainer->writeUnsignedByte(power);

		genericTagContainer->writeUnsignedByte(TAG_MSGID);
		genericTagContainer->writeInt(messageId);

		NS_LOG_INFO("\n");
		NS_LOG_INFO(
				"[ns3][iTETRISApplication]RECEPTION: service=" << m_servicetype << " messageId=" << messageId << " sender nodeID=" << senderId
						<< "timestep=" << timeStep << " timestepSeqNo=" << timeStepSN << " packet RSSI=" << rssi
						<< " packet SNR=" << snr << " packet TX_power=" << power);

		m_forwardIcs(senderId, m_servicetype, timeStep, timeStepSN, messageId, genericTagContainer);
	}

	void iTETRISApplication::InitializeINCIvariables(void)
	{
		m_previousTimeStep = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
		m_currentTimeStep = 0;
		m_stepSequenceNumber = -1;
	}

	void iTETRISApplication::SetApplicationId(uint64_t applicationId)
	{
	}

	void iTETRISApplication::StartApplication(void)
	{
	}

	void iTETRISApplication::StopApplication(void)
	{
	}

	void iTETRISApplication::StartTransmitting(Ipv4Address address)
	{
	}

	void iTETRISApplication::StartTransmitting(Ipv6Address address)
	{
	}

	void iTETRISApplication::StartTransmitting(Ptr<c2cAddress> address)
	{
	}

	void iTETRISApplication::StopTransmitting(void)
	{
	}

	void iTETRISApplication::SetFrequency(double frequency)
	{
		m_frequency = frequency;
	}

	void iTETRISApplication::SetMessRegenerationTime(double MessRegenerationTime)
	{
		m_MessRegenerationTime = MessRegenerationTime;
	}

	void iTETRISApplication::SetPacketSize(uint32_t PacketSize)
	{
		m_packetSize = PacketSize;
	}

	void iTETRISApplication::SetMsgLifeTime(uint8_t MsgLifeTime)
	{
		m_MsgLifeTime = MsgLifeTime;
	}

	void iTETRISApplication::SetSockets(void)
	{
	}

	void iTETRISApplication::SetReceiveCallback(iTETRISApplication::ReceiveCallback cb)
	{
		m_forwardIcs = cb;
	}

	void iTETRISApplication::UnsetReceiveCallback(void)
	{
		m_forwardIcs.Nullify();
	}

	void iTETRISApplication::SetC2CAddress(Ptr<c2cAddress> address)
	{
	}

	void iTETRISApplication::SetIPAddress(Ipv4Address address)
	{
	}

	void iTETRISApplication::SetChTag(ChannelTag ch_tag)
	{
	}

	void iTETRISApplication::SetMessageId(uint32_t messageId)
	{
		m_messageId = messageId;
	}

} // Namespace ns3
