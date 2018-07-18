/****************************************************************************************
 * Copyright (c) 2015 The Regents of the University of Bologna.
 * This code has been developed in the context of the
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software must display
 * the following acknowledgement: ''This product includes software developed by the
 * University of Bologna and its contributors''.
 * 4. Neither the name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************/
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/
#include "data-manager.h"
#include "log/log.h"
#include "behaviour-rsu.h"
#include "ics-interface.h"
#include "protocols.h"
#include "log/console.h"

namespace testapp
{
	namespace application
	{

		bool NodeInfoOrdering::operator()(const NodeInfo* left, const NodeInfo* right)
		{
//			NS_LOG_FUNCTION(left->lastSeen<<"<"<<right->lastSeen);
			return left->lastSeen < right->lastSeen;
		}

		uint16_t DataManager::ExecuteTime = 1000;
		bool DataManager::EnableCentralizedProtocol = true;
		bool DataManager::Enabled = true;

		DataManager::DataManager(iCSInterface* controller) :
				Behaviour(controller)
		{
			m_enabled = Enabled;
			m_executeAtThisStep = false;
			m_eventExecute = 0;

			//Register protocols
			if (EnableCentralizedProtocol)
				m_protocols.push_back(new CentralizedProtocol());
			for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol)
			{
				ExecuteBase* eb = *(protocol);
				for (TraceMap::iterator it = eb->GetTracedCallbacks().begin(); it != eb->GetTracedCallbacks().end(); ++it)
					RegisterTrace(it->first, *(it->second));
			}
		}

		DataManager::~DataManager()
		{
			for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol)
				delete *protocol;
			m_protocols.clear();
			RemoveAll();
			Scheduler::Cancel(m_eventExecute);
		}
		void DataManager::RemoveAll()
		{
			for (DataMap::iterator dir = m_dataMap.begin(); dir != m_dataMap.end(); ++dir)
			{
				for (NodeDataMap::iterator node = dir->second.begin(); node != dir->second.end(); ++node)
					RemoveData(node);
				dir->second.clear();
			}
			m_dataMap.clear();
		}
		bool DataManager::AddData(NodeInfo* info)
		{
			DataMap::iterator dir = m_dataMap.find(info->conformantDirection.getId());
			if (dir == m_dataMap.end())
				dir = m_dataMap.insert(std::make_pair(info->conformantDirection.getId(), NodeDataMap())).first;
			NodeDataMap::iterator node = dir->second.find(info->nodeId);
			if (node == dir->second.end())
				node = dir->second.insert(std::make_pair(info->nodeId, NodeDataCollection())).first;
			bool val = node->second.insert(info).second;
			return val;
		}
		bool DataManager::GetNodeCollection(NodeInfo* info, NodeDataCollection& collection)
		{
			DataMap::iterator dir = m_dataMap.find(info->conformantDirection.getId());
			if (dir != m_dataMap.end())
			{
				NodeDataMap::iterator node = dir->second.find(info->nodeId);
				if (node != dir->second.end())
				{
					collection = node->second;
					return true;
				}
			}
			return false;
		}
		void DataManager::RemoveData(const std::string& dir, const int& node)
		{
			DataMap::iterator it = m_dataMap.find(dir);
			if (it != m_dataMap.end())
			{
				NodeDataMap::iterator itNDM = it->second.find(node);
				if (itNDM != it->second.end())
				{
					RemoveData(itNDM);
					it->second.erase(node);
				}
			}
		}
		void DataManager::RemoveData(const NodeDataMap::iterator & nodeIt)
		{
			for (NodeDataCollection::iterator info = nodeIt->second.begin(); info != nodeIt->second.end(); ++info)
				delete *info;
			nodeIt->second.clear();
		}
		void DataManager::Start()
		{
			if (!m_enabled)
				return;
			NS_LOG_FUNCTION(Log());
			BehaviourRsu* rsu = (BehaviourRsu*) GetController()->GetBehaviour(BehaviourRsu::Type());
			rsu->TraceConnect("NodeReceiveData", MakeCallback(&DataManager::OnBeaconResponse, this));
			rsu->TraceConnect("NodeTimeOut", MakeCallback(&DataManager::OnTimeOutNode, this));
			rsu->TraceConnect("NodeLastMessage", MakeCallback(&DataManager::OnLastMessageNode, this));
			rsu->TraceConnect("NodeNoLongerConforman", MakeCallback(&DataManager::OnNoLongerConforman, this));
			m_eventExecute = Scheduler::Schedule(ExecuteTime, &DataManager::EventExecute, this);
			const std::vector<VehicleDirection> dirs = rsu->GetDirections();
			for (std::vector<VehicleDirection>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
				m_dataMap[dir->getId()] = NodeDataMap();
			Behaviour::Start();
			m_executeAtThisStep = false;
		}
		void DataManager::Stop()
		{
			NS_LOG_FUNCTION(Log());
			TraceManager* rsu = GetController()->GetBehaviour(BehaviourRsu::Type());
			rsu->TraceDisconnect("NodeReceiveData", MakeCallback(&DataManager::OnBeaconResponse, this));
			rsu->TraceDisconnect("NodeTimeOut", MakeCallback(&DataManager::OnTimeOutNode, this));
			rsu->TraceDisconnect("NodeLastMessage", MakeCallback(&DataManager::OnLastMessageNode, this));
			rsu->TraceDisconnect("NodeNoLongerConforman", MakeCallback(&DataManager::OnNoLongerConforman, this));
			Scheduler::Cancel(m_eventExecute);
			RemoveAll();
			Behaviour::Stop();
		}

		bool DataManager::IsSubscribedTo(ProtocolId pid) const
		{
			//Don't want to receive any message
			return false;
		}
		void DataManager::Receive(server::Payload *payload, double snr)
		{	//Should never be called. Do nothing
		}

		void DataManager::EventExecute()
		{
			m_executeAtThisStep = true;
			m_eventExecute = Scheduler::Schedule(ExecuteTime, &DataManager::EventExecute, this);
		}

		bool DataManager::Execute(DirectionValueMap &data)
		{
			if (m_executeAtThisStep)
			{
				m_executeAtThisStep = false;
				if (m_dataMap.size() == 0)
					return false;
				NS_LOG_FUNCTION(Log() << "Executing protocols");

				bool executed = false;
				for (ProtocolList::iterator protocol = m_protocols.begin(); protocol != m_protocols.end(); ++protocol)
				{
					ExecuteBase* eb = *(protocol);
					if (eb->Execute(data, m_dataMap))
						executed = true;
				}
				//Now remove the marked nodes
				for (RemoveList::iterator it = m_ToRemove.begin(); it != m_ToRemove.end(); ++it)
					RemoveData(it->first, it->second);
				m_ToRemove.clear();
				return executed;
			}
			return false;
		}

		void DataManager::OnBeaconResponse(NodeInfo* info)
		{
			bool newData = AddData(info);
			NS_LOG_FUNCTION((newData ? "true ":"false ")<<info->nodeId<<" "<<info->lastSeen);
		}
		void DataManager::OnNoLongerConforman(NodeInfo* info)
		{
			NodeDataCollection collection;
			if (GetNodeCollection(info, collection))
			{
				NodeInfo* firstMessage = *(collection.begin());
				NodeInfo* lastMessage = *(collection.rbegin());
				//Use the time of the last valid message or of the current invalid one?
				lastMessage->totalTime = lastMessage->lastSeen - firstMessage->lastSeen;
				lastMessage->toRemove = true;
				lastMessage->lastMessage = true;
//			Mark the data for removal on the next execution
				m_ToRemove.push_back(std::make_pair(info->conformantDirection.getId(), info->nodeId));
			}
			delete info;
		}
		void DataManager::OnTimeOutNode(NodeInfo* info)
		{
			RemoveData(info->conformantDirection.getId(), info->nodeId);
			delete info;
		}
		void DataManager::OnLastMessageNode(NodeInfo* info)
		{
			NodeDataCollection collection;
			if (GetNodeCollection(info, collection))
			{
				NodeInfo* firstMessage = *(collection.begin());
				info->totalTime = info->lastSeen - firstMessage->lastSeen;
			} else
				info->totalTime = 0;
			bool newData = AddData(info);
//			Mark the data for removal on the next execution
			m_ToRemove.push_back(std::make_pair(info->conformantDirection.getId(), info->nodeId));
			NS_LOG_FUNCTION((newData ? "true ":"false ")<<info->nodeId<<" "<<info->lastSeen);
		}

	} /* namespace application */
} /* namespace protocol */
