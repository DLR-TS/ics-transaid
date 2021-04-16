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
 * Author Federico Caselli <f.caselli@unibo.it>
 * University of Bologna
 ***************************************************************************************/

#ifndef PROGRAM_CONFIGURATION_H_
#define PROGRAM_CONFIGURATION_H_

#include <map>
#include <string>
#include <memory>
#include "structs.h"
#include "vector.h"

namespace tinyxml2 {
class XMLElement;
}

namespace baseapp {

enum {
    LOG_FILE = 0, DATA_FILE = 2, NS_LOG_FILE = 1
} typedef LogType;


struct TLLane {
    double dir;
    std::string controlledLane;
    std::string followingLane;
    std::string friendlyName;
};

struct Direction {
    Direction() {
        leaving = false;
        approaching = true;
        direction = INVALID_DIRECTION;
        approachingTime = leavingTime = 0;
    }
    double direction;
    bool approaching;
    bool leaving;
    uint16_t approachingTime;
    uint16_t leavingTime;
};

struct RsuData {
    int id;
    application::Vector2D position;
    std::vector<Direction> directions;
    std::vector<TLLane> lanes;
    Circle cam_area;
    Circle car_area;
} typedef RsuData;


class ProgramConfiguration {
public:
    static int LoadConfiguration(const char* fileName, int port);

    static int GetStartTime() {
        return m_instance->m_start;
    }
    static int GetSocketPort() {
        return m_instance->m_socket;
    }
    static void SetSocketPort(int port) {
        m_instance->m_socket = port;
    }
    static unsigned GetMessageLifetime() {
        return m_instance->m_messageLifetime;
    }
    static unsigned GetSumoTotalDemandLevel() {
        return m_instance->m_sumoTotalDemandLevel;
    }

    static const std::string& GetTestCase() {
        return m_instance->m_testCase;
    }

    static bool GetLogFileName(LogType type, std::string& fileName);
    static bool IsRsu(const int id);
    static const RsuData& GetRsuData(const int id);

    // constructor and destructor should be protected as far as I am concerned but gcc thinks differently
    ProgramConfiguration();
    virtual ~ProgramConfiguration();
protected:
    int ParseGeneral(tinyxml2::XMLElement* general);
    int ParseInfrastructure(tinyxml2::XMLElement* infrastructure);
    int ParseSetup(tinyxml2::XMLElement* setup);
    int ParseOutput(tinyxml2::XMLElement* output);
    int ParseNodeSampler(tinyxml2::XMLElement* nodeSampler);
    void ParseLog(const tinyxml2::XMLElement* element, const LogType type);
    static std::unique_ptr<ProgramConfiguration> m_instance;

private:
    int m_start;
    int m_socket;
    unsigned m_messageLifetime;
    unsigned m_sumoTotalDemandLevel;
    std::string m_testCase;
    std::map<int, RsuData> m_rsus;
    std::map<LogType, std::string> m_logs;
};

} /* namespace protocol */

#endif /* PROGRAM_CONFIGURATION_H_ */
