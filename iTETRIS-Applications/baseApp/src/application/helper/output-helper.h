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
 * Author Enrico Zamagni
 * University of Bologna
 ***************************************************************************************/

#ifndef ITS_OUTPUT_HELPER_H_
#define ITS_OUTPUT_HELPER_H_

#include "headers.h"
#include "ics-interface.h"
#include <fstream>
#include <map>
#include <vector>

namespace baseapp
{
	namespace application
	{

		/*
		 * This class acts like a simple struct and stores
		 * different kinds of node info retrieved during
		 * "Gode Mode" samplings
		 */
		class NodeMeta
		{
			public:

				//  static TypeId GetTypeId (void)
				//  {
				//    static TypeId tid =
				//            TypeId ("ns3::NodeMeta")
				//            .SetParent<Object> ()
				//            .AddConstructor<NodeMeta> ();
				//    return tid;
				//  }

				NodeMeta() :
						sample_enteredRsuRange(false), flagAsEnteredRsuRange(false), flagAsEnteredSector(false)
				{
				}

				//  virtual TypeId GetInstanceTypeId (void) const
				//  {
				//    return GetTypeId ();
				//  }

				void ResetSample()
				{
					sample_enteredRsuRange = false;
					sample_enteredRsuDirection.dir = DIR_INVALID;
				}

				VehicleDirection sample_enteredRsuDirection;
				bool sample_enteredRsuRange;

				bool flagAsEnteredRsuRange, flagAsEnteredSector;
				VehicleDirection enteredRsuDirection;
				NodeType type;
		};

		/**
		 * Class used for logging. One instance per node.
		 * Calls the namesake method in OutputHelper
		 */
		class OutputHelperWrapper
		{
			public:
				OutputHelperWrapper(iCSInterface *);
				virtual ~OutputHelperWrapper();

				void DisconnectCallbacks();
				iCSInterface * GetController();

				void OnPacketSend(server::Payload *);
				void OnPacketReceive(server::Payload *);
				void OnNodeReceiveData(NodeInfo*);
				void OnNodeNoLongerConforman(NodeInfo*);
				void OnNodeSendData(NodeInfo&);
				void OnNodeTimeOut(NodeInfo*);
				void OnNodeLastMessage(NodeInfo*);
				void OnRsuFlowSample(std::vector<std::string>&);
				void OnCurrentDensity(double, int);
				void OnMeanDensity(std::string);
			private:
				void RegisterCallbacks();
				iCSInterface * m_controller;
		};

		/**
		 * Class used to log information and the protocol data
		 */
		class OutputHelper
		{
			public:
				// defaults
				static int SinkDistanceThresholdMin;
				static int SinkDistanceThresholdMax;
				static double SinkOrientationTolerance;
				static int SampleInterval;
				static bool SamplePackets;

				static int Start(std::string outputFile);
				virtual ~OutputHelper();
				static OutputHelper * Instance();
				void RegisterNode(iCSInterface *);
				void RemoveNode(iCSInterface *);
				void AddRsu(iCSInterface *);

				// trace sinks
				void OnPacketSend(iCSInterface *, server::Payload * payload);
				void OnPacketReceive(iCSInterface *, server::Payload * payload);
				void OnNodeReceiveData(iCSInterface *, NodeInfo&);
				void OnNodeNoLongerConforman(iCSInterface *, NodeInfo&);
				void OnNodeSendData(iCSInterface *, NodeInfo&);
				void OnNodeTimeOut(iCSInterface *, NodeInfo&);
				void OnNodeLastMessage(iCSInterface *, NodeInfo&);
				void OnRsuFlowSample(iCSInterface *, std::vector<std::string>&);
				void OnMeanDensity(iCSInterface *, std::string &);
				void OnCurrentDensity(iCSInterface *, double, int);
			private:
				OutputHelper(std::string outputFile);

				static OutputHelper * m_instance;
				std::map<int, OutputHelperWrapper *> m_wrappers;
				std::map<int, NodeMeta *> m_nodeMeta;

				// parameters
				int mp_sink_threshold_min, mp_sink_threshold_max, mp_t_sample;
				bool m_samplePackets;
				double mp_sink_tolerance;

				struct Sector
				{

						Sector(VehicleDirection dir) :
								direction(dir)
						{
							nodeCount = packetCount = 0;
							avgSpeed = 0;
						}
						VehicleDirection direction;
						int nodeCount, packetCount;
						double avgSpeed;
				};

				void Log(iCSInterface *, std::string);
				void OnSimulationStart();
				void OnSimulationEnd();
				/**
				 * @brief Method called periodically. It summarize and logs the real state of the simulation
				 */
				void GMSample();
				void SectorReset();
				NodeMeta* GetMeta(const Node * node, bool create = false);
				// vars
				int m_lastMsTime;
				std::ofstream out;

				//godmode vars
				unsigned int m_maxRsuDensity, m_rsuDensityCount, m_rsuDensityAccum;
				unsigned long m_psent, m_precv;
				std::vector<Sector> m_sectors;

				// rsu infos
				iCSInterface * m_rsuController;
				std::vector<VehicleDirection> m_rsuDirections;

				// utility functions
				std::string InspectHeader(server::Payload * payload);
				static double GetRelativeSpeed(Vector2D first, Vector2D second);
		};

	} /* namespace application */
} /* namespace protocol */

#endif /* ITS_OUTPUT_HELPER_H_ */
