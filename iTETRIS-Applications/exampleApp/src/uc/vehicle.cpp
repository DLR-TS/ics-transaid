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

#include "vehicle.h"
#include "vehicleManager.h"
#include <app-commands-subscriptions-constants.h>
#include "current-time.h"

namespace ucapp {
namespace application {
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
Vehicle::Vehicle(const std::string& _vehID, iCSInterface* _interface, tracked_vehicle_t& _trackedData) : //
    vehID(_vehID),
    iface(_interface),
    trackedData(_trackedData),
    vehicleType(VehicleType::VEH_UNKNOWN),
    automationType(AutomationType::AT_UNKNOWN),
    initialized(false),
    useMobilityInfo(false),
    useNS3Messages(false),
    vehIdName(""),
    currentXposBegin(0.0),
    currentSpeed(0.0),
    currentLaneIndex(-1),
    is_CAV(false),
    is_CV(false),
    nodeID(-1),
    nodeController(NULL) {

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::initialise() {
    executeId.reset();

    if (findTypesFromID(vehID)) {
        try {
            general.optimizeForSpeed = JsonReader::get()["optimizeForSpeed"].get<std::string>() == "True";

            general.debugMain = JsonReader::get()["mainDebug"].get<std::string>() == "True";
            general.debugToc = JsonReader::get()["tocDebug"].get<std::string>() == "True";

            general.debugCamMessages = JsonReader::get()["camMessageDebug"].get<std::string>() == "True";
            general.debugDenmMessages = JsonReader::get()["denmMessageDebug"].get<std::string>() == "True";

            useMobilityInfo = JsonReader::get()["useMobilityInfo"].get<std::string>() == "True";
            useNS3Messages = JsonReader::get()["useNS3Messages"].get<std::string>() == "True";

            if (useNS3Messages) {
                useMobilityInfo = true;
            }

            general.handleTeleported = JsonReader::get()["handleTeleported"].get<std::string>() == "True";

            functions.isTocEnable = JsonReader::get()[vehIdName]["tocEnable"].get<std::string>() == "True";
            functions.isExitEnable = JsonReader::get()[vehIdName]["exitEnable"].get<std::string>() == "True";

            //-----------------------------
            // exit point
            //-----------------------------
            if (functions.isExitEnable) {
                exit_n.pos = JsonReader::get()[vehIdName]["exitPos"].get<double>();
                exit_n.edge = JsonReader::get()[vehIdName]["exitEdge"].get<std::string>();
                exit_n.posBegin = JsonReader::get()[vehIdName]["exitPosBegin"].get<double>();
                exit_n.done = false;
            }

            //-----------------------------
            // Transition of Control - toc - Take Over Control
            //-----------------------------
            if (functions.isTocEnable) {
                toc.probability = JsonReader::get()[vehIdName]["tocProbability"].get<double>();
                toc.leadTime = JsonReader::get()[vehIdName]["tocLeadTime"].get<double>();
                toc.downwardEdge = JsonReader::get()[vehIdName]["tocDownwardEdge"].get<std::string>();
                toc.downwardPos = JsonReader::get()[vehIdName]["tocDownwardPos"].get<double>();
                toc.downwardPosBegin = JsonReader::get()[vehIdName]["tocDownwardPosBegin"].get<double>();
                toc.downwardFromRsuDenm = JsonReader::get()[vehIdName]["tocDownwardFromRsuDenm"].get<int>();
                toc.downwardDenmHighlight = JsonReader::get()[vehIdName]["tocDownwardDenmHighlight"].get<std::string>()  == "True";

                toc.downwardTOCDone = false;
                toc.downwardFromRsuDone = false;
            }
        } catch (json::exception& e) {
            std::cout << "Vehicle : " << vehID << " Json error : " << e.what() << '\n'
                      << " Exception id : " << e.id << std::endl;

            exit(0);
        }

        if (general.debugMain) {
            general.debugToc = general.debugCamMessages = general.debugDenmMessages =  true;
        }

        if (general.optimizeForSpeed) {
            general.debugMain = false;
            general.debugToc = general.debugCamMessages = general.debugDenmMessages = false;
            toc.downwardDenmHighlight = false;
        }

        initialized = true;
        if (general.debugMain) {
            std::cout << "Vehicle created : " << vehID << std::endl;
        }
    } else {
        if (general.debugMain) {
            std::cout << "Error: Vehicle " << vehID << " not initialized." << std::endl;
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool Vehicle::findTypesFromID(const std::string& vehicleID) {
    if (vehicleID.empty()) {
        return false;
    }

    for (std::pair<std::string, AutomationType> pair : AutomationTypeIdentifierMap) {
        if (pair.first.length() == 0) {
            std::cout << "WARNING Vehicle::findTypesFromID AutomationType " << pair.first << " maps to empty string!" << std::endl;
            continue;
        }

        // vehicleID starts with identifier
        if (vehicleID.find(pair.first) != std::string::npos) {
            vehIdentifier = pair.first;
            automationType = pair.second;
            break;
        }
    }

    if (automationType == AutomationType::AT_UNKNOWN) {
        std::cout << "WARNING Vehicle::findTypesFromID AutomationType of " << vehicleID << " is unknown!" << std::endl;
        return false;
    }

    const unsigned int numElements = sizeof(vehicleList) / sizeof(vehicleList[0]);

    for (unsigned int i = 1; i < numElements; i++) {
        if (vehicleID.find(vehicleList[i]) != std::string::npos) {
            vehicleType = (VehicleType)i;
            vehIdName = vehicleList[i];
            break;
        }
    }

    if (vehicleType == VehicleType::VEH_UNKNOWN) {
        std::cout << "WARNING Vehicle::findTypesFromID vehicleType of " << vehicleID << " is unknown!" << std::endl;
        return false;
    }

    if (vehicleType == VEH_CAV) {
        is_CAV = true;
    }

    if (vehicleType == VEH_CV) {
        is_CV = true;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
const std::string& Vehicle::getIdentifier() const {
    return vehIdentifier;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool Vehicle::isConnected() const {
    return (isConnected(automationType));
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
bool Vehicle::isConnected(AutomationType type) {
    return (type == AT_CV || type == AT_CAV);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::sendCommands() {
    if (useMobilityInfo) {
        if (nodeController) {
            currentXposBegin = nodeController->GetNode()->getPosition().x;
            currentSpeed = nodeController->GetNode()->getSpeed();
            currentLaneIndex = nodeController->GetNode()->getLaneIndex();
        } else {
            currentXposBegin = 0.0;
            currentSpeed = 0.0;
            currentLaneIndex = -1;
        }
    } else {
        if (functions.isExitEnable) {
            if (!exit_n.done) {
                executeId.distanceExit = iface->vehicleGetDrivingDistance(vehID, exit_n.edge, exit_n.pos);
            }
        }

        if (functions.isTocEnable) {
            if (!toc.downwardTOCDone) {
                executeId.distanceDownwardToc = iface->vehicleGetDrivingDistance(vehID, toc.downwardEdge, toc.downwardPos);
            }
        }

        if (trackedData.enable) {
            executeId.distanceTrackedVeh = iface->vehicleGetDrivingDistance(vehID, trackedData.edge, trackedData.pos);
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::updateExit() {
    if (functions.isExitEnable) {
        if (!exit_n.done) {
            if (useMobilityInfo) {
                if (currentXposBegin >= exit_n.posBegin) {
                    setExit();
                }
            } else {
                if (executeId.distanceExit != -1) {
                    auto& response = iface->getTraCIResponse(executeId.distanceExit);

                    if (response.second != nullptr) {
                        double pos = std::dynamic_pointer_cast<libsumo::TraCIDouble>(response.second)->value;

                        if (pos <= 0.0) {
                            setExit();
                        }
                    } else {
                        std::cout << "Error reading exit distance : " << vehID << std::endl;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::setExit() {
    exit_n.done = true;

    if (general.debugMain) {
        std::cout << "Exit done for vehicle : " << vehID << ". Edge : " << exit_n.edge << " Pos : " << exit_n.pos << std::endl;
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::updateTeleportation() {
    //see in VehicleManager::findTeleportedVehicles()
}

//-------------------------------------------------------------------------------------------------------
// perform "take over control" (TOC) downward or upward
//-------------------------------------------------------------------------------------------------------
void Vehicle::updateToC() {
    if (functions.isTocEnable) {
        if (useNS3Messages && toc.downwardFromRsuDenm != -1) {
            // perform "take over control" (TOC) downward from mcm message coming from rsu
            if (!toc.downwardTOCDone && toc.downwardFromRsuDone) {
                setDownwardToc();
            }
        } else {
            if (!toc.downwardTOCDone) {
                if (useMobilityInfo) {
                    if (currentXposBegin >= toc.downwardPosBegin) {
                        setDownwardToc();
                    }
                } else {
                    auto& response = iface->getTraCIResponse(executeId.distanceDownwardToc);

                    if (response.second != nullptr) {
                        double pos = std::dynamic_pointer_cast<libsumo::TraCIDouble>(response.second)->value;

                        if (pos <= 0.0) {
                            setDownwardToc();
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::setDownwardToc() {
    toc.downwardTOCDone = true;

    if (randomDouble(0.0, 1.0) < toc.probability) {
        iface->vehicleSetToC(vehID, toc.leadTime); // This vehicle has to perform a ToC

        if (general.debugToc) {
            std::cout << "Downward TOC done at vehicle : " << vehID << std::endl;
        }
    } else {
        // vehicle will manage situation without a ToC. There is no need to perform a upward TOC as well

        if (general.debugToc) {
            std::cout << "Downward or updward TOC skipped due to probability, vehicle : " << vehID << std::endl;
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::updateTrackedVehicle() {
    if (trackedData.enable) {
        if (useMobilityInfo) {
            if (currentXposBegin >= trackedData.posBegin) {
                setTrackedVehicle();
            }
        } else {
            if (executeId.distanceTrackedVeh != -1) {
                auto& response = iface->getTraCIResponse(executeId.distanceTrackedVeh);

                if (response.second != nullptr) {
                    double pos = std::dynamic_pointer_cast<libsumo::TraCIDouble>(response.second)->value;

                    if (pos <= 0.0) {
                        setTrackedVehicle();
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::setTrackedVehicle() {
    trackedData.enable = false;
    iface->guiTrackVehicle(trackedData.view, vehID);
    std::cout << "Start tracking : " << vehID << std::endl;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::onAddSubscriptions() {
    //initialization occurs on next step
    if (!initialized) {
        initialise();
    }

    if (!exit_n.done) {
        sendCommands();
    }
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
std::string Vehicle::info() {
    std::stringstream temp;

    if (useMobilityInfo) {
        temp << ", pos : " << currentXposBegin
             << ", speed : " << currentSpeed
             << ", lane : " << currentLaneIndex;
    } else {
        temp << ", mobility info is disabled.";
    }

    return temp.str();
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Vehicle::execute() {
    if (initialized && !exit_n.done) {
        updateExit();
        updateTeleportation();

        updateToC();

        updateTrackedVehicle();

        executeId.reset();
    }
}

//-------------------------------------------------------------------------------------------------------
double Vehicle::randomDouble(double min, double max) {
    assert(max > min);
    double random = ((float)rand()) / (double)RAND_MAX;

    double range = max - min;
    return (random * range) + min;
}

//-------------------------------------------------------------------------------------------------------
bool Vehicle::setInfo(const int _nodeID, iCSInterface* _nodeController) {
    nodeID = _nodeID;
    nodeController = _nodeController;
    return (nodeID == -1 || nodeController == NULL) ? false : true;
}

//-------------------------------------------------------------------------------------------------------
//CAM = Cooperative Awareness Message
//DENM = Decentralised Environmental Notification Message
//MCM = Manoeuvre Coordination Message
//RSU = Road-Side Unit
//-------------------------------------------------------------------------------------------------------
void Vehicle::receiveMessage(server::Payload* payload, double snr) {
    /*CommHeader *commHeader;
    iface->GetHeader(payload, server::PAYLOAD_FRONT, commHeader);

    NodeInfo src;
    src.nodeId = commHeader->getSourceId();
    src.position = commHeader->getSourcePosition();*/

    TransaidHeader* transHeader;
    iface->GetHeader(payload, server::PAYLOAD_END, transHeader);

    MessageType msgType = transHeader->getMessageType();

    switch (msgType) {
        case TRANSAID_CAM:
            handle_CAM_message(transHeader);
            break;
        case TRANSAID_CPM:
            break;
        case TRANSAID_MCM_VEHICLE:
            break;
        case TRANSAID_MAP:
            break;
        case TRANSAID_DENM:
            handle_DENM_message(transHeader);
            break;
        case TRANSAID_IVI:
            break;
        case TRANSAID_MCM_RSU:
            break;
        case MT_ALL:
            break;
        default:
            return;
    }
}

//-------------------------------------------------------------------------------------------------------
void Vehicle::handle_CAM_message(TransaidHeader* header) {
    const TransaidHeader::CamInfo* camInfo = header->getCamInfo();

    std::string objectSendData("");

    //look in rsus list
    bool msgFromRSU(false);
    for (std::pair<std::string, int> rsu : VehicleManager::get().rsus) {
        if (camInfo->senderID == rsu.second) {
            msgFromRSU = true;
            objectSendData = rsu.first;
            break;
        }
    }

    if (!msgFromRSU) {
        objectSendData = VehicleManager::getInstance().getVehId(camInfo->senderID);

        if (objectSendData.empty()) {
            std::cout << "ERROR: CAM : " << camInfo->senderID << " not found" << std::endl;
        }
    }

    if (general.debugCamMessages) {
        std::cout << "Vehicle " << vehID << " (" << nodeID << ")"
                  << " received CAM msg at " << CurrentTime::Now()
                  << " from " << objectSendData << " (" << camInfo->senderID << ")"
                  << ", generationTime=" << camInfo->generationTime
                  << ", position=" << camInfo->position
                  << ", speed=" << camInfo->speed
                  << ", laneIndex=" << camInfo->laneIndex
                  << ", acceleration=" << camInfo->acceleration
                  << ", heading=" << camInfo->heading
                  << std::endl;
    }

}

//-------------------------------------------------------------------------------------------------------
void Vehicle::handle_DENM_message(TransaidHeader* header) {
    const TransaidHeader::DenmInfo* denmInfo = header->getDenmInfo();

    if (general.debugDenmMessages) {
        std::cout << "Vehicle " << vehID << " (" << nodeID << ")"
                  << " received DENM msg at " << CurrentTime::Now()
                  << ", from RSU (" << denmInfo->senderID << ")"
                  << ", generationTime=" << denmInfo->generationTime
                  << ", startingPoint=" << denmInfo->startingPoint
                  << ", endPoint=" << denmInfo->endPoint
                  << ", type of DENM=" << denmInfo->denmType
                  << std::endl;
    }

    if (denmInfo->denmType == DenmType::ROAD_WORKS) {
        if (functions.isTocEnable && toc.downwardFromRsuDenm == denmInfo->senderID) {
            if (!toc.downwardFromRsuDone) {
                if (denmInfo->startingPoint >= 0.0 && currentXposBegin >= denmInfo->startingPoint) {
                    toc.downwardFromRsuDone = true;

                    if (general.debugDenmMessages) {
                        std::cout << "Vehicle : " << vehID << " performing downward ToC due to ROAD_WORKS DENM msg. Point on road : " << denmInfo->startingPoint << std::endl;
                    }

                    if (toc.downwardDenmHighlight) {
                        nodeController->Highlight(sendColors[TRANSAID_DENM], HighlightSize[DENM_VEH], TRANSAID_DENM, HighlightDuration[DENM_VEH]);
                    }
                }
            }
        }
    }
}

} /* namespace application */
} // namespace ucapp
