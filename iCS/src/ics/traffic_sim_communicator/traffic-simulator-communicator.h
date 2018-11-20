/****************************************************************************/
/// @file    traffic-simulator-communicator.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef TRAFFIC_SIMULATOR_COMMUNICATOR_H
#define TRAFFIC_SIMULATOR_COMMUNICATOR_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utility>

#include "../../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class ITetrisNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TrafficSimulatorCommunicator
* @brief Contains virtual functions to manage the connection with SUMO.
*/
class TrafficSimulatorCommunicator
{

public:
    virtual ~TrafficSimulatorCommunicator() {};

    /**
    * @brief Creates a socket and establishes the connection with SUMO
    * @return True: If the connection establishes successfully
    * @return False: If it is impossible to establish the connection
    */
    virtual bool Connect() = 0;

    /**
    * @brief Closes the connection with SUMO
    * @return EXIT_SUCCESS if the socket is closed successfully, EXIT_FAILURE otherwise
    */
    virtual int Close() = 0;

    /**
    * @brief Sends a message SUMO in order to start its simulation
    * @param[in] time Timestep in which the simulation of SUMO should start
    * @param[in] vehicles Group of SUMO vehicles
    * @return EXIT_SUCCESS if traffic simulator simulated correctly, EXIT_FAILURE otherwise
    */
    virtual int CommandSimulationStep(int time, std::vector<std::string> &departed, std::vector<std::string> &arrived) = 0;

    /**
    * @brief Sends a command close message
    * @return EXIT_SUCCESS if traffic simulator exited successfully, EXIT_FAILURE otherwise
    */
    virtual int CommandClose() = 0;

    /**
    * @brief Query the simulation steplength of the simulator
    * @return The simulation step length in seconds
    */
    virtual double getSimstepLength() = 0;

    /**
    * @brief Query the simulation time of the simulator
    * @return The simulation time in milliseconds
    */
    virtual int getCurrentTime() = 0;

    /**
    * @brief S6 Sends a message to SUMO in order to establish the value of the maximum speed for a certain vehicle.
    * @param[in] node The node
    * @param[in] maxSeep The value of the maximum speed to assign to the node
    * @return EXIT_SUCCESS if the speed was successfuly updated, EXIT_FAILURE otherwise
    */
    virtual int CommandSetMaximumSpeed(const ITetrisNode &node, float maxSpeed) = 0;

    /**
    * @brief Gets the speed of certain station from SUMO.
    * @param[in,out] &node The node to get information from.
    * @return The value of the speed.
    */
    virtual float GetSpeed(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the direction of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the direction.
    */
    virtual float GetDirection(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the vehicle length of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the vehicle length
    */
    virtual float GetVehicleLength(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the vehicle width of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the vehicle width.
    */
    virtual float GetVehicleWidth(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the vehicle's position.
    * @param[in,out] &node The node to get information from.
    * @return The coordinates correspondig to the node's position
    */
    virtual std::pair<float,float> GetPosition(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the status of the exterior lights.
    * @bug LOW PRIORITY.
    */
    virtual bool GetExteriorLights(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the lane identifier the node is at.
    * @param[in,out] &node The node to get information from.
    * @return The lane identifier
    */
    virtual std::string GetLane(const ITetrisNode &node) = 0;

    /**
    * @brief Gets the type of the vehicle.
    * @param[in,out] &node The node to get information from.
    * @return The type of the vehicle
    */
    virtual std::string GetVehicleType(const ITetrisNode &node) = 0;

    /**
    * @brief Bus Lane Management.
    * Lets one station run in a lane that is "bus-only"
    * @todo to be commented
    */
    virtual int SetVehicleToRunInLane(const ITetrisNode &node, std::string laneId)= 0;

    /**
    * @brief Bus Lane Management.
    * Lets every station to run in a lane that is "bus-only"
    * @todo to be commented
    */
    virtual int SetVehicleToRunInLane(std::string laneId)= 0;

    /**
    * @brief Request Based Personalized Navigation
    *
    * Reroute a vehicle giving it a new route
    * @todo to be commented
    */
    virtual bool ReRoute(const ITetrisNode &node, std::vector<std::string> route)= 0;

    /**
    * @brief Request Based Personalized Navigation
    *
    * Reroute a vehicle forcing it to compute a new route by itself.
    * @todo to be commented
    */
    virtual bool ReRoute(const ITetrisNode &node)= 0;

    /**
    * @brief Changes the states of certain traffic light.
    * @todo to be commented
    */
    virtual bool ChangeTrafficLightStatus(std::string trafficLightId, std::string lightStates) = 0;

    /// @todo To be commented
    virtual bool ChangeEdgeWeight(const ITetrisNode &node, std::string edgeId, float weight) = 0;

    /// @todo To be commented
    virtual bool SetEdgeWeight(std::string edgeId, float weight) = 0;

    /// @todo To be commented
    virtual float GetEdgeWeight(std::string edgeId) = 0;

    /// @todo To be commented
    virtual std::vector<std::string> GetRouteEdges(const ITetrisNode &node) = 0;

    /// @todo To be commented
    virtual std::vector<std::string> GetRouteEdges(std::string routeID) = 0;

    /// @todo To be commented
    virtual int GetTrafficLights(std::vector<ics_types::trafficLightID_t>& trafficLigthIds) = 0;

    /// @todo To be commented
    virtual int GetTrafficLightStatus(ics_types::trafficLightID_t trafficLightId, std::string &state) = 0;

    /// @todo to be commented
    virtual int GetTrafficLightControlledLanes(ics_types::trafficLightID_t trafficLightId, std::vector<std::string> &lanes) = 0;

    /// @todo to be commented
    virtual int GetTrafficLightControlledLinks(ics_types::trafficLightID_t trafficLightId, std::vector<std::vector<std::string> > &lanes) = 0;

    /// @todo to be commented
    virtual int GetLaneLinksConsecutiveLane(std::string laneId, std::vector<std::pair<std::string,std::string> > &consecutiveLanes) = 0;

    /// @todo to be commented
    virtual float GetLaneMaxSpeed(std::string laneId) = 0;

    /// @todo to be commented
    virtual std::string GetVehicleClass(const ITetrisNode &node) = 0;

    /// @todo to be commented
		virtual int TraciCommand(tcpip::Storage & command,tcpip::Storage & result) = 0;

    /**
      * @brief generic Proxy method to forward an unknown external TraCI request to SUMO
      * @param[in,out] &inMsg
      * @param[in,out] &outMsg
      * @todo to be commented
    */
    virtual void controlTraCI(tcpip::Storage &inMsg, tcpip::Storage &outMsg) = 0;

    /// @brief returns the start time of the traffic simulation
    virtual int getStartTime() = 0;
};

}

#endif
