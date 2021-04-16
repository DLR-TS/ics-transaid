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
 * UCConfig.h
 *
 *  Created on: Jan 31, 2019
 *  Author: schwamborn
 *  Author: Vasilios Karagounis
 */

#ifndef UC_CONFIG_H_
#define UC_CONFIG_H_

#include <iostream>
#include <map>
#include <vector>
#include "application/model/common.h"

#define CHECK_BIT(var, offset) ((var) & (1 << (offset)))
#define SAFE_DELETE(p) if ((p) != NULL) { delete (p); (p) = NULL; }
#define SAFE_DELETE_ARRAY(a) { delete [] (a); (a) = NULL; }

namespace ucapp {
namespace application {
enum AutomationType {
    AT_UNKNOWN = 0,
    AT_MANUAL,
    AT_CV,
    AT_CAV
};

//DO NOT TOUCH ORDER OF ENUMS
enum VehicleType {
    VEH_UNKNOWN = 0,
    VEH_LV,
    VEH_CV,
    VEH_CAV
};

static const std::string vehicleList[] = {
    "UNKNOWN_VEHICLE",
    "LV",
    "CVToC",
    "CAVToC",
    "COUNT_MAX"
};

// map: AutomationType -> vType identifier
static std::map<std::string, AutomationType> AutomationTypeIdentifierMap = {
    std::pair<std::string, AutomationType>("LV", AT_MANUAL),
    std::pair<std::string, AutomationType>("CV", AT_CV),
    std::pair<std::string, AutomationType>("CAV", AT_CAV)
};

static std::map<baseapp::application::MessageType, std::string> receiveColors = {
    {baseapp::application::TRANSAID_CAM, {"blue"}},
    {baseapp::application::TRANSAID_DENM, {"yellow"}},
    {baseapp::application::TRANSAID_MCM_RSU, {"red"}},
    {baseapp::application::TRANSAID_MCM_VEHICLE, {"red"}}
};

static std::map<baseapp::application::MessageType, std::string> sendColors = {
    {baseapp::application::TRANSAID_CAM, {"blue"}},
    {baseapp::application::TRANSAID_DENM, {"yellow"}},
    {baseapp::application::TRANSAID_MCM_RSU, {"red"}},
    {baseapp::application::TRANSAID_MCM_VEHICLE, {"red"}}
};

enum Highlight {
    CAM_VEH = 0,
    DENM_VEH,
    MCM_VEH,
    MAPEM_VEH,

    CAM_RSU,
    DENM_RSU,
    MCM_RSU,

    CFM_NO_LEADER_VEH,
    CFM_LEADER_NO_CAV_VEH,
    CFM_LEADER_CAV_VEH
};

static std::map<Highlight, double> HighlightSize = {
    {CAM_VEH, 3.0},
    {DENM_VEH, 4.5},
    {MCM_VEH, 6.0},
    {MAPEM_VEH, 7.5},

    {CAM_RSU, 4.0},
    {DENM_RSU, 5.5},
    {MCM_RSU, 7.0},

    {CFM_NO_LEADER_VEH, 8.5},
    {CFM_LEADER_NO_CAV_VEH, 8.5},
    {CFM_LEADER_CAV_VEH, 8.5}
};

static std::map<Highlight, double> HighlightDuration = {
    {CAM_VEH, 2.0},
    {DENM_VEH, 1.0},
    {MCM_VEH, 1.0},
    {MAPEM_VEH, 1.0},

    {CAM_RSU, 1.0},
    {DENM_RSU, 1.0},
    {MCM_RSU, 1.0},

    {CFM_NO_LEADER_VEH, 3.0},
    {CFM_LEADER_NO_CAV_VEH, 3.0},
    {CFM_LEADER_CAV_VEH, 3.0}
};

static const std::string emptyString("");
static const std::string viewID = "View #0";

} /* namespace application */
} // namespace ucapp

#endif /* UC_CONFIG_H_ */
