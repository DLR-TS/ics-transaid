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
#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "vehicle.h"

#include <libsumo/TraCIDefs.h>

using namespace baseapp;
using namespace baseapp::application;

namespace ucapp {
namespace application {

struct vehicleData_t {
    std::map<std::string, std::shared_ptr<Vehicle>> vehicles; //
    std::map<std::string, int> rsus;
};

class VehicleManager {
    struct execute_ids_t {
        int departed;
        int arrived;
        int startTeleported;
        int endTeleported;
        int guiOffset;

        void reset() {
            departed = arrived = startTeleported = endTeleported = guiOffset = -1;
        }
    };

public:
    VehicleManager();
    ~VehicleManager();

    void create(const bool use_ns3);
    void initialize(iCSInterface* interface);

    void onAddSubscriptions();
    void execute();

    void addVehicle(const std::string& vehID);
    bool setInfo(const std::string& vehID, const int nodeID, iCSInterface* nodeController);
    int getNodeId(const std::string& vehID);

    const std::string& getVehId(const int nodeID);

    static vehicleData_t& get() {
        return data;
    }
    static VehicleManager& getInstance() {
        return m_instance;
    }

    iCSInterface* getNodeInterface(const std::string& vehID);
    double getMobilitySpeed(const std::string& vehID);

private:
    void findVehicles();
    void findTeleportedVehicles();
    void printInfo();

    void changeScene();
    void handleViews();
    void setGuiOffset();

private:
    execute_ids_t executeId;

    iCSInterface* iface;

    std::map<std::string, tracked_vehicle_t> trackedVehicles;

    libsumo::TraCIPosition offset;
    libsumo::TraCIPosition finalOffset;

    bool offsetRead;
    bool setViewOffset;

    bool optimizeForSpeed;
    bool handleTeleported;
    bool useNS3;
    bool printVehiclesList;

    static vehicleData_t data;
    static VehicleManager m_instance;

};
} // namespace application
} // namespace ucapp
#endif //VEHICLE_MANAGER_H
