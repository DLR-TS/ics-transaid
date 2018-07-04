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
#ifndef SRC_APPLICATION_MODEL_DATA_MANAGER_H_
#define SRC_APPLICATION_MODEL_DATA_MANAGER_H_

#include "behaviour.h"
#include "scheduler.h"
#include <map>
#include <set>
#include <vector>

namespace protocol
{
	namespace application
	{
		class ExecuteBase;

		struct NodeInfoOrdering
		{
				bool operator()(const NodeInfo* left, const NodeInfo* right);
		};

		/**
		 * Data structures
		 * NodeDataCollection set of information about a node. The last position contains the most recent data
		 * NodeDataMap map of the node information of the nodes for a direction. key is the node id. value is the information for the node
		 * DataMap map of information for the different direction. key is the id of a direction. value is the data known about that direction
		 */
		typedef std::set<NodeInfo*, NodeInfoOrdering> NodeDataCollection;
		typedef std::map<const int, NodeDataCollection> NodeDataMap;
		typedef std::map<const std::string, NodeDataMap> DataMap;

		/**
		 * Class which saves the information about the nodes. Installed only on he rsu
		 * It received the data from the class BehavioutRsu through the traces
		 */
		class DataManager: public Behaviour
		{
			public:
				static uint16_t ExecuteTime;
				static bool EnableCentralizedProtocol;
				static bool Enabled;

				DataManager(iCSInterface*);
				virtual ~DataManager();

				void Start();
				void Stop();
				/**
				 * @brief Always returns false.
				 */
				virtual bool IsSubscribedTo(ProtocolId pid) const;
				/**
				 * @brief Never called.
				 */
				virtual void Receive(server::Payload *payload, double snr);
				/**
				 * @brief Calls every protocol so in can execute
				 */
				virtual bool Execute(DirectionValueMap &data);

				TypeBehaviour GetType() const
				{
					return Type();
				}

				static TypeBehaviour Type()
				{
					return TYPE_DATA_MANAGER;
				}
			private:
				bool AddData(NodeInfo*);
				bool GetNodeCollection(NodeInfo*, NodeDataCollection&);
				void RemoveData(const std::string&, const int&);
				void RemoveData(const NodeDataMap::iterator &);
				void RemoveAll();

				//Events
				/**
				 * @brief Called when a repose message from a node is received
				 */
				void OnBeaconResponse(NodeInfo*);
				/**
				 * @brief Called when a no longer conformant message from a node is received
				 */
				void OnNoLongerConforman(NodeInfo*);
				/**
				 * @brief Called when a node times out
				 */
				void OnTimeOutNode(NodeInfo*);
				/**
				 * @brief Called when the last repose message from a node is received
				 */
				void OnLastMessageNode(NodeInfo*);

				DataMap m_dataMap;

				event_id m_eventExecute;
				void EventExecute();
				bool m_executeAtThisStep;

				typedef std::vector<std::pair<std::string, int> > RemoveList;
				RemoveList m_ToRemove;

				/**
				 * @brief List of protocol that can process the data
				 */
				typedef std::vector<ExecuteBase *> ProtocolList;
				ProtocolList m_protocols;
		};


	} /* namespace application */
} /* namespace protocol */

#endif /* SRC_APPLICATION_MODEL_DATA_MANAGER_H_ */
