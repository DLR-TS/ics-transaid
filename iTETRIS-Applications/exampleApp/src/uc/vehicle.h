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
 * Author Vasilios Karagounis
  ***************************************************************************************/

#ifndef VEHICLE_H
#define VEHICLE_H

#include "headers.h"
#include "UCConfig.h"
#include "vehicleData.h"
#include "ics-interface.h"
#include "uc/jsonReader.h"

using namespace baseapp;
using namespace baseapp::application;

using namespace vehicleData;

//using namespace ucapp;
//using namespace ucapp::application;

namespace ucapp {
namespace application {
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
class Vehicle {

public:
    Vehicle(const std::string& _vehID, iCSInterface* _interface, tracked_vehicle_t& _trackedData);
    virtual ~Vehicle() {};

    static const AutomationType deriveAutomationTypeFromID(const std::string& vehicleID) {
        return AutomationType::AT_UNKNOWN;
    }

    // @brief Decide whether AutomationType is a connected type
    static bool isConnected(AutomationType type);

    // @brief Get vehicle identifier (prefix)
    const std::string& getIdentifier() const;

    // @brief Return whether this vehicle is connected (CV or CAV)
    bool isConnected() const;

    //// getter and setter methods

    AutomationType getAutomationType() const {
        return automationType;
    }

    const std::string& getSumoId() const {
        return vehID;
    }

    bool isInitialised() const {
        return initialized;
    }

    void setTeleported(bool set) {
        teleported = set;
    }
    bool isTeleported() {
        return teleported;
    }

    bool isCAV() const {
        return is_CAV;
    }
    bool isCV() const {
        return is_CV;
    }

    void onAddSubscriptions();
    void execute();

    iCSInterface* getNodeInterface() {
        return nodeController;
    }

    bool setInfo(const int _nodeID, iCSInterface* _nodeController);
    int getNodeId() {
        return nodeID;
    }
    std::string& getVehId() {
        return vehID;
    }

    double getMobilitySpeed() {
        return useMobilityInfo ? currentSpeed : 0.0;
    }

    void receiveMessage(server::Payload* payload, double snr);

    std::string info();
private:
    void initialise();
    void initialiseData();

    // @brief Map SUMO ID of vehicle to AutomationType
    bool findTypesFromID(const std::string& vehicleID);

    void sendCommands();

    void updateExit();
    void setExit();

    void updateTeleportation();

    void updateTrackedVehicle();
    void setTrackedVehicle();

    void updateToC();
    void setDownwardToc();

    double randomDouble(double min, double max);

    void handle_CAM_message(TransaidHeader* header);
    void handle_DENM_message(TransaidHeader* header);

private:
    iCSInterface* iface;
    iCSInterface* nodeController;

    general_t general;
    functions_t functions;
    exit_network_t exit_n;
    toc_t toc;

    execute_ids_t executeId;
    tracked_vehicle_t trackedData;

    std::string vehID;
    AutomationType automationType;
    VehicleType vehicleType;

    std::string vehIdentifier;
    std::string vehIdName;

    bool initialized;
    bool teleported;

    bool is_CAV;
    bool is_CV;

    bool useMobilityInfo;
    bool useNS3Messages;

    double currentXposBegin;
    double currentSpeed;
    int currentLaneIndex;

    int nodeID;
};

} /* namespace application */
} // namespace ucapp

#endif /* VEHICLE_H */
