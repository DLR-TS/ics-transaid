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
#ifndef NODE_SAMPLER_H_
#define NODE_SAMPLER_H_

#include "vector.h"
#include "random-variable.h"
#include "trace-manager.h"
#include "scheduler.h"
#include "circular-buffer.h"
#include "ics-interface.h"

namespace baseapp {

struct MobilityInfo;

namespace application {

struct NodeTypeSamplerAttributes {
    NodeTypeSamplerAttributes();
    NodeType nodeType;
    uint16_t resolution;
    double positionRadius;
    double positionVariance;
    double directionVariance;
    double speedError;
};

class NodeSampler: public TraceManager {
public:
    static uint16_t DefaultResolution;
    static uint16_t Quantity;
    static double DefaultPositionRadius;
    static double DefaultPositionVariance;
    static double DefaultDirectionVariance;
    static double MovementThreshold;
    static double SteerFilterThreshold;
    static double DefaultSpeedError;
    static double SpeedBound;
    static double SpeedVariance;
    static uint8_t SteerFilterCount;

    NodeSampler(iCSInterface* controller);
    virtual ~NodeSampler();

    virtual Vector2D GetPosition() const;
    virtual Vector2D GetSpeedVector(int numSamples) const;
    virtual double GetSpeed(int numSamples) const;
    virtual double GetDirection() const;
    uint16_t GetResolution() const;

    void UpdatePosition(MobilityInfo*);

    static bool GetNodeTypeFromString(const char*, NodeType&);
    static void AddNodeTypeSamplerAttributes(NodeTypeSamplerAttributes);

private:

    void OnStateChange(bool active);

    void SetResolution(uint16_t resolution);
    double SteerFilter(double angle);
    virtual void Sample();
    event_id m_eventSample;

    double ComputeDirection() const;
    Vector2D ComputePosition() const;

    // parameters
    uint16_t m_resolution;
    double m_movimentThreshold, m_steerFilterThreshold, m_speedError;
    uint8_t m_steerFilterCount;

    Vector2D m_position;
    Vector2D m_lastPosition;
    double m_lastDirection;
    double m_direction;
    iCSInterface* m_controller;
    TracedCallback<double> m_traceDirection;
    TracedCallback<Vector2D> m_tracePosition;
    uint8_t m_currentSteerFilterCount;
    MobilityInfo* m_currentVehicle;

    // circular buffers
    server::CircularBuffer<Vector2D>* m_positionBuffer;    // position buffer
    server::CircularBuffer<Vector2D>* m_speedBuffer;    // speed buffer
    server::CircularBuffer<double>* m_directionBuffer;      // direction buffer

    // random generators
    ns3::UniformVariable m_positionErrorAngle;
    ns3::NormalVariable m_positionErrorRadius;
    ns3::NormalVariable m_directionErrorAngle;
    ns3::NormalVariable m_speedErrorVariable;

    NodeTypeSamplerAttributes GetNodeTypeSamplerAttributes(NodeType);
    static std::map<NodeType, NodeTypeSamplerAttributes> m_attributeMap;
};

} /* namespace application */
} /* namespace protocol */

#endif /* NODE_SAMPLER_H_ */
