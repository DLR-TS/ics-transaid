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

#include "vehicleManager.h"

#include "behaviour-uc-tmc.h"
#include "behaviour-uc-node.h"
#include "program-configuration.h"
#include "current-time.h"
#include "application/model/ics-interface.h"
#include "application/helper/scheduler.h"
#include <app-commands-subscriptions-constants.h>

using namespace baseapp;
using namespace baseapp::application;

const static bool debug(false);

namespace ucapp {
namespace application {
VehicleManager VehicleManager::m_instance;
vehicleData_t VehicleManager::data;

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
VehicleManager::VehicleManager() :
    iface(NULL),
    useNS3(false),
    offsetRead(false),
    setViewOffset(false),
    optimizeForSpeed(false),
    handleTeleported(false) {

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
VehicleManager::~VehicleManager() {

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::create(const bool use_ns3) {
    useNS3 = use_ns3;
    executeId.reset();

    try {
        optimizeForSpeed = JsonReader::get()["optimizeForSpeed"].get<std::string>() == "True";
        handleTeleported = JsonReader::get()["handleTeleported"].get<std::string>() == "True";
        printVehiclesList = JsonReader::get()["printVehiclesList"].get<std::string>() == "True";

        data.rsus = JsonReader::get()["rsus"].get<std::map<std::string, int>>();
    } catch (json::exception& e) {
        std::cout << "Json error in VehicleManager create : " << e.what();
        exit(0);
    }

    if (optimizeForSpeed) {
        printVehiclesList = false;
    }
}

//-------------------------------------------------------------------------------------------------------
// should run once
//-------------------------------------------------------------------------------------------------------
void VehicleManager::initialize(iCSInterface* interface) {
    iface = interface;
    changeScene();
    handleViews();
}

//-------------------------------------------------------------------------------------------------------
// change position or dissapear some pois
//-------------------------------------------------------------------------------------------------------
void VehicleManager::changeScene() {
    if (!iface || optimizeForSpeed) {
        return;
    }

    try {
        std::vector<std::string> pois = JsonReader::get()["removeItems"].get<std::vector<std::string>>();
        for (auto& item : pois) {
            iface->poiRemove(item);

            if (debug) {
                std::cout << "Removing scene item: " << item << std::endl;
            }
        }

        std::unordered_map<std::string, nlohmann::json> reposItems = JsonReader::get()["rePositionItems"].get<std::unordered_map<std::string, nlohmann::json>>();

        for (auto& item : reposItems) {
            double xPos(0.0);
            double yPos(0.0);

            auto it_xpos = item.second.find("xPos");
            if (it_xpos != item.second.end()) {
                xPos = *it_xpos;
            }

            auto it_ypos = item.second.find("yPos");
            if (it_ypos != item.second.end()) {
                yPos = *it_ypos;
            }

            iface->poiSetPosition(item.first, xPos, yPos);

            if (debug) {
                std::cout << "Reposition scene item: " << item << std::endl;
            }
        }
    } catch (json::exception& e) {
        std::cout << "Json error in VehicleManager changeScene : " << e.what() << " Exception id : " << e.id << std::endl;
        exit(0);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::handleViews() {
    if (!iface || optimizeForSpeed) {
        return;
    }
    try {
        std::unordered_map<std::string, nlohmann::json> view0 = JsonReader::get()["view0"].get<std::unordered_map<std::string, nlohmann::json>>();

        offset = {};
        finalOffset = {};
        tracked_vehicle_t tracked;

        for (auto& item : view0) {
            if (item.first == "zoom") {
                double zoom = item.second.get<double>();
                if (zoom != 0.0) {
                    iface->guiSetZoom(viewID, zoom);
                    if (debug) {
                        std::cout << "Setting gui zoom : " << std::to_string(zoom) << std::endl;
                    }
                }
            } else if (item.first == "offsetX") {
                offset.x = item.second.get<double>();
            } else if (item.first == "offsetY") {
                offset.y = item.second.get<double>();
            } else if (item.first == "trackVehicle") {
                std::string vehicle = item.second.get<std::string>();
                if (!vehicle.empty()) {
                    tracked.enable = true;
                    tracked.view = viewID;
                    tracked.vehName = vehicle;
                }
            } else if (item.first == "trackEdge") {
                tracked.edge = item.second.get<std::string>();
            } else if (item.first == "trackPos") {
                tracked.pos = item.second.get<double>();
            } else if (item.first == "trackPosBegin") {
                tracked.posBegin = item.second.get<double>();
            }
        }

        if (!tracked.vehName.empty()) {
            trackedVehicles.insert(std::make_pair(tracked.vehName, tracked));
            if (debug) {
                std::cout << "Tracked vehicle : " << tracked.vehName << " on edge : " << tracked.edge << ", pos : " << std::to_string(tracked.pos) << std::endl;
            }
        }

        if (offset.x != 0.0 || offset.y != 0.0) {
            executeId.guiOffset = iface->guiGetOffset(viewID);
            setViewOffset = true;
        }
    } catch (json::exception& e) {
        std::cout << "Json error in VehicleManager handleViews : " << e.what() << " Exception id : " << e.id << std::endl;
        exit(0);
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::setGuiOffset() {
    if (!iface || optimizeForSpeed) {
        return;
    }

    if (setViewOffset) {
        if (offsetRead) {
            iface->guiSetOffset(viewID, finalOffset.x, finalOffset.y);
            setViewOffset = false;

            if (debug) {
                std::cout << "Setting gui offset,  x : " << std::to_string(finalOffset.x) << ", y : " << std::to_string(finalOffset.y) << std::endl;
            }
        } else {
            if (executeId.guiOffset != -1) {
                auto& response = iface->getTraCIResponse(executeId.guiOffset);

                if (response.second != nullptr) {
                    finalOffset.x = std::dynamic_pointer_cast<libsumo::TraCIPosition>(response.second)->x + offset.x;
                    finalOffset.y = std::dynamic_pointer_cast<libsumo::TraCIPosition>(response.second)->y + offset.y;

                    iface->guiSetOffset(viewID, finalOffset.x, finalOffset.y);
                    setViewOffset = false;
                    offsetRead = true;

                    if (debug) {
                        std::cout << "Setting gui offset,  x : " << std::to_string(finalOffset.x) << ", y : " << std::to_string(finalOffset.y) << std::endl;
                    }
                } else {
                    std::cout << "Error reading gui offset of " << viewID << std::endl;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::onAddSubscriptions() {
    if (!iface) {
        return;
    }

    executeId.departed = iface->getDepartedIDList();
    executeId.arrived = iface->getArrivedIDList();

    if (handleTeleported) {
        executeId.startTeleported = iface->getStartingTeleportIDList();
        executeId.endTeleported = iface->getEndingTeleportIDList();
    }

    for (auto& vehID : data.vehicles) {
        vehID.second->onAddSubscriptions();
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::execute() {
    if (!iface) {
        return;
    }

    findVehicles();
    findTeleportedVehicles();

    printInfo();

    for (auto& vehID : data.vehicles) {
        vehID.second->execute();
    }

    setGuiOffset();

    executeId.reset();
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::findVehicles() {
    if (!iface) {
        return;
    }

    if (executeId.departed != -1) {
        auto& responseDeparted = iface->getTraCIResponse(executeId.departed);

        if (responseDeparted.second != nullptr) {
            const std::vector<std::string>& departedVehIDs = std::dynamic_pointer_cast<libsumo::TraCIStringList>(responseDeparted.second)->value;

            bool newDepartures = false;
            for (auto& vehID : departedVehIDs) {
                auto it = data.vehicles.find(vehID);
                if (it == data.vehicles.end()) {
                    tracked_vehicle_t trackedData;
                    for (auto& tracked : trackedVehicles) {
                        if (tracked.first == vehID) {
                            trackedData = tracked.second;
                            break;
                        }
                    }

                    data.vehicles.insert(std::pair<std::string, std::shared_ptr<Vehicle>>(vehID, new Vehicle(vehID, iface, trackedData)));

                    if (debug) {
                        if (!newDepartures) {
                            std::cout << "Vehicles added : ";
                        } else {
                            std::cout << ",";
                        }

                        std::cout << vehID;
                        newDepartures = true;
                    }
                }
            }

            if (newDepartures) {
                std::cout << std::endl;
            }
        } else {
            std::cout << "Error reading departed vehicles." << std::endl;
        }
    }

    if (executeId.arrived != -1) {
        auto& responseArrived = iface->getTraCIResponse(executeId.arrived);

        if (responseArrived.second != nullptr) {
            const std::vector<std::string>& arrivedVehIDs = std::dynamic_pointer_cast<libsumo::TraCIStringList>(responseArrived.second)->value;

            bool newArrivals = false;

            for (auto& vehID : arrivedVehIDs) {
                auto it = data.vehicles.find(vehID);

                if (it != data.vehicles.end()) {
                    data.vehicles.erase(it);

                    if (offsetRead) {
                        for (auto& tracked : trackedVehicles) {
                            if (tracked.first == vehID) {
                                setViewOffset = true;
                                break;
                            }
                        }
                    }

                    if (debug) {
                        if (!newArrivals) {
                            std::cout << "Vehicles removed : ";
                        } else {
                            std::cout << ",";
                        }

                        std::cout << vehID;
                        newArrivals = true;
                    }
                    break;
                }
            }
            if (newArrivals) {
                std::cout << std::endl;
            }
        } else {
            std::cout << "Error reading arrived vehicles." << std::endl;
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::addVehicle(const std::string& vehID) {
    auto it = data.vehicles.find(vehID);
    if (it == data.vehicles.end()) {
        tracked_vehicle_t trackedData;
        for (auto& tracked : trackedVehicles) {
            if (tracked.first == vehID) {
                trackedData = tracked.second;
                break;
            }
        }

        data.vehicles.insert(std::pair<std::string, std::shared_ptr<Vehicle>>(vehID, new Vehicle(vehID, iface, trackedData)));

        if (debug) {
            std::cout << "Vehicles added : " << vehID << std::endl;
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::findTeleportedVehicles() {
    if (!iface) {
        return;
    }

    if (handleTeleported) {
        if (executeId.startTeleported != -1) {
            auto& startingTeleported = iface->getTraCIResponse(executeId.startTeleported);

            if (startingTeleported.second != nullptr) {
                const std::vector<std::string>& startingList = std::dynamic_pointer_cast<libsumo::TraCIStringList>(startingTeleported.second)->value;

                bool newTeleported = false;
                for (auto& vehID : startingList) {
                    auto it = data.vehicles.find(vehID);
                    if (it != data.vehicles.end()) { //found
                        if (!it->second->isTeleported()) {
                            it->second->setTeleported(true);

                            if (debug) {
                                if (!newTeleported) {
                                    std::cout << "Vehicles start teleportation : ";
                                } else {
                                    std::cout << ",";
                                }

                                std::cout << vehID;
                                newTeleported = true;
                            }
                        }
                    }
                }

                if (newTeleported) {
                    std::cout << std::endl;
                }
            } else {
                std::cout << "Error reading staring teleporting vehicles." << std::endl;
            }
        }

        if (executeId.endTeleported != -1) {
            auto& endingTeleported = iface->getTraCIResponse(executeId.endTeleported);

            if (endingTeleported.second != nullptr) {
                const std::vector<std::string>& endingList = std::dynamic_pointer_cast<libsumo::TraCIStringList>(endingTeleported.second)->value;

                bool endTeleported = false;

                for (auto& vehID : endingList) {
                    auto it = data.vehicles.find(vehID);
                    if (it != data.vehicles.end()) { //found
                        if (it->second->isTeleported()) {
                            it->second->setTeleported(false);

                            if (debug) {
                                if (!endTeleported) {
                                    std::cout << "Vehicles finish teleportation : ";
                                } else {
                                    std::cout << ",";
                                }

                                std::cout << vehID;
                                endTeleported = true;
                            }
                        }
                    }
                }
                if (endTeleported) {
                    std::cout << std::endl;
                }
            } else {
                std::cout << "Error reading ending teleporting vehicles." << std::endl;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool VehicleManager::setInfo(const std::string& vehID, const int nodeID, iCSInterface* controller) {
    auto it = data.vehicles.find(vehID);
    if (it != data.vehicles.end()) {
        return it->second->setInfo(nodeID, controller);
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
iCSInterface* VehicleManager::getNodeInterface(const std::string& vehID) {
    auto it = data.vehicles.find(vehID);
    if (it != data.vehicles.end()) {
        return it->second->getNodeInterface();
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
int VehicleManager::getNodeId(const std::string& vehID) {
    auto it = data.vehicles.find(vehID);
    if (it != data.vehicles.end()) {
        return it->second->getNodeId();
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
const std::string& VehicleManager::getVehId(const int nodeID) {
    std::map<std::string, std::shared_ptr<Vehicle>>::iterator it = data.vehicles.begin();

    for (std::pair<std::string, std::shared_ptr<Vehicle>> element : data.vehicles) {
        if (element.second->getNodeId() == nodeID) {
            return element.second->getVehId();
        }
    }

    return emptyString;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
double VehicleManager::getMobilitySpeed(const std::string& vehID) {
    auto it = data.vehicles.find(vehID);
    if (it != data.vehicles.end()) {
        return it->second->getMobilitySpeed();
    }

    return 0.0;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void VehicleManager::printInfo() {
    if (printVehiclesList) {
        std::map<std::string, std::shared_ptr<Vehicle>>::iterator it = data.vehicles.begin();

        for (std::pair<std::string, std::shared_ptr<Vehicle>> element : data.vehicles) {
            if (element.second->getNodeId() != -1) {
                std::cout << element.second->getVehId() << " : " << element.second->getNodeId() << element.second->info() << std::endl;
            }
        }
    }
}

} // namespace application
} // namespace ucapp


