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
/****************************************************************************************
 * Author Federico Caselli <f.caselli@unibo.it>
 * Author Enrico Zamagni
 * University of Bologna
 ***************************************************************************************/

#ifndef ITS_OUTPUT_HELPER_H_
#define ITS_OUTPUT_HELPER_H_

#include "headers.h"
#include "ics-interface.h"
#include "protocols.h"
#include <fstream>
#include <map>
#include <vector>

namespace protocol {
namespace application {

/*
 * This class acts like a simple struct and stores
 * different kinds of node info retrieved during
 * "Gode Mode" samplings
 */
class NodeMeta {
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
        sample_enteredRsuRange(false), flagAsEnteredRsuRange(false), flagAsEnteredSector(false) {
    }

    //  virtual TypeId GetInstanceTypeId (void) const
    //  {
    //    return GetTypeId ();
    //  }

    void ResetSample() {
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
class OutputHelperWrapper {
public:
    OutputHelperWrapper(iCSInterface*);
    virtual ~OutputHelperWrapper();

    void DisconnectCallbacks();
    iCSInterface* GetController();

    void OnPacketSend(server::Payload*);
    void OnPacketReceive(server::Payload*);
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
    iCSInterface* m_controller;
};

/**
 * Class used to log information and the protocol data
 */
class OutputHelper {
public:
    // defaults
    static int SinkDistanceThresholdMin;
    static int SinkDistanceThresholdMax;
    static double SinkOrientationTolerance;
    static int SampleInterval;
    static bool SamplePackets;

    static int Start(std::string outputFile);
    virtual ~OutputHelper();
    static OutputHelper* Instance();
    void RegisterNode(iCSInterface*);
    void RemoveNode(iCSInterface*);
    void AddRsu(iCSInterface*);

    // trace sinks
    void OnPacketSend(iCSInterface*, server::Payload* payload);
    void OnPacketReceive(iCSInterface*, server::Payload* payload);
    void OnNodeReceiveData(iCSInterface*, NodeInfo&);
    void OnNodeNoLongerConforman(iCSInterface*, NodeInfo&);
    void OnNodeSendData(iCSInterface*, NodeInfo&);
    void OnNodeTimeOut(iCSInterface*, NodeInfo&);
    void OnNodeLastMessage(iCSInterface*, NodeInfo&);
    void OnRsuFlowSample(iCSInterface*, std::vector<std::string>&);
    void OnMeanDensity(iCSInterface*, std::string&);
    void OnCurrentDensity(iCSInterface*, double, int);
private:
    OutputHelper(std::string outputFile);

    static OutputHelper* m_instance;
    std::map<int, OutputHelperWrapper*> m_wrappers;
    std::map<int, NodeMeta*> m_nodeMeta;

    // parameters
    int mp_sink_threshold_min, mp_sink_threshold_max, mp_t_sample;
    bool m_samplePackets;
    double mp_sink_tolerance;

    struct Sector {

        Sector(VehicleDirection dir) :
            direction(dir) {
            nodeCount = packetCount = 0;
            avgSpeed = 0;
        }
        VehicleDirection direction;
        int nodeCount, packetCount;
        double avgSpeed;
    };

    void Log(iCSInterface*, std::string);
    void OnSimulationStart();
    void OnSimulationEnd();
    /**
     * @brief Method called periodically. It summarize and logs the real state of the simulation
     */
    void GMSample();
    void SectorReset();
    NodeMeta* GetMeta(const Node* node, bool create = false);
    // vars
    int m_lastMsTime;
    std::ofstream out;

    //godmode vars
    unsigned int m_maxRsuDensity, m_rsuDensityCount, m_rsuDensityAccum;
    unsigned long m_psent, m_precv;
    std::vector<Sector> m_sectors;

    // rsu infos
    iCSInterface* m_rsuController;
    std::vector<VehicleDirection> m_rsuDirections;

    // utility functions
    std::string InspectHeader(server::Payload* payload);
    static double GetRelativeSpeed(Vector2D first, Vector2D second);
};

} /* namespace application */
} /* namespace protocol */

#endif /* ITS_OUTPUT_HELPER_H_ */
