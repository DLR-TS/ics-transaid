/*
 * BaseVehicle.h
 *
 *  Created on: Feb 14, 2020
 *      Author: leo, matthias
 */

#ifndef SRC_APPLICATION_MODEL_BASEVEHICLE_H_
#define SRC_APPLICATION_MODEL_BASEVEHICLE_H_

#include <map>
#include <set>
#include <vector>
#include <memory>
#include "application/model/headers.h"

#define DEBUG_BASEVEHICLE false

/// Parameters for handling uncertainties in vehicle states
// Factor by which the extrapolated acceleration for CAVs since their last V2X
// state update is damped
// @see BaseVehicle::extrapolateState()
#define ACCEL_ESTIMATION_DAMPING 0.75

namespace baseapp {
namespace application {

enum AutomationType {
    AT_UNKNOWN = 0,
    AT_MANUAL,
    AT_CV,
    AT_CAV,
    AT_TOR_ISSUED
};

/// @brief map: vType identifier -> AutomationType
static std::map<std::string, AutomationType> vTypeAutomationTypeMap = {
    {"CAV", AT_CAV},
    {"CAVGA", AT_CAV},
    {"CAVGB", AT_CAV},
    {"CV", AT_CV},
    {"LV", AT_MANUAL},
    {"HGV", AT_MANUAL},
    {"LGV", AT_MANUAL}
};

/// @brief Available ToC lead times for the different vehicle types
/// map: identifier -> double
static std::map<AutomationType, double> ToC_lead_times = {
    {AT_CAV, 10.0},
    {AT_CV, 1.5},
    {AT_MANUAL, -1.0}
};

class BaseVehicle {
public:

    BaseVehicle();
    BaseVehicle(double pos, double speed, double accel, int laneIndex,
                double length, double minGap, const std::string& sumoID, int icsID,
                AutomationType automationType);
    BaseVehicle(const std::string& sumoID, int icsID);
    virtual ~BaseVehicle(){};

    // Returns the positive front to backbumper distance between this and the
    // given leader if applicable, -1 otherwise.
    // @note: this assumes that both vehicles are on the same edge to avoid
    // complicated calculations or a delayed result by waiting for response
    // from a traci.getDrivingDistance() call.
    double getSpacing(const std::shared_ptr<BaseVehicle> leader) const;

    /// @brief Update the vehicle state according to the received CAM info
    void updateState(int currentTime, const baseapp::application::TransaidHeader::CamInfo* info);

    /// @brief Estimates the current state at time t based on the last received CAM
    /// @note Uses a very rough estimation assuming exponentially damped
    /// acceleration since the last V2X reception
    /// @see ACCEL_ESTIMATION_DAMPING
    void extrapolateState(int currentTime);

    // State variables that represent the current estimated state
    double xPosition;
    double speed;
    double accel;
    int lastExtrapolationTime;
    // State variables retrieved from a V2X message at @lastUpdateTime
    double xPosition_received;
    double speed_received;
    double accel_received;
    int lastUpdateTime;
    // automation related state flag
    AutomationType automationType;
    // "Static" vehicle properties
    int laneIndex;
    double minGap;
    double length;
    std::string sumoID;
    int icsID;
    // whether the vehicles static properties are initialized
    bool initialInformationReceived;
};

} /* namespace application */
} /* namespace baseapp */

#endif /* SRC_APPLICATION_MODEL_BASEVEHICLE_H_ */
