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
/*
 *  Created on: Mar 31, 2020
 *  Author: Vasilios Karagounis
 */

#ifndef VEHICLE_DATA_H_
#define VEHICLE_DATA_H_

#include <iostream>

namespace vehicleData {
struct general_t {
    bool optimizeForSpeed;

    bool debugMain;
    bool debugToc;

    bool debugCamMessages;
    bool debugDenmMessages;

    bool handleTeleported;
};

struct functions_t {
    bool isTocEnable;
    bool isExitEnable;
};

struct exit_network_t {
    std::string edge;
    double pos;
    double posBegin;
    bool done;
};

struct toc_t {
    double probability;
    double leadTime;
    double downwardPos;
    double downwardPosBegin;

    int downwardFromRsuDenm;

    bool downwardFromRsuDone;
    bool downwardDenmHighlight;
    bool downwardTOCDone;

    std::string downwardEdge;
};

struct tracked_vehicle_t {
    bool enable;
    double pos;
    double posBegin;
    std::string edge;
    std::string view;
    std::string vehName;

    tracked_vehicle_t() {
        enable = false;
        view = edge = vehName = "";
        pos = 0.0;
    }
};

struct execute_ids_t {
    int distanceExit;
    int distanceDownwardToc;
    int distanceTrackedVeh;

    void reset() {
        distanceExit = distanceDownwardToc = distanceTrackedVeh = -1;
    }
};
} // namespace vehicleData

#endif /* VEHICLE_DATA_H_ */
