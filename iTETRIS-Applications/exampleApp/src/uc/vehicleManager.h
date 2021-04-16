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
 * Copyright (c) 2020 Centre for Research and Technology-Hellas (CERTH)
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
 * CERTH and its contributors''.
 * 4. Neither the name of the Centre nor the names of its contributors may be used to
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
