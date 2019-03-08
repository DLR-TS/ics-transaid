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
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include "node-sampler.h"
#include <cmath>
#include "log/log.h"
#include "structs.h"

namespace baseapp
{
	namespace application
	{

		NodeTypeSamplerAttributes::NodeTypeSamplerAttributes()
		{
			resolution = NodeSampler::DefaultResolution;
			positionRadius = NodeSampler::DefaultPositionRadius;
			positionVariance = NodeSampler::DefaultPositionVariance;
			directionVariance = NodeSampler::DefaultDirectionVariance;
			speedError = NodeSampler::DefaultSpeedError;
		}

		uint16_t NodeSampler::DefaultResolution = 1000;
		uint16_t NodeSampler::Quantity = 5;
		double NodeSampler::DefaultPositionRadius = 20;
		double NodeSampler::DefaultPositionVariance = 1.8;
		double NodeSampler::DefaultDirectionVariance = 0.002;
		double NodeSampler::MovementThreshold = 2;
		double NodeSampler::SteerFilterThreshold = 20;
		uint8_t NodeSampler::SteerFilterCount = 3;
		double NodeSampler::DefaultSpeedError = 0.15;
		double NodeSampler::SpeedBound = 2;
		double NodeSampler::SpeedVariance = 0.25;

		std::map<NodeType, NodeTypeSamplerAttributes> NodeSampler::m_attributeMap;

		NodeSampler::NodeSampler(iCSInterface * controller)
		{
			m_currentVehicle = NULL;
			m_controller = controller;
			m_direction = m_lastDirection = DIR_INVALID;
			m_position = Vector2D(-1, -1);
			m_currentSteerFilterCount = 0;
			m_eventSample = 0;
			m_positionBuffer = new server::CircularBuffer<Vector2D>(Quantity);
			//It has to be the same value of the High speed average
			m_speedBuffer = new server::CircularBuffer<Vector2D>(iCSInterface::AverageSpeedSampleHigh);
			m_directionBuffer = new server::CircularBuffer<double>(Quantity);
			m_positionErrorAngle = ns3::UniformVariable((double) -1 * M_PI, (double) M_PI);
			m_speedErrorVariable = ns3::NormalVariable(0, SpeedVariance, SpeedBound);
			m_movimentThreshold = MovementThreshold;
			m_steerFilterThreshold = SteerFilterThreshold;
			m_steerFilterCount = SteerFilterCount;

			//Set specific attributes for the type
			NodeTypeSamplerAttributes typeAttribute = GetNodeTypeSamplerAttributes(controller->GetNodeType());
			m_directionErrorAngle = ns3::NormalVariable(0, typeAttribute.directionVariance, (double) M_PI_2);
			m_positionErrorRadius = ns3::NormalVariable(0, typeAttribute.positionVariance, typeAttribute.positionRadius);
			m_resolution = typeAttribute.resolution;
			m_speedError = typeAttribute.speedError;

			RegisterTrace("Direction", m_traceDirection);
			RegisterTrace("Position", m_tracePosition);

			controller->TraceConnect("StateChange", MakeCallback(&NodeSampler::OnStateChange, this));
		}

		NodeSampler::~NodeSampler()
		{
			delete m_positionBuffer;
			delete m_directionBuffer;
			delete m_speedBuffer;
			m_directionBuffer = NULL;
			m_speedBuffer = NULL;
			m_positionBuffer = NULL;
			Scheduler::Cancel(m_eventSample);
		}

		/*
		 * GETTERS & SETTERS
		 */
		Vector2D NodeSampler::GetPosition() const
		{
			return m_position;
		}

		double NodeSampler::GetSpeed(int numSamples) const
		{
			Vector2D speed = GetSpeedVector(numSamples);
			//multiply is faster than pow
			return sqrt((speed.x * speed.x) + (speed.y * speed.y));
		}

		Vector2D NodeSampler::GetSpeedVector(int numSamples) const
		{
			if (m_speedBuffer->size() == 0 || numSamples < 1)
				return Vector2D();
			if (m_speedBuffer->size() < numSamples)
				numSamples = m_speedBuffer->size();
			// compute vector mean
			double x = 0, y = 0;
			for (unsigned int i = 0; i < numSamples; i++)
			{
				x += m_speedBuffer->at(i).x;
				y += m_speedBuffer->at(i).y;
			}
			x /= (double) numSamples;
			y /= (double) numSamples;

			return Vector2D(x, y);
		}

		double NodeSampler::GetDirection() const
		{
			return m_direction;
		}

		uint16_t NodeSampler::GetResolution() const
		{
			return m_resolution;
		}

		void NodeSampler::SetResolution(uint16_t resolution)
		{
			m_resolution = resolution;
		}

		void NodeSampler::OnStateChange(bool active)
		{
			if (active)
			{
				m_lastPosition = Vector2D();
				m_lastDirection = DIR_INVALID;
				m_eventSample = Scheduler::Schedule(m_resolution, &NodeSampler::Sample, this);
			} else
			{
				m_positionBuffer->clear();
				m_speedBuffer->clear();
				m_directionBuffer->clear();
				Scheduler::Cancel(m_eventSample);
			}
		}

		double NodeSampler::ComputeDirection() const
// reference: http://en.wikipedia.org/wiki/Mean_of_circular_quantities
		{
			static const double RAD2DEG = 180.0f / M_PI;
			//std::ostringstream debug; ///////////////////
			//debug << Simulator::Now().GetSeconds() << " ("; ////////////////////////

			double avg_x = 0, avg_y = 0;
			unsigned int nSamples = m_directionBuffer->size();
			if (nSamples == 0)
			{
				// no valid sample:
				// impossible to compute direction
				return DIR_INVALID;
			}

			for (unsigned int i = 0; i < m_directionBuffer->size(); i++)
			{
				double angle = m_directionBuffer->at(i);
				// discard invalid samples
				if (angle == DIR_INVALID)
				{
					//debug << "nan, "; //////////////////////
					nSamples--;
					continue;
				}

				//debug << angle * RAD2DEG << "°, ";//////////////////////
				avg_x += cos(angle);
				avg_y += sin(angle);
			}
			//debug << ") =";//////////////////////

			if (nSamples == 0)
				//Division by 0 otherwise...
				return DIR_INVALID;
			avg_x /= (double) nSamples;
			avg_y /= (double) nSamples;

			//double angl = atan2(avg_y, avg_x) * RAD2DEG; ////////////////
			//debug << angl << "°"; ///////////////////////////////////////
			//if (m_id == 71)
			//    NS_LOG_UNCOND (debug.str());

			return atan2(avg_y, avg_x) * RAD2DEG;
		}

		Vector2D NodeSampler::ComputePosition() const
		{
			if (m_positionBuffer->size() == 0)
				return Vector2D(-1, -1);

			/*
			 // compute vector mean
			 double x = 0, y = 0;
			 for (unsigned int i = 0; i < m_pb.size (); i++)
			 {
			 x += m_pb.at (i).x;
			 y += m_pb.at (i).y;
			 }
			 x /= (double) m_pb.size ();
			 y /= (double) m_pb.size ();

			 return Vector2D (x, y);
			 */
			return m_positionBuffer->front();
		}

		double NodeSampler::SteerFilter(double sample)
		{
			static const double RAD2DEG = 180.0f / M_PI;
			static const double DEG2RAD = M_PI / 180.0f;
			if (m_lastDirection == DIR_INVALID || sample == DIR_INVALID)
				return sample;

			double sampleDEG = sample * RAD2DEG;
			double diff = AngleDifference(m_lastDirection, sampleDEG);
			if (std::abs(diff) > m_steerFilterThreshold)
			{
				// sample is over threshold
				if (m_currentSteerFilterCount > m_steerFilterCount)
				{
					m_currentSteerFilterCount = 0;
					m_directionBuffer->clear();
					return sample;
				} else
				{
					m_currentSteerFilterCount++;
					return m_lastDirection * DEG2RAD;
				}
			} else if (m_currentSteerFilterCount)
			{
				m_currentSteerFilterCount--;
			}
			return sample;
		}

		void NodeSampler::UpdatePosition(MobilityInfo* vehicle)
		{
			m_currentVehicle = vehicle;
		}

		std::string formatInvalid(double val, bool conv = true)
		{
			if (val == DIR_INVALID)
				return "DIR_INVALID";
			std::ostringstream oss;
			if (conv)
				oss << val * 180.0 / M_PI;
			else
				oss << val;
			return oss.str();
		}

		void NodeSampler::Sample()
		{
			NS_LOG_FUNCTION(m_controller->NodeName() << ": NodeSampler: ");
			// schedule next sampling
			m_eventSample = Scheduler::Schedule(m_resolution, &NodeSampler::Sample, this);

			if (m_currentVehicle == NULL)
			{
				NS_LOG_WARN(m_controller->NodeName() << ": Current vehicle is NULL");
				return;
			}
			// sample current speed & position
			Vector2D position = m_currentVehicle->position;
			//MYCOM could modify the function to avoid this conversion
			//Add error to speed value, only if not 0
			double spd = m_currentVehicle->speed;
			if (spd != 0)
				spd += m_speedError * m_speedErrorVariable.GetValue();
			//Get the speed components from module and angle
			double speed_x = (double) spd * cos((double) m_currentVehicle->direction * M_PI / 180.0);
			double speed_y = (double) spd * sin((double) m_currentVehicle->direction * M_PI / 180.0);
			Vector2D speed(speed_x, speed_y);

			// store speed sample. Need to be stored here otherwise the speed will not go to zero when the vehicle has stopped
			m_speedBuffer->push_front(speed);

			// sample direction
			double direction;
			if (std::abs(speed.x) < 0.01 && std::abs(speed.y) < 0.01)
			{
				// movement speed is too low: direction is unreliable
				direction = DIR_INVALID;
				// if present, we recycle the previous direction sample
				if (m_directionBuffer->size() > 0)
					direction = m_directionBuffer->front();
			} else
				direction = atan2(speed.y, speed.x);

			// compute real distance between previous sample


			double dist = std::sqrt(pow(m_lastPosition.x - position.x, 2) + pow(m_lastPosition.y - position.y, 2));
			/* Commented by acorrea to update more frequently the position
			if (dist < m_movimentThreshold)
			{
				// the node didn't move enough:
				// discard this sample
				NS_LOG_DEBUG(
						m_controller->NodeName() << ": sample dropped. The node didn't move enough: " << dist << " . Min is: " << m_movimentThreshold);
				return;
			}
			*/

			// compute position error
			double radius = m_positionErrorRadius.GetValue();
			double angle = m_positionErrorAngle.GetValue();
			Vector2D position_err = Vector2D(position.x + radius * std::cos(angle), position.y + radius * std::sin(angle));
			// compute direction error
			double direction_err = DIR_INVALID;
			if (direction != DIR_INVALID)
				direction_err = direction + m_directionErrorAngle.GetValue();

			// apply steer filter
			direction_err = SteerFilter(direction_err);

			// store position & direction samples
			m_positionBuffer->push_front(position_err);
			m_directionBuffer->push_front(direction_err);
			m_lastPosition = position;

			// update means
			m_direction = ComputeDirection();
			m_traceDirection(m_direction);
			m_position = ComputePosition();
			m_tracePosition(m_position);
			m_lastDirection = m_direction;

			NS_LOG_INFO(
					m_controller->NodeName() << ": pos=(" << position_err.x << ", " << position_err.y << ") spd=(" << speed.x << ", " << speed.y << ") avgDir=" << formatInvalid(m_direction, false) << " dir=" << formatInvalid(direction_err));
			NS_LOG_DEBUG(
					m_controller->NodeName() << ": real: dir= " << formatInvalid(direction) << " pos=(" << position.x << ", " << position.y << ") dist= " << dist);
		}

		bool NodeSampler::GetNodeTypeFromString(const char* typeChar, NodeType & nodeType)
		{
			const std::string type = std::string(typeChar);
			if (type == "full" || type == "FULL" || type == "Full")
			{
				nodeType = NT_VEHICLE_FULL;
				return true;
			}
			if (type == "medium" || type == "Medium" || type == "MEDIUM" || type == "med" || type == "Med" || type == "MED")
			{
				nodeType = NT_VEHICLE_MEDIUM;
				return true;
			}
			return false;
		}
		void NodeSampler::AddNodeTypeSamplerAttributes(NodeTypeSamplerAttributes attributes)
		{
			m_attributeMap.insert(std::make_pair(attributes.nodeType, attributes));
		}
		NodeTypeSamplerAttributes NodeSampler::GetNodeTypeSamplerAttributes(NodeType type)
		{
			std::map<NodeType, NodeTypeSamplerAttributes>::iterator it = m_attributeMap.find(type);
			if (it == m_attributeMap.end())
				return NodeTypeSamplerAttributes();
			return it->second;
		}

	} /* namespace application */
} /* namespace protocol */
